//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::DynamoDb {

    /**
     * @brief Scan request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ScanRequest final : Common::BaseCounter<ScanRequest> {

        /**
         * Table name
         */
        std::string tableName;

      private:

        friend ScanRequest tag_invoke(boost::json::value_to_tag<ScanRequest>, boost::json::value const &v) {
            ScanRequest r;
            r.tableName = Core::Json::GetStringValue(v, "TableName");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ScanRequest const &obj) {
            jv = {
                    {"TableName", obj.tableName},
            };
        }
    };
}// namespace Awsmock::Dto::DynamoDb