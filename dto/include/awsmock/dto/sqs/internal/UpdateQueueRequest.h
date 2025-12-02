//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_UPDATE_QUEUE_REQUEST_H
#define AWSMOCK_DTO_SQS_UPDATE_QUEUE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

namespace AwsMock::Dto::SQS {
    /**
     * @brief Update queue request
     *
     * Example (from Java v2 client):
     * @code{.json}
     * {
     *   "queueArn": "string",
     *   "delay": number,
     *   "visibilityTimeout": number
     *   "retentionPeriod": number
     * }
     * @endcode
     */
    struct UpdateQueueRequest final : Common::BaseCounter<UpdateQueueRequest> {

        /**
         * Source queue ARN
         */
        std::string queueArn;

        /**
         * Delay in seconds
         */
        long delay;

        /**
         * Visibility timeout
         */
        long visibilityTimeout;

        /**
         * Retention period
         */
        long retentionPeriod;

    private:

        friend UpdateQueueRequest tag_invoke(boost::json::value_to_tag<UpdateQueueRequest>, boost::json::value const &v) {
            UpdateQueueRequest r;
            r.queueArn = Core::Json::GetStringValue(v, "queueArn");
            r.delay = Core::Json::GetLongValue(v, "delay");
            r.visibilityTimeout = Core::Json::GetLongValue(v, "visibilityTimeout");
            r.retentionPeriod = Core::Json::GetLongValue(v, "retentionPeriod");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateQueueRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"queueArn", obj.queueArn},
                    {"delay", obj.delay},
                    {"visibilityTimeout", obj.visibilityTimeout},
                    {"retentionPeriod", obj.retentionPeriod},
            };
        }
    };
}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_UPDATE_QUEUE_REQUEST_H
