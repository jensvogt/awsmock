//
// Created by vogje01 on 12/21/23.
//

#pragma once

// C++ includes
#include <string>

// AwsMock include
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/common/BaseEntity.h>
#include <awsmock/utils/MongoUtils.h>

namespace Awsmock::Database::Entity::DynamoDb {

    /**
     * @brief DynamoDB key schema
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct KeySchema final : Common::BaseEntity<KeySchema> {

        /**
         * Attribute name
         */
        std::string attributeName;

        /**
         * Key type
         */
        std::string keyType;

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
