//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_LIST_TABLE_ARNS_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_LIST_TABLE_ARNS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/StringUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief List DynamoDB table ARNs response.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListTableArnsResponse final : Common::BaseCounter<ListTableArnsResponse> {

        /**
         * List of table ARNs
         */
        std::vector<std::string> tableArns;

      private:

        friend ListTableArnsResponse tag_invoke(boost::json::value_to_tag<ListTableArnsResponse>, boost::json::value const &v) {
            ListTableArnsResponse r;
            r.tableArns = boost::json::value_to<std::vector<std::string>>(v.at("tableArns"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListTableArnsResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"tableArns", boost::json::value_from(obj.tableArns)},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_LIST_TABLE_ARNS_RESPONSE_H
