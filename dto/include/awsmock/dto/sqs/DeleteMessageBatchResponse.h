//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_DELETE_MESSAGE_BATCH_RESPONSE_H
#define AWSMOCK_DTO_SQS_DELETE_MESSAGE_BATCH_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/StringUtils.h>
#include <awsmock/dto/sqs/model/BatchResultErrorEntry.h>
#include <awsmock/dto/sqs/model/DeleteMessageBatchResultEntry.h>

namespace AwsMock::Dto::SQS {

    struct DeleteMessageBatchResponse final : Common::BaseCounter<DeleteMessageBatchResponse> {

        /**
         * Resource
         */
        std::string resource = "SQS";

        /**
         * Failed
         */
        std::vector<BatchResultErrorEntry> failed;

        /**
         * Successful
         */
        std::vector<DeleteMessageBatchResultEntry> successfull;

      private:

        friend DeleteMessageBatchResponse tag_invoke(boost::json::value_to_tag<DeleteMessageBatchResponse>, boost::json::value const &v) {
            DeleteMessageBatchResponse r;
            r.resource = Core::Json::GetStringValue(v, "Resource");
            if (Core::Json::AttributeExists(v, "Successful")) {
                r.successfull = boost::json::value_to<std::vector<DeleteMessageBatchResultEntry>>(v.at("Successful"));
            }
            if (Core::Json::AttributeExists(v, "Failed")) {
                r.failed = boost::json::value_to<std::vector<BatchResultErrorEntry>>(v.at("Failed"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteMessageBatchResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Resource", obj.resource},
                    {"Successful", boost::json::value_from(obj.successfull)},
                    {"Failed", boost::json::value_from(obj.failed)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_DELETE_MESSAGE_BATCH_RESPONSE_H
