//
// Created by vogje01 on 12/21/23.
//

#pragma once

// AwsMock include
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/common/BaseEntity.h>

namespace Awsmock::Database::Entity::DynamoDb {

    /**
     * @brief DynamoDB attribute definition
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct AttributeDefinition final : Common::BaseEntity<AttributeDefinition> {

        /**
         * Attribute name
         */
        std::string attributeName;

        /**
         * Attribute type
         */
        std::string attributeType;

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
        void FromDocument(view_or_value<view, value> mResult);
    };
}// namespace Awsmock::Database::Entity::DynamoDb
