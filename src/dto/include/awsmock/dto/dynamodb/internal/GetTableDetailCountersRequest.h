//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_GET_TABLE_DETAIL_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_DYNAMODB_GET_TABLE_DETAIL_COUNTERS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB get table details request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetTableDetailCountersRequest final : Common::BaseCounter<GetTableDetailCountersRequest> {

        /**
         * Name of the table
         */
        std::string tableName;

      private:

        friend GetTableDetailCountersRequest tag_invoke(boost::json::value_to_tag<GetTableDetailCountersRequest>, boost::json::value const &v) {
            GetTableDetailCountersRequest r;
            r.tableName = Core::Json::GetStringValue(v, "tableName");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetTableDetailCountersRequest const &obj) {
            jv = {
                    {"tableName", obj.tableName},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_GET_TABLE_DETAIL_COUNTERS_REQUEST_H
