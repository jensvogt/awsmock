//
// Created by vogje01 on 11/26/23.
//

#include <awsmock/dto/common/S3ClientCommand.h>

namespace Awsmock::Dto::Common {

    void S3ClientCommand::FromRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {

        UserAgent userAgent;
        userAgent.FromRequest(request);

        this->region = region;
        this->user = user;
        method = request.method();
        contentType = Core::HttpUtils::GetContentType(request);
        contentLength = Core::HttpUtils::GetContentLength(request);
        url = request.target();
        host = Core::HttpUtils::GetHost(request);
        requestId = Core::HttpUtils::GetHeaderValue(request, "RequestId", Core::AwsUtils::CreateRequestId());
        headers = Core::HttpUtils::GetHeaders(request);
        payload = Core::HttpUtils::GetBodyAsString(request);
        contentMd5 = Core::HttpUtils::GetHeaderValue(request, "Content-MD5");
        storageClass = Core::HttpUtils::GetHeaderValue(request, "x-amz-storage-class");

        bucket = Core::AwsUtils::GetS3BucketName(request);
        key = Core::AwsUtils::GetS3ObjectKey(request);

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
            command = S3CommandTypeFromString(Core::HttpUtils::GetHeaderValue(request, "x-awsmock-action"));
        } else {
            switch (method) {
                case http::verb::get:
                    if (bucket.empty()) {
                        command = S3CommandType::LIST_BUCKETS;
                    } else if (key.empty()) {
                        if (Core::HttpUtils::HasQueryParameter(request.target(), "policy")) {
                            command = S3CommandType::GET_BUCKET_POLICY;
                        } else if (Core::HttpUtils::HasQueryParameter(request.target(), "acl")) {
                            command = S3CommandType::GET_BUCKET_ACL;
                        } else if (Core::HttpUtils::HasQueryParameter(request.target(), "location")) {
                            command = S3CommandType::GET_BUCKET_LOCATION;
                        } else if (Core::HttpUtils::HasQueryParameter(request.target(), "cors")) {
                            command = S3CommandType::GET_BUCKET_CORS;
                        } else if (Core::HttpUtils::HasQueryParameter(request.target(), "tagging")) {
                            command = S3CommandType::GET_BUCKET_TAGGING;
                        } else if (Core::HttpUtils::HasQueryParameter(request.target(), "accelerate")) {
                            command = S3CommandType::GET_BUCKET_ACCELERATE;
                        } else if (Core::HttpUtils::HasQueryParameter(request.target(), "object-lock")) {
                            command = S3CommandType::GET_BUCKET_OBJECT_LOCK;
                        } else if (Core::HttpUtils::HasQueryParameter(request.target(), "website")) {
                            command = S3CommandType::GET_BUCKET_WEBSITE;
                        } else if (Core::HttpUtils::HasQueryParameter(request.target(), "requestPayment")) {
                            command = S3CommandType::GET_BUCKET_REQUEST_PAYMENT;
                        } else if (Core::HttpUtils::HasQueryParameter(request.target(), "ownershipControls")) {
                            command = S3CommandType::GET_BUCKET_OWNERSHIP_CONTROLS;
                        } else if (Core::HttpUtils::HasQueryParameter(request.target(), "publicAccessBlock")) {
                            command = S3CommandType::GET_BUCKET_PUBLIC_ACCESS_BLOCK;
                        } else if (Core::HttpUtils::HasQueryParameter(request.target(), "replication")) {
                            command = S3CommandType::GET_BUCKET_REPLICATION;
                        } else if (Core::HttpUtils::HasQueryParameter(request.target(), "versions")) {
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
                        command = uploadPartCopy ? S3CommandType::UPLOAD_PART_COPY : S3CommandType::UPLOAD_PART;
                    } else if (copyRequest) {
                        command = S3CommandType::COPY_OBJECT;
                    } else if (encryptionRequest) {
                        command = S3CommandType::PUT_BUCKET_ENCRYPTION;
                    } else if (notificationRequest) {
                        command = S3CommandType::BUCKET_NOTIFICATION;
                    } else if (versionRequest) {
                        command = S3CommandType::PUT_BUCKET_VERSIONING;
                    } else if (lifecycleRequest) {
                        command = S3CommandType::PUT_BUCKET_LIFECYCLE_CONFIGURATION;
                    } else if (Core::HttpUtils::HasQueryParameter(request.target(), "policy")) {
                        command = S3CommandType::PUT_BUCKET_POLICY;
                    } else if (Core::HttpUtils::HasQueryParameter(request.target(), "acl")) {
                        command = S3CommandType::PUT_BUCKET_ACL;
                    } else if (Core::HttpUtils::HasQueryParameter(request.target(), "cors")) {
                        command = S3CommandType::PUT_BUCKET_CORS;
                    } else if (Core::HttpUtils::HasQueryParameter(request.target(), "tagging")) {
                        command = S3CommandType::PUT_BUCKET_TAGGING;
                    } else if (Core::HttpUtils::HasQueryParameter(request.target(), "accelerate")) {
                        command = S3CommandType::PUT_BUCKET_ACCELERATE;
                    } else if (Core::HttpUtils::HasQueryParameter(request.target(), "object-lock")) {
                        command = S3CommandType::PUT_BUCKET_OBJECT_LOCK;
                    } else if (Core::HttpUtils::HasQueryParameter(request.target(), "website")) {
                        command = S3CommandType::PUT_BUCKET_WEBSITE;
                    } else if (Core::HttpUtils::HasQueryParameter(request.target(), "ownershipControls")) {
                        command = S3CommandType::PUT_BUCKET_OWNERSHIP_CONTROLS;
                    } else if (Core::HttpUtils::HasQueryParameter(request.target(), "publicAccessBlock")) {
                        command = S3CommandType::PUT_BUCKET_PUBLIC_ACCESS_BLOCK;
                    } else if (key.empty()) {
                        command = S3CommandType::CREATE_BUCKET;
                    } else {
                        command = S3CommandType::PUT_OBJECT;
                    }
                    break;

                case http::verb::post:
                    if (!bucket.empty() && !key.empty()) {
                        if (uploads) {
                            command = S3CommandType::CREATE_MULTIPART_UPLOAD;
                        } else if (!uploadId.empty()) {
                            command = S3CommandType::COMPLETE_MULTIPART_UPLOAD;
                        }
                    } else if (!bucket.empty()) {
                        command = S3CommandType::DELETE_OBJECTS;
                    }
                    break;

                case http::verb::head:
                    break;

                case http::verb::delete_:
                    if (multipartRequest) {
                        command = S3CommandType::ABORT_MULTIPART_UPLOAD;
                    } else if (Core::HttpUtils::HasQueryParameter(request.target(), "policy")) {
                        command = S3CommandType::DELETE_BUCKET_POLICY;
                    } else if (Core::HttpUtils::HasQueryParameter(request.target(), "cors")) {
                        command = S3CommandType::DELETE_BUCKET_CORS;
                    } else if (Core::HttpUtils::HasQueryParameter(request.target(), "tagging")) {
                        command = S3CommandType::DELETE_BUCKET_TAGGING;
                    } else if (Core::HttpUtils::HasQueryParameter(request.target(), "website")) {
                        command = S3CommandType::DELETE_BUCKET_WEBSITE;
                    } else if (Core::HttpUtils::HasQueryParameter(request.target(), "ownershipControls")) {
                        command = S3CommandType::DELETE_BUCKET_OWNERSHIP_CONTROLS;
                    } else if (Core::HttpUtils::HasQueryParameter(request.target(), "publicAccessBlock")) {
                        command = S3CommandType::DELETE_BUCKET_PUBLIC_ACCESS_BLOCK;
                    } else if (Core::HttpUtils::HasQueryParameter(request.target(), "replication")) {
                        command = S3CommandType::DELETE_BUCKET_REPLICATION;
                    } else if (key.empty()) {
                        command = S3CommandType::DELETE_BUCKET;
                    } else {
                        command = S3CommandType::DELETE_OBJECT;
                    }
                    break;

                default:
                    log_error << "Unknown S3 command, method: " << method << " bucket: " << bucket << " key: " << key;
                    break;
            }
        }
        log_debug << "S3 client command: " << S3CommandTypeToString(command);
    }

