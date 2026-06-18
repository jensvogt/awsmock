//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <map>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::DynamoDb {

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

}// namespace Awsmock::Dto::DynamoDb
