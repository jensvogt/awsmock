//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_RELOAD_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_SQS_RELOAD_COUNTERS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Reload the counters for a queue
     *
     * @code{.json}
     * {
     *   "QueueArn":"<string>",
     * }
     * @endcode
     */
    struct ReloadCountersRequest final : Common::BaseCounter<ReloadCountersRequest> {

        /**
         * Queue ARN
         */
        std::string queueArn;

      private:

        friend ReloadCountersRequest tag_invoke(boost::json::value_to_tag<ReloadCountersRequest>, boost::json::value const &v) {
            ReloadCountersRequest r;
            r.queueArn = Core::Json::GetStringValue(v, "queueArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ReloadCountersRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"queueArn", obj.queueArn},
            };
        }
    };
}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_RELOAD_COUNTERS_REQUEST_H
