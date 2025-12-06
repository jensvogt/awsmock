//
// Created by vogje01 on 6/7/25.
//

#include <awsmock/dto/secretsmanager/mapper/Mapper.h>

namespace AwsMock::Dto::SecretsManager {

    GetSecretDetailsResponse Mapper::map(const Database::Entity::SecretsManager::Secret &secret, const std::string &decryptedSecretString) {

        // Convert to DTO
        GetSecretDetailsResponse response;
        response.secretId = secret.secretId;
        response.secretName = secret.name;
        response.secretArn = secret.arn;
        response.secretString = decryptedSecretString;
        response.rotationLambdaARN = secret.rotationLambdaARN;
        response.rotationRules = map(secret.rotationRules);
        response.lastRotatedDate = secret.lastRotatedDate;
        response.nextRotatedDate = secret.nextRotatedDate;
        response.lastAccessedDate = secret.lastAccessedDate;
        response.created = secret.created;
        response.modified = secret.modified;
        return response;
    }

    UpdateSecretDetailsResponse Mapper::mapUpdate(const Database::Entity::SecretsManager::Secret &secret, const std::string &decryptedSecretString) {

        // Convert to DTO
        UpdateSecretDetailsResponse response;
        response.secretDetails.secretId = secret.secretId;
        response.secretDetails.secretName = secret.name;
        response.secretDetails.secretArn = secret.arn;
        response.secretDetails.secretString = decryptedSecretString;
        response.secretDetails.rotationRules = map(secret.rotationRules);
        response.secretDetails.created = secret.created;
        response.secretDetails.modified = secret.modified;
        return response;
    }

    Database::Entity::SecretsManager::Secret Mapper::map(const SecretCounter &secretCounter) {

        // Convert to DTO
        Database::Entity::SecretsManager::Secret result;
        result.secretId = secretCounter.secretId;
        result.name = secretCounter.secretName;
        result.arn = secretCounter.secretArn;
        result.rotationRules = map(secretCounter.rotationRules);
        return result;
    }

    Database::Entity::SecretsManager::RotationRules Mapper::map(const RotationRules &rotationRuleDto) {
        Database::Entity::SecretsManager::RotationRules result;
        result.automaticallyAfterDays = rotationRuleDto.automaticallyAfterDays;
        result.duration = rotationRuleDto.duration;
        result.scheduleExpression = rotationRuleDto.scheduleExpression;
        return result;
    }

    RotationRules Mapper::map(const Database::Entity::SecretsManager::RotationRules &rotationRule) {
        RotationRules result;
        result.automaticallyAfterDays = rotationRule.automaticallyAfterDays;
        result.duration = rotationRule.duration;
        result.scheduleExpression = rotationRule.scheduleExpression;
        return result;
    }
}// namespace AwsMock::Dto::SecretsManager
