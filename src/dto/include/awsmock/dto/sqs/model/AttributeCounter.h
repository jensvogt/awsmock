//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_ATTRIBUTE_COUNTER_H
#define AWSMOCK_DTO_SQS_ATTRIBUTE_COUNTER_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    struct AttributeCounter final : Common::BaseCounter<AttributeCounter> {

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

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_ATTRIBUTE_COUNTER_H
