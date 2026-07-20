//
// Created by vogje01 on 12/21/23.
//

#pragma once

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseObject.h>
#include <awsmock/dto/dynamodb/model/StreamViewType.h>

namespace Awsmock::Dto::DynamoDb {

    /**
     * @brief DynamoDB on demand throughput
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct OnDemandThroughput final : Common::BaseObject<OnDemandThroughput> {

        /**
         * @brief Max request units
         */
        int maxReadRequestUnits{};

        /**
         * @brief Max request units
         */
        int maxWriteRequestUnits{};

      private:

        friend OnDemandThroughput tag_invoke(boost::json::value_to_tag<OnDemandThroughput>, boost::json::value const &v) {
            OnDemandThroughput r = {};
            r.maxReadRequestUnits = Core::Json::GetIntValue(v, "MaxReadRequestUnits");
            r.maxWriteRequestUnits = Core::Json::GetIntValue(v, "MaxWriteRequestUnits");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, OnDemandThroughput const &obj) {
            jv = {
                    {"MaxReadRequestUnits", obj.maxReadRequestUnits},
                    {"MaxWriteRequestUnits", obj.maxWriteRequestUnits},
            };
        }
    };
}// namespace Awsmock::Dto::DynamoDb
