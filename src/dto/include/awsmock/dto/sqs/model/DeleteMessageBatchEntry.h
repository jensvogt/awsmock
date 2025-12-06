//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_DELETE_MESSAGE_BATCH_ENTRY_H
#define AWSMOCK_DTO_SQS_DELETE_MESSAGE_BATCH_ENTRY_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    struct DeleteMessageBatchEntry final : Common::BaseCounter<DeleteMessageBatchEntry> {

        /**
         * Entry ID
         */
        std::string id;

        /**
         * Receipt handle
         */
        std::string receiptHandle;

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {
                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "Id", id);
                Core::Bson::BsonUtils::SetStringValue(document, "ReceiptHandle", receiptHandle);
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend DeleteMessageBatchEntry tag_invoke(boost::json::value_to_tag<DeleteMessageBatchEntry>, boost::json::value const &v) {
            DeleteMessageBatchEntry r;
            r.id = Core::Json::GetStringValue(v, "Id");
            r.receiptHandle = Core::Json::GetStringValue(v, "ReceiptHandle");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteMessageBatchEntry const &obj) {
            jv = {
                    {"Id", obj.id},
                    {"ReceiptHandle", obj.receiptHandle},
            };
        }
    };

    typedef std::vector<DeleteMessageBatchEntry> DeleteMessageBatchEntries;

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_DELETE_MESSAGE_BATCH_ENTRY_H
