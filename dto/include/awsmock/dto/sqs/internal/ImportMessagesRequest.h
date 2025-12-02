//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_IMPORT_MESSAGES_REQUEST_H
#define AWSMOCK_DTO_SQS_IMPORT_MESSAGES_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Import messages request
     *
     * @code{.json}
     * {
     *   "queueArn":"string",
     *   "messages":"string",
     * }
     * @endcode
     */
    struct ImportMessagesRequest final : Common::BaseCounter<ImportMessagesRequest> {

        /**
         * Queue ARN
         */
        std::string queueArn;

        /**
         * Message array as JSON string
         */
        std::string messages;

      private:

        friend ImportMessagesRequest tag_invoke(boost::json::value_to_tag<ImportMessagesRequest>, boost::json::value const &v) {
            ImportMessagesRequest r;
            r.queueArn = Core::Json::GetStringValue(v, "queueArn");
            r.messages = Core::Json::GetStringValue(v, "messages");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ImportMessagesRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"queueArn", obj.queueArn},
                    {"messages", obj.messages},
            };
        }
    };
}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_IMPORT_MESSAGES_REQUEST_H
