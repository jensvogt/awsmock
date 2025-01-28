//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_LIST_LAMBDA_TRIGGER_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_SNS_LIST_LAMBDA_TRIGGER_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/sqs/model/LambdaTriggerCounter.h>

namespace AwsMock::Dto::SQS {

    struct ListLambdaTriggerCountersResponse {

        /**
         * List of lambda trigger counters
         */
        std::vector<LambdaTriggerCounter> lambdaTriggerCounters;

        /**
         * Total number of queues
         */
        long total = 0;

        /**
         * @brief Convert to JSON representation
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const ListLambdaTriggerCountersResponse &r);
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SNS_LIST_LAMBDA_TRIGGER_COUNTERS_RESPONSE_H
