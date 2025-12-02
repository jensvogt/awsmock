//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_S3_CREATE_MULTIPART_UPLOAD_REQUEST_H
#define AWSMOCK_CORE_DTO_S3_CREATE_MULTIPART_UPLOAD_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/XmlUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    struct CreateMultipartUploadRequest final : Common::BaseCounter<CreateMultipartUploadRequest> {

        /**
         * Bucket name
         */
        std::string bucket;

        /**
         * Bucket owner
         */
        std::string key;

        /**
         * AWS upload ID
         */
        std::string uploadId;

        /**
         * Metadata
         */
        std::map<std::string, std::string> metadata;

        /**
         * @brief Convert from XML representation
         *
         * @param xmlString XML string
         */
        void FromXml(const std::string &xmlString) {

            try {

                boost::property_tree::ptree pt;
                read_xml(xmlString, pt);
                region = pt.get<std::string>("CreateBucketConfiguration.LocationConstraint");

            } catch (std::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend CreateMultipartUploadRequest tag_invoke(boost::json::value_to_tag<CreateMultipartUploadRequest>, boost::json::value const &v) {
            CreateMultipartUploadRequest r;
            r.bucket = Core::Json::GetStringValue(v, "bucket");
            r.key = Core::Json::GetStringValue(v, "key");
            r.uploadId = Core::Json::GetStringValue(v, "uploadId");
            if (Core::Json::AttributeExists(v, "metadata")) {
                r.metadata = boost::json::value_to<std::map<std::string, std::string>>(v.at("metadata"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateMultipartUploadRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"bucket", obj.bucket},
                    {"key", obj.key},
                    {"uploadId", obj.uploadId},
                    {"metadata", boost::json::value_from(obj.metadata)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_CORE_DTO_S3_CREATE_MULTIPART_UPLOAD_REQUEST_H
