//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_MOVEOBJECTSRESPONSE_H
#define AWSMOCK_DTO_S3_MOVEOBJECTSRESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief Move object response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct MoveObjectResponse final : Common::BaseCounter<MoveObjectResponse> {

        /**
         * Etag
         */
        std::string eTag;

        /**
         * Last modified
         */
        std::string lastModified;

        /**
         * @brief Convert to XML representation
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const {

            try {

                boost::property_tree::ptree root;
                root.add("MoveObjectResult.ETag", eTag);
                root.add("MoveObjectResult.LastModified", lastModified);
                return Core::XmlUtils::ToXmlString(root);

            } catch (std::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend MoveObjectResponse tag_invoke(boost::json::value_to_tag<MoveObjectResponse>, boost::json::value const &v) {
            MoveObjectResponse r;
            r.eTag = Core::Json::GetStringValue(v, "eTag");
            r.lastModified = Core::Json::GetStringValue(v, "lastModified");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, MoveObjectResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"eTag", obj.eTag},
                    {"lastModified", obj.lastModified},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_MOVEOBJECTSRESPONSE_H
