//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_DELETE_MESSAGE_REQUEST_H
#define AWSMOCK_DTO_SQS_DELETE_MESSAGE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    struct DeleteMessageRequest final : Common::BaseCounter<DeleteMessageRequest> {

        /**
         * Queue URL
         */
        std::string queueUrl;

        /**
         * Receipt handle
         */
        std::string receiptHandle;

      private:

        friend DeleteMessageRequest tag_invoke(boost::json::value_to_tag<DeleteMessageRequest>, boost::json::value const &v) {
            DeleteMessageRequest r;
            r.queueUrl = Core::Json::GetStringValue(v, "QueueUrl");
            r.receiptHandle = Core::Json::GetStringValue(v, "ReceiptHandle");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteMessageRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"queueUrl", obj.queueUrl},
                    {"receiptHandle", obj.receiptHandle},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_DELETE_MESSAGE_REQUEST_H
