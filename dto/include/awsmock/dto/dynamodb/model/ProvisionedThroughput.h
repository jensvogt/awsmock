//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::DynamoDb {

    using std::chrono::system_clock;

    /**
     * @brief DynamoDB provisioned throughput
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ProvisionedThroughput final : Common::BaseObject<ProvisionedThroughput> {

        /**
         * Read capacity units
         */
        long readCapacityUnits{};

        /**
         * Write capacity units
         */
        long writeCapacityUnits{};

        /**
         * Last decrease time
         */
        system_clock::time_point lastDecreaseDateTime = system_clock::now();

        /**
         * Last increase time
         */
        system_clock::time_point lastIncreaseDateTime = system_clock::now();

        /**
         * Number of decreases
         */
        long numberOfDecreasesToday{};

        /**
         * @brief Convert from a BSON document
         */
        void FromDocument(view_or_value<view, value> document) {
            readCapacityUnits = Core::Bson::BsonUtils::GetLongValue(document, "ReadCapacityUnits");
            writeCapacityUnits = Core::Bson::BsonUtils::GetLongValue(document, "WriteCapacityUnits");
            lastDecreaseDateTime = Core::Bson::BsonUtils::GetDateValue(document, "LastDecreaseDateTime");
            lastIncreaseDateTime = Core::Bson::BsonUtils::GetDateValue(document, "LastIncreaseDateTime");
            numberOfDecreasesToday = Core::Bson::BsonUtils::GetLongValue(document, "NumberOfDecreasesToday");
        }

        /**
         * @brief Convert to a BSON document
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {
            document document;
            Core::Bson::BsonUtils::SetLongValue(document, "ReadCapacityUnits", readCapacityUnits);
            Core::Bson::BsonUtils::SetLongValue(document, "WriteCapacityUnits", writeCapacityUnits);
            Core::Bson::BsonUtils::SetDateValue(document, "LastDecreaseDateTime", lastDecreaseDateTime);
            Core::Bson::BsonUtils::SetDateValue(document, "LastIncreaseDateTime", lastIncreaseDateTime);
            Core::Bson::BsonUtils::SetLongValue(document, "NumberOfDecreasesToday", numberOfDecreasesToday);
            return document.extract();
        }

      private:

        friend ProvisionedThroughput tag_invoke(boost::json::value_to_tag<ProvisionedThroughput>, boost::json::value const &v) {
            ProvisionedThroughput r;
            r.readCapacityUnits = Core::Json::GetLongValue(v, "ReadCapacityUnits", 1);
            r.writeCapacityUnits = Core::Json::GetLongValue(v, "WriteCapacityUnits", 1);
            r.numberOfDecreasesToday = Core::Json::GetLongValue(v, "NumberOfDecreasesToday");
            r.lastDecreaseDateTime = Core::DateTimeUtils::FromUnixTimestamp(Core::Json::GetLongValue(v, "LastDecreaseDateTime"));
            r.lastIncreaseDateTime = Core::DateTimeUtils::FromUnixTimestamp(Core::Json::GetLongValue(v, "LastIncreaseDateTime"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ProvisionedThroughput const &obj) {
            jv = {
                    {"ReadCapacityUnits", boost::json::value_from(obj.readCapacityUnits)},
                    {"WriteCapacityUnits", boost::json::value_from(obj.writeCapacityUnits)},
                    {"NumberOfDecreasesToday", boost::json::value_from(obj.numberOfDecreasesToday)},
                    {"LastDecreaseDateTime", Core::DateTimeUtils::UnixTimestamp(obj.lastDecreaseDateTime)},
                    {"LastIncreaseDateTime", Core::DateTimeUtils::UnixTimestamp(obj.lastIncreaseDateTime)},
            };
        }
    };
}// namespace Awsmock::Dto::DynamoDb
