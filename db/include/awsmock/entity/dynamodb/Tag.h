//
// Created by vogje01 on 07/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_DYNAMODB_TAG_H
#define AWSMOCK_DB_ENTITY_DYNAMODB_TAG_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/common/BaseEntity.h>
#include <awsmock/entity/dynamodb/AttributeValue.h>

namespace AwsMock::Database::Entity::DynamoDb {

    /**
     * @brief DynamoDB item primary key
     * @code(.json)
     * {
     *   "featureCustom": {
     *     "N": "1024"
     *   },
     *   "featureName": {
     *     "S": "ONIX_PARSING"
     *   },
     *   "featureState": {
     *     "S": "{\"enabled\":true,\"strategyId\":null,\"parameters\":{}}"
     *   }
     * }
     * @endcode
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Tag final : Common::BaseEntity<Tag> {

        /**
         * Key
         */
        std::string tagKey;

        /**
         * Value
         */
        std::string tagValue;

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const override;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult query result.
         */
        Tag FromDocument(const view_or_value<view, value> &mResult);
    };

}// namespace AwsMock::Database::Entity::DynamoDb

#endif// AWSMOCK_DB_ENTITY_DYNAMODB_TAG_H
