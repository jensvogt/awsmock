//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_GET_EVENT_SOURCE_RESPONSE_H
#define AWSMOCK_DTO_SNS_GET_EVENT_SOURCE_RESPONSE_H

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/s3/model/LambdaConfiguration.h>
#include <awsmock/dto/sns/model/LambdaTriggerCounter.h>

namespace AwsMock::Dto::SNS {

    /**
     * @brief Get the event source as lambda configuration
     *
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetEventSourceResponse final : Common::BaseCounter<GetEventSourceResponse> {

        /**
         * Lambda configuration
         */
        LambdaTriggerCounter lambdaConfiguration;

      private:

        friend GetEventSourceResponse tag_invoke(boost::json::value_to_tag<GetEventSourceResponse>, boost::json::value const &v) {
            GetEventSourceResponse r;
            r.lambdaConfiguration = boost::json::value_to<LambdaTriggerCounter>(v.at("functionArn"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetEventSourceResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"lambdaConfiguration", boost::json::value_from(obj.lambdaConfiguration)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_GET_EVENT_SOURCE_RESPONSE_H
