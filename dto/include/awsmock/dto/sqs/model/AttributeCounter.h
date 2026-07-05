//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::SQS {

    struct AttributeCounter final : Common::BaseObject<AttributeCounter> {

        /**
         * Key
         */
        std::string attributeKey;

        /**
         * Value
         */
        std::string attributeValue;

      private:

        friend AttributeCounter tag_invoke(boost::json::value_to_tag<AttributeCounter>, boost::json::value const &v) {
            AttributeCounter r;
            r.attributeKey = Core::Json::GetStringValue(v, "attributeKey");
            r.attributeValue = Core::Json::GetStringValue(v, "attributeValue");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AttributeCounter const &obj) {
            jv = {
                    {"attributeKey", obj.attributeKey},
                    {"attributeValue", obj.attributeValue},
            };
        }
    };
}// namespace Awsmock::Dto::SQS
