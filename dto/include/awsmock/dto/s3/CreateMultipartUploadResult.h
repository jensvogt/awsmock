//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_CREATE_MULTIPART_UPLOAD_RESULT_H
#define AWSMOCK_DTO_S3_CREATE_MULTIPART_UPLOAD_RESULT_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/XmlUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    struct CreateMultipartUploadResult final : Common::BaseCounter<CreateMultipartUploadResult> {

        /**
         * Bucket name
         */
        std::string bucket;

        /**
         * Key name
         */
        std::string key;

        /**
         * Upload ID
         */
        std::string uploadId;

        /**
         * Convert to XML representation
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const {

            boost::property_tree::ptree root;
            root.add("InitiateMultipartUploadResult.Bucket", bucket);
            root.add("InitiateMultipartUploadResult.Key", key);
            root.add("InitiateMultipartUploadResult.UploadId", uploadId);
            return Core::XmlUtils::ToXmlString(root);
        }

      private:

        friend CreateMultipartUploadResult tag_invoke(boost::json::value_to_tag<CreateMultipartUploadResult>, boost::json::value const &v) {
            CreateMultipartUploadResult r;
            r.bucket = Core::Json::GetStringValue(v, "bucket");
            r.key = Core::Json::GetStringValue(v, "key");
            r.uploadId = Core::Json::GetStringValue(v, "uploadId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateMultipartUploadResult const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"bucket", obj.bucket},
                    {"key", obj.key},
                    {"uploadId", obj.uploadId},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_CREATE_MULTIPART_UPLOAD_RESULT_H
