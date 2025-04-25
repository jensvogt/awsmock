//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_GET_QUEUE_DETAILS_REQUEST_H
#define AWSMOCK_DTO_SQS_GET_QUEUE_DETAILS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    struct GetQueueDetailsRequest final : Common::BaseCounter<GetQueueDetailsRequest> {

        /**
         * Queue ARN
         */
        std::string queueArn;

      private:

        friend GetQueueDetailsRequest tag_invoke(boost::json::value_to_tag<GetQueueDetailsRequest>, boost::json::value const &v) {
            GetQueueDetailsRequest r;
            r.region = v.at("region").as_string();
            r.user = v.at("user").as_string();
            r.requestId = v.at("requestId").as_string();
            r.queueArn = v.at("queueArn").as_string();
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetQueueDetailsRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"queueArn", obj.queueArn},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_GET_QUEUE_DETAILS_REQUEST_H
