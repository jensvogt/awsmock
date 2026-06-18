//
// Created by vogje01 on 07/06/2023.
//

#pragma once

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::DynamoDb {

    /**
     * @brief DynamoDB table class summary
     * @code(.json)
     *
     * "TableClassSummary": {
     *   "LastUpdateDateTime": number,
     *   "TableClass": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct TableClassSummary final : Common::BaseObject<TableClassSummary> {

        /**
         * Last update Unix timestamp
         */
        long lastUpdateDateTime{};

        /**
         * Table class
         */
        std::string tableCLass;

      private:

        friend TableClassSummary tag_invoke(boost::json::value_to_tag<TableClassSummary>, boost::json::value const &v) {
            TableClassSummary r;
            r.lastUpdateDateTime = Core::Json::GetLongValue(v, "LastUpdateDateTime");
            r.tableCLass = Core::Json::GetStringValue(v, "TableCLass");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, TableClassSummary const &obj) {
            jv = {
                    {"LastUpdateDateTime", obj.lastUpdateDateTime},
                    {"TableCLass", obj.tableCLass},
            };
        }
    };
}// namespace Awsmock::Dto::DynamoDb
