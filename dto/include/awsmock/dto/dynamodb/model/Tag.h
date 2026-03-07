//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_TAG_H
#define AWSMOCK_DTO_DYNAMODB_TAG_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::DynamoDb {

    struct Tag final : Common::BaseCounter<Tag> {

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

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_SECRETSMANAGER_TAG_COUNTER_H
