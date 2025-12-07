//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_LIST_EVENT_SOURCE_MAPPINGS_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_LIST_EVENT_SOURCE_MAPPINGS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Create event source mapping request
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
     *   "Enabled": boolean,
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
     *   "Topics": [ "string" ],
     *   "TumblingWindowInSeconds": number
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListEventSourceMappingsRequest final : Common::BaseCounter<ListEventSourceMappingsRequest> {

        /**
         * Name of the function
         */
        std::string functionName;

        /**
         * Event source ARN
         */
        std::string eventSourceArn;

        /**
         * Marker
         */
        std::string marker;

        /**
         * Maximal item count
         */
        int maxItems{};

      private:

        friend ListEventSourceMappingsRequest tag_invoke(boost::json::value_to_tag<ListEventSourceMappingsRequest>, boost::json::value const &v) {
            ListEventSourceMappingsRequest r;
            r.functionName = Core::Json::GetStringValue(v, "FunctionName");
            r.eventSourceArn = Core::Json::GetStringValue(v, "EventSourceArn");
            r.marker = Core::Json::GetStringValue(v, "Marker");
            r.maxItems = Core::Json::GetIntValue(v, "MaxItems");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListEventSourceMappingsRequest const &obj) {
            jv = {
                    {"FunctionName", obj.functionName},
                    {"EventSourceArn", obj.eventSourceArn},
                    {"Marker", obj.marker},
                    {"MaxItems", obj.maxItems},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_LIST_EVENT_SOURCE_MAPPINGS_REQUEST_H
