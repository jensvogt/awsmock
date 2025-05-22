//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_QUERY_REQUEST_H
#define AWSMOCK_DTO_DYNAMODB_QUERY_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief Query request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct QueryRequest final : Common::BaseCounter<QueryRequest> {

        /**
         * Table name
         */
        std::string tableName;

      private:

        friend QueryRequest tag_invoke(boost::json::value_to_tag<QueryRequest>, boost::json::value const &v) {
            QueryRequest r;
            r.tableName = Core::Json::GetStringValue(v, "TableName");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, QueryRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"TableName", obj.tableName},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_QUERY_REQUEST_H
