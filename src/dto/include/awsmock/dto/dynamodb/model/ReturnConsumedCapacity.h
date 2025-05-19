//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_DYNAMODB_RETURN_CONSUMED_CAPACITY_H
#define AWSMOCK_DTO_DYNAMODB_RETURN_CONSUMED_CAPACITY_H

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief Return consumed capacity
     *
     * Determines the level of detail about either provisioned or on-demand throughput consumption that is returned in the response:
     *
     *  - INDEXES - The response includes the aggregate ConsumedCapacity for the operation, together with ConsumedCapacity for each table and secondary index that was accessed.
     *  Note that some operations, such as GetItem and BatchGetItem, do not access any indexes at all. In these cases, specifying INDEXES will only return ConsumedCapacity information for table(s).
     *  - TOTAL - The response includes only the aggregate ConsumedCapacity for the operation.
     *  - NONE - No ConsumedCapacity details are included in the response.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class ReturnConsumedCapacityType {
        INDEXES,
        TOTAL,
        NONE,
    };

    static std::map<ReturnConsumedCapacityType, std::string> ReturnConsumedCapacityTypeNames{
            {ReturnConsumedCapacityType::INDEXES, "INDEXES"},
            {ReturnConsumedCapacityType::TOTAL, "TOTAL"},
            {ReturnConsumedCapacityType::NONE, "NONE"},
    };

    [[maybe_unused]] static std::string ReturnConsumedCapacityTypeToString(ReturnConsumedCapacityType returnConsumedCapacityType) {
        return ReturnConsumedCapacityTypeNames[returnConsumedCapacityType];
    }

    [[maybe_unused]] static ReturnConsumedCapacityType ReturnConsumedCapacityTypeFromString(const std::string &tableStatusName) {
        for (auto &[fst, snd]: ReturnConsumedCapacityTypeNames) {
            if (snd == tableStatusName) {
                return fst;
            }
        }
        return ReturnConsumedCapacityType::NONE;
    }

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_RETURN_CONSUMED_CAPACITY_H
