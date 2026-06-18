//
// Created by vogje01 on 5/22/24.
//

#pragma once

// C++ includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseObject.h>
#include <awsmock/dto/dynamodb/model/AttributeValue.h>

namespace Awsmock::Dto::DynamoDb {

    /**
     * @brief DynamoDB table key schema
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct KeySchema final : Common::BaseObject<KeySchema> {

        /**
         * Attribute name
         */
        std::string attributeName;

        /**
         * Key type
         */
        std::string keyType;

      private:

        friend KeySchema tag_invoke(boost::json::value_to_tag<KeySchema>, boost::json::value const &v) {
            KeySchema r;
            r.attributeName = Core::Json::GetStringValue(v, "AttributeName");
            r.keyType = Core::Json::GetStringValue(v, "KeyType");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, KeySchema const &obj) {
            jv = {
                    {"AttributeName", obj.attributeName},
                    {"KeyType", obj.keyType},
            };
        }
    };
}// namespace Awsmock::Dto::DynamoDb
