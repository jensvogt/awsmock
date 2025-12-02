//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_SCAN_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_SCAN_RESPONSE_H

// C++ standard includes
#include <map>
#include <ranges>
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/dynamodb/model/Item.h>
#include <awsmock/dto/dynamodb/model/TableStatus.h>

namespace AwsMock::Dto::DynamoDb {

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
    struct ScanResponse final : Common::BaseCounter<ScanResponse> {

        /**
         * Table name
         */
        std::string tableName;

        /**
         * Item count
         */
        long count{};

        /**
         * Scanned item count
         */
        long scannedCount{};

        /**
         * Item array
         */
        std::vector<std::map<std::string, AttributeValue>> items;

      private:

        friend ScanResponse tag_invoke(boost::json::value_to_tag<ScanResponse>, boost::json::value const &v) {
            ScanResponse r;
            r.tableName = Core::Json::GetStringValue(v, "TableName");
            r.count = Core::Json::GetLongValue(v, "Count");
            r.scannedCount = Core::Json::GetLongValue(v, "ScannedCount");
            if (Core::Json::AttributeExists(v, "Items")) {
                for (boost::json::array itemsJson = v.at("Items").as_array(); auto &item: itemsJson) {
                    std::map<std::string, AttributeValue> itemMap;
                    for (auto &attribute: item.as_object()) {
                        itemMap[attribute.key()] = boost::json::value_to<AttributeValue>(attribute.value());
                    }
                    r.items.push_back(itemMap);
                }
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ScanResponse const &obj) {
            jv = {
                    {"TableName", obj.tableName},
                    {"Count", obj.count},
                    {"ScannedCount", obj.scannedCount},
                    {"ScannedCount", obj.scannedCount},
            };
            if (!obj.items.empty()) {
                boost::json::array itemsJson;
                for (const auto &a: obj.items) {
                    boost::json::object itemJson;
                    for (const auto &[fst, snd]: a) {
                        itemJson[fst] = boost::json::value_from(snd);
                    }
                    itemsJson.emplace_back(itemJson);
                }
                jv.as_object()["Items"] = itemsJson;
            }
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_SCAN_RESPONSE_H
