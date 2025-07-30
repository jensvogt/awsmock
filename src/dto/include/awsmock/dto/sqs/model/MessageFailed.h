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
        bool senderFault;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        view_or_value<view, value> ToDocument() const {

            try {
                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "Id", id);
                Core::Bson::BsonUtils::SetStringValue(document, "Code", code);
                Core::Bson::BsonUtils::SetStringValue(document, "Message", message);
                Core::Bson::BsonUtils::SetBoolValue(document, "SenderFault", senderFault);

                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Converts a JSON representation to s DTO.
         *
         * @param document JSON object.
         */
        void FromDocument(const view_or_value<view, value> &document) {

            try {

                id = Core::Bson::BsonUtils::GetStringValue(document, "Id");
                code = Core::Bson::BsonUtils::GetStringValue(document, "code");
                message = Core::Bson::BsonUtils::GetStringValue(document, "message");
                senderFault = Core::Bson::BsonUtils::GetBoolValue(document, "senderFault");

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

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
