//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_SET_VISIBILITY_TIMEOUT_REQUEST_H
#define AWSMOCK_DTO_SQS_SET_VISIBILITY_TIMEOUT_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    struct ChangeMessageVisibilityRequest final : Common::BaseCounter<ChangeMessageVisibilityRequest> {

        /**
         * Queue URL
         */
        std::string queueUrl;

        /**
         * Request handle
         */
        std::string receiptHandle;

        /**
         * Visibility timeout
         */
        long visibilityTimeout = 30;

      private:

        friend ChangeMessageVisibilityRequest tag_invoke(boost::json::value_to_tag<ChangeMessageVisibilityRequest>, boost::json::value const &v) {
            ChangeMessageVisibilityRequest r;
            r.queueUrl = Core::Json::GetStringValue(v, "QueueUrl");
            r.receiptHandle = Core::Json::GetStringValue(v, "ReceiptHandle");
            r.visibilityTimeout = Core::Json::GetLongValue(v, "VisibilityTimeout");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ChangeMessageVisibilityRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"QueueUrl", obj.queueUrl},
                    {"ReceiptHandle", obj.receiptHandle},
                    {"VisibilityTimeout", obj.visibilityTimeout},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_SET_VISIBILITY_TIMEOUT_REQUEST_H
