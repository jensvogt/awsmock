//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_UNSUBSCRIBE_REQUEST_H
#define AWSMOCK_DTO_SNS_UNSUBSCRIBE_REQUEST_H

// C++ standard includes
#include <sstream>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SNS {

    /**
     * @brief Unsubscribe a method from the topic
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UnsubscribeRequest final : Common::BaseCounter<UnsubscribeRequest> {

        /**
         * Subscription ARN
         */
        std::string subscriptionArn;

      private:

        friend UnsubscribeRequest tag_invoke(boost::json::value_to_tag<UnsubscribeRequest>, boost::json::value const &v) {
            UnsubscribeRequest r;
            r.subscriptionArn = Core::Json::GetStringValue(v, "ResourceArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UnsubscribeRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"subscriptionArn", obj.subscriptionArn},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_UNSUBSCRIBE_REQUEST_H
