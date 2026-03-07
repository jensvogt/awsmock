//
// Created by vogje01 on 5/22/24.
//

#ifndef AWSMOCK_DTO_DYNAMODB_KEY_SCHEMA_H
#define AWSMOCK_DTO_DYNAMODB_KEY_SCHEMA_H

// C++ includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/dynamodb/model/AttributeValue.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB table key schema
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct KeySchema final : Common::BaseCounter<KeySchema> {

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

}// namespace AwsMock::Dto::DynamoDb

#endif//AWSMOCK_DTO_DYNAMODB_KEY_H
