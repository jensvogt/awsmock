//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_ADD_ATTRIBUTE_REQUEST_H
#define AWSMOCK_DTO_SQS_ADD_ATTRIBUTE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/StringUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    struct AddAttributeRequest final : Common::BaseCounter<AddAttributeRequest> {

        /**
         * Message ID
         */
        std::string messageId;

        /**
         * Attribute name
         */
        std::string name;

        /**
         * Attribute data type
         */
        std::string dataType;

        /**
         * Attribute value
         */
        std::string value;

      private:

        friend AddAttributeRequest tag_invoke(boost::json::value_to_tag<AddAttributeRequest>, boost::json::value const &v) {
            AddAttributeRequest r;
            r.messageId = Core::Json::GetStringValue(v, "messageId");
            r.name = Core::Json::GetStringValue(v, "name");
            r.dataType = Core::Json::GetStringValue(v, "dataType");
            r.value = Core::Json::GetStringValue(v, "value");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AddAttributeRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"messageId", obj.messageId},
                    {"name", obj.name},
                    {"dataType", obj.dataType},
                    {"value", obj.value},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_ADD_ATTRIBUTE_REQUEST_H
