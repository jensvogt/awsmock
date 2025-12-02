//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_GET_EVENT_SOURCE_REQUEST_H
#define AWSMOCK_DTO_SQS_GET_EVENT_SOURCE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Get event source
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetEventSourceRequest final : Common::BaseCounter<GetEventSourceRequest> {

        /**
         * Function ARN
         */
        std::string functionArn;

        /**
         * Event source ARN
         */
        std::string eventSourceArn;

      private:

        friend GetEventSourceRequest tag_invoke(boost::json::value_to_tag<GetEventSourceRequest>, boost::json::value const &v) {
            GetEventSourceRequest r;
            r.functionArn = Core::Json::GetStringValue(v, "functionArn");
            r.eventSourceArn = Core::Json::GetStringValue(v, "eventSourceArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetEventSourceRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"functionArn", obj.functionArn},
                    {"eventSourceArn", obj.eventSourceArn},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_GET_EVENT_SOURCE_REQUEST_H
