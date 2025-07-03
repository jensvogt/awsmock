//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_DELETE_QUEUE_REQUEST_H
#define AWSMOCK_DTO_SQS_DELETE_QUEUE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Delete queue request.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteQueueRequest final : Common::BaseCounter<DeleteQueueRequest> {

        /**
         * Queue URL
         */
        std::string queueUrl;

        /**
         * Resource
         */
        std::string resource = "Unknown resource";

      private:

        friend DeleteQueueRequest tag_invoke(boost::json::value_to_tag<DeleteQueueRequest>, boost::json::value const &v) {
            DeleteQueueRequest r;
            r.queueUrl = Core::Json::GetStringValue(v, "QueueUrl");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteQueueRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"QueueUrl", obj.queueUrl},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_DELETE_QUEUE_REQUEST_H
