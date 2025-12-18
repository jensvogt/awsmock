//
// Created by vogje01 on 07/06/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_ITEM_H
#define AWSMOCK_DTO_DYNAMODB_ITEM_H

// C++ includes
#include <chrono>
#include <string>
#include <vector>

// AwsMock includes
#include "AttributeValue.h"


#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::DynamoDb {

    using std::optional;
    using std::chrono::system_clock;

    /**
     * @brief DynamoDB item primary key
     * @code(.json)
     * {
     *   "featureCustom": {
     *     "N": "1024"
     *   },
     *   "featureName": {
     *     "S": "ONIX_PARSING"
     *   },
     *   "featureState": {
     *     "S": "{\"enabled\":true,\"strategyId\":null,\"parameters\":{}}"
     *   }
     * }
     * @endcode
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Item final : Common::BaseCounter<Item> {

        /**
         * ID
         */
        std::string oid;

        /**
         * Table name
         */
        std::string tableName;

        /**
         * Attributes
         */
        std::map<std::string, AttributeValue> attributes;

        /**
         * Keys
         */
        std::map<std::string, AttributeValue> keys;

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified = system_clock::now();

      private:

        friend Item tag_invoke(boost::json::value_to_tag<Item>, boost::json::value const &v) {
            Item r;
            r.oid = Core::Json::GetStringValue(v, "oid");
            r.tableName = Core::Json::GetStringValue(v, "tableName");
            r.created = Core::Json::GetDatetimeValue(v, "created");
            r.modified = Core::Json::GetDatetimeValue(v, "modified");

            if (Core::Json::AttributeExists(v, "attributes")) {
                r.attributes = boost::json::value_to<std::map<std::string, AttributeValue>>(v, "attributes");
            }
            if (Core::Json::AttributeExists(v, "keys")) {
                r.keys = boost::json::value_to<std::map<std::string, AttributeValue>>(v, "keys");
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Item const &obj) {
            jv = {
                    {"oid", obj.oid},
                    {"tableName", obj.tableName},
                    {"attributes", boost::json::value_from(obj.attributes)},
                    {"keys", boost::json::value_from(obj.keys)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

    typedef std::vector<Item> ItemList;

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_ITEM_H
