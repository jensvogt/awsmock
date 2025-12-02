//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_DYNAMODB_RETURN_ITEM_COLLECTION_METRICS_H
#define AWSMOCK_DTO_DYNAMODB_RETURN_ITEM_COLLECTION_METRICS_H

// C*+ includes
#include <map>
#include <string>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief Return consumed capacity
     *
     * Determines whether item collection metrics are returned. If set to SIZE, the response includes statistics about item collections, if any, that were modified during the operation are returned in the response. If set to NONE (the default), no statistics are returned.
     * Type: String
     * Valid Values: SIZE | NONE
     * Required: No
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class ReturnItemCollectionMetricsType {
        SIZE,
        NONE,
    };

    static std::map<ReturnItemCollectionMetricsType, std::string> ReturnItemCollectionMetricsTypeNames{
            {ReturnItemCollectionMetricsType::SIZE, "SIZE"},
            {ReturnItemCollectionMetricsType::NONE, "NONE"},
    };

    [[maybe_unused]] static std::string ReturnItemCollectionMetricsToString(const ReturnItemCollectionMetricsType &returnItemCollectionMetricsType) {
        return ReturnItemCollectionMetricsTypeNames[returnItemCollectionMetricsType];
    }

    [[maybe_unused]] static ReturnItemCollectionMetricsType ReturnItemCollectionMetricsFromString(const std::string &returnItemCollectionMetricsType) {
        for (auto &[fst, snd]: ReturnItemCollectionMetricsTypeNames) {
            if (snd == returnItemCollectionMetricsType) {
                return fst;
            }
        }
        return ReturnItemCollectionMetricsType::NONE;
    }

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_RETURN_ITEM_COLLECTION_METRICS_H
