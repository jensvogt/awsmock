//
// Created by vogje01 on 01/09/2025
//

#ifndef AWSMOCK_DTO_API_GATEWAY_CREATE_API_KEY_RESPONSE_H
#define AWSMOCK_DTO_API_GATEWAY_CREATE_API_KEY_RESPONSE_H

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::ApiGateway {

    using std::chrono::system_clock;

    /**
     * @brief Create API gateway key response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateApiKeyResponse final : Common::BaseCounter<CreateApiKeyResponse> {

        /**
         * ID
         */
        std::string id;

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
         * Stage keys
         */
        std::vector<std::string> stageKeys;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Created timestamp
         */
        system_clock::time_point created;

        /**
         * Modified timestamp
         */
        system_clock::time_point modified;

      private:

        friend CreateApiKeyResponse tag_invoke(boost::json::value_to_tag<CreateApiKeyResponse>, boost::json::value const &v) {
            CreateApiKeyResponse r;
            r.id = Core::Json::GetStringValue(v, "id");
            r.name = Core::Json::GetStringValue(v, "name");
            r.customerId = Core::Json::GetStringValue(v, "customerId");
            r.description = Core::Json::GetStringValue(v, "description");
            r.enabled = Core::Json::GetBoolValue(v, "enabled");
            r.created = Core::Json::GetDatetimeValue(v, "createdDate");
            r.modified = Core::Json::GetDatetimeValue(v, "lastUpdatedDate");
            if (Core::Json::AttributeExists(v, "stageKeys")) {
                r.stageKeys = boost::json::value_to<std::vector<std::string>>(v.at("stageKeys"));
            }
            if (Core::Json::AttributeExists(v, "tags")) {
                r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("tags"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateApiKeyResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"id", obj.id},
                    {"name", obj.name},
                    {"customerId", obj.customerId},
                    {"description", obj.description},
                    {"enabled", obj.enabled},
                    {"createdDate", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"lastUpdatedDate", Core::DateTimeUtils::ToISO8601(obj.modified)},
                    {"stageKeys", boost::json::value_from(obj.stageKeys)},
                    {"tags", boost::json::value_from(obj.tags)},
            };
        }
    };

}// namespace AwsMock::Dto::ApiGateway

#endif// AWSMOCK_DTO_API_GATEWAY_CREATE_API_KEY_RESPONSE_H
