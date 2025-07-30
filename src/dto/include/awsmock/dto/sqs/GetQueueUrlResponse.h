//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_GET_QUEUE_URL_RESPONSE_H
#define AWSMOCK_DTO_SQS_GET_QUEUE_URL_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Get SQS queue URL response.
     *
     * Example:
     * @code{.xml}
     * <GetQueueUrlResponse xmlns="http://queue.amazonaws.com/doc/2012-11-05/">
     *     <GetQueueUrlResult>
     *         <QueueUrl>https://sqs.us-east-1.amazonaws.com/177715257436/MyQueue</QueueUrl>
     *     </GetQueueUrlResult>
     *     <ResponseMetadata>
     *         <RequestId>552d6f30-4c8e-5b32-aaed-33408c7d6c38</RequestId>
     *     </ResponseMetadata>
     * </GetQueueUrlResponse>
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetQueueUrlResponse final : Common::BaseCounter<GetQueueUrlResponse> {

        /**
         * Name of the queue
         */
        std::string queueUrl;

      private:

        friend GetQueueUrlResponse tag_invoke(boost::json::value_to_tag<GetQueueUrlResponse>, boost::json::value const &v) {
            GetQueueUrlResponse r;
            r.queueUrl = Core::Json::GetStringValue(v, "QueueUrl");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetQueueUrlResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"QueueUrl", obj.queueUrl},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_GET_QUEUE_URL_RESPONSE_H
