//
// Created by vogje01 on 08/04/2024.
//

#include <awsmock/service/secretsmanager/SecretsManagerService.h>

namespace AwsMock::Service {

    SecretsManagerService::SecretsManagerService() : _secretsManagerDatabase(Database::SecretsManagerDatabase::instance()), _lambdaDatabase(Database::LambdaDatabase::instance()) {

        // Initialize environment
        _accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
    }

    Dto::SecretsManager::CreateSecretResponse SecretsManagerService::CreateSecret(const Dto::SecretsManager::CreateSecretRequest &request) const {
        Monitoring::MetricServiceTimer measure(SECRETSMANAGER_SERVICE_TIMER, "action", "create_secret");
        Monitoring::MetricService::instance().IncrementCounter(SECRETSMANAGER_SERVICE_TIMER, "action", "create_secret");
        log_trace << "Create secret request, request: " << request.ToString();

        // Get the region
        const std::string region = request.region;

        // Check existence
        if (_secretsManagerDatabase.SecretExists(request.region, request.name)) {
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

        Database::Entity::SecretsManager::Secret secret;
        secret.region = region;
        secret.name = request.name;

        try {

            // Update database
            Database::Entity::SecretsManager::SecretVersion version;
            std::string versionId = Core::StringUtils::CreateRandomUuid();
            version.stages.emplace_back(Dto::SecretsManager::VersionStateToString(Dto::SecretsManager::VersionStateType::AWSCURRENT));

            if (request.kmsKeyId.empty()) {
                CreateKmsKey(secret);
            } else {
                secret.kmsKeyId = request.kmsKeyId;
            }

            // Either string or binary data
            if (!request.secretString.empty()) {
                EncryptSecret(version, secret.kmsKeyId, request.secretString);
            } else {
                version.secretBinary = request.secretBinary;
            }

            secret.secretId = request.name + "-" + Core::StringUtils::GenerateRandomHexString(6);
            secret.arn = Core::AwsUtils::CreateSecretArn(request.region, _accountId, secret.secretId);
            secret.createdDate = Core::DateTimeUtils::UnixTimestampNow();
            secret.description = request.description;
            secret.versionIdsToStages.versions[versionId] = {Dto::SecretsManager::VersionStateToString(Dto::SecretsManager::VersionStateType::AWSCURRENT)};
            secret.versions[versionId] = version;
            secret = _secretsManagerDatabase.CreateSecret(secret);

            // Create the response
            Dto::SecretsManager::CreateSecretResponse response;
            response.region = secret.region;
            response.name = secret.name;
            response.arn = secret.arn;
            response.versionId = versionId;

            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    Dto::SecretsManager::DescribeSecretResponse SecretsManagerService::DescribeSecret(const Dto::SecretsManager::DescribeSecretRequest &request) const {
        Monitoring::MetricServiceTimer measure(SECRETSMANAGER_SERVICE_TIMER, "action", "describe_secret");
        Monitoring::MetricService::instance().IncrementCounter(SECRETSMANAGER_SERVICE_TIMER, "action", "describe_secret");
        log_trace << "Describe secret request: " << request.ToString();

        // Check bucket existence
        if (!_secretsManagerDatabase.SecretExists(request.secretId)) {
            log_warning << "Secret does not exist, secretId: " << request.secretId;
            throw Core::NotFoundException("Secret does not exist, secretId: " + request.secretId);
        }

        try {
            // Get the object from the database
            const Database::Entity::SecretsManager::Secret secret = _secretsManagerDatabase.GetSecretBySecretId(request.secretId);

            // Convert to DTO
            Dto::SecretsManager::DescribeSecretResponse response;
            response.region = secret.region;
            response.name = secret.name;
            response.arn = secret.arn;
            response.deletedDate = secret.deletedDate;
            response.description = secret.description;
            response.kmsKeyId = secret.kmsKeyId;
            response.rotationEnabled = secret.rotationEnabled;
            response.rotationLambdaARN = secret.rotationLambdaARN;

            // Version stages
            for (const auto &[fst, snd]: secret.versionIdsToStages.versions) {
                response.versionIdsToStages.versions[fst] = snd;
            }
            log_debug << "Database secret described, secretId: " << request.secretId;
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "Secret describe secret failed, message: " + exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SecretsManager::GetSecretValueResponse SecretsManagerService::GetSecretValue(const Dto::SecretsManager::GetSecretValueRequest &request) const {
        Monitoring::MetricServiceTimer measure(SECRETSMANAGER_SERVICE_TIMER, "action", "get_secret_value");
        Monitoring::MetricService::instance().IncrementCounter(SECRETSMANAGER_SERVICE_TIMER, "action", "get_secret_value");
        log_trace << "Get secret value request: " << request.ToString();

        // Check whether we have a name of ARN
        std::string arn = Core::StringUtils::Contains(request.secretId, ":") ? request.secretId : Core::AwsUtils::CreateSecretArn(request.region, _accountId, request.secretId);

        // Check bucket existence
        if (!_secretsManagerDatabase.SecretExistsByArn(arn)) {
            log_warning << "Secret does not exist, arn: " << arn;
            throw Core::ServiceException("Secret does not exist, arn: " + arn);
        }

        try {
            Dto::SecretsManager::GetSecretValueResponse response;

            // Get the object from the database
            Database::Entity::SecretsManager::Secret secret = _secretsManagerDatabase.GetSecretByArn(arn);

            if (!request.versionId.empty() && !secret.HasVersion(request.versionId)) {
                log_warning << "Secret version does not exist, versionId: " << request.versionId;
                throw Core::ServiceException("Secret version does not exist, versionId: " + request.versionId);
            }
            std::string versionId = request.versionId.empty() ? secret.GetCurrentVersionId() : request.versionId;
            Database::Entity::SecretsManager::SecretVersion version = secret.versions[versionId];

            // Convert to DTO
            response.name = secret.name;
            response.arn = secret.arn;
            response.versionId = versionId;
            response.createdDate = secret.createdDate;
            response.versionStages = secret.versionIdsToStages.versions[request.versionId];

            if (!secret.kmsKeyId.empty()) {
                Dto::KMS::DecryptRequest decryptRequest;
                decryptRequest.keyId = secret.kmsKeyId;
                decryptRequest.ciphertext = version.secretString;
                Dto::KMS::DecryptResponse kmsResponse = _kmsService.Decrypt(decryptRequest);
                response.secretString = Core::Crypto::Base64Decode(kmsResponse.plaintext);
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
            secret = _secretsManagerDatabase.UpdateSecret(secret);
            log_trace << "Secret updated, secretId: " << secret.oid;

            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "Secret describe secret failed, message: " + exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SecretsManager::ListSecretsResponse SecretsManagerService::ListSecrets(const Dto::SecretsManager::ListSecretsRequest &request) const {
        Monitoring::MetricServiceTimer measure(SECRETSMANAGER_SERVICE_TIMER, "action", "list_secrets");
        Monitoring::MetricService::instance().IncrementCounter(SECRETSMANAGER_SERVICE_TIMER, "action", "list_secrets");
        log_trace << "List secrets request: " << request;

        try {
            Dto::SecretsManager::ListSecretsResponse response;

            // Get the object from the database
            for (const Database::Entity::SecretsManager::SecretList secrets = _secretsManagerDatabase.ListSecrets(); const auto &s: secrets) {
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

            // Convert to DTO
            log_debug << "Database list secrets, region: " << request.region;
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "List secrets failed, message: " + exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SecretsManager::ListSecretCountersResponse SecretsManagerService::ListSecretCounters(const Dto::SecretsManager::ListSecretCountersRequest &request) const {
        Monitoring::MetricServiceTimer measure(SECRETSMANAGER_SERVICE_TIMER, "action", "list_secrets");
        Monitoring::MetricService::instance().IncrementCounter(SECRETSMANAGER_SERVICE_TIMER, "action", "list_secrets");
        log_trace << "List secret counters request: " << request;

        try {
            Dto::SecretsManager::ListSecretCountersResponse response;
            response.total = _secretsManagerDatabase.CountSecrets(request.region);

            // TODO: use mapper
            // Get the object from the database
            for (const Database::Entity::SecretsManager::SecretList secrets = _secretsManagerDatabase.ListSecrets(); const auto &s: secrets) {
                Dto::SecretsManager::SecretCounter secretCounter;
                secretCounter.secretName = s.name;
                secretCounter.secretArn = s.arn;
                secretCounter.secretId = s.secretId;
                secretCounter.createdDate = s.createdDate;
                secretCounter.deletedDate = s.deletedDate;
                secretCounter.lastAccessedDate = s.lastAccessedDate;
                secretCounter.lastChangedDate = s.lastChangedDate;
                secretCounter.lastRotatedDate = s.lastRotatedDate;
                secretCounter.nextRotatedDate = s.nextRotatedDate;
                secretCounter.rotationEnabled = s.rotationEnabled;
                secretCounter.rotationLambdaARN = s.rotationLambdaARN;
                secretCounter.created = s.created;
                secretCounter.modified = s.modified;

                // Rotation rules
                secretCounter.rotationRules.automaticallyAfterDays = s.rotationRules.automaticallyAfterDays;
                secretCounter.rotationRules.duration = s.rotationRules.duration;
                secretCounter.rotationRules.scheduleExpression = s.rotationRules.scheduleExpression;

                response.secretCounters.emplace_back(secretCounter);
            }

            // Convert to DTO
            log_debug << "Database list secrets, region: " << request.region;
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "List secrets failed, message: " + exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SecretsManager::ListSecretVersionCountersResponse SecretsManagerService::ListSecretVersionCounters(const Dto::SecretsManager::ListSecretVersionCountersRequest &request) const {
        Monitoring::MetricServiceTimer measure(SECRETSMANAGER_SERVICE_TIMER, "action", "list_secret_versions");
        Monitoring::MetricService::instance().IncrementCounter(SECRETSMANAGER_SERVICE_TIMER, "action", "list_secret_versions");
        log_trace << "List secret versions request: " << request;

        try {
            Dto::SecretsManager::ListSecretVersionCountersResponse response;
            const Database::Entity::SecretsManager::Secret secret = _secretsManagerDatabase.GetSecretBySecretId(request.secretId);
            response.total = static_cast<long>(secret.versions.size());

            // TODO: use mapper
            // Get the object from the database
            for (const auto &[fst, snd]: secret.versions) {
                Dto::SecretsManager::SecretVersionCounter secretVersionCounter;
                secretVersionCounter.versionId = fst;
                for (const auto &s: snd.stages) {
                    secretVersionCounter.states.emplace_back(s);
                }
                response.secretVersionCounters.emplace_back(secretVersionCounter);
            }

            // Convert to DTO
            log_debug << "List secret versions, secretId: " << request.secretId;
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "List secrets failed, message: " + exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SecretsManager::GetSecretDetailsResponse SecretsManagerService::GetSecretDetails(const Dto::SecretsManager::GetSecretDetailsRequest &request) const {
        Monitoring::MetricServiceTimer measure(SECRETSMANAGER_SERVICE_TIMER, "action", "get_secret");
        Monitoring::MetricService::instance().IncrementCounter(SECRETSMANAGER_SERVICE_TIMER, "action", "get_secret");
        log_trace << "Get secret details request: " << request;

        // Check bucket existence
        if (!_secretsManagerDatabase.SecretExists(request.secretId)) {
            log_warning << "Secret does not exist, secretId: " << request.secretId;
            throw Core::ServiceException("Secret does not exist, secretId: " + request.secretId);
        }

        try {
            Dto::SecretsManager::GetSecretDetailsResponse response;
            Database::Entity::SecretsManager::Secret secret = _secretsManagerDatabase.GetSecretBySecretId(request.secretId);

            // TODO: use mapper
            // Convert to DTO
            log_debug << "Get secret details, secretId: " << request.secretId;
            response.secretId = secret.secretId;
            response.secretName = secret.name;
            response.secretArn = secret.arn;
            response.secretString = GetSecretString(secret);
            response.created = secret.created;
            response.modified = secret.modified;
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "Get secret details failed, message: " + exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SecretsManager::UpdateSecretResponse SecretsManagerService::UpdateSecret(const Dto::SecretsManager::UpdateSecretRequest &request) const {
        Monitoring::MetricServiceTimer measure(SECRETSMANAGER_SERVICE_TIMER, "action", "update_secrets");
        Monitoring::MetricService::instance().IncrementCounter(SECRETSMANAGER_SERVICE_TIMER, "action", "update_secrets");
        log_trace << "Update secret request: " << request.ToString();

        // Check bucket existence
        if (!_secretsManagerDatabase.SecretExists(request.secretId)) {
            log_warning << "Secret does not exist, secretId: " << request.secretId;
            throw Core::ServiceException("Secret does not exist, secretId: " + request.secretId);
        }

        try {

            // Get the object from the database
            Database::Entity::SecretsManager::Secret secret = _secretsManagerDatabase.GetSecretBySecretId(request.secretId);

            // Update database
            Database::Entity::SecretsManager::SecretVersion version;
            std::string versionId = Core::StringUtils::CreateRandomUuid();
            version.secretString = request.secretString;
            version.secretBinary = request.secretBinary;
            version.stages.emplace_back(Dto::SecretsManager::VersionStateToString(Dto::SecretsManager::VersionStateType::AWSCURRENT));

            secret.kmsKeyId = request.kmsKeyId;
            secret.versions[versionId] = version;
            secret.description = request.description;
            secret.lastChangedDate = Core::DateTimeUtils::UnixTimestampNow();
            secret.ResetVersions(versionId);

            secret = _secretsManagerDatabase.UpdateSecret(secret);

            // Convert to DTO
            log_debug << "Database secret described, secretId: " << request.secretId;
            Dto::SecretsManager::UpdateSecretResponse response;
            response.region = secret.region;
            response.name = secret.name;
            response.arn = secret.arn;
            response.versionId = versionId;
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "Secret describe secret failed, message: " + exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SecretsManager::RotateSecretResponse SecretsManagerService::RotateSecret(const Dto::SecretsManager::RotateSecretRequest &request) const {
        Monitoring::MetricServiceTimer measure(SECRETSMANAGER_SERVICE_TIMER, "action", "rotate_secrets");
        Monitoring::MetricService::instance().IncrementCounter(SECRETSMANAGER_SERVICE_TIMER, "action", "rotate_secrets");
        log_trace << "Rotate secret request: " << request.ToString();

        // Check whether we have a name of ARN
        std::string arn = Core::StringUtils::Contains(request.secretId, ":") ? request.secretId : Core::AwsUtils::CreateSecretArn(request.region, _accountId, request.secretId);

        // Check bucket existence
        if (!_secretsManagerDatabase.SecretExistsByArn(arn)) {
            log_warning << "Secret does not exist, arn: " << arn;
            throw Core::NotFoundException("Secret does not exist, arn: " + arn);
        }

        try {

            // Get the object from the database
            Database::Entity::SecretsManager::Secret secret = _secretsManagerDatabase.GetSecretByArn(arn);
            secret.lastRotatedDate = Core::DateTimeUtils::UnixTimestampNow();
            secret.rotationLambdaARN = request.rotationLambdaARN;
            secret.rotationEnabled = true;

            // Create a copy of the current
            std::string versionId = secret.GetCurrentVersionId();
            Database::Entity::SecretsManager::SecretVersion version = secret.versions[versionId];
            version.stages.clear();
            version.stages.emplace_back(Dto::SecretsManager::VersionStateToString(Dto::SecretsManager::VersionStateType::AWSPENDING));
            secret.versions[request.clientRequestToken] = version;

            secret.versionIdsToStages.versions[request.clientRequestToken] = {Dto::SecretsManager::VersionStateToString(Dto::SecretsManager::VersionStateType::AWSPENDING)};
            secret = _secretsManagerDatabase.UpdateSecret(secret);

            if (request.rotateImmediately) {
                if (!secret.rotationLambdaARN.empty()) {

                    // Get lambda function from database
                    const Database::Entity::Lambda::Lambda lambda = Database::LambdaDatabase::instance().GetLambdaByArn(secret.rotationLambdaARN);
                    log_debug << "Secret rotation starting, lambda: " << lambda.function;

                    CreateSecret(secret, lambda, request.clientRequestToken);
                    log_debug << "Secret created, arn: " << secret.arn;

                    SetSecret(secret, lambda, request.clientRequestToken);
                    log_debug << "Secret set in resource, arn: " << secret.arn;

                    TestSecret(secret, lambda, request.clientRequestToken);
                    log_debug << "Secret testet, arn: " << secret.arn;

                    FinishSecret(secret, lambda, request.clientRequestToken);
                    log_debug << "Secret testet, arn: " << secret.arn;
                }
            }

            // Convert to DTO
            log_debug << "Database secret described, arn: " << arn;
            Dto::SecretsManager::RotateSecretResponse response;
            response.region = secret.region;
            response.arn = secret.arn;
            response.name = secret.name;
            response.versionId = secret.GetCurrentVersionId();
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "Secret describe secret failed, message: " + exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SecretsManager::DeleteSecretResponse SecretsManagerService::DeleteSecret(const Dto::SecretsManager::DeleteSecretRequest &request) const {
        Monitoring::MetricServiceTimer measure(SECRETSMANAGER_SERVICE_TIMER, "action", "delete_secrets");
        Monitoring::MetricService::instance().IncrementCounter(SECRETSMANAGER_SERVICE_TIMER, "action", "delete_secrets");
        log_trace << "Delete secret request: " << request.ToString();

        // Check whether we have a name of ARN
        std::string arn = Core::StringUtils::Contains(request.secretId, ":") ? request.secretId : Core::AwsUtils::CreateSecretArn(request.region, _accountId, request.secretId);

        // Check bucket existence
        if (!_secretsManagerDatabase.SecretExistsByArn(arn)) {
            log_warning << "Secret does not exist, name: " << arn;
            throw Core::ServiceException("Secret does not exist, arn: " + arn);
        }

        try {

            // Get an object from the database
            const Database::Entity::SecretsManager::Secret secret = _secretsManagerDatabase.GetSecretByArn(arn);

            // Delete from database
            _secretsManagerDatabase.DeleteSecret(secret);
            log_debug << "Database secret deleted, region: " << request.region << " name: " << arn;
            Dto::SecretsManager::DeleteSecretResponse response;
            response.region = request.region;
            response.name = secret.name;
            response.arn = secret.arn;
            response.deletionDate = static_cast<double>(Core::DateTimeUtils::UnixTimestampNow() - secret.lastRotatedDate);
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "SecretManager delete secret failed, message: " + exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    void SecretsManagerService::CreateSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) const {

        // Sent create request to lambda function
        Dto::SecretsManager::LambdaInvocationRequest invocationRequest;
        invocationRequest.secretId = secret.secretId;
        invocationRequest.region = secret.region;
        invocationRequest.clientRequestToken = clientRequestToken;
        invocationRequest.requestId = clientRequestToken;
        invocationRequest.step = TaskTypeToString(createSecret);
        SendLambdaInvocationRequest(lambda, invocationRequest.ToJson());
    }

    void SecretsManagerService::SetSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) const {

        // Sent create request to lambda function
        Dto::SecretsManager::LambdaInvocationRequest invocationRequest;
        invocationRequest.secretId = secret.secretId;
        invocationRequest.region = secret.region;
        invocationRequest.clientRequestToken = clientRequestToken;
        invocationRequest.requestId = clientRequestToken;
        invocationRequest.step = TaskTypeToString(setSecret);
        SendLambdaInvocationRequest(lambda, invocationRequest.ToJson());
    }

    void SecretsManagerService::TestSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) const {

        // Sent create request to lambda function
        Dto::SecretsManager::LambdaInvocationRequest invocationRequest;
        invocationRequest.secretId = secret.secretId;
        invocationRequest.region = secret.region;
        invocationRequest.clientRequestToken = clientRequestToken;
        invocationRequest.requestId = clientRequestToken;
        invocationRequest.step = TaskTypeToString(testSecret);
        SendLambdaInvocationRequest(lambda, invocationRequest.ToJson());
    }

    void SecretsManagerService::FinishSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) const {

        // Sent create request to lambda function
        Dto::SecretsManager::LambdaInvocationRequest invocationRequest;
        invocationRequest.secretId = secret.secretId;
        invocationRequest.region = secret.region;
        invocationRequest.clientRequestToken = clientRequestToken;
        invocationRequest.requestId = clientRequestToken;
        invocationRequest.step = TaskTypeToString(finishSecret);
        SendLambdaInvocationRequest(lambda, invocationRequest.ToJson());
    }

    void SecretsManagerService::SendLambdaInvocationRequest(const Database::Entity::Lambda::Lambda &lambda, const std::string &body) const {
        log_debug << "Invoke lambda function request, function: " << lambda.function << " body: " << body;

        const auto region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");
        _lambdaService.InvokeLambdaFunction(region, lambda.function, body, {}, false);
        log_debug << "Lambda send invocation request finished, function: " << lambda.function;
    }

    void SecretsManagerService::CreateKmsKey(Database::Entity::SecretsManager::Secret &secret) const {
        Dto::KMS::CreateKeyRequest kmsRequest;
        kmsRequest.description = "KMS key for secret " + secret.name;
        kmsRequest.keySpec = Dto::KMS::KeySpec::SYMMETRIC_DEFAULT;
        kmsRequest.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;
        Dto::KMS::CreateKeyResponse kmsResponse = _kmsService.CreateKey(kmsRequest);
        _kmsService.WaitForAesKey(kmsResponse.key.keyId, 5);
        secret.kmsKeyId = kmsResponse.key.keyId;
    }

    void SecretsManagerService::EncryptSecret(Database::Entity::SecretsManager::SecretVersion &version, const std::string &kmsKeyId, const std::string &secretString) const {
        Dto::KMS::EncryptRequest encryptRequest;
        encryptRequest.keyId = kmsKeyId;
        encryptRequest.plainText = Core::Crypto::Base64Encode(secretString);
        const Dto::KMS::EncryptResponse encryptResponse = _kmsService.Encrypt(encryptRequest);
        version.secretString = encryptResponse.ciphertext;
    }

    std::string SecretsManagerService::GetSecretString(Database::Entity::SecretsManager::Secret &secret) const {
        const std::string versionId = secret.GetCurrentVersionId();
        const Database::Entity::SecretsManager::SecretVersion version = secret.versions[versionId];

        Dto::KMS::DecryptRequest decryptRequest;
        decryptRequest.keyId = secret.kmsKeyId;
        decryptRequest.ciphertext = version.secretString;
        const Dto::KMS::DecryptResponse decryptResponse = _kmsService.Decrypt(decryptRequest);
        return Core::Crypto::Base64Decode(decryptResponse.plaintext);
    }
}// namespace AwsMock::Service
