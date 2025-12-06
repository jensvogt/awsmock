//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_CREATE_EVENT_SOURCE_MAPPINGS_RESPONSE_H
#define AWSMOCK_DTO_LAMBDA_CREATE_EVENT_SOURCE_MAPPINGS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Create event source mapping response
     *
     * Example:
     * @code{.json}
     * {
     *   "AmazonManagedKafkaEventSourceConfig": {
     *     "ConsumerGroupId": "string"
     *   },
     *   "BatchSize": number,
     *   "BisectBatchOnFunctionError": boolean,
     *   "DestinationConfig": {
     *     "OnFailure": {
     *       "Destination": "string"
     *     },
     *     "OnSuccess": {
     *       "Destination": "string"
     *     }
     *   },
     *   "DocumentDBEventSourceConfig": {
     *     "CollectionName": "string",
     *     "DatabaseName": "string",
     *     "FullDocument": "string"
     *   },
     *   "EventSourceArn": "string",
     *   "FilterCriteria": {
     *     "Filters": [
     *       {
     *         "Pattern": "string"
     *       }
     *     ]
     *   },
     *   "FunctionName": "string",
     *   "FunctionResponseTypes": [ "string" ],
     *   "LastModified": number,
     *   "LastProcessingResult": "string",
     *   "MaximumBatchingWindowInSeconds": number,
     *   "MaximumRecordAgeInSeconds": number,
     *   "MaximumRetryAttempts": number,
     *   "ParallelizationFactor": number,
     *   "Queues": [ "string" ],
     *   "ScalingConfig": {
     *     "MaximumConcurrency": number
     *   },
     *   "SelfManagedEventSource": {
     *     "Endpoints": {
     *       "string" : [ "string" ]
     *     }
     *   },
     *   "SelfManagedKafkaEventSourceConfig": {
     *     "ConsumerGroupId": "string"
     *   },
     *   "SourceAccessConfigurations": [
     *     {
     *       "Type": "string",
     *       "URI": "string"
     *     }
     *   ],
     *   "StartingPosition": "string",
     *   "StartingPositionTimestamp": number,
     *   "State": string,
     *   "StateTransitionReason": string,
     *   "Topics": [ "string" ],
     *   "TumblingWindowInSeconds": number
     *   "UUID": string
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateEventSourceMappingsResponse final : Common::BaseCounter<CreateEventSourceMappingsResponse> {

        /**
         * Name of the function
         */
        std::string functionName;

        /**
         * Event source ARN
         */
        std::string eventSourceArn;

        /**
         * Batch size
         */
        long batchSize{};

        /**
         * Batch size
         */
        long maximumBatchingWindowInSeconds{};

        /**
         * Enabled
         */
        bool enabled = true;

        /**
         * UUID
         */
        std::string uuid;

      private:

        friend CreateEventSourceMappingsResponse tag_invoke(boost::json::value_to_tag<CreateEventSourceMappingsResponse>, boost::json::value const &v) {
            CreateEventSourceMappingsResponse r;
            r.functionName = Core::Json::GetStringValue(v, "FunctionName");
            r.eventSourceArn = Core::Json::GetStringValue(v, "EventSourceArn");
            r.batchSize = Core::Json::GetLongValue(v, "BatchSize");
            r.maximumBatchingWindowInSeconds = Core::Json::GetLongValue(v, "MaximumBatchingWindowInSeconds");
            r.enabled = Core::Json::GetBoolValue(v, "Enabled");
            r.uuid = Core::Json::GetStringValue(v, "UUID");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateEventSourceMappingsResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"FunctionName", obj.functionName},
                    {"EventSourceArn", obj.eventSourceArn},
                    {"BatchSize", obj.batchSize},
                    {"MaximumBatchingWindowInSeconds", obj.maximumBatchingWindowInSeconds},
                    {"Enabled", obj.enabled},
                    {"UUID", obj.uuid},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_CREATE_EVENT_SOURCE_MAPPINGS_RESPONSE_H
