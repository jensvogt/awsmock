//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_ADD_DEFAULT_MESSAGE_ATTRIBUTE_REQUEST_H
#define AWSMOCK_DTO_SQS_ADD_DEFAULT_MESSAGE_ATTRIBUTE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

namespace AwsMock::Dto::SQS {

    struct AddDefaultMessageAttributeRequest final : Common::BaseCounter<AddDefaultMessageAttributeRequest> {

        /**
         * Message ID
         */
        std::string queueArn;

        /**
         * Name
         */
        std::string name;

        /**
         * Message ID
         */
        MessageAttribute messageAttribute;

      private:

        friend AddDefaultMessageAttributeRequest tag_invoke(boost::json::value_to_tag<AddDefaultMessageAttributeRequest>, boost::json::value const &v) {
            AddDefaultMessageAttributeRequest r;
            r.queueArn = Core::Json::GetStringValue(v, "queueArn");
            r.name = Core::Json::GetStringValue(v, "name");
            if (Core::Json::AttributeExists(v, "messageAttribute")) {
                r.messageAttribute = boost::json::value_to<MessageAttribute>(v.at("messageAttribute"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AddDefaultMessageAttributeRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"queueArn", obj.queueArn},
                    {"name", obj.name},
                    {"messageAttribute", boost::json::value_from(obj.messageAttribute)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_ADD_DEFAULT_MESSAGE_ATTRIBUTE_REQUEST_H
