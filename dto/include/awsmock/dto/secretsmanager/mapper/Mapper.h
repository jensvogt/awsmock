//
// Created by vogje01 on 6/7/25.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_MAPPER_H
#define AWSMOCK_DTO_SECRETSMANAGER_MAPPER_H

// AwsMock includes)
#include "awsmock/dto/secretsmanager/internal/UpdateSecretDetailsResponse.h"
#include "awsmock/dto/secretsmanager/model/SecretCounter.h"


#include <awsmock/dto/secretsmanager/internal/GetSecretDetailsResponse.h>
#include <awsmock/entity/secretsmanager/Secret.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief Maps an entity to the corresponding DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class Mapper {

      public:

        /**
         * @brief Maps a secrets manager secret entity to a get secret response
         *
         * @param secret secret entity
         * @param decryptedSecretString decrypted secrets string
         * @return GetSecretDetailsResponse
         * @see GetSecretDetailsResponse
         */
        static GetSecretDetailsResponse map(const Database::Entity::SecretsManager::Secret &secret, const std::string &decryptedSecretString);

        /**
         * @brief Maps a secrets manager secret entity to a secret update response
         *
         * @param secret secret entity
         * @param decryptedSecretString decrypted secrets string
         * @return GetSecretDetailsResponse
         * @see GetSecretDetailsResponse
         */
        static UpdateSecretDetailsResponse mapUpdate(const Database::Entity::SecretsManager::Secret &secret, const std::string &decryptedSecretString);

        /**
         * @brief Maps a secrets manager secret entity to a get secret response
         *
         * @param secretCounter secret counter DTO
         * @return Database::Entity::SecretsManager::Secret
         * @see Database::Entity::SecretsManager::Secret
         */
        static Database::Entity::SecretsManager::Secret map(const SecretCounter &secretCounter);

        /**
         * @brief Maps a secrets manager rotation rule entity to a get secret manager rotation DTO
         *
         * @param rotationRule rotation rule entity
         * @return RotationRules DTO
         * @see RotationRules
         */
        static RotationRules map(const Database::Entity::SecretsManager::RotationRules &rotationRule);

        /**
         * @brief Maps a secrets manager rotation rule DTO to a get secret manager rotation rule entity
         *
         * @param rotationRule rotation rule entity
         * @return RotationRules DTO
         * @see RotationRules
         */
        static Database::Entity::SecretsManager::RotationRules map(const RotationRules &rotationRule);
    };

}// namespace AwsMock::Dto::SecretsManager

#endif//AWSMOCK_DTO_SECRETSMANAGER_MAPPER_H
