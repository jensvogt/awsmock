//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_CONSUMED_CAPACITY_H
#define AWSMOCK_DTO_DYNAMODB_CONSUMED_CAPACITY_H

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::DynamoDb {

    using std::chrono::system_clock;

    /**
     * @brief DynamoDB provisioned throughput
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ConsumedCapacity final : Common::BaseCounter<ConsumedCapacity> {

        /**
         * Table name
         */
        std::string tableName;

        /**
         * Read capacity units
         */
        long readCapacityUnits = 0;

        /**
         * Write capacity units
         */
        long writeCapacityUnits = 0;

        /**
         * @brief Convert to a BSON document
         */
        [[nodiscard]] view ToDocument() const;

        /**
         * @brief Convert from a BSON document
         */
        void FromDocument(view document);

      private:

        friend ConsumedCapacity tag_invoke(boost::json::value_to_tag<ConsumedCapacity>, boost::json::value const &v) {
            ConsumedCapacity r;
            r.tableName = Core::Json::GetStringValue(v, "TableName");
            r.readCapacityUnits = Core::Json::GetLongValue(v, "ReadCapacityUnits");
            r.writeCapacityUnits = Core::Json::GetLongValue(v, "WriteCapacityUnits");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ConsumedCapacity const &obj) {
            jv = {
                    {"TableName", obj.tableName},
                    {"ReadCapacityUnits", obj.readCapacityUnits},
                    {"WriteCapacityUnits", obj.writeCapacityUnits},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_CONSUMED_CAPACITY_H
