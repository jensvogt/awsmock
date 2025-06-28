//
// Created by vogje01 on 01/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_SECRETSMANAGER_SECRET_VERSION_H
#define AWSMOCK_DB_ENTITY_SECRETSMANAGER_SECRET_VERSION_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/entity/common/BaseEntity.h>
#include <awsmock/entity/secretsmanager/RotationRules.h>
#include <awsmock/entity/secretsmanager/VersionIdsToStages.h>

namespace AwsMock::Database::Entity::SecretsManager {

    using std::chrono::system_clock;

    /**
     * @brief SecretManager secret version entity.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct SecretVersion final : Common::BaseEntity<SecretVersion> {

        /**
         * Secret string
         */
        std::string secretString;

        /**
         * Base64 encoded secret binary
         */
        std::string secretBinary;

        /**
         * Version ID stages
         */
        std::vector<std::string> stages;

        /**
         * Created timestamp
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last access timestamp
         */
        system_clock::time_point lastAccessed = system_clock::now();

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const override;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document.
         */
        void FromDocument(const std::optional<view> &mResult);
    };

}// namespace AwsMock::Database::Entity::SecretsManager

#endif// AWSMOCK_DB_ENTITY_SECRETSMANAGER_SECRET_VERSION_H
