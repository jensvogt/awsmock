//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_LIST_TABLE_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_LIST_TABLE_RESPONSE_H

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
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/common/BaseDto.h>

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
    struct ListTableResponse final : Common::BaseDto<ListTableResponse> {

        /**
         * Region
         */
        std::string region;

        /**
         * Table names
         */
        std::vector<std::string> tableNames;

        /**
         * Last evaluated table name
         */
        std::string lastEvaluatedTableName;

        /**
         * HTTP response body
         */
        std::string body;

        /**
         * HTTP headers
         */
        std::map<std::string, std::string> headers;

        /**
         * HTTP status from docker image
         */
        http::status status;

        /**
         * @brief Scans the response and fills in the attributes
         */
        void ScanResponse();

        /**
         * Parse a JSON stream
         *
         * @param body JSON body
         * @param headers HTTP headers
         */
        void FromJson(const std::string &body, const std::map<std::string, std::string> &headers);

        /**
         * Creates a JSON string from the object.
         *
         * @return JSON string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_LIST_TABLE_RESPONSE_H
