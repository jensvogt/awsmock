//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_ADD_EVENT_SOURCE_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_ADD_EVENT_SOURCE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
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
         * Events, comma separated list of S3 events
         */
        std::vector<std::string> events;

        /**
         * Filter rule type; can either prefix or postfix
         */
        std::string filterRuleType;

        /**
         * Filter rule type value
         */
        std::string filterRuleValue;

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
            r.filterRuleType = Core::Json::GetStringValue(v, "FilterRuleType");
            r.filterRuleValue = Core::Json::GetStringValue(v, "FilterRuleValue");
            r.batchSize = Core::Json::GetLongValue(v, "BatchSize");
            r.maximumBatchingWindowInSeconds = Core::Json::GetLongValue(v, "MaximumBatchingWindowInSeconds");
            r.uuid = Core::Json::GetStringValue(v, "UUID");
            r.enabled = Core::Json::GetBoolValue(v, "Enabled");
            if (Core::Json::AttributeExists(v, "Events")) {
                r.events = boost::json::value_to<std::vector<std::string>>(v.at("Events"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AddEventSourceRequest const &obj) {
            jv = {
                    {"Type", obj.type},
                    {"FunctionArn", obj.functionArn},
                    {"EventSourceArn", obj.eventSourceArn},
                    {"Events", boost::json::value_from(obj.events)},
                    {"FilterRuleType", obj.filterRuleType},
                    {"FilterRuleValue", obj.filterRuleValue},
                    {"BatchSize", obj.batchSize},
                    {"MaximumBatchingWindowInSeconds", obj.maximumBatchingWindowInSeconds},
                    {"UUID", obj.uuid},
                    {"Enabled", obj.enabled},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_ADD_EVENT_SOURCE_REQUEST_H
