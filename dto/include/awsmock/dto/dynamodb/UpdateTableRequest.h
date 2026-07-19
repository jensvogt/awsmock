//
// Created by vogje01 on 19/07/2026.
//

#pragma once

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/dynamodb/model/AttributeDefinition.h>
#include <awsmock/dto/dynamodb/model/KeySchema.h>
#include <awsmock/dto/dynamodb/model/ProvisionedThroughput.h>

namespace Awsmock::Dto::DynamoDb {

    /**
     * @brief DynamoDB update table request
     */
    struct UpdateTableRequest final : Common::BaseCounter<UpdateTableRequest> {

        /**
         * Table name
         */
        std::string tableName;

        /**
         * Billing mode
         */
        std::string billingMode;

        /**
         * Provisioned throughput
         */
        ProvisionedThroughput provisionedThroughput;

        /**
         * Server-side encryption enabled
         */
        bool sseEnabled{false};

      private:

        friend UpdateTableRequest tag_invoke(boost::json::value_to_tag<UpdateTableRequest>, boost::json::value const &v) {
            UpdateTableRequest r;
            r.tableName = Core::Json::GetStringValue(v, "TableName");
            r.billingMode = Core::Json::GetStringValue(v, "BillingMode");
            if (Core::Json::AttributeExists(v, "ProvisionedThroughput")) {
                r.provisionedThroughput = boost::json::value_to<ProvisionedThroughput>(v.at("ProvisionedThroughput"));
            }
            if (Core::Json::AttributeExists(v, "SSESpecification")) {
                r.sseEnabled = Core::Json::GetBoolValue(v.at("SSESpecification"), "Enabled");
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateTableRequest const &obj) {
            jv = {{"TableName", obj.tableName}};
        }
    };

}// namespace Awsmock::Dto::DynamoDb
