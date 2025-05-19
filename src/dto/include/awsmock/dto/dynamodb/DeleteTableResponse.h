//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_DELETE_TABLE_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_DELETE_TABLE_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/dynamodb/model/ProvisionedThroughput.h>
#include <awsmock/dto/dynamodb/model/TableStatus.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB delete table response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteTableResponse final : Common::BaseCounter<DeleteTableResponse> {

        /**
         * Table name
         */
        std::string tableName;

        /**
         * Table ID
         */
        std::string tableId;

        /**
         * Table ARN
         */
        std::string tableArn;

        /**
         * Provisioned throughput
         */
        ProvisionedThroughput provisionedThroughput;

        /**
         * Table status
         */
        TableStatusType tableStatus = TableStatusType::UNKNOWN;

        /**
         * Item count
         */
        long itemCount{};

        /**
         * Table size in bytes
         */
        long size{};

      private:

        friend DeleteTableResponse tag_invoke(boost::json::value_to_tag<DeleteTableResponse>, boost::json::value const &v) {
            DeleteTableResponse r;
            r.tableName = Core::Json::GetStringValue(v, "TableName");
            r.tableId = Core::Json::GetStringValue(v, "TableId");
            r.tableArn = Core::Json::GetStringValue(v, "TableArn");
            r.provisionedThroughput = boost::json::value_to<ProvisionedThroughput>(v, "ProvisionedThroughput");
            r.tableStatus = TableStatusTypeFromString(Core::Json::GetStringValue(v, "TableStatus"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteTableResponse const &obj) {
            jv = {
                    {"TableName", obj.tableName},
                    {"TableId", obj.tableId},
                    {"TableArn", obj.tableArn},
                    {"ProvisionedThroughput", boost::json::value_from(obj.provisionedThroughput)},
                    {"TableStatus", TableStatusTypeToString(obj.tableStatus)},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_DELETE_TABLE_RESPONSE_H
