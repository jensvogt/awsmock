//
// Created by vogje01 on 4/28/24.
//

#ifndef AWSMOCK_DB_ENTITY_S3_FILTER_RULE_H
#define AWSMOCK_DB_ENTITY_S3_FILTER_RULE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/common/BaseEntity.h>

namespace AwsMock::Database::Entity::S3 {

    /**
     * @brief Filter rule for the S3 bucket notification to SQS queues
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct FilterRule final : Common::BaseEntity<FilterRule> {

        /**
         * Name
         */
        std::string name;

        /**
         * Value
         */
        std::string value;

        /**
         * Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[maybe_unused]] [[nodiscard]] view_or_value<view, bsoncxx::document::value> ToDocument() const override;

        /**
         * Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document.
         */
        [[maybe_unused]] void FromDocument(const view &mResult);
    };

}// namespace AwsMock::Database::Entity::S3

#endif// AWSMOCK_DB_ENTITY_S3_FILTER_RULE_H
