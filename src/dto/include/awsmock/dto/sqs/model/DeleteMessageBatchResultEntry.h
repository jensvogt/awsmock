//
// Created by vogje01 on 7/6/24.
//

#ifndef AWSMOCK_DTO_SQS_MODEL_DELETE_MESSAGE_BATCH_RESULT_ENTRY_H
#define AWSMOCK_DTO_SQS_MODEL_DELETE_MESSAGE_BATCH_RESULT_ENTRY_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Sent when successful delete was processed.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteMessageBatchResultEntry final : Common::BaseCounter<DeleteMessageBatchResultEntry> {


        /**
         * @brief Default constructor
         */
        explicit DeleteMessageBatchResultEntry() = default;

        /**
         * @brief Constructor
         *
         * @param id message ID
         */
        explicit DeleteMessageBatchResultEntry(const std::string &id) {
            this->id = id;
        }

        /**
         * Id
         */
        std::string id;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        view_or_value<view, value> ToDocument() const {

            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "Id", id);
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend DeleteMessageBatchResultEntry tag_invoke(boost::json::value_to_tag<DeleteMessageBatchResultEntry>, boost::json::value const &v) {
            DeleteMessageBatchResultEntry r;
            r.id = Core::Json::GetStringValue(v, "Id");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteMessageBatchResultEntry const &obj) {
            jv = {
                    {"Id", obj.id},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_MODEL_DELETE_MESSAGE_BATCH_RESULT_ENTRY_H
