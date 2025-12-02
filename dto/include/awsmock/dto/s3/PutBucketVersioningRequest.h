//
// Created by vogje01 on 13/10/2023.
//

#ifndef AWSMOCK_CORE_DTO_PUT_BUCKET_VERSIONING_REQUEST_H
#define AWSMOCK_CORE_DTO_PUT_BUCKET_VERSIONING_REQUEST_H

// C++ standard includes
#include <sstream>
#include <string>

// AwsMock include
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    struct PutBucketVersioningRequest final : Common::BaseCounter<PutBucketVersioningRequest> {

        /**
         * Bucket name
         */
        std::string bucket;

        /**
         * Status
         */
        std::string status;

        /**
         * @brief Convert from XML representation
         *
         * @param xmlString XML string
         */
        void FromXml(const std::string &xmlString) {

            try {

                boost::property_tree::ptree pt;
                Core::XmlUtils::ReadXml(xmlString, &pt);
                status = pt.get<std::string>("VersioningConfiguration.Status");

            } catch (std::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend PutBucketVersioningRequest tag_invoke(boost::json::value_to_tag<PutBucketVersioningRequest>, boost::json::value const &v) {
            PutBucketVersioningRequest r;
            r.bucket = Core::Json::GetStringValue(v, "bucket");
            r.status = Core::Json::GetStringValue(v, "status");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PutBucketVersioningRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"bucket", obj.bucket},
                    {"status", obj.status},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_CORE_DTO_PUT_BUCKET_VERSIONING_REQUEST_H
