//
// Created by vogje01 on 11/26/23.
//

#include <awsmock/dto/common/S3ClientCommand.h>

namespace AwsMock::Dto::Common {

    void S3ClientCommand::FromRequest(const http::request<http::dynamic_body> &request, const std::string &awsRegion, const std::string &awsUser) {

        UserAgent userAgent;
        userAgent.FromRequest(request);

        //Core::HttpUtils::DumpRequest(request);

        // Basic values
        this->region = awsRegion;
        this->user = awsUser;
        this->method = request.method();
        this->contentType = Core::HttpUtils::GetContentType(request);
        this->contentLength = Core::HttpUtils::GetContentLength(request);
        this->url = request.target();
        this->host = Core::HttpUtils::GetHost(request);
        this->requestId = Core::HttpUtils::GetHeaderValue(request, "RequestId", Core::AwsUtils::CreateRequestId());
        this->headers = Core::HttpUtils::GetHeaders(request);
        this->payload = Core::HttpUtils::GetBodyAsString(request);
        this->contentMd5 = Core::HttpUtils::GetHeaderValue(request, "Content-MD5");
        this->storageClass = Core::HttpUtils::GetHeaderValue(request, "x-amz-storage-class");

        // Core values
        bucket = Core::AwsUtils::GetS3BucketName(request);
        key = Core::AwsUtils::GetS3ObjectKey(request);

        // Multipart uploads/downloads
        uploads = Core::HttpUtils::HasQueryParameter(request.target(), "uploads");
        uploadId = Core::HttpUtils::GetStringParameter(request.target(), "uploadId");
        partNumber = Core::HttpUtils::HasQueryParameter(request.target(), "partNumber");
        lifecycleRequest = Core::HttpUtils::HasQueryParameter(request.target(), "lifecycle");
        uploadPartCopy = Core::HttpUtils::HasHeader(request, "x-amz-copy-source") && Core::HttpUtils::HasHeader(request, "x-amz-copy-source-range");
        rangeRequest = Core::HttpUtils::HasHeader(request, "Range");
        multipartRequest = uploads || !uploadId.empty() || partNumber;

        notificationRequest = Core::HttpUtils::HasQueryParameter(request.target(), "notification");
        versionRequest = Core::HttpUtils::HasQueryParameter(request.target(), "versioning");
        copyRequest = Core::HttpUtils::HasHeader(request, "x-amz-copy-source");
        encryptionRequest = Core::HttpUtils::HasQueryParameter(request.target(), "encryption");

        if (!userAgent.clientCommand.empty()) {

            GetCommandFromUserAgent(method, userAgent);

        } else if (Core::HttpUtils::HasHeader(request, "x-awsmock-target")) {

            const std::string action = Core::HttpUtils::GetHeaderValue(request, "x-awsmock-action");
            command = S3CommandTypeFromString(action);

        } else {
            switch (method) {

                case http::verb::get:
                    if (bucket.empty() && key.empty()) {
                        command = S3CommandType::LIST_BUCKETS;
                    } else if (!bucket.empty() && key.empty()) {
                        if (Core::HttpUtils::HasQueryParameter(request.target(), "versions")) {
                            prefix = Core::HttpUtils::GetStringParameter(request.target(), "prefix");
                            command = S3CommandType::LIST_OBJECT_VERSIONS;
                        } else {
                            command = S3CommandType::LIST_OBJECTS;
                        }
                    } else if (rangeRequest && !partNumber) {
                        command = S3CommandType::GET_OBJECT_RANGE;
                    } else {
                        command = S3CommandType::GET_OBJECT;
                    }
                    break;

                case http::verb::put:
                    if (multipartRequest) {
                        if (uploadPartCopy) {
                            command = S3CommandType::UPLOAD_PART_COPY;
                        } else {
                            command = S3CommandType::UPLOAD_PART;
                        }
                    } else if (encryptionRequest) {
                        command = S3CommandType::PUT_BUCKET_ENCRYPTION;
                    } else if (notificationRequest) {
                        command = S3CommandType::BUCKET_NOTIFICATION;
                    } else if (versionRequest) {
                        command = S3CommandType::PUT_BUCKET_VERSIONING;
                    } else if (lifecycleRequest) {
                        command = S3CommandType::PUT_BUCKET_LIFECYCLE_CONFIGURATION;
                    } else if (!bucket.empty() && key.empty()) {
                        command = S3CommandType::CREATE_BUCKET;
                    } else if (!bucket.empty() && !key.empty()) {
                        command = S3CommandType::PUT_OBJECT;
                    }
                    break;

                case http::verb::post:
                    if (!bucket.empty() && !key.empty()) {
                        if (uploads) {
                            command = S3CommandType::CREATE_MULTIPART_UPLOAD;
                        } else if (multipartRequest) {
                            command = S3CommandType::COMPLETE_MULTIPART_UPLOAD;
                        }
                    } else if (!bucket.empty()) {
                        command = S3CommandType::DELETE_OBJECTS;
                    }
                    break;

                case http::verb::delete_:
                    if (multipartRequest) {
                        command = S3CommandType::ABORT_MULTIPART_UPLOAD;
                    } else if (!bucket.empty() && key.empty()) {
                        command = S3CommandType::DELETE_BUCKET;
                    } else if (!bucket.empty() && !key.empty()) {
                        command = S3CommandType::DELETE_OBJECT;
                    }
                    break;

                case http::verb::head:
                    break;

                default:
                    log_error << "Unknown command, method: " << method << " multipartRequest: " << multipartRequest << " bucket: " << bucket << " key: " << key << uploads;
                    break;
            }
        }
        log_debug << "Client command: " << S3CommandTypeToString(command);
    }

