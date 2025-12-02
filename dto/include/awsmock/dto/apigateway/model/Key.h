//
// Created by vogje01 on 9/5/25.
//

#ifndef AWSMOCK_DTO_API_GATEWAY_MODEL_KEY_H
#define AWSMOCK_DTO_API_GATEWAY_MODEL_KEY_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::ApiGateway {

    struct Key {

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

        /**
         * Created
         */
        system_clock::time_point created;

        /**
         * Modified
         */
        system_clock::time_point modified;

      private:

        friend Key tag_invoke(boost::json::value_to_tag<Key>, boost::json::value const &v) {
            Key r;
            r.id = Core::Json::GetStringValue(v, "id");
            r.name = Core::Json::GetStringValue(v, "name");
            r.customerId = Core::Json::GetStringValue(v, "customerId");
            r.description = Core::Json::GetStringValue(v, "description");
            r.enabled = Core::Json::GetBoolValue(v, "enabled");
            r.generateDistinct = Core::Json::GetBoolValue(v, "generateDistinct");
            r.value = Core::Json::GetStringValue(v, "value");
            r.created = Core::Json::GetDatetimeValue(v, "created");
            r.modified = Core::Json::GetDatetimeValue(v, "modified");
            if (Core::Json::AttributeExists(v, "tags")) {
                r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("tags"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Key const &obj) {
            jv = {
                    {"id", obj.id},
                    {"name", obj.name},
                    {"customerId", obj.customerId},
                    {"description", obj.description},
                    {"enabled", obj.enabled},
                    {"generateDistinct", obj.generateDistinct},
                    {"value", obj.value},
                    {"tags", boost::json::value_from(obj.tags)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::ApiGateway

#endif// AWSMOCK_DTO_API_GATEWAY_MODEL_KEY_H
