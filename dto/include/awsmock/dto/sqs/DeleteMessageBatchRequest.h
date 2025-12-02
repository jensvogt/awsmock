//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_DELETE_MESSAGE_BATCH_REQUEST_H
#define AWSMOCK_DTO_SQS_DELETE_MESSAGE_BATCH_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/dto/sqs/model/DeleteMessageBatchEntry.h>

namespace AwsMock::Dto::SQS {

    struct DeleteMessageBatchRequest final : Common::BaseCounter<DeleteMessageBatchRequest> {

        /**
         * Queue URL
         */
        std::string queueUrl;

        /**
         * Entries
         */
        std::vector<DeleteMessageBatchEntry> entries;

        /**
         * Resource
         */
        std::string resource = "SQS";

      private:

        friend DeleteMessageBatchRequest tag_invoke(boost::json::value_to_tag<DeleteMessageBatchRequest>, boost::json::value const &v) {
            DeleteMessageBatchRequest r;
            r.queueUrl = Core::Json::GetStringValue(v, "QueueUrl");
            r.resource = Core::Json::GetStringValue(v, "Resource");
            if (Core::Json::AttributeExists(v, "Entries")) {
                r.entries = boost::json::value_to<std::vector<DeleteMessageBatchEntry>>(v.at("Entries"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteMessageBatchRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"QueueUrl", obj.queueUrl},
                    {"Resource", obj.resource},
                    {"Entries", boost::json::value_from(obj.entries)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_DELETE_MESSAGE_BATCH_REQUEST_H