    void S3ClientCommand::GetCommandFromUserAgent(const http::verb &httpMethod, const UserAgent &userAgent) {

        // s3api sub-commands map directly to a command type
        static const std::map<std::string, S3CommandType> s3apiCommands{
                {"list-buckets", S3CommandType::LIST_BUCKETS},
                {"create-multipart-upload", S3CommandType::CREATE_MULTIPART_UPLOAD},
                {"upload-part", S3CommandType::UPLOAD_PART},
                {"complete-multipart-upload", S3CommandType::COMPLETE_MULTIPART_UPLOAD},
                {"put-bucket-notification-configuration", S3CommandType::PUT_BUCKET_NOTIFICATION_CONFIGURATION},
                {"put-bucket-encryption", S3CommandType::PUT_BUCKET_ENCRYPTION},
                {"put-bucket-versioning", S3CommandType::PUT_BUCKET_VERSIONING},
                {"list-object-versions", S3CommandType::LIST_OBJECT_VERSIONS},
                {"put-bucket-lifecycle-configuration", S3CommandType::PUT_BUCKET_LIFECYCLE_CONFIGURATION},
                {"get-bucket-lifecycle-configuration", S3CommandType::GET_BUCKET_LIFECYCLE_CONFIGURATION},
                {"delete-bucket-lifecycle", S3CommandType::DELETE_BUCKET_LIFECYCLE},
        };

        // s3 high-level commands that map unconditionally
        static const std::map<std::string, S3CommandType> s3SimpleCommands{
                {"mb", S3CommandType::CREATE_BUCKET},
                {"rb", S3CommandType::DELETE_BUCKET},
                {"rm", S3CommandType::DELETE_OBJECT},
                {"mv", S3CommandType::MOVE_OBJECT},
        };

        if (userAgent.clientModule == "s3api") {
            if (const auto it = s3apiCommands.find(userAgent.clientCommand); it != s3apiCommands.end()) {
                command = it->second;
            }
            return;
        }

        if (const auto it = s3SimpleCommands.find(userAgent.clientCommand); it != s3SimpleCommands.end()) {
            command = it->second;
            return;
        }

        if (userAgent.clientCommand == "ls") {
            command = bucket.empty() ? S3CommandType::LIST_BUCKETS : S3CommandType::LIST_OBJECTS;
        } else if (userAgent.clientCommand == "cp") {
            if (httpMethod == http::verb::put) {
                command = multipartRequest ? S3CommandType::UPLOAD_PART : S3CommandType::PUT_OBJECT;
            } else if (httpMethod == http::verb::get) {
                command = S3CommandType::GET_OBJECT;
            } else if (httpMethod == http::verb::post) {
                command = (multipartRequest && uploadId.empty()) ? S3CommandType::CREATE_MULTIPART_UPLOAD : S3CommandType::COMPLETE_MULTIPART_UPLOAD;
            }
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
            Core::Bson::BsonUtils::SetBoolValue(document, "partNumber", partNumber);
            Core::Bson::BsonUtils::SetBoolValue(document, "copyRequest", copyRequest);
            Core::Bson::BsonUtils::SetStringValue(document, "uploadId", uploadId);
            Core::Bson::BsonUtils::SetStringValue(document, "storageClass", storageClass);
            return Core::Bson::BsonUtils::ToJsonString(document);
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace Awsmock::Dto::Common
