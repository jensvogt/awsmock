//
// Created by vogje01 on 12/21/23.
//

#pragma once

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::DynamoDb {

    /**
     * @brief DynamoDB warm throughput
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct WarmThroughput final : Common::BaseObject<WarmThroughput> {

        /**
         * @brief Read units per second
         */
        int readUnitsPerSecond{};

        /**
         * @brief Write units per second
         */
        int writeUnitsPerSecond{};

        /**
         * @brief warm throughput status
         */
        std::string status{"ACTIVE"};

      private:

        /**
         * @brief Deserialization
         *
         * @param v current value
         * @return object
         */
        friend WarmThroughput tag_invoke(boost::json::value_to_tag<WarmThroughput>, boost::json::value const &v) {
            WarmThroughput r = {};
            r.readUnitsPerSecond = Core::Json::GetIntValue(v, "ReadUnitsPerSecond");
            r.writeUnitsPerSecond = Core::Json::GetIntValue(v, "WriteUnitsPerSecond");
            r.status = Core::Json::GetStringValue(v, "Status");
            return r;
        }

        /**
         * @brief Serialization
         *
         * @param jv JSON value
         * @param obj current object
         */
        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, WarmThroughput const &obj) {
            jv = {
                    {"ReadUnitsPerSecond", obj.readUnitsPerSecond},
                    {"WriteUnitsPerSecond", obj.writeUnitsPerSecond},
                    {"Status", obj.status},
            };
        }
    };
}// namespace Awsmock::Dto::DynamoDb
