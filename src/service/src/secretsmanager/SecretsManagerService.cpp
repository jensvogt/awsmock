//
// Created by vogje01 on 08/04/2024.
//

#include <awsmock/service/secretsmanager/SecretsManagerService.h>

namespace AwsMock::Service {

    SecretsManagerService::SecretsManagerService() : _database(Database::SecretsManagerDatabase::instance()) {

        // Initialize environment
        _accountId = Core::Configuration::instance().GetValueString("awsmock.access.account-id");

        // Simulation of KMS key
        _kmsKey = "aGYlaHJGZk5FMjNXN05kJmpvWVpvem9GT1M+WE1qWlg=";
    }

    Dto::SecretsManager::CreateSecretResponse SecretsManagerService::CreateSecret(const Dto::SecretsManager::CreateSecretRequest &request) const {
        log_trace << "Create secret request, request: " << request.ToString();

        // Get region
        const std::string region = request.region;

        // Check existence
        if (_database.SecretExists(request.region, request.name)) {
            log_error << "Secrets exists already";
            throw Core::ServiceException("Secret exists already");
        }

        // Check consistency
        if (!request.secretString.empty() && !request.secretBinary.empty()) {
            log_error << "Secret must not have secretString and secretBinary";
            throw Core::ServiceException("Secret must not have secretString and secretBinary");
        }

        if (request.secretString.empty() && request.secretBinary.empty()) {
            log_error << "Secret must have either secretString or secretBinary";
            throw Core::ServiceException("Secret must have either secretString or secretBinary");
        }

        Database::Entity::SecretsManager::Secret secret = {.region = region, .name = request.name};
        try {

            // Update database
            secret.versionId = Core::StringUtils::CreateRandomUuid();
            secret.secretId = request.name + "-" + Core::StringUtils::GenerateRandomHexString(6);
            secret.arn = Core::AwsUtils::CreateSecretArn(request.region, _accountId, secret.secretId);
            secret.createdDate = Core::DateTimeUtils::UnixTimestampNow();

            // Either string or binary data
            if (!request.secretString.empty()) {
                auto *plaintext = (unsigned char *) request.secretString.c_str();
                int len = static_cast<int>(strlen(reinterpret_cast<const char *>(plaintext)));
                unsigned char *encrypted = Core::Crypto::Aes256EncryptString(plaintext, &len, (unsigned char *) _kmsKey.c_str());
                secret.secretString = Core::Crypto::Base64Encode({reinterpret_cast<char *>(encrypted), static_cast<size_t>(len)});
            } else {
                secret.secretBinary = request.secretBinary;
            }
            secret = _database.CreateSecret(secret);

        } catch (Core::DatabaseException &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
        return {.region = secret.region, .name = secret.name, .arn = secret.arn, .versionId = secret.versionId};
    }

    Dto::SecretsManager::DescribeSecretResponse SecretsManagerService::DescribeSecret(const Dto::SecretsManager::DescribeSecretRequest &request) const {
        log_trace << "Describe secret request: " << request.ToString();

        // Check bucket existence
        if (!_database.SecretExists(request.secretId)) {
            log_warning << "Secret does not exist, secretId: " << request.secretId;
            throw Core::NotFoundException("Secret does not exist, secretId: " + request.secretId);
        }

        try {
            Dto::SecretsManager::DescribeSecretResponse response;

            // Get object from database
            const Database::Entity::SecretsManager::Secret secret = _database.GetSecretBySecretId(request.secretId);

            // Convert to DTO
            response.region = secret.region;
            response.name = secret.name;
            response.arn = secret.arn;
            log_debug << "Database secret described, secretId: " << request.secretId;
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "Secret describe secret failed, message: " + exc.message();
            throw Core::ServiceException(exc.message());
        }

        return {};
    }

    Dto::SecretsManager::GetSecretValueResponse SecretsManagerService::GetSecretValue(const Dto::SecretsManager::GetSecretValueRequest &request) const {
        log_trace << "Get secret value request: " << request.ToString();

        // Check bucket existence
        if (!_database.SecretExists(request.secretId)) {
            log_warning << "Secret does not exist, secretId: " << request.secretId;
            throw Core::ServiceException("Secret does not exist, secretId: " + request.secretId);
        }

        try {
            Dto::SecretsManager::GetSecretValueResponse response;

            // Get object from database
            Database::Entity::SecretsManager::Secret secret = _database.GetSecretBySecretId(request.secretId);

            // Convert to DTO
            response.name = secret.name;
            response.arn = secret.arn;
            response.versionId = secret.versionId;
            if (!secret.secretString.empty()) {
                int len;
                //const auto base64Decoded = (unsigned char *) (Core::Crypto::Base64Decode(secret.secretString).c_str());
                response.secretString = std::string(reinterpret_cast<char *>(Core::Crypto::Aes256DecryptString((unsigned char *) Core::Crypto::Base64Decode(secret.secretString).c_str(), &len, (unsigned char *) _kmsKey.c_str())));
                response.secretString[len] = '\0';
            } /*else if (!secret.secretString.empty()) {
                std::string base64Decoded = Core::Crypto::Base64Decode(secret.secretString);
                int len = (int) base64Decoded.length();
                response.secretBinary = std::string(reinterpret_cast<char *>(Core::Crypto::Aes256DecryptString((unsigned char *) base64Decoded.c_str(), &len, (unsigned char *) _kmsKey.c_str())));
            }*/
            else {
                log_warning << "Neither string nor binary, secretId: " << request.secretId;
            }
            log_debug << "Get secret value, secretId: " << request.secretId;

            // Update database
            secret.lastAccessedDate = Core::DateTimeUtils::UnixTimestampNow();
            secret = _database.UpdateSecret(secret);
            log_trace << "Secret updated, secretId: " << secret.oid;

            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "Secret describe secret failed, message: " + exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SecretsManager::ListSecretsResponse SecretsManagerService::ListSecrets(const Dto::SecretsManager::ListSecretsRequest &request) const {
        log_trace << "List secrets request: " << request.ToString();

        try {
            Dto::SecretsManager::ListSecretsResponse response;

            // Get object from database
            for (Database::Entity::SecretsManager::SecretList secrets = _database.ListSecrets(); const auto &s: secrets) {
                Dto::SecretsManager::Secret secret;
                secret.primaryRegion = s.primaryRegion;
                secret.arn = s.arn;
                secret.name = s.name;
                secret.description = s.description;
                secret.kmsKeyId = s.kmsKeyId;
                secret.createdDate = s.createdDate;
                secret.deletedDate = s.deletedDate;
                secret.lastAccessedDate = s.lastAccessedDate;
                secret.lastChangedDate = s.lastChangedDate;
                secret.lastRotatedDate = s.lastRotatedDate;
                secret.nextRotatedDate = s.nextRotatedDate;
                secret.rotationEnabled = s.rotationEnabled;
                secret.rotationLambdaARN = s.rotationLambdaARN;

                // Rotation rules
                secret.rotationRules.automaticallyAfterDays = s.rotationRules.automaticallyAfterDays;
                secret.rotationRules.duration = s.rotationRules.duration;
                secret.rotationRules.scheduleExpression = s.rotationRules.scheduleExpression;

                response.secretList.emplace_back(secret);
            }
            //response.nextToken = secrets.back().secretId;

            // Convert to DTO
            log_debug << "Database list secrets, region: " << request.region;
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "List secrets failed, message: " + exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SecretsManager::UpdateSecretResponse SecretsManagerService::UpdateSecret(const Dto::SecretsManager::UpdateSecretRequest &request) const {
        log_trace << "Update secret request: " << request.ToString();

        // Check bucket existence
        if (!_database.SecretExists(request.secretId)) {
            log_warning << "Secret does not exist, secretId: " << request.secretId;
            throw Core::ServiceException("Secret does not exist, secretId: " + request.secretId);
        }

        try {

            // Get object from database
            Database::Entity::SecretsManager::Secret secret = _database.GetSecretBySecretId(request.secretId);

            // Update database
            secret.kmsKeyId = request.kmsKeyId;
            secret.secretString = request.secretString;
            secret.secretBinary = request.secretBinary;
            secret.description = request.description;
            secret.lastChangedDate = Core::DateTimeUtils::UnixTimestampNow();

            secret = _database.UpdateSecret(secret);

            // Convert to DTO
            log_debug << "Database secret described, secretId: " << request.secretId;
            return {.region = secret.region, .name = secret.name, .arn = secret.arn, .versionId = secret.versionId};

        } catch (Core::DatabaseException &exc) {
            log_error << "Secret describe secret failed, message: " + exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SecretsManager::RotateSecretResponse SecretsManagerService::RotateSecret(const Dto::SecretsManager::RotateSecretRequest &request) const {
        log_trace << "Rotate secret request: " << request.ToString();

        // Check bucket existence
        if (!_database.SecretExists(request.secretId)) {
            log_warning << "Secret does not exist, secretId: " << request.secretId;
            throw Core::NotFoundException("Secret does not exist, secretId: " + request.secretId);
        }

        try {

            // Get object from database
            Database::Entity::SecretsManager::Secret secret = _database.GetSecretBySecretId(request.secretId);
            secret.lastRotatedDate = Core::DateTimeUtils::UnixTimestampNow();

            // Update database
            //secret.rotationRules = request.rotationRules;
            //secret.secretString = request.secretString;
            //secret.secretBinary = request.secretBinary;
            //secret.description = request.description;

            secret = _database.UpdateSecret(secret);

            // Convert to DTO
            log_debug << "Database secret described, secretId: " << request.secretId;
            return {.region = secret.region, .arn = secret.arn};

        } catch (Core::DatabaseException &exc) {
            log_error << "Secret describe secret failed, message: " + exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SecretsManager::DeleteSecretResponse SecretsManagerService::DeleteSecret(const Dto::SecretsManager::DeleteSecretRequest &request) const {
        log_trace << "Delete secret request: " << request.ToString();

        // Check bucket existence
        if (!_database.SecretExists(request.region, request.name)) {
            log_warning << "Secret does not exist, name: " << request.name;
            throw Core::ServiceException("Secret does not exist, name: " + request.name);
        }

        Dto::SecretsManager::DeleteSecretResponse response;
        try {

            // Get object from database
            const Database::Entity::SecretsManager::Secret secret = _database.GetSecretByRegionName(request.region, request.name);

            // Delete from database
            _database.DeleteSecret(secret);
            log_debug << "Database secret deleted, region: " << request.region << " name: " << request.name;
            return {.region = request.region, .name = secret.name, .arn = secret.arn, .deletionDate = static_cast<double>(Core::DateTimeUtils::UnixTimestampNow() - secret.lastRotatedDate)};

        } catch (Core::DatabaseException &exc) {
            log_error << "SecretManager delete secret failed, message: " + exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

}// namespace AwsMock::Service
