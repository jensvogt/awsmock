//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_QUEUE_ATTRIBUTE_H
#define AWSMOCK_DTO_SQS_QUEUE_ATTRIBUTE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    struct QueueAttribute final : Common::BaseCounter<QueueAttribute> {

        /**
         * MessageAttribute name
         */
        std::string attributeName;

        /**
         * MessageAttribute value
         */
        std::string attributeValue;

      private:

        friend QueueAttribute tag_invoke(boost::json::value_to_tag<QueueAttribute>, boost::json::value const &v) {
            QueueAttribute r;
            r.attributeName = Core::Json::GetStringValue(v, "AttributeName");
            r.attributeValue = Core::Json::GetStringValue(v, "AttributeValue");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, QueueAttribute const &obj) {
            jv = {
                    {"AttributeName", obj.attributeName},
                    {"AttributeValue", obj.attributeValue},
            };
        }
    };

    typedef std::vector<QueueAttribute> QueueAttributeList;

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_QUEUEATTRIBUTE_H
