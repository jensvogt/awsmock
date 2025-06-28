//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_DELETE_DEFAULT_MESSAGE_ATTRIBUTE_REQUEST_H
#define AWSMOCK_DTO_SQS_DELETE_DEFAULT_MESSAGE_ATTRIBUTE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

namespace AwsMock::Dto::SQS {

    struct DeleteDefaultMessageAttributeRequest final : Common::BaseCounter<DeleteDefaultMessageAttributeRequest> {

        /**
         * Message ID
         */
        std::string queueArn;

        /**
         * Name
         */
        std::string name;

      private:

        friend DeleteDefaultMessageAttributeRequest tag_invoke(boost::json::value_to_tag<DeleteDefaultMessageAttributeRequest>, boost::json::value const &v) {
            DeleteDefaultMessageAttributeRequest r;
            r.queueArn = Core::Json::GetStringValue(v, "queueArn");
            r.name = Core::Json::GetStringValue(v, "name");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteDefaultMessageAttributeRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"queueArn", obj.queueArn},
                    {"name", obj.name},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_DELETE_DEFAULT_MESSAGE_ATTRIBUTE_REQUEST_H