    void S3ClientCommand::GetCommandFromUserAgent(const http::verb &httpMethod, const UserAgent &userAgent) {
        if (userAgent.clientCommand == "mb") {
            command = S3CommandType::CREATE_BUCKET;
        } else if (userAgent.clientCommand == "ls") {
            if (bucket.empty() && key.empty()) {
                command = S3CommandType::LIST_BUCKETS;
            } else if (!bucket.empty()) {
                command = S3CommandType::LIST_OBJECTS;
            }
        } else if (userAgent.clientCommand == "cp") {
            if (httpMethod == http::verb::put) {
                if (multipartRequest) {
                    command = S3CommandType::UPLOAD_PART;
                } else {
                    command = S3CommandType::PUT_OBJECT;
                }
            } else if (httpMethod == http::verb::get) {
                command = S3CommandType::GET_OBJECT;
            } else if (httpMethod == http::verb::post) {
                if (multipartRequest && uploadId.empty()) {
                    command = S3CommandType::CREATE_MULTIPART_UPLOAD;
                } else {
                    command = S3CommandType::COMPLETE_MULTIPART_UPLOAD;
                }
            }
        } else if (userAgent.clientCommand == "mv") {
            command = S3CommandType::MOVE_OBJECT;
        } else if (userAgent.clientCommand == "rb") {
            command = S3CommandType::DELETE_BUCKET;
        } else if (userAgent.clientCommand == "rm") {
            command = S3CommandType::DELETE_OBJECT;
        } else if (userAgent.clientModule == "s3api" && userAgent.clientCommand == "create-multipart-upload") {
            command = S3CommandType::CREATE_MULTIPART_UPLOAD;
        } else if (userAgent.clientModule == "s3api" && userAgent.clientCommand == "upload-part") {
            command = S3CommandType::UPLOAD_PART;
        } else if (userAgent.clientModule == "s3api" && userAgent.clientCommand == "complete-multipart-upload") {
            command = S3CommandType::COMPLETE_MULTIPART_UPLOAD;
        } else if (userAgent.clientModule == "s3api" && userAgent.clientCommand == "put-bucket-notification-configuration") {
            command = S3CommandType::PUT_BUCKET_NOTIFICATION_CONFIGURATION;
        } else if (userAgent.clientModule == "s3api" && userAgent.clientCommand == "put-bucket-encryption") {
            command = S3CommandType::PUT_BUCKET_ENCRYPTION;
        } else if (userAgent.clientModule == "s3api" && userAgent.clientCommand == "put-bucket-versioning") {
            command = S3CommandType::PUT_BUCKET_VERSIONING;
        } else if (userAgent.clientModule == "s3api" && userAgent.clientCommand == "list-object-versions") {
            command = S3CommandType::LIST_OBJECT_VERSIONS;
        } else if (userAgent.clientModule == "s3api" && userAgent.clientCommand == "put-bucket-lifecycle-configuration") {
            command = S3CommandType::PUT_BUCKET_LIFECYCLE_CONFIGURATION;
        } else if (userAgent.clientModule == "s3api" && userAgent.clientCommand == "get-bucket-lifecycle-configuration") {
            command = S3CommandType::GET_BUCKET_LIFECYCLE_CONFIGURATION;
        }
    }

    std::string S3ClientCommand::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "method", std::string(to_string(method)));
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "user", user);
            Core::Bson::BsonUtils::SetStringValue(document, "command", S3CommandTypeToString(command));
            Core::Bson::BsonUtils::SetStringValue(document, "bucket", bucket);
            Core::Bson::BsonUtils::SetStringValue(document, "key", key);
            Core::Bson::BsonUtils::SetStringValue(document, "prefix", prefix);
            Core::Bson::BsonUtils::SetStringValue(document, "contentType", contentType);
            Core::Bson::BsonUtils::SetLongValue(document, "contentLength", contentLength);
            Core::Bson::BsonUtils::SetBoolValue(document, "versionRequest", versionRequest);
            Core::Bson::BsonUtils::SetBoolValue(document, "notificationRequest", notificationRequest);
            Core::Bson::BsonUtils::SetBoolValue(document, "multipartRequest", multipartRequest);
            Core::Bson::BsonUtils::SetBoolValue(document, "uploads", uploads);
            Core::Bson::BsonUtils::SetIntValue(document, "partNumber", partNumber);
            Core::Bson::BsonUtils::SetBoolValue(document, "copyRequest", copyRequest);
            Core::Bson::BsonUtils::SetStringValue(document, "uploadId", uploadId);
            Core::Bson::BsonUtils::SetStringValue(document, "storageClass", storageClass);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string S3ClientCommand::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const S3ClientCommand &r) {
        os << "S3ClientCommand=" << r.ToJson();
        return os;
    }
}// namespace AwsMock::Dto::Common
