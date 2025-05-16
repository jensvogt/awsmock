//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_PROVISIONED_THROUGHPUT_H
#define AWSMOCK_DTO_DYNAMODB_PROVISIONED_THROUGHPUT_H

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::DynamoDb {

    using std::chrono::system_clock;

    /**
     * @brief DynamoDB provisioned throughput
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ProvisionedThroughput final : Common::BaseCounter<ProvisionedThroughput> {

        /**
         * Read capacity units
         */
        int readCapacityUnits = 0;

        /**
         * Write capacity units
         */
        int writeCapacityUnits = 0;

        /**
         * Last decrease time
         */
        system_clock::time_point lastDecreaseDateTime;

        /**
         * Last increase time
         */
        system_clock::time_point lastIncreaseDateTime;

        /**
         * NUmber of decreases
         */
        long numberOfDecreasesToday = 0;

        /**
         * @brief Convert from a BSON document
         */
        void FromDocument(view_or_value<view, value> document) {
            readCapacityUnits = Core::Bson::BsonUtils::GetIntValue(document, "ReadCapacityUnits");
            writeCapacityUnits = Core::Bson::BsonUtils::GetIntValue(document, "WriteCapacityUnits");
            lastDecreaseDateTime = Core::Bson::BsonUtils::GetDateValue(document, "LastDecreaseDateTime");
            lastIncreaseDateTime = Core::Bson::BsonUtils::GetDateValue(document, "LastIncreaseDateTime");
            numberOfDecreasesToday = Core::Bson::BsonUtils::GetLongValue(document, "NumberOfDecreasesToday");
        }

        /**
         * @brief Convert to a BSON document
         */
        view_or_value<view, value> ToDocument() const {
            document document;
            Core::Bson::BsonUtils::SetIntValue(document, "ReadCapacityUnits", readCapacityUnits);
            Core::Bson::BsonUtils::SetIntValue(document, "WriteCapacityUnits", writeCapacityUnits);
            Core::Bson::BsonUtils::SetDateValue(document, "LastDecreaseDateTime", lastDecreaseDateTime);
            Core::Bson::BsonUtils::SetDateValue(document, "LastIncreaseDateTime", lastIncreaseDateTime);
            Core::Bson::BsonUtils::SetLongValue(document, "NumberOfDecreasesToday", numberOfDecreasesToday);
            return document.extract();
        }

      private:

        friend ProvisionedThroughput tag_invoke(boost::json::value_to_tag<ProvisionedThroughput>, boost::json::value const &v) {
            ProvisionedThroughput r;
            r.readCapacityUnits = Core::Json::GetLongValue(v, "readCapacityUnits");
            r.writeCapacityUnits = Core::Json::GetLongValue(v, "writeCapacityUnits");
            r.numberOfDecreasesToday = Core::Json::GetLongValue(v, "numberOfDecreasesToday");
            r.lastDecreaseDateTime = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "lastDecreaseDateTime"));
            r.lastIncreaseDateTime = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "lastIncreaseDateTime"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ProvisionedThroughput const &obj) {
            jv = {
                    {"readCapacityUnits", boost::json::value_from(obj.readCapacityUnits)},
                    {"writeCapacityUnits", boost::json::value_from(obj.writeCapacityUnits)},
                    {"numberOfDecreasesToday", boost::json::value_from(obj.numberOfDecreasesToday)},
                    {"lastDecreaseDateTime", Core::DateTimeUtils::ToISO8601(obj.lastDecreaseDateTime)},
                    {"lastIncreaseDateTime", Core::DateTimeUtils::ToISO8601(obj.lastIncreaseDateTime)},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_PROVISIONED_THROUGHPUT_H
