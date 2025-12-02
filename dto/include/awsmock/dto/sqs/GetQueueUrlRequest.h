//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_GET_QUEUE_URL_REQUEST_H
#define AWSMOCK_DTO_SQS_GET_QUEUE_URL_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Get SQS queue URL request.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetQueueUrlRequest final : Common::BaseCounter<GetQueueUrlRequest> {

        /**
         * Name of the queue
         */
        std::string queueName;

      private:

        friend GetQueueUrlRequest tag_invoke(boost::json::value_to_tag<GetQueueUrlRequest>, boost::json::value const &v) {
            GetQueueUrlRequest r;
            r.queueName = Core::Json::GetStringValue(v, "QueueName");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetQueueUrlRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"queueName", obj.queueName},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_GET_QUEUE_URL_REQUEST_H
