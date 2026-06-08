//
// Created by vogje01 on 07/06/2023.
//

#pragma once

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/common/BaseEntity.h>

namespace Awsmock::Database::Entity::DynamoDb {

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

}// namespace Awsmock::Database::Entity::DynamoDb
