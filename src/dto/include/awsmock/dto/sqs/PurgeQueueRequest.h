//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_PURGE_QUEUE_REQUEST_H
#define AWSMOCK_DTO_SQS_PURGE_QUEUE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Purge SQS queue request.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct PurgeQueueRequest final : Common::BaseCounter<PurgeQueueRequest> {

        /**
         * Queue URL
         */
        std::string queueUrl;

    private:

        friend PurgeQueueRequest tag_invoke(boost::json::value_to_tag<PurgeQueueRequest>, boost::json::value const &v) {
            PurgeQueueRequest r;
            r.queueUrl = Core::Json::GetStringValue(v, "QueueUrl");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PurgeQueueRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"QueueUrl", obj.queueUrl},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_PURGE_QUEUE_REQUEST_H
