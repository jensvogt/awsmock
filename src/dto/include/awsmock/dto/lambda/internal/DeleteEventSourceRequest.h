//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_DELETE_EVENT_SOURCE_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_DELETE_EVENT_SOURCE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Delete function event source
     *
     * Example:
     * @code{.json}
     * {
     *   "FunctionArn": "string" ,
     *   "EventSourceArn": string
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteEventSourceRequest final : Common::BaseCounter<DeleteEventSourceRequest> {

        /**
         * Lambda function ARN
         */
        std::string functionArn;

        /**
         * Event source ARN
         */
        std::string eventSourceArn;

      private:

        friend DeleteEventSourceRequest tag_invoke(boost::json::value_to_tag<DeleteEventSourceRequest>, boost::json::value const &v) {
            DeleteEventSourceRequest r;
            r.functionArn = Core::Json::GetStringValue(v, "FunctionArn");
            r.eventSourceArn = Core::Json::GetStringValue(v, "EventSourceArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteEventSourceRequest const &obj) {
            jv = {
                    {"FunctionArn", obj.functionArn},
                    {"EventSourceArn", obj.eventSourceArn},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_DELETE_EVENT_SOURCE_REQUEST_H
