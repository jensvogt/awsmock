//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_SCAN_REQUEST_H
#define AWSMOCK_DTO_DYNAMODB_SCAN_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::DynamoDb {

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

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_SCAN_REQUEST_H
