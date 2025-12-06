//
// Created by vogje01 on 9/5/25.
//

#ifndef AWSMOCK_DTO_API_GATEWAY_MODEL_REST_API_H
#define AWSMOCK_DTO_API_GATEWAY_MODEL_REST_API_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::ApiGateway {

    struct RestApi final : Common::BaseCounter<RestApi> {

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
         * API Key source
         */
        ApiKeySourceType apiKeySource;

        /**
         * Enabled
         */
        bool enabled{};

        /**
         * Generate distinct
         */
        bool generateDistinct{};

        /**
         * Policy
         */
        std::string policy;

        /**
         * Root resource ID
         */
        std::string rootResourceId;

        /**
         * Binary media types
         */
        std::vector<std::string> binaryMediaTypes;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Value
         */
        std::string value;

        /**
         * Warnings
         */
        std::vector<std::string> warnings;

        /**
         * Created
         */
        system_clock::time_point created;

        /**
         * Modified
         */
        system_clock::time_point modified;

      private:

        friend RestApi tag_invoke(boost::json::value_to_tag<RestApi>, boost::json::value const &v) {
            RestApi r;
            r.id = Core::Json::GetStringValue(v, "id");
            r.name = Core::Json::GetStringValue(v, "name");
            r.customerId = Core::Json::GetStringValue(v, "customerId");
            r.description = Core::Json::GetStringValue(v, "description");
            r.enabled = Core::Json::GetBoolValue(v, "enabled");
            r.generateDistinct = Core::Json::GetBoolValue(v, "generateDistinct");
            r.value = Core::Json::GetStringValue(v, "value");
            r.policy = Core::Json::GetStringValue(v, "policy");
            r.rootResourceId = Core::Json::GetStringValue(v, "policy");
            r.apiKeySource = ApiKeySourceTypeFromString(Core::Json::GetStringValue(v, "value"));
            r.created = Core::Json::GetDatetimeValue(v, "created");
            r.modified = Core::Json::GetDatetimeValue(v, "modified");
            if (Core::Json::AttributeExists(v, "binaryMediaTypes")) {
                r.binaryMediaTypes = boost::json::value_to<std::vector<std::string>>(v.at("binaryMediaTypes"));
            }
            if (Core::Json::AttributeExists(v, "tags")) {
                r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("tags"));
            }
            if (Core::Json::AttributeExists(v, "warnings")) {
                r.warnings = boost::json::value_to<std::vector<std::string>>(v.at("warnings"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, RestApi const &obj) {
            jv = {
                    {"id", obj.id},
                    {"name", obj.name},
                    {"customerId", obj.customerId},
                    {"description", obj.description},
                    {"enabled", obj.enabled},
                    {"generateDistinct", obj.generateDistinct},
                    {"value", obj.value},
                    {"apiKeySource", ApiKeySourceTypeToString(obj.apiKeySource)},
                    {"policy", obj.policy},
                    {"rootResourceId", obj.rootResourceId},
                    {"binaryMediaTypes", boost::json::value_from(obj.binaryMediaTypes)},
                    {"tags", boost::json::value_from(obj.tags)},
                    {"warnings", boost::json::value_from(obj.warnings)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::ApiGateway

#endif// AWSMOCK_DTO_API_GATEWAY_MODEL_REST_API_H
