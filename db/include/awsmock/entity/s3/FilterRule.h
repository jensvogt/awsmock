//
// Created by vogje01 on 4/28/24.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/common/BaseEntity.h>

namespace Awsmock::Database::Entity::S3 {

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
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[maybe_unused]] [[nodiscard]] view_or_value<view, bsoncxx::document::value> ToDocument() const override;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document.
         * @return filter rule entity
         */
        static FilterRule FromDocument(const view &mResult);
    };

}// namespace Awsmock::Database::Entity::S3
