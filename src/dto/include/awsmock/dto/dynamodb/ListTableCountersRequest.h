//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_LIST_TABLE_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_DYNAMODB_LIST_TABLE_COUNTERS_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseDto.h>
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB list table counters request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListTableCountersRequest final : Common::BaseDto<ListTableCountersRequest> {

        /**
         * AWS region
         */
        std::string region;

        /**
         * TAble name prefix.
         */
        std::string prefix;

        /**
         * Page size
         */
        int pageSize;

        /**
         * Page index
         */
        int pageIndex;

        /**
         * Page index
         */
        std::vector<Database::SortColumn> sortColumns;

        /**
         * @brief Parse a JSON stream
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

        /**
         * @brief Creates a JSON string from the object.
         *
         * @return JSON string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_LIST_TABLE_COUNTERS_REQUEST_H
