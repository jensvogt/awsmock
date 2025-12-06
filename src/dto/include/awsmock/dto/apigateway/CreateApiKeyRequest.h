//
// Created by vogje01 on 01/09/2025
//

#ifndef AWSMOCK_DTO_API_GATEWAY_CREATE_API_KEY_REQUEST_H
#define AWSMOCK_DTO_API_GATEWAY_CREATE_API_KEY_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::ApiGateway {

    /**
     * @brief Create API gateway key request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateApiKeyRequest final : Common::BaseCounter<CreateApiKeyRequest> {

        /**
         * Name
         */
        std::string name;

        /**
         * Customer ID
         */
        std::string customerId;

        /**
         * Description
         */
        std::string description;

        /**
         * Enabled
         */
        bool enabled{};

        /**
         * Generate distinct
         */
        bool generateDistinct{};

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Value
         */
        std::string value;

      private:

        friend CreateApiKeyRequest tag_invoke(boost::json::value_to_tag<CreateApiKeyRequest>, boost::json::value const &v) {
            CreateApiKeyRequest r;
            r.customerId = Core::Json::GetStringValue(v, "customerId");
            r.description = Core::Json::GetStringValue(v, "description");
            r.enabled = Core::Json::GetBoolValue(v, "enabled");
            r.generateDistinct = Core::Json::GetBoolValue(v, "generateDistinct");
            r.name = Core::Json::GetStringValue(v, "name");
            if (Core::Json::AttributeExists(v, "tags")) {
                r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("tags"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateApiKeyRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"customerId", obj.customerId},
                    {"description", obj.description},
                    {"enabled", obj.enabled},
                    {"generateDistinct", obj.generateDistinct},
                    {"name", obj.name},
                    {"tags", boost::json::value_from(obj.tags)},
            };
        }
    };

}// namespace AwsMock::Dto::ApiGateway

#endif// AWSMOCK_DTO_API_GATEWAY_CREATE_API_KEY_REQUEST_H
