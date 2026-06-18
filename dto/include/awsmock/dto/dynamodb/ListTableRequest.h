//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

#define DEFAULT_TABLE_LIMIT 100

namespace Awsmock::Dto::DynamoDb {

    /**
     * @brief DynamoDB list table request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListTableRequest final : Common::BaseCounter<ListTableRequest> {

        /**
         * The first table name that this operation will evaluate. Use the value that was returned for LastEvaluatedTableName
         * in a previous operation so that you can obtain the next page of results.
         */
        std::string exclusiveStartTableName;

        /**
         * Limit, default is 100
         */
        long limit = DEFAULT_TABLE_LIMIT;

      private:

        friend ListTableRequest tag_invoke(boost::json::value_to_tag<ListTableRequest>, boost::json::value const &v) {
            ListTableRequest r;
            r.exclusiveStartTableName = Core::Json::GetStringValue(v, "ExclusiveStartTableName");
            r.limit = Core::Json::GetLongValue(v, "Limit", DEFAULT_TABLE_LIMIT);
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListTableRequest const &obj) {
            jv = {
                    {"Limit", obj.limit},
            };
            if (!obj.exclusiveStartTableName.empty()) {
                jv.as_object()["ExclusiveStartTableName"] = obj.exclusiveStartTableName;
            }
        }
    };
}// namespace Awsmock::Dto::DynamoDb
