//
// Created by vogje01 on 9/5/25.
//

#pragma once

// AwsMock includes
#include <awsmock/dto/apigateway/model/Authorizer.h>
#include <awsmock/dto/apigateway/model/Resource.h>
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::ApiGateway {

    struct RestApi final : Common::BaseObject<RestApi> {

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
         * Authorizers
         */
        std::map<std::string, Authorizer> authorizers;

        /**
         * Resources
         */
        std::map<std::string, Resource> resources;

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
            r.apiKeySource = ApiKeySourceTypeFromString(Core::Json::GetStringValue(v, "apiKeySource"));
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
            if (Core::Json::AttributeExists(v, "authorizers")) {
                r.authorizers = boost::json::value_to<std::map<std::string, Authorizer>>(v.at("authorizers"));
            }
            if (Core::Json::AttributeExists(v, "resources")) {
                r.resources = boost::json::value_to<std::map<std::string, Resource>>(v.at("resources"));
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
                    {"authorizers", boost::json::value_from(obj.authorizers)},
                    {"resources", boost::json::value_from(obj.resources)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
