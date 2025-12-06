//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_DELETE_ATTRIBUTE_REQUEST_H
#define AWSMOCK_DTO_SQS_DELETE_ATTRIBUTE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/StringUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    struct DeleteAttributeRequest final : Common::BaseCounter<DeleteAttributeRequest> {

        /**
         * Message ID
         */
        std::string messageId;

        /**
         * Attribute name
         */
        std::string name;

      private:

        friend DeleteAttributeRequest tag_invoke(boost::json::value_to_tag<DeleteAttributeRequest>, boost::json::value const &v) {
            DeleteAttributeRequest r;
            r.messageId = Core::Json::GetStringValue(v, "messageId");
            r.name = Core::Json::GetStringValue(v, "name");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteAttributeRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"messageId", obj.messageId},
                    {"name", obj.name},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_DELETE_ATTRIBUTE_REQUEST_H
