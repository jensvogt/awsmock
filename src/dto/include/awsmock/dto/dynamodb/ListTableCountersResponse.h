//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_LIST_TABLE_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_LIST_TABLE_COUNTERS_RESPONSE_H

// C++ standard includes
#include <map>
#include <string>
#include <vector>

// Boost include<
#include <boost/beast.hpp>

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseRequest.h>
#include <awsmock/dto/dynamodb/model/TableCounter.h>

namespace AwsMock::Dto::DynamoDb {

    namespace http = boost::beast::http;

    using bsoncxx::view_or_value;
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::document::value;
    using bsoncxx::document::view;
    using std::chrono::system_clock;

    /**
     * @brief DynamoDB list table response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListTableCountersResponse : Common::BaseRequest<ListTableCountersResponse> {

        /**
         * Table names
         */
        std::vector<TableCounter> tableCounters;

        /**
         * Total number of tables
         */
        long total = 0;

        /**
         * @brief Creates a JSON string from the object.
         *
         * @return JSON string
         */
        std::string ToJson() override;
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_LIST_TABLE_COUNTERS_RESPONSE_H
