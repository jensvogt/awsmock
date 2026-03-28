//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_EXPORT_ITEMS_REQUEST_H
#define AWSMOCK_DTO_DYNAMODB_EXPORT_ITEMS_REQUEST_H

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::DynamoDb {

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

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_EXPORT_ITEMS_REQUEST_H
