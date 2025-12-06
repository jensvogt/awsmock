//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_COPY_OBJECT_RESPONSE_H
#define AWSMOCK_DTO_S3_COPY_OBJECT_RESPONSE_H

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    using std::chrono::system_clock;

    struct CopyObjectResponse final : Common::BaseCounter<CopyObjectResponse> {

        /**
         * Etag
         */
        std::string eTag;

        /**
         * Last modified
         */
        system_clock::time_point modified;

        /**
         * Convert to XML representation
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const {

            boost::property_tree::ptree root;
            root.add("CopyObjectResult.ETag", eTag);
            root.add("CopyObjectResult.LastModified", Core::DateTimeUtils::ToISO8601(modified));
            return Core::XmlUtils::ToXmlString(root);
        }

      private:

        friend CopyObjectResponse tag_invoke(boost::json::value_to_tag<CopyObjectResponse>, boost::json::value const &v) {
            CopyObjectResponse r;
            r.eTag = Core::Json::GetStringValue(v, "eTag");
            r.modified = Core::Json::GetDatetimeValue(v, "modified");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CopyObjectResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"eTag", obj.eTag},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_COPY_OBJECT_RESPONSE_H
