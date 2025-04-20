//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_SCAN_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_SCAN_RESPONSE_H

// C++ standard includes
#include <map>
#include <ranges>
#include <string>

// Boost include<
#include <boost/beast.hpp>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseRequest.h>
#include <awsmock/dto/dynamodb/model/TableStatus.h>
#include <awsmock/entity/dynamodb/Item.h>

namespace AwsMock::Dto::DynamoDb {

    namespace http = boost::beast::http;

    /**
     * @brief Scan response
     *
     * Example:
     * @code(.json)
     * {
     * "Items": [
     *   {
     *       "featureCustom": {
     *           "N": "1024"
     *       },
     *       "featureName": {
     *           "S": "ONIX_PARSING"
     *       },
     *       "featureState": {
     *           "S": "{\"enabled\":true,\"strategyId\":null,\"parameters\":{}}"
     *       }
     *   },
     *   {
     *       "featureName": {
     *           "S": "ONIX_SPLITTING"
     *       },
     *       "featureState": {
     *           "S": "{\"enabled\":true,\"strategyId\":null,\"parameters\":{}}"
     *       }
     *   }
     *  ],
     *  "Count": 2,
     *  "ScannedCount": 2,
     *  "ConsumedCapacity": null
     *}
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ScanResponse final : Common::BaseRequest<ScanResponse> {

        /**
         * Region
         */
        std::string region;

        /**
         * Table name
         */
        std::string tableName;

        /**
         * Original HTTP response body
         */
        std::string body;

        /**
         * Original HTTP response headers
         */
        std::map<std::string, std::string> headers;

        /**
         * HTTP status from docker image
         */
        http::status status;

        /**
         * Item count
         */
        long count;

        /**
         * Scanned item count
         */
        long scannedCount;
        /**
         * Items array
         */
        std::vector<Database::Entity::DynamoDb::Item> items;

        /**
         * @brief Scans the response and fills in the attributes
         *
         * @param table table entity
         */
        void PrepareResponse(const Database::Entity::DynamoDb::Table &table);

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
        std::string ToJson() override;
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_SCAN_RESPONSE_H
