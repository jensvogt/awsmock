//
// Created by vogje01 on 01/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_SECRETSMANAGER_SECRET_H
#define AWSMOCK_DB_ENTITY_SECRETSMANAGER_SECRET_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/entity/secretsmanager/RotationRules.h>
#include <awsmock/entity/secretsmanager/VersionIdsToStages.h>

namespace AwsMock::Database::Entity::SecretsManager {

    using std::chrono::system_clock;

    /**
     * @brief SecretManager secrets entity.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Secret {

        /**
         * ID
         */
        std::string oid;

        /**
         * Aws region name
         */
        std::string region;

        /**
         * Secret name
         */
        std::string name;

        /**
         * Arn
         */
        std::string arn;

        /**
         * Secret ID
         */
        std::string secretId;

        /**
         * KMS key ID ID
         */
        std::string kmsKeyId;

        /**
         * Secret string
         */
        std::string secretString;

        /**
         * Base64 encoded secret binary
         */
        std::string secretBinary;

        /**
         * Version Id
         */
        std::string versionId;

        /**
         * Description
         */
        std::string description;

        /**
         * Owning service
         */
        std::string owningService;

        /**
         * Primary region
         */
        std::string primaryRegion;

        /**
         * RotationRules
         */
        RotationRules rotationRules;

        /**
         * Creation date
         */
        long createdDate = 0;

        /**
         * Deleted date
         */
        long deletedDate = 0;

        /**
         * Last access date
         */
        long lastAccessedDate = 0;

        /**
         * Last changed date
         */
        long lastChangedDate = 0;

        /**
         * Last rotation date
         */
        long lastRotatedDate = 0;

        /**
         * Next rotation date
         */
        long nextRotatedDate = 0;

        /**
         * Next rotation date
         */
        bool rotationEnabled = false;

        /**
         * Rotation lambda ARN
         */
        std::string rotationLambdaARN;

        /**
         * Version ID stages
         */
        VersionIdsToStages versionIdsToStages;

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified = system_clock::now();

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document.
         */
        void FromDocument(const std::optional<view> &mResult);

        /**
         * @brief Converts the DTO to a JSON string representation.
         *
         * @return DTO as JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @param os output stream
         * @param m message
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const Secret &m);
    };

    typedef std::vector<Secret> SecretList;

}// namespace AwsMock::Database::Entity::SecretsManager

#endif// AWSMOCK_DB_ENTITY_SECRETSMANAGER_SECRET_H
