//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_DYNAMODB_RETURN_VALUES_H
#define AWSMOCK_DTO_DYNAMODB_RETURN_VALUES_H

// C++ includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/s3/model/Owner.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief Return values
     *
     * @par
     * Use ReturnValues if you want to get the item attributes as they appeared before they were updated with the PutItem request. For PutItem, the valid values are:
     * 
     * NONE - If ReturnValues is not specified, or if its value is NONE, then nothing is returned. (This setting is the default for ReturnValues.)
     * ALL_OLD - If PutItem overwrote an attribute name-value pair, then the content of the old item is returned.
     *
     * @par
     * The values returned are strongly consistent.
     *
     * @par
     * There is no additional cost associated with requesting a return value aside from the small network and processing overhead of receiving a larger response. No
     * read capacity units are consumed.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class ReturnValuesType {
        NONE,
        ALL_OLD,
        UPDATED_OLD,
        ALL_NEW,
        UPDATED_NEW
    };

    static std::map<ReturnValuesType, std::string> ReturnValuesTypeNames{
            {ReturnValuesType::NONE, "NONE"},
            {ReturnValuesType::ALL_OLD, "ALL_OLD"},
            {ReturnValuesType::UPDATED_OLD, "UPDATED_OLD"},
            {ReturnValuesType::ALL_NEW, "ALL_NEW"},
            {ReturnValuesType::UPDATED_NEW, "UPDATED_NEW"},
    };

    [[maybe_unused]] static std::string ReturnValuesTypeToString(const ReturnValuesType &returnValuesType) {
        return ReturnValuesTypeNames[returnValuesType];
    }

    [[maybe_unused]] static ReturnValuesType ReturnValuesTypeFromString(const std::string &returnValuesType) {
        for (auto &[fst, snd]: ReturnValuesTypeNames) {
            if (snd == returnValuesType) {
                return fst;
            }
        }
        return ReturnValuesType::NONE;
    }

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_RETURN_VALUES_H
