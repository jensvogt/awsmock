//
// Created by vogje01 on 12/21/23.
//

#pragma once

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::DynamoDb {

    /**
     * @brief DynamoDB attribute definition
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct AttributeDefinition final : Common::BaseObject<AttributeDefinition> {

        /**
         * Attribute name
         */
        std::string attributeName;

        /**
         * Attribute type
         */
        std::string attributeType;

      private:

        friend AttributeDefinition tag_invoke(boost::json::value_to_tag<AttributeDefinition>, boost::json::value const &v) {
            AttributeDefinition r = {};
            r.attributeName = Core::Json::GetStringValue(v, "AttributeName");
            r.attributeType = Core::Json::GetStringValue(v, "AttributeType");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AttributeDefinition const &obj) {
            jv = {
                    {"AttributeName", obj.attributeName},
                    {"AttributeType", obj.attributeType},
            };
        }
    };
}// namespace Awsmock::Dto::DynamoDb
