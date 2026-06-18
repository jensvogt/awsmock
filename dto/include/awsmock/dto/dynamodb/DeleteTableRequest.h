//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::DynamoDb {

    /**
     * @brief DynamoDB delete table request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteTableRequest final : Common::BaseCounter<DeleteTableRequest> {

        /**
         * Table name
         */
        std::string tableName;

      private:

        friend DeleteTableRequest tag_invoke(boost::json::value_to_tag<DeleteTableRequest>, boost::json::value const &v) {
            DeleteTableRequest r;
            r.tableName = Core::Json::GetStringValue(v, "TableName");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteTableRequest const &obj) {
            jv = {
                    {"TableName", obj.tableName},
            };
        }
    };
}// namespace Awsmock::Dto::DynamoDb
