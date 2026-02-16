
#include <awsmock/service/s3/S3Handler.h>

namespace AwsMock::Service {
    http::response<http::dynamic_body> S3Handler::HandleGetRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_debug << "S3 GET request, URI: " << request.target() << " region: " << region << " user: " + user;

        // Get the command
        Dto::Common::S3ClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        log_debug << "S3 GET request, URI: " << request.target() << " region: " << clientCommand.region << " user: " + clientCommand.user;

        try {
            switch (clientCommand.command) {
                case Dto::Common::S3CommandType::LIST_BUCKETS: {
                    Dto::S3::ListAllBucketResponse s3Response = _s3Service.ListAllBuckets();
                    log_info << "List buckets, count: " << s3Response.total;
                    log_trace << s3Response;
                    return SendResponse(request, http::status::ok, s3Response.ToXml());
                }

                case Dto::Common::S3CommandType::LIST_OBJECTS: {
                    Dto::S3::ListBucketRequest s3Request;

                    if (Core::HttpUtils::HasQueryParameter(request.target(), "list-type")) {
                        int listType = Core::HttpUtils::GetIntParameter(request.target(), "list-type", 0, 1000, 0);

                        std::string delimiter;
                        if (Core::HttpUtils::HasQueryParameter(request.target(), "delimiter")) {
                            delimiter = Core::HttpUtils::GetStringParameter(request.target(), "delimiter");
                        }

                        std::string prefix;
                        if (Core::HttpUtils::HasQueryParameter(request.target(), "prefix")) {
                            prefix = Core::HttpUtils::GetStringParameter(request.target(), "prefix");
                        }

                        std::string encodingType = "url";
                        if (Core::HttpUtils::HasQueryParameter(request.target(), "encoding_type")) {
                            encodingType = Core::HttpUtils::GetStringParameter(request.target(), "encoding_type");
                        }

                        // Return an object list
                        s3Request.region = clientCommand.region;
                        s3Request.name = clientCommand.bucket;
                        s3Request.prefix = prefix;
                        s3Request.delimiter = delimiter;
                        s3Request.encodingType = encodingType;
                        s3Request.listType = listType;
                    } else {
                        s3Request.region = clientCommand.region;
                        s3Request.name = clientCommand.bucket;
                        s3Request.listType = 1;
                        s3Request.encodingType = "url";
                    }

                    Dto::S3::ListBucketResponse s3Response = _s3Service.ListBucket(s3Request);

                    log_info << "List objects, bucket: " << clientCommand.bucket;
                    return SendResponse(request, http::status::ok, s3Response.ToXml());
                }

                case Dto::Common::S3CommandType::GET_OBJECT: {
                    // Get object request
                    log_debug << "S3 get object request, bucket: " << clientCommand.bucket << " key: " << clientCommand.key;
                    Dto::S3::GetObjectRequest s3Request;
                    s3Request.region = clientCommand.region;
                    s3Request.bucket = clientCommand.bucket;
                    s3Request.key = clientCommand.key;

                    // Get version ID
                    if (std::string versionId = Core::HttpUtils::GetStringParameter(request.target(), "versionId"); !versionId.empty()) {
                        s3Request.versionId = versionId;
                    }

                    // Get the range
                    long size;
                    GetRange(request, s3Request.min, s3Request.max, size);

                    // Get the object
                    Dto::S3::GetObjectResponse s3Response = _s3Service.GetObject(s3Request);

                    std::map<std::string, std::string> headerMap;
                    headerMap["ETag"] = Core::StringUtils::Quoted(s3Response.md5sum);
                    headerMap["Content-Type"] = s3Response.contentType;
                    headerMap["Last-Modified"] = Core::DateTimeUtils::HttpFormat(s3Response.modified);
                    headerMap["x-amz-storage-class"] = s3Response.storageClass;

                    if (!s3Response.storageClass.empty() && s3Response.storageClass != Database::Entity::S3::StorageClassToString(Database::Entity::S3::StorageClass::STANDARD)) {
                        headerMap["Content-Type"] = s3Response.contentType;
                        std::string body = "<Error><Code>InvalidObjectState</Code><Message>The action is not valid for the object's storage class</Message></Error>";
                        return SendResponse(request, http::status::forbidden, body, headerMap);
                    }

                    // Set user headers
                    for (const auto &[fst, snd]: s3Response.metadata) {
                        headerMap["x-amz-meta-" + fst] = snd;
                    }

                    if (Core::HttpUtils::HasHeader(request, "Range")) {
                        log_info << "Multi-part download progress: " << std::to_string(s3Request.min) << "-" << std::to_string(s3Request.max) << "/" << std::to_string(s3Response.size);
                        return SendResponse(request, s3Response.filename, s3Request.min, s3Request.max, size, s3Response.size, http::status::partial_content, headerMap);
                    }
                    log_info << "Get object, bucket: " << clientCommand.bucket << " key: " << clientCommand.key;
                    return SendResponse(request, http::status::ok, s3Response.filename, s3Response.size, headerMap);
                }

                case Dto::Common::S3CommandType::GET_OBJECT_RANGE: {
                    // Get object request
                    log_debug << "S3 get object request, bucket: " << clientCommand.bucket << " key: " << clientCommand.key;
                    Dto::S3::GetObjectRequest s3Request;
                    s3Request.region = clientCommand.region;
                    s3Request.bucket = clientCommand.bucket;
                    s3Request.key = clientCommand.key;

                    // Get range
                    long size;
                    GetRange(request, s3Request.min, s3Request.max, size);

                    // Get an object
                    Dto::S3::GetObjectResponse s3Response = _s3Service.GetObject(s3Request);

                    std::map<std::string, std::string> headerMap;
                    headerMap["ETag"] = Core::StringUtils::Quoted(s3Response.md5sum);
                    headerMap["Content-Type"] = s3Response.contentType;
                    headerMap["Last-Modified"] = Core::DateTimeUtils::HttpFormat(s3Response.modified);

                    // Set user headers
                    for (const auto &[fst, snd]: s3Response.metadata) {
                        headerMap["x-amz-meta-" + fst] = snd;
                    }

                    // Send range response
                    log_debug << "Range download request: " << std::to_string(s3Request.min) << "-" << std::to_string(s3Request.max) << "/" << std::to_string(s3Response.size);
                    return SendResponse(request, s3Response.filename, s3Request.min, s3Request.max, size, s3Response.size, http::status::partial_content, headerMap);
                }

                case Dto::Common::S3CommandType::LIST_OBJECT_VERSIONS: {
                    // Get object request
                    log_debug << "S3 list object versions request, bucket: " << clientCommand.bucket << " prefix: " << clientCommand.prefix;

                    std::string delimiter = Core::HttpUtils::GetStringParameter(request.target(), "delimiter");
                    std::string encodingType = Core::HttpUtils::GetStringParameter(request.target(), "encoding-type");
                    std::string keyMarker = Core::HttpUtils::GetStringParameter(request.target(), "key-marker");
                    std::string versionIdMarker = Core::HttpUtils::GetStringParameter(request.target(), "version-id-marker");
                    std::string sPageSize = Core::HttpUtils::GetStringParameter(request.target(), "max-keys");

                    // Convert maxKeys
                    int pageSize = 1000;
                    if (!sPageSize.empty()) {
                        pageSize = std::stoi(sPageSize);
                    }

                    // Build request
                    Dto::S3::ListObjectVersionsRequest s3Request;
                    s3Request.region = clientCommand.region;
                    s3Request.bucket = clientCommand.bucket;
                    s3Request.prefix = clientCommand.prefix;
                    s3Request.delimiter = delimiter;
                    s3Request.encodingType = encodingType;
                    s3Request.maxKeys = pageSize;
                    s3Request.versionIdMarker = versionIdMarker;

                    // Get object versions
                    Dto::S3::ListObjectVersionsResponse s3Response = _s3Service.ListObjectVersions(s3Request);

                    log_info << "List object versions";
                    return SendResponse(request, http::status::ok, s3Response.ToXml());
                }

                // Delete an object (rm) with recursive option, issues first a list request
                case Dto::Common::S3CommandType::DELETE_OBJECT: {
                    Dto::S3::ListBucketRequest s3Request;

                    if (Core::HttpUtils::HasQueryParameter(request.target(), "list-type")) {
                        int listType = Core::HttpUtils::GetIntParameter(request.target(), "list-type", 0, 1000, 0);

                        std::string delimiter;
                        if (Core::HttpUtils::HasQueryParameter(request.target(), "delimiter")) {
                            delimiter = Core::HttpUtils::GetStringParameter(request.target(), "delimiter");
                        }

                        std::string prefix;
                        if (Core::HttpUtils::HasQueryParameter(request.target(), "prefix")) {
                            prefix = Core::HttpUtils::GetStringParameter(request.target(), "prefix");
                        }

                        std::string encodingType = "url";
                        if (Core::HttpUtils::HasQueryParameter(request.target(), "encoding_type")) {
                            encodingType = Core::HttpUtils::GetStringParameter(request.target(), "encoding_type");
                        }

                        // Return the object list
                        s3Request.region = clientCommand.region;
                        s3Request.name = clientCommand.bucket;
                        s3Request.prefix = prefix;
                        s3Request.delimiter = delimiter;
                        s3Request.encodingType = encodingType;
                        s3Request.listType = listType;
                    } else {
                        s3Request.region = clientCommand.region;
                        s3Request.name = clientCommand.bucket;
                        s3Request.listType = 1;
                        s3Request.encodingType = "url";
                    }

                    Dto::S3::ListBucketResponse s3Response = _s3Service.ListBucket(s3Request);
                    log_info << "List objects, bucket: " << clientCommand.bucket;
                    return SendResponse(request, http::status::ok, s3Response.ToXml());
                }

                // Delete a bucket (rb), bucket must be empty
                case Dto::Common::S3CommandType::DELETE_BUCKET: {
                    Dto::S3::DeleteBucketRequest s3Request;
                    s3Request.region = clientCommand.region;
                    s3Request.user = clientCommand.user;
                    s3Request.requestId = clientCommand.requestId;
                    s3Request.bucket = clientCommand.bucket;
                    _s3Service.DeleteBucket(s3Request);
                    log_info << "Delete bucket, bucket: " << clientCommand.bucket;
                    return SendResponse(request, http::status::no_content);
                }

                case Dto::Common::S3CommandType::GET_BUCKET_LIFECYCLE_CONFIGURATION: {
                    log_debug << "Get bucket lifecycle configuration request, bucket: " << clientCommand.bucket;

                    // S3 lifecycle configuration
                    Dto::S3::GetBucketLifecycleConfigurationRequest s3Request;
                    s3Request.region = clientCommand.region;
                    s3Request.user = clientCommand.user;
                    s3Request.requestId = clientCommand.requestId;
                    s3Request.bucket = clientCommand.bucket;

                    Dto::S3::GetBucketLifecycleConfigurationResponse s3Response = _s3Service.GetBucketLifecycleConfiguration(s3Request);

                    return SendResponse(request, http::status::ok, s3Response.ToXml());
                }

                default:
                    log_error << "Unknown method";
                    return SendResponse(request, http::status::bad_request, "Unknown method");
            }

        } catch (Core::NotFoundException &exc) {
            log_error << "Bucket or object not found, exception: " << exc.what();
            return SendResponse(request, http::status::not_found, exc.what());
        } catch (std::exception &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.what());
        } catch (...) {
            log_error << "Invalid request";
            return SendResponse(request, http::status::internal_server_error, "Invalid request");
        }
    }

    http::response<http::dynamic_body> S3Handler::HandlePutRequest(http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_debug << "S3 PUT request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::S3ClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {
            switch (clientCommand.command) {
                case Dto::Common::S3CommandType::CREATE_BUCKET: {
                    Dto::S3::CreateBucketRequest s3Request;
                    s3Request.region = clientCommand.region;
                    s3Request.name = clientCommand.bucket;
                    s3Request.owner = clientCommand.user;
                    Dto::S3::CreateBucketResponse s3Response = _s3Service.CreateBucket(s3Request);
                    return SendResponse(request, http::status::ok, s3Response.ToXml());
                    log_info << "Create bucket, bucket: " << clientCommand.bucket << " key: " << clientCommand.key;
                }

                case Dto::Common::S3CommandType::PUT_OBJECT: {
                    // Get the user metadata
                    std::map<std::string, std::string> metadata = GetMetadata(request);
                    if (clientCommand.copyRequest) {
                        Dto::S3::CopyObjectRequest s3Request;
                        s3Request.region = clientCommand.region;
                        s3Request.user = clientCommand.user;
                        s3Request.targetBucket = clientCommand.bucket;
                        s3Request.targetKey = clientCommand.key;
                        s3Request.metadata = metadata;

                        // Get S3 source bucket/key
                        GetBucketKeyFromHeader(Core::HttpUtils::GetHeaderValue(request, "x-amz-copy-source"), s3Request.sourceBucket, s3Request.sourceKey);

                        Dto::S3::CopyObjectResponse s3Response = _s3Service.CopyObject(s3Request);

                        log_info << "Copy object, bucket: " << clientCommand.bucket << " key: " << clientCommand.key;
                        return SendResponse(request, http::status::ok, s3Response.ToXml());
                    }

                    // Checksum/chunked encoding
                    std::string checksumAlgorithm = Core::HttpUtils::GetHeaderValue(request, "x-amz-sdk-checksum-algorithm");
                    bool chunked = Core::HttpUtils::HasHeaderValue(request, "Content-Encoding", "aws-chunked");

                    // S3 put object request
                    Dto::S3::PutObjectRequest putObjectRequest;
                    putObjectRequest.region = clientCommand.region;
                    putObjectRequest.bucket = clientCommand.bucket;
                    putObjectRequest.key = clientCommand.key;
                    putObjectRequest.owner = clientCommand.user;
                    putObjectRequest.md5Sum = clientCommand.contentMd5;
                    putObjectRequest.contentType = clientCommand.contentType;
                    putObjectRequest.checksumAlgorithm = checksumAlgorithm;
                    putObjectRequest.metadata = metadata;
                    putObjectRequest.storageClass = clientCommand.storageClass;

                    boost::beast::net::streambuf sb;
                    putObjectRequest.contentLength = PrepareBody(request, sb);
                    std::istream stream(&sb);

                    // If chunked, we take the content length from the decoded content length header field.
                    if (chunked) {
                        putObjectRequest.contentLength = std::stol(Core::HttpUtils::GetHeaderValue(request, "x-amz-decoded-content-length"));
                    }
                    Dto::S3::PutObjectResponse putObjectResponse = _s3Service.PutObject(putObjectRequest, stream);

                    log_info << "Put object, bucket: " << clientCommand.bucket << " key: " << clientCommand.key << " size: " << putObjectResponse.contentLength;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::S3CommandType::MOVE_OBJECT: {
                    log_debug << "Object move request, bucket: " << clientCommand.bucket << " key: " << clientCommand.key;


                    Dto::S3::MoveObjectRequest s3Request;
                    s3Request.region = clientCommand.region;
                    s3Request.user = clientCommand.user;
                    s3Request.targetBucket = clientCommand.bucket;
                    s3Request.targetKey = clientCommand.key;

                    // Get S3 source bucket/key
                    GetBucketKeyFromHeader(Core::HttpUtils::GetHeaderValue(request, "x-amz-copy-source"), s3Request.sourceBucket, s3Request.sourceKey);

                    // Get the user metadata
                    s3Request.metadata = GetMetadata(request);

                    Dto::S3::MoveObjectResponse s3Response = _s3Service.MoveObject(s3Request);

                    return SendResponse(request, http::status::ok, s3Response.ToXml());
                    log_info << "Move object, bucket: " << clientCommand.bucket << " key: " << clientCommand.key;
                }

                case Dto::Common::S3CommandType::UPLOAD_PART: {
                    std::string partNumber = Core::HttpUtils::GetStringParameter(request.target(), "partNumber");
                    std::string uploadId = Core::HttpUtils::GetStringParameter(request.target(), "uploadId");
                    long contentLength = std::stol(request.base()[http::field::content_length]);
                    log_debug << "S3 multipart upload part: " << partNumber << " size: " << contentLength;

                    // If chunked, we take the content length from the decoded content length header field.
                    boost::beast::net::streambuf sb;
                    contentLength = PrepareBody(request, sb);
                    std::istream stream(&sb);
                    std::string eTag = S3Service::UploadPart(stream, std::stoi(partNumber), uploadId, contentLength);

                    std::map<std::string, std::string> headerMap;
                    headerMap["ETag"] = Core::StringUtils::Quoted(eTag);
                    log_info << "Finished S3 multipart upload part: " << partNumber;

                    return SendResponse(request, http::status::ok, {}, headerMap);
                }

                case Dto::Common::S3CommandType::UPLOAD_PART_COPY: {
                    std::string partNumber = Core::HttpUtils::GetStringParameter(request.target(), "partNumber");
                    std::string uploadId = Core::HttpUtils::GetStringParameter(request.target(), "uploadId");

                    // Get range
                    Dto::S3::UploadPartCopyRequest s3Request;
                    s3Request.region = region;
                    s3Request.uploadId = uploadId;
                    s3Request.partNumber = std::stol(partNumber);
                    s3Request.targetBucket = clientCommand.bucket;
                    s3Request.targetKey = clientCommand.key;

                    // Get S3 source bucket/key
                    GetBucketKeyFromHeader(Core::HttpUtils::GetHeaderValue(request, "x-amz-copy-source"), s3Request.sourceBucket, s3Request.sourceKey);

                    if (Core::HttpUtils::HasHeader(request, "x-amz-copy-source-range")) {
                        std::string rangeStr = Core::HttpUtils::GetHeaderValue(request, "x-amz-copy-source-range");
                        std::string parts = Core::StringUtils::Split(rangeStr, "=")[1];
                        s3Request.min = std::stol(Core::StringUtils::Split(parts, "-")[0]);
                        s3Request.max = std::stol(Core::StringUtils::Split(parts, "-")[1]);
                        log_debug << "Requested multipart download range: " << std::to_string(s3Request.min) << "-" << std::to_string(s3Request.max);
                    }
                    log_debug << "S3 multipart upload part copy: " << partNumber;

                    Dto::S3::UploadPartCopyResponse s3Response = _s3Service.UploadPartCopy(s3Request);

                    log_info << "Finished S3 multipart upload part copy: " << partNumber;

                    return SendResponse(request, http::status::ok, s3Response.ToXml());
                }

                case Dto::Common::S3CommandType::PUT_BUCKET_NOTIFICATION_CONFIGURATION: {
                    log_debug << "Put bucket notification configuration request, bucket: " << clientCommand.bucket;

                    // S3 notification setup
                    std::string body = Core::HttpUtils::GetBodyAsString(request);
                    Dto::S3::PutBucketNotificationConfigurationRequest s3Request;
                    s3Request.FromXml(body);
                    s3Request.region = clientCommand.region;
                    s3Request.bucket = clientCommand.bucket;

                    Dto::S3::PutBucketNotificationConfigurationResponse s3Response = _s3Service.PutBucketNotificationConfiguration(s3Request);

                    return SendResponse(request, http::status::ok, s3Response.ToXml());
                }

                case Dto::Common::S3CommandType::PUT_BUCKET_ENCRYPTION: {
                    log_debug << "Put bucket encryption configuration request, bucket: " << clientCommand.bucket;

                    // S3 bucket encryption
                    std::string body = Core::HttpUtils::GetBodyAsString(request);
                    Dto::S3::PutBucketEncryptionRequest s3Request;
                    s3Request.FromXml(body);
                    s3Request.region = clientCommand.region;
                    s3Request.bucket = clientCommand.bucket;

                    _s3Service.PutBucketEncryption(s3Request);

                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::S3CommandType::PUT_BUCKET_VERSIONING: {
                    log_debug << "Put bucket versioning configuration request, bucket: " << clientCommand.bucket;

                    // S3 versioning setup
                    std::string body = Core::HttpUtils::GetBodyAsString(request);
                    Dto::S3::PutBucketVersioningRequest s3Request;
                    s3Request.FromXml(body);
                    s3Request.region = clientCommand.region;
                    s3Request.bucket = clientCommand.bucket;

                    _s3Service.PutBucketVersioning(s3Request);

                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::S3CommandType::PUT_BUCKET_LIFECYCLE_CONFIGURATION: {
                    log_debug << "Put bucket lifecycle configuration request, bucket: " << clientCommand.bucket;

                    // S3 lifecycle configuration
                    Dto::S3::PutBucketLifecycleConfigurationRequest s3Request;
                    s3Request.FromXml(Core::HttpUtils::GetBodyAsString(request));
                    s3Request.region = clientCommand.region;
                    s3Request.bucket = clientCommand.bucket;

                    _s3Service.PutBucketLifecycleConfiguration(s3Request);

                    return SendResponse(request, http::status::ok);
                }

                default:
                    log_error << "Bad request, method: PUT clientCommand: " << Dto::Common::S3CommandTypeToString(clientCommand.command);
                    return SendResponse(request, http::status::bad_request, "Unknown method");
            }
        } catch (Core::NotFoundException &exc) {
            log_error << exc.message();
            return SendResponse(request, http::status::not_found, exc.message());
        } catch (std::exception &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.what());
        } catch (...) {
            log_error << "Unknown exception";
            return SendResponse(request, http::status::internal_server_error, "Unknown exception");
        }
    }

    http::response<http::dynamic_body> S3Handler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_debug << "S3 POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::S3ClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {
            switch (clientCommand.command) {
                case Dto::Common::S3CommandType::COPY_OBJECT: {
                    if (clientCommand.multipartRequest) {
                        log_debug << "Starting multipart upload";

                        Dto::S3::CreateMultipartUploadRequest s3Request;
                        s3Request.region = clientCommand.region;
                        s3Request.bucket = clientCommand.bucket;
                        s3Request.key = clientCommand.key;
                        s3Request.user = clientCommand.user;
                        Dto::S3::CreateMultipartUploadResult result = _s3Service.CreateMultipartUpload(s3Request);

                        log_info << "Copy object, bucket: " << clientCommand.bucket << " key: " << clientCommand.key;
                        return SendResponse(request, http::status::ok, result.ToXml());
                    }
                    std::string uploadId = Core::HttpUtils::GetStringParameter(request.target(), "uploadId");
                    log_debug << "Finish multipart upload request, uploadId: " << uploadId;

                    Dto::S3::CompleteMultipartUploadRequest s3Request;
                    s3Request.region = clientCommand.region;
                    s3Request.bucket = clientCommand.bucket;
                    s3Request.key = clientCommand.key;
                    s3Request.uploadId = uploadId;
                    Dto::S3::CompleteMultipartUploadResult result = _s3Service.CompleteMultipartUpload(s3Request);

                    std::map<std::string, std::string> headers;
                    headers["ETag"] = Core::StringUtils::Quoted(result.etag);

                    log_info << "Copy object, bucket: " << clientCommand.bucket << " key: " << clientCommand.key;
                    return SendResponse(request, http::status::ok, result.ToXml(), headers);
                }

                case Dto::Common::S3CommandType::DELETE_OBJECTS: {
                    log_debug << "Starting delete objects request";

                    const std::string &payload = Core::HttpUtils::GetBodyAsString(request);
                    if (payload.empty()) {
                        return SendResponse(request, http::status::no_content);
                    }
                    Dto::S3::DeleteObjectsRequest s3Request;
                    s3Request.FromXml(payload);
                    s3Request.region = clientCommand.region;
                    s3Request.bucket = clientCommand.bucket;

                    Dto::S3::DeleteObjectsResponse s3Response = _s3Service.DeleteObjects(s3Request);
                    log_info << "Object deleted, bucket: " << clientCommand.bucket << " key: " << clientCommand.key;
                    return SendResponse(request, http::status::ok, s3Response.ToXml());
                }

                case Dto::Common::S3CommandType::CREATE_MULTIPART_UPLOAD: {
                    log_debug << "Starting multipart upload, bucket: " << clientCommand.bucket << " key: " << clientCommand.key;

                    Dto::S3::CreateMultipartUploadRequest s3Request;
                    s3Request.region = clientCommand.region;
                    s3Request.bucket = clientCommand.bucket;
                    s3Request.key = clientCommand.key;
                    s3Request.user = clientCommand.user;
                    s3Request.metadata = GetMetadata(request);

                    Dto::S3::CreateMultipartUploadResult result = _s3Service.CreateMultipartUpload(s3Request);

                    log_info << "Created multi-part upload, bucket: " << clientCommand.bucket << " key: " << clientCommand.key;
                    return SendResponse(request, http::status::ok, result.ToXml());
                }

                case Dto::Common::S3CommandType::COMPLETE_MULTIPART_UPLOAD: {
                    log_debug << "Completing multipart upload, bucket: " << clientCommand.bucket << " key: " << clientCommand.key;
                    std::string uploadId = Core::HttpUtils::GetStringParameter(request.target(), "uploadId");
                    Dto::S3::CompleteMultipartUploadRequest s3Request;
                    s3Request.region = clientCommand.region;
                    s3Request.bucket = clientCommand.bucket;
                    s3Request.key = clientCommand.key;
                    s3Request.uploadId = uploadId;
                    s3Request.contentType = clientCommand.contentType;
                    Dto::S3::CompleteMultipartUploadResult s3Response = _s3Service.CompleteMultipartUpload(s3Request);

                    std::map<std::string, std::string> headers;
                    headers["ETag"] = Core::StringUtils::Quoted(s3Response.etag);

                    log_info << "Completed multipart upload, bucket: " << clientCommand.bucket << " key: " << clientCommand.key;
                    return SendResponse(request, http::status::ok, s3Response.ToXml(), headers);
                }

                case Dto::Common::S3CommandType::LIST_BUCKET_COUNTERS: {
                    // Get bucket counter request
                    Dto::S3::ListBucketCounterRequest s3Request = Dto::S3::ListBucketCounterRequest::FromJson(clientCommand);
                    Dto::S3::ListBucketCounterResponse s3Response = _s3Service.ListBucketCounters(s3Request);
                    log_info << "List bucket counters, total: " << s3Response.total << ", count: " << s3Response.bucketCounters.size();
                    return SendResponse(request, http::status::ok, s3Response.ToJson());
                }

                case Dto::Common::S3CommandType::LIST_BUCKET_ARNS: {
                    // Get bucket ARNs request
                    Dto::S3::ListBucketArnsResponse s3Response = _s3Service.ListBucketArns();
                    log_info << "List bucket arns, total: " << s3Response.bucketArns.size();
                    return SendResponse(request, http::status::ok, s3Response.ToJson());
                }

                case Dto::Common::S3CommandType::LIST_OBJECT_COUNTERS: {
                    // Get object request
                    Dto::S3::ListObjectCounterRequest s3Request = Dto::S3::ListObjectCounterRequest::FromJson(clientCommand);
                    Dto::S3::ListObjectCounterResponse s3Response = _s3Service.ListObjectCounters(s3Request);
                    log_info << "List object counters, total: " << s3Response.total << ", count: " << s3Response.total;
                    return SendResponse(request, http::status::ok, s3Response.ToJson());
                }

                case Dto::Common::S3CommandType::GET_BUCKET: {
                    log_debug << "S3 get bucket request";

                    // Build request
                    Dto::S3::GetBucketRequest s3Request = Dto::S3::GetBucketRequest::FromJson(Core::HttpUtils::GetBodyAsString(request));
                    Dto::S3::GetBucketResponse s3Response = _s3Service.GetBucket(s3Request);

                    log_info << "Get bucket, name: " << s3Request.bucketName << ", body: " << s3Response.ToJson();
                    return SendResponse(request, http::status::ok, s3Response.ToJson());
                }

                case Dto::Common::S3CommandType::PURGE_BUCKET: {
                    log_debug << "S3 purge bucket request, bucket: " << clientCommand.bucket;

                    // Build request
                    Dto::S3::PurgeBucketRequest s3Request = Dto::S3::PurgeBucketRequest::FromJson(Core::HttpUtils::GetBodyAsString(request));
                    const long deleted = _s3Service.PurgeBucket(s3Request);
                    log_info << "Purge bucket, name: " << s3Request.bucketName << ", deleted: " << deleted;
                    return SendResponse(request, http::status::ok, {});
                }

                case Dto::Common::S3CommandType::UPDATE_BUCKET: {
                    log_debug << "S3 update bucket request";

                    // Build request
                    Dto::S3::UpdateBucketRequest s3Request = Dto::S3::UpdateBucketRequest::FromJson(Core::HttpUtils::GetBodyAsString(request));

                    // Get object versions
                    _s3Service.UpdateBucket(s3Request);

                    log_info << "Update bucket, name: " << s3Request.bucket;
                    return SendResponse(request, http::status::ok, {});
                }

                case Dto::Common::S3CommandType::GET_OBJECT_COUNTER: {
                    log_debug << "S3 get object counter request";

                    // Build request
                    Dto::S3::GetObjectCounterRequest s3Request = Dto::S3::GetObjectCounterRequest::FromJson(clientCommand);
                    Dto::S3::GetObjectCounterResponse s3Response = _s3Service.GetObjectCounters(s3Request);

                    log_info << "Get object counter, name: " << s3Request.oid;
                    return SendResponse(request, http::status::ok, s3Response.ToJson());
                }

                case Dto::Common::S3CommandType::UPLOAD_OBJECT_COUNTER: {
                    log_debug << "Upload upload counter request";

                    // Build request
                    Dto::S3::UploadObjectCounterRequest s3Request = Dto::S3::UploadObjectCounterRequest::FromJson(clientCommand);
                    _s3Service.UploadObjectCounter(s3Request);

                    log_info << "Upload object counter, bucketName: " << s3Request.bucketName << ", key: " << s3Request.objectKey;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::S3CommandType::TOUCH_OBJECT: {
                    log_debug << "S3 touch object";

                    // Build request
                    Dto::S3::TouchObjectRequest s3Request = Dto::S3::TouchObjectRequest::FromJson(clientCommand);
                    boost::asio::post(_ioc, [this, s3Request] {
                        _s3Service.TouchObject(s3Request);
                    });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::S3CommandType::UPDATE_OBJECT: {
                    log_debug << "S3 update object";

                    // Build request
                    Dto::S3::UpdateObjectRequest s3Request = Dto::S3::UpdateObjectRequest::FromJson(clientCommand);
                    boost::asio::post(_ioc, [this, s3Request] {
                        _s3Service.UpdateObject(s3Request);
                    });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::S3CommandType::GET_EVENT_SOURCE: {
                    Dto::S3::GetEventSourceRequest s3Request = Dto::S3::GetEventSourceRequest::FromJson(clientCommand);
                    Dto::S3::GetEventSourceResponse s3Response = _s3Service.GetEventSource(s3Request);
                    log_trace << "Get event source, functionArn: " << s3Request.functionArn;

                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::S3CommandType::DELETE_BUCKET_COUNTER: {
                    Dto::S3::DeleteBucketRequest s3Request = Dto::S3::DeleteBucketRequest::FromJson(clientCommand);
                    _s3Service.DeleteBucket(s3Request);
                    log_trace << "Bucket deleted, bucketName: " << s3Request.bucket;

                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::S3CommandType::ADD_BUCKET_COUNTER: {
                    Dto::S3::CreateBucketRequest s3Request = Dto::S3::CreateBucketRequest::FromJson(clientCommand);
                    Dto::S3::CreateBucketResponse s3Response = _s3Service.CreateBucket(s3Request);
                    log_trace << "Bucket added, bucketName: " << s3Request.name;

                    return SendResponse(request, http::status::ok, s3Response.ToJson());
                }

                case Dto::Common::S3CommandType::DELETE_ALL_OBJECTS: {
                    // Build request
                    Dto::S3::DeleteObjectsRequest s3Request = Dto::S3::DeleteObjectsRequest::FromJson(clientCommand);
                    boost::asio::post(_ioc, [this, s3Request] {
                        const auto s3response = _s3Service.DeleteObjects(s3Request);
                        log_info << "Delete all objects, region: " << s3Request.region << ", bucket: " << s3Request.bucket << ", count: " << s3response.keys.size();
                    });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::S3CommandType::DELETE_OBJECT_COUNTER: {
                    // Build request
                    Dto::S3::DeleteObjectRequest s3Request = Dto::S3::DeleteObjectRequest::FromJson(clientCommand);
                    _s3Service.DeleteObject(s3Request);
                    log_info << "Delete object, region: " << s3Request.region << ", bucket: " << s3Request.bucket;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::S3CommandType::UNKNOWN: {
                    Core::HttpUtils::DumpRequest(request);
                    log_error << "Unknown method";
                    return SendResponse(request, http::status::bad_request, "Unknown method");
                }

                default:
                    Core::HttpUtils::DumpRequest(request);
                    log_error << "Unknown method";
                    return SendResponse(request, http::status::bad_request, "Unknown method");
            }
        } catch (std::exception &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.what());
        } catch (...) {
            log_error << "Unknown exception";
            return SendResponse(request, http::status::internal_server_error, "Unknown exception");
        }
    }

    http::response<http::dynamic_body> S3Handler::HandleDeleteRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_debug << "S3 DELETE request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::S3ClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {
            switch (clientCommand.command) {
                case Dto::Common::S3CommandType::DELETE_BUCKET: {
                    Dto::S3::DeleteBucketRequest deleteBucketRequest;
                    deleteBucketRequest.region = clientCommand.region;
                    deleteBucketRequest.bucket = clientCommand.bucket;
                    _s3Service.DeleteBucket(deleteBucketRequest);

                    log_info << "Delete bucket, bucket: " << clientCommand.bucket;
                    return SendResponse(request, http::status::no_content);
                }

                case Dto::Common::S3CommandType::MOVE_OBJECT:
                case Dto::Common::S3CommandType::DELETE_OBJECT: {
                    Dto::S3::DeleteObjectRequest deleteRequest;
                    deleteRequest.region = clientCommand.region;
                    deleteRequest.user = clientCommand.user;
                    deleteRequest.bucket = clientCommand.bucket;
                    deleteRequest.key = clientCommand.key;
                    _s3Service.DeleteObject(deleteRequest);
                    log_info << "Delete object, bucket: " << clientCommand.bucket << " key: " << clientCommand.key;
                    return SendResponse(request, http::status::no_content);
                }

                case Dto::Common::S3CommandType::ABORT_MULTIPART_UPLOAD: {
                    log_info << "Abort multipart upload request, bucket: " << clientCommand.bucket << " key: " << clientCommand.key;
                    return SendResponse(request, http::status::no_content);
                }

                case Dto::Common::S3CommandType::DELETE_BUCKET_LIFECYCLE: {
                    log_debug << "Delete bucket lifecycle configuration request, bucket: " << clientCommand.bucket;

                    // S3 lifecycle configuration
                    Dto::S3::DeleteBucketLifecycleRequest s3Request;
                    s3Request.region = clientCommand.region;
                    s3Request.bucket = clientCommand.bucket;

                    _s3Service.DeleteBucketLifecycle(s3Request);

                    return SendResponse(request, http::status::no_content);
                }

                default:
                    log_error << "Unknown method, command: " << Dto::Common::S3CommandTypeToString(clientCommand.command);
                    return SendResponse(request, http::status::bad_request, "Unknown method");
            }
        } catch (Core::NotFoundException &exc) {
            log_error << "Bucket or object not found, exception: " << exc.what();
            return SendResponse(request, http::status::not_found, exc.what());
        } catch (std::exception &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.what());
        } catch (...) {
            log_error << "Unknown exception";
            return SendResponse(request, http::status::internal_server_error, "Unknown exception");
        }
    }

    http::response<http::dynamic_body> S3Handler::HandleHeadRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        Monitoring::MonitoringTimer measure(S3_SERVICE_TIMER);
        log_trace << "S3 HEAD request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::S3ClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {
            Dto::S3::GetMetadataResponse s3Response;
            if (clientCommand.key.empty()) {
                // Bucket metadata
                Dto::S3::GetMetadataRequest s3Request;
                s3Request.region = clientCommand.region;
                s3Request.bucket = clientCommand.bucket;
                s3Response = _s3Service.GetBucketMetadata(s3Request);
            } else {
                // Object metadata
                Dto::S3::GetMetadataRequest s3Request;
                s3Request.region = clientCommand.region;
                s3Request.bucket = clientCommand.bucket;
                s3Request.key = clientCommand.key;
                s3Response = _s3Service.GetObjectMetadata(s3Request);
            }

            std::map<std::string, std::string> headers;
            headers["accept-ranges"] = "bytes";
            headers["Handler"] = "awsmock";
            headers["Content-Type"] = "application/json";
            headers["Content-Length"] = std::to_string(s3Response.size);
            headers["Last-Modified"] = Core::DateTimeUtils::HttpFormat(s3Response.modified);
            headers["ETag"] = Core::StringUtils::Quoted(s3Response.md5Sum);
            headers["x-amz-bucket-region"] = s3Response.region;
            headers["x-amz-location-name"] = s3Response.region;
            headers["x-amz-storage-class"] = s3Response.storageClass;

            // User supplied metadata
            if (!s3Response.metadata.empty()) {
                for (const auto &[fst, snd]: s3Response.metadata) {
                    headers["x-amz-meta-" + fst] = snd;
                }
                log_info << "Get metadata, count: " << s3Response.metadata.size();
            }

            // Check storage class
            if (!s3Response.storageClass.empty() && s3Response.storageClass != Database::Entity::S3::StorageClassToString(Database::Entity::S3::StorageClass::STANDARD)) {
                headers["Content-Type"] = "application/xml";
                std::string body = "<Error><Code>InvalidObjectState</Code><Message>The action is not valid for the object's storage class</Message></Error>";
                return SendResponse(request, http::status::forbidden, body, headers);
            }

            return SendResponse(request, http::status::ok, {}, headers);
        } catch ([[maybe_unused]] Core::NotFoundException &exc) {
            return SendResponse(request, http::status::not_found);
        } catch (std::exception &exc) {
            return SendResponse(request, http::status::internal_server_error, exc.what());
        } catch (...) {
            return SendResponse(request, http::status::internal_server_error, "Unknown exception");
        }
    }

    void S3Handler::GetRange(const http::request<http::dynamic_body> &request, long &min, long &max, long &size) {
        if (!Core::HttpUtils::HasHeader(request, "Range")) {
            return;
        }
        const std::string rangeStr = Core::HttpUtils::GetHeaderValue(request, "Range");
        if (const std::string headerValue = Core::StringUtils::Split(rangeStr, "=")[1]; Core::StringUtils::Contains(headerValue, "/")) {
            const std::string parts = Core::StringUtils::Split(rangeStr, "/")[0];
            min = std::stol(Core::StringUtils::Split(parts, "-")[0]);
            max = std::stol(Core::StringUtils::Split(parts, "-")[1]);
            size = max - min + 1;
            const long total = std::stol(Core::StringUtils::Split(headerValue, "/")[1]);
            log_debug << "Requested range: " << std::to_string(min) << "-" << std::to_string(max) << ", size: " << size << " total: " << total;
        } else {
            min = std::stol(Core::StringUtils::Split(headerValue, "-")[0]);
            max = std::stol(Core::StringUtils::Split(headerValue, "-")[1]);
            size = max - min + 1;
            log_debug << "Requested range: " << std::to_string(min) << "-" << std::to_string(max) << ", size: " << size;
        }
    }

    std::map<std::string, std::string> S3Handler::GetMetadata(const http::request<http::dynamic_body> &request) {
        std::map<std::string, std::string> metadata;
        for (const auto &meta: request.base()) {
            if (Core::StringUtils::StartsWith(meta.name_string(), "x-amz-meta")) {
                std::string name = meta.name_string().substr(sizeof("x-amz-meta-") - 1);
                metadata[name] = meta.value();
            }
        }
        log_debug << "Get user metadata, size: " << metadata.size();
        return metadata;
    }

    long S3Handler::PrepareBody(http::request<http::dynamic_body> &request, boost::beast::net::streambuf &sb) {
        sb.commit(boost::beast::net::buffer_copy(sb.prepare(request.body().size()), request.body().cdata()));
        if (Core::HttpUtils::HasHeaderValue(request, "content-encoding", "aws-chunked")) {
            // Get decoded length from the header.
            const long decodedContentLength = std::stol(Core::HttpUtils::GetHeaderValue(request, "x-amz-decoded-content-length"));

            // Skip first line, AWS bug in binary chunk encoding
            int count = 0;
            do {
                if (const int c = sb.sbumpc(); c == '\r') {
                    if (sb.sbumpc() == '\n') {
                        count++;
                    }
                    break;
                }
                count++;
            } while (sb.sgetc() != boost::asio::error::eof);
            log_trace << "Skipped count: " << count << " decodedContentLength: " << decodedContentLength;

            request.body().consume(count);
            sb.commit(boost::beast::net::buffer_copy(sb.prepare(decodedContentLength), request.body().cdata()));
            return decodedContentLength;
        }
        sb.commit(boost::beast::net::buffer_copy(sb.prepare(request.body().size()), request.body().cdata()));
        return static_cast<long>(request.body().size());
    }

    void S3Handler::GetBucketKeyFromHeader(const std::string &path, std::string &bucket, std::string &key) {
        bucket = Core::StringUtils::SubStringUntil(path, "/");
        key = Core::StringUtils::SubStringAfter(path, "/");
        log_debug << "GetBucketKeyFromHeader: " << bucket << " " << key;
    }
}// namespace AwsMock::Service
