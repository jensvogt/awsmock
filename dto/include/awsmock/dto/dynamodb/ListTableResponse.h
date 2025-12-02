//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_LIST_TABLE_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_LIST_TABLE_RESPONSE_H

// C++ standard includes
#include <map>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB list table response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListTableResponse final : Common::BaseCounter<ListTableResponse> {

        /**
         * Table names
         */
        std::vector<std::string> tableNames;

        /**
         * Last evaluated table name
         */
        std::string lastEvaluatedTableName;

      private:

        friend ListTableResponse tag_invoke(boost::json::value_to_tag<ListTableResponse>, boost::json::value const &v) {
            ListTableResponse r;
            r.tableNames = Core::Json::GetStringArrayValue(v, "TableNames");
            r.lastEvaluatedTableName = Core::Json::GetStringValue(v, "LastEvaluatedTableName");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListTableResponse const &obj) {
            jv = {
                    {"TableNames", boost::json::value_from(obj.tableNames)},
                    {"LastEvaluatedTableName", obj.lastEvaluatedTableName},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_LIST_TABLE_RESPONSE_H
