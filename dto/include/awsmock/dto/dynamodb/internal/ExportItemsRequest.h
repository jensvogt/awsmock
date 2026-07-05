//
// Created by vogje01 on 30/05/2023.
//

#pragma once
// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::DynamoDb {

    /**
     * @brief DynamoDB export items request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ExportItemsRequest final : Common::BaseCounter<ExportItemsRequest> {

        /**
         * Table name
         */
        std::string tableName;

      private:

        friend ExportItemsRequest tag_invoke(boost::json::value_to_tag<ExportItemsRequest>, boost::json::value const &v) {
            ExportItemsRequest r;
            r.tableName = Core::Json::GetStringValue(v, "TableName");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ExportItemsRequest const &obj) {
            jv = {
                    {"TableName", obj.tableName},
            };
        }
    };

}// namespace Awsmock::Dto::DynamoDb

