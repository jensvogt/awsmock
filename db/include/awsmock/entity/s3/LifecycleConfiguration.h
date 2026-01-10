//
// Created by vogje01 on 4/28/24.
//

#ifndef AWSMOCK_DB_ENTITY_S3_LIFE_CYCLE_CONFIGURATION_H
#define AWSMOCK_DB_ENTITY_S3_LIFE_CYCLE_CONFIGURATION_H

// C++ standard includes
#include <string>

// AwsMock includes
#include "LifecycleTransition.h"
#include "awsmock/dto/s3/model/LifecycleTransition.h"


#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/common/BaseEntity.h>
#include <awsmock/entity/s3/LifecycleStatus.h>

namespace AwsMock::Database::Entity::S3 {

    /**
     * @brief Life cycle rule for a S3 bucket
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct LifecycleConfiguration final : Common::BaseEntity<LifecycleConfiguration> {

        /**
         * @brief ID of the life cycle configuration
         *
         * @par
         * Default is ramdom UUID.
         */
        std::string id = Core::StringUtils::CreateRandomUuid();

        /**
         * @brief S3 object prefix
         *
         * @par
         * Default is empty
         */
        std::string prefix;

        /**
         * @brief Life cycle status
         *
         * @par
         * Default is LIFECYCLE_DISABLED
         */
        LifecycleStatus status = LIFECYCLE_DISABLED;

        /**
         * @brief Life cycle transitions
         */
        std::vector<LifecycleTransition> transitions;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document.
         */
        explicit LifecycleConfiguration(const view &mResult);

        /**
         * @brief Default constructor
         */
        explicit LifecycleConfiguration() = default;

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[maybe_unused]] [[nodiscard]] view_or_value<view, value> ToDocument() const override;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document.
         */
        [[maybe_unused]] void FromDocument(const view &mResult);
    };

}// namespace AwsMock::Database::Entity::S3

#endif// AWSMOCK_DB_ENTITY_S3_LIFE_CYCLE_CONFIGURATION_H
