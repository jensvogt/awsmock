//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_DELETE_QUEUE_RESPONSE_H
#define AWSMOCK_DTO_SQS_DELETE_QUEUE_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Delete queue response.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteQueueResponse final : Common::BaseCounter<DeleteQueueResponse> {

        /**
         * Queue URL
         */
        std::string queueUrl;

      private:

        friend DeleteQueueResponse tag_invoke(boost::json::value_to_tag<DeleteQueueResponse>, boost::json::value const &v) {
            DeleteQueueResponse r;
            r.queueUrl = Core::Json::GetStringValue(v, "QueueUrl");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteQueueResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"QueueUrl", obj.queueUrl},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_DELETE_QUEUE_RESPONSE_H
