//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_UPDATE_DEFAULT_MESSAGE_ATTRIBUTE_REQUEST_H
#define AWSMOCK_DTO_SQS_UPDATE_DEFAULT_MESSAGE_ATTRIBUTE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

namespace AwsMock::Dto::SQS {

    struct UpdateDefaultMessageAttributeRequest final : Common::BaseCounter<UpdateDefaultMessageAttributeRequest> {

        /**
         * Message ID
         */
        std::string queueArn;

        /**
         * Name
         */
        std::string name;

        /**
         * Value
         */
        std::string value;

        /**
         * Data type
         */
        std::string dataType;

      private:

        friend UpdateDefaultMessageAttributeRequest tag_invoke(boost::json::value_to_tag<UpdateDefaultMessageAttributeRequest>, boost::json::value const &v) {
            UpdateDefaultMessageAttributeRequest r;
            r.queueArn = Core::Json::GetStringValue(v, "queueArn");
            r.name = Core::Json::GetStringValue(v, "name");
            r.value = Core::Json::GetStringValue(v, "value");
            r.dataType = Core::Json::GetStringValue(v, "dataType");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateDefaultMessageAttributeRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"queueArn", obj.queueArn},
                    {"name", obj.name},
                    {"value", obj.value},
                    {"dataType", obj.dataType},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_UPDATE_DEFAULT_MESSAGE_ATTRIBUTE_REQUEST_H
