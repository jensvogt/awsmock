//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_UPDATE_DQL_REQUEST_H
#define AWSMOCK_DTO_SQS_UPDATE_DQL_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

namespace AwsMock::Dto::SQS {
    /**
     * @brief Update message request
     *
     * Example (from Java v2 client):
     * @code{.json}
     * {
     *   "QueueArn":"string",
     *   "TargetArn":"string",
     *   "Retries": number
     * }
     * @endcode
     */
    struct UpdateDqlRequest final : Common::BaseCounter<UpdateDqlRequest> {

        /**
         * Source queue ARN
         */
        std::string queueArn;

        /**
         * Target ARN
         */
        std::string targetArn;

        /**
         * Max number of receivers
         */
        long retries{};

      private:

        friend UpdateDqlRequest tag_invoke(boost::json::value_to_tag<UpdateDqlRequest>, boost::json::value const &v) {
            UpdateDqlRequest r;
            r.queueArn = v.at("prefix").as_string();
            r.targetArn = v.at("targetArn").as_string();
            r.retries = v.at("retries").as_int64();
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateDqlRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"queueArn", obj.queueArn},
                    {"targetArn", obj.targetArn},
                    {"retries", obj.retries},
            };
        }
    };
}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_UPDATE_DQL_REQUEST_H
