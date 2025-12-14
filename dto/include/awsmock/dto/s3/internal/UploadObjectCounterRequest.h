//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_S3_UPLOAD_OBJECT_COUNTER_REQUEST_H
#define AWSMOCK_DTO_S3_UPLOAD_OBJECT_COUNTER_REQUEST_H

// C++ Standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {
    struct UploadObjectCounterRequest final : Common::BaseCounter<UploadObjectCounterRequest> {
        /**
         * Bucket name
         */
        std::string bucketName;

        /**
         * Bucket ARN
         */
        std::string bucketArn;

        /**
         * Object key
         */
        std::string objectKey;

        /**
         * Base64 encoded content string
         */
        std::string content;

        /**
         * Mime type
         */
        std::string contentType;

        /**
         * S3 meta data
         */
        std::map<std::string, std::string> metadata;

    private:
        friend UploadObjectCounterRequest tag_invoke(boost::json::value_to_tag<UploadObjectCounterRequest>, boost::json::value const &v) {
            UploadObjectCounterRequest r;
            r.bucketName = Core::Json::GetStringValue(v, "bucketName");
            r.bucketArn = Core::Json::GetStringValue(v, "bucketArn");
            r.objectKey = Core::Json::GetStringValue(v, "objectKey");
            r.content = Core::Json::GetStringValue(v, "content");
            r.contentType = Core::Json::GetStringValue(v, "contentType");
            if (Core::Json::AttributeExists(v, "metadata")) {
                r.metadata = boost::json::value_to<std::map<std::string, std::string> >(v.at("metadata"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UploadObjectCounterRequest const &obj) {
            jv = {
                {"region", obj.region},
                {"user", obj.user},
                {"requestId", obj.requestId},
                {"bucketArn", obj.bucketArn},
                {"bucketName", obj.bucketName},
                {"objectKey", obj.objectKey},
                {"content", obj.content},
                {"contentType", obj.contentType},
                {"metadata", boost::json::value_from(obj.metadata)},
            };
        }
    };
} // namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_UPLOAD_OBJECT_COUNTER_REQUEST_H
