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
#include <awsmock/entity/common/BaseEntity.h>
#include <awsmock/entity/secretsmanager/RotationRules.h>
#include <awsmock/entity/secretsmanager/SecretVersion.h>
#include <awsmock/entity/secretsmanager/VersionIdsToStages.h>

namespace AwsMock::Database::Entity::SecretsManager {

    using std::chrono::system_clock;

    /**
     * @brief SecretManager secrets entity.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Secret final : Common::BaseEntity<Secret> {

        /**
         * ID
         */
        std::string oid;

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
        std::map<std::string, SecretVersion> versions;

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
        system_clock::time_point createdDate;

        /**
         * Deleted date
         */
        system_clock::time_point deletedDate;

        /**
         * Last access date
         */
        system_clock::time_point lastAccessedDate;

        /**
         * Last changed date
         */
        system_clock::time_point lastChangedDate;

        /**
         * Last rotation date
         */
        system_clock::time_point lastRotatedDate;

        /**
         * Next rotation date
         */
        system_clock::time_point nextRotatedDate;

        /**
         * Next rotation date
         */
        bool rotationEnabled = false;

        /**
         * Rotation lambda ARN
         */
        std::string rotationLambdaARN;

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified = system_clock::now();

        /**
         * @brief Checks if the secret has a version with the given version ID.
         *
         * @param versionId version ID
         * @return true, if found, otherwise false
         */
        [[nodiscard]] bool HasVersion(const std::string &versionId) const {
            return std::ranges::find_if(versions, [versionId](const std::pair<std::string, SecretVersion> &version) {
                       return version.first == versionId;
                   }) != versions.end();
        }

        /**
         * @brief Returns the version with the given version ID, or an empty object if not found.
         *
         * @par
         * The version is not checked for existence. Use HasVersion() to check for existence before calling this method.
         *
         * @param versionId version ID
         * @return version with the given ID, or empty object
         */
        [[nodiscard]] SecretVersion GetVersion(const std::string &versionId) const {
            const auto it =
                    std::ranges::find_if(versions, [versionId](const std::pair<std::string, SecretVersion> &version) {
                        return version.first == versionId;
                    });
            if (it != versions.end()) {
                return it->second;
            }
            return {};
        }

        /**
         * @brief Returns the current version.
         *
         * @return current version, or empty object
         */
        [[nodiscard]] std::string GetCurrentVersionId() const {

            const auto it =
                    std::ranges::find_if(versions, [](const std::pair<std::string, SecretVersion> &version) {
                        return std::ranges::find(version.second.stages, "AWSCURRENT") != version.second.stages.end();
                    });
            if (it != versions.end()) {
                return it->first;
            }
            return {};
        }

        /**
         * @brief Returns the previous version.
         *
         * @return previous version, or empty object
         */
        [[nodiscard]] std::string GetPreviousVersionId() const {

            const auto it =
                    std::ranges::find_if(versions, [](const std::pair<std::string, SecretVersion> &version) {
                        return std::ranges::find(version.second.stages, "AWSPREVIOUS") != version.second.stages.end();
                    });
            if (it != versions.end()) {
                return it->first;
            }
            return {};
        }


        /**
         * @brief Returns the previous version.
         *
         * @return previous version, or empty object
         */
        [[nodiscard]] std::string GetVersionIdByStage(const std::string &stage) const {

            const auto it =
                    std::ranges::find_if(versions, [stage](const std::pair<std::string, SecretVersion> &version) {
                        return std::ranges::find(version.second.stages, stage) != version.second.stages.end();
                    });
            if (it != versions.end()) {
                return it->first;
            }
            return {};
        }

        /**
         * @brief Resets all versions to AWSPREVIOUS, except the one with the given versionID.
         *
         * @param versionId current version ID
         */
        void ResetVersions(const std::string &versionId) {
            for (auto &[fst, snd]: versions) {
                if (fst != versionId) {
                    snd.stages.clear();
                    snd.stages.push_back("AWSPREVIOUS");
                }
            }
        }

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

    typedef std::vector<Secret> SecretList;

}// namespace AwsMock::Database::Entity::SecretsManager

#endif// AWSMOCK_DB_ENTITY_SECRETSMANAGER_SECRET_H
