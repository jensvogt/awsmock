//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_REDRIVE_MESSAGES_REQUEST_H
#define AWSMOCK_DTO_SQS_REDRIVE_MESSAGES_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Redrive messages request.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct RedriveMessagesRequest final : Common::BaseCounter<RedriveMessagesRequest> {

        /**
         * Queue URL
         */
        std::string queueArn;

    private:

        friend RedriveMessagesRequest tag_invoke(boost::json::value_to_tag<RedriveMessagesRequest>, boost::json::value const &v) {
            RedriveMessagesRequest r;
            r.queueArn = Core::Json::GetStringValue(v, "QueueArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, RedriveMessagesRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"QueueArn", obj.queueArn},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_REDRIVE_MESSAGES_REQUEST_H
