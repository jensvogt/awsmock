//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_DELETE_OBJECTS_RESPONSE_H
#define AWSMOCK_DTO_S3_DELETE_OBJECTS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief Delete all objects response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteObjectsResponse final : Common::BaseCounter<DeleteObjectsResponse> {

        /**
         * Keys
         */
        std::vector<std::string> keys;

        /**
         * @brief Convert to XML representation
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const {

            boost::property_tree::ptree root;
            boost::property_tree::ptree parent = root.add("DeleteResult", "");
            for (const auto &key: keys) {
                parent.add("Deleted.Key", key);
                parent.add("Deleted.VersionId", "");
            }
            return Core::XmlUtils::ToXmlString(root);
        }

      private:

        friend DeleteObjectsResponse tag_invoke(boost::json::value_to_tag<DeleteObjectsResponse>, boost::json::value const &v) {
            DeleteObjectsResponse r;
            if (Core::Json::AttributeExists(v, "keys")) {
                r.keys = boost::json::value_to<std::vector<std::string>>(v.at("keys"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteObjectsResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"keys", boost::json::value_from(obj.keys)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_DELETE_OBJECTS_RESPONSE_H
