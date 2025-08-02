//
// Created by vogje01 on 8/23/24.
//

#ifndef AWSMOCK_DTO_SQS_MESSAGE_ENTRY_FAILED_H
#define AWSMOCK_DTO_SQS_MESSAGE_ENTRY_FAILED_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief SQS message entry failed
     *
     * @code{.json}
     * {
     *   "Code": "string",
     *   "Id": "string",
     *   "Message": "string",
     *   "SenderFault": boolean
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct MessageFailed final : Common::BaseCounter<MessageFailed> {

        /**
         * Error code
         */
        std::string code;

        /**
         * Message ID
         */
        std::string id;

        /**
         * Message
         */
        std::string message;

        /**
         * Sender fault
         */
        bool senderFault{};

      private:

        friend MessageFailed tag_invoke(boost::json::value_to_tag<MessageFailed>, boost::json::value const &v) {
            MessageFailed r;
            r.id = Core::Json::GetStringValue(v, "Id");
            r.code = Core::Json::GetStringValue(v, "Code");
            r.message = Core::Json::GetStringValue(v, "Message");
            r.senderFault = Core::Json::GetBoolValue(v, "SenderFault");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, MessageFailed const &obj) {
            jv = {
                    {"Id", obj.id},
                    {"Code", obj.code},
                    {"Message", obj.message},
                    {"SenderFault", obj.senderFault},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_MESSAGE_ENTRY_FAILED_H
