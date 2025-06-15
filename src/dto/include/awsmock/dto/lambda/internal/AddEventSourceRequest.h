//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_ADD_EVENT_SOURCE_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_ADD_EVENT_SOURCE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Add function event source mapping
     *
     * Example:
     * @code{.json}
     * {
     *   "FunctionArn": "string",
     *   "EventSourceArn": "string",
     *   "BatchSize": number,
     *   "MaximumBatchingWindowInSeconds": number
     *   "UUID": string
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct AddEventSourceRequest final : Common::BaseCounter<AddEventSourceRequest> {

        /**
         * Notification type
         */
        std::string type;

        /**
         * Lambda function ARN
         */
        std::string functionArn;

        /**
         * Lambda event source ARN
         */
        std::string eventSourceArn;

        /**
         * Batch size
         */
        long batchSize = 10;

        /**
         * Maximal batch windows
         */
        long maximumBatchingWindowInSeconds = 5;

        /**
         * Lambda event source UUID
         */
        std::string uuid = Core::StringUtils::CreateRandomUuid();

        /**
         * Enabled flag
         */
        bool enabled = false;

      private:

        friend AddEventSourceRequest tag_invoke(boost::json::value_to_tag<AddEventSourceRequest>, boost::json::value const &v) {
            AddEventSourceRequest r;
            r.type = Core::Json::GetStringValue(v, "Type");
            r.functionArn = Core::Json::GetStringValue(v, "FunctionArn");
            r.eventSourceArn = Core::Json::GetStringValue(v, "EventSourceArn");
            r.batchSize = Core::Json::GetLongValue(v, "BatchSize");
            r.maximumBatchingWindowInSeconds = Core::Json::GetLongValue(v, "MaximumBatchingWindowInSeconds");
            r.uuid = Core::Json::GetStringValue(v, "UUID");
            r.enabled = Core::Json::GetBoolValue(v, "Enabled");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AddEventSourceRequest const &obj) {
            jv = {
                    {"Type", obj.type},
                    {"FunctionArn", obj.functionArn},
                    {"EventSourceArn", obj.eventSourceArn},
                    {"BatchSize", obj.batchSize},
                    {"MaximumBatchingWindowInSeconds", obj.maximumBatchingWindowInSeconds},
                    {"UUID", obj.uuid},
                    {"Enabled", obj.enabled},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_ADD_EVENT_SOURCE_REQUEST_H
