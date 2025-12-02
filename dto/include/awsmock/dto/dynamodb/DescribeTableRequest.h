//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_DESCRIBE_TABLE_REQUEST_H
#define AWSMOCK_DTO_DYNAMODB_DESCRIBE_TABLE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB describe table request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DescribeTableRequest final : Common::BaseCounter<DescribeTableRequest> {

        /**
         * Table name
         */
        std::string tableName;

      private:

        friend DescribeTableRequest tag_invoke(boost::json::value_to_tag<DescribeTableRequest>, boost::json::value const &v) {
            DescribeTableRequest r;
            r.tableName = Core::Json::GetStringValue(v, "TableName");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DescribeTableRequest const &obj) {
            jv = {
                    {"TableName", obj.tableName},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_DESCRIBE_TABLE_REQUEST_H
