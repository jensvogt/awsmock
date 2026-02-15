//
// Created by vogje01 on 7/6/24.
//

#ifndef AWSMOCK_DTO_SQS_MODEL_BATCH_RESULT_ERROR_ENTRY_H
#define AWSMOCK_DTO_SQS_MODEL_BATCH_RESULT_ERROR_ENTRY_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Sent when a delete batch error occurs.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct BatchResultErrorEntry final : Common::BaseCounter<BatchResultErrorEntry> {

        /**
         * @brief Default constructor
         */
        BatchResultErrorEntry() = default;

        /**
         * @brief Constructor
         *
         * @param id message ID
         */
        explicit BatchResultErrorEntry(const std::string &id) {
            this->id = id;
        }

        /**
         * Id
         */
        std::string id;

        /**
         * Code
         */
        std::string code;

        /**
         * Sender fault
         */
        bool senderFault = false;

        /**
         * Error message
         */
        std::string message;

      private:

        friend BatchResultErrorEntry tag_invoke(boost::json::value_to_tag<BatchResultErrorEntry>, boost::json::value const &v) {
            BatchResultErrorEntry r;
            r.id = Core::Json::GetStringValue(v, "Id");
            r.code = Core::Json::GetStringValue(v, "Code");
            r.senderFault = Core::Json::GetBoolValue(v, "SenderFault");
            r.message = Core::Json::GetStringValue(v, "Message");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, BatchResultErrorEntry const &obj) {
            jv = {
                    {"Id", obj.id},
                    {"Code", obj.code},
                    {"SenderFault", obj.senderFault},
                    {"Message", obj.message},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_MODEL_BATCH_RESULT_ERROR_ENTRY_H
