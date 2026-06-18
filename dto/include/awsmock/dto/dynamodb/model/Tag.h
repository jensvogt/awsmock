//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// AwsMock includes
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::DynamoDb {

    struct Tag final : Common::BaseObject<Tag> {

        /**
         * Key
         */
        std::string tagKey;

        /**
         * Value
         */
        std::string tagValue;

      private:

        friend Tag tag_invoke(boost::json::value_to_tag<Tag>, boost::json::value const &v) {
            Tag r;
            r.tagKey = Core::Json::GetStringValue(v, "Key");
            r.tagValue = Core::Json::GetStringValue(v, "Value");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Tag const &obj) {
            jv = {
                    {"Key", obj.tagKey},
                    {"Value", obj.tagValue},
            };
        }
    };
}// namespace Awsmock::Dto::DynamoDb
