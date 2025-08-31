//
// Created by vogje01 on 08/04/2024.
//

#include <awsmock/service/secretsmanager/SecretsManagerService.h>

namespace AwsMock::Service {

    SecretsManagerService::SecretsManagerService(boost::asio::io_context &ioc) : _secretsManagerDatabase(Database::SecretsManagerDatabase::instance()), _lambdaDatabase(Database::LambdaDatabase::instance()), _lambdaService(ioc) {

        // Initialize environment
        _accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
    }

    Dto::SecretsManager::CreateSecretResponse SecretsManagerService::CreateSecret(const Dto::SecretsManager::CreateSecretRequest &request) const {
        Monitoring::MonitoringTimer measure(SECRETSMANAGER_SERVICE_TIMER, "action", "create_secret");
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
            secret.createdDate = system_clock::now();
            secret.description = request.description;
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
        Monitoring::MonitoringTimer measure(SECRETSMANAGER_SERVICE_TIMER, "action", "describe_secret");
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
            response.deletedDate = Core::DateTimeUtils::UnixTimestamp(secret.deletedDate);
            response.description = secret.description;
            response.kmsKeyId = secret.kmsKeyId;
            response.rotationEnabled = secret.rotationEnabled;
            response.rotationLambdaARN = secret.rotationLambdaARN;

            // Version stages
            for (const auto &[fst, snd]: secret.versions) {
                response.versionIdsToStages.versions[fst] = snd.stages;
            }
            log_debug << "Database secret described, secretId: " << request.secretId;
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "Secret describe secret failed, message: " + exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SecretsManager::GetSecretValueResponse SecretsManagerService::GetSecretValue(const Dto::SecretsManager::GetSecretValueRequest &request) const {
        Monitoring::MonitoringTimer measure(SECRETSMANAGER_SERVICE_TIMER, "action", "get_secret_value");
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

            std::string versionId;
            if (!request.versionId.empty()) {
                versionId = request.versionId;
            } else if (!request.versionStage.empty()) {
                versionId = secret.GetVersionIdByStage(request.versionStage);
            } else {
                versionId = secret.GetVersionIdByStage(Dto::SecretsManager::VersionStateToString(Dto::SecretsManager::VersionStateType::AWSCURRENT));
            }
            Database::Entity::SecretsManager::SecretVersion version = secret.versions[versionId];

            // Convert to DTO
            response.name = secret.name;
            response.arn = secret.arn;
            response.versionId = versionId;
            response.createdDate = Core::DateTimeUtils::UnixTimestamp(secret.createdDate);
            response.versionStages = secret.versions[versionId].stages;

            if (!secret.kmsKeyId.empty()) {
                response.secretString = DecryptSecret(version, secret.kmsKeyId, version.secretString);
            } else if (!version.secretString.empty()) {
                response.secretBinary = Core::Crypto::Base64Decode(version.secretBinary);
            }
            log_debug << "Get secret value, secretId: " << request.secretId;

            // Update database
            secret.lastAccessedDate = system_clock::now();
            secret = _secretsManagerDatabase.UpdateSecret(secret);
            log_trace << "Secret updated, secretId: " << secret.secretId;

            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "Secret describe secret failed, message: " + exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SecretsManager::PutSecretValueResponse SecretsManagerService::PutSecretValue(const Dto::SecretsManager::PutSecretValueRequest &request) const {
        Monitoring::MonitoringTimer measure(SECRETSMANAGER_SERVICE_TIMER, "action", "put_secret_value");
        Monitoring::MetricService::instance().IncrementCounter(SECRETSMANAGER_SERVICE_TIMER, "action", "put_secret_value");
        log_trace << "Put secret value request: " << request.ToString();

        // Check whether we have a name of ARN
        std::string arn = Core::StringUtils::Contains(request.secretId, ":") ? request.secretId : Core::AwsUtils::CreateSecretArn(request.region, _accountId, request.secretId);

        // Check bucket existence
        if (!_secretsManagerDatabase.SecretExistsByArn(arn)) {
            log_warning << "Secret does not exist, arn: " << arn;
            throw Core::ServiceException("Secret does not exist, arn: " + arn);
        }

        try {
            Dto::SecretsManager::PutSecretValueResponse response;

            // Get the object from the database
            Database::Entity::SecretsManager::Secret secret = _secretsManagerDatabase.GetSecretByArn(arn);

            std::string currentVersionId = secret.GetCurrentVersionId();
            secret.versions[currentVersionId].stages.clear();
            secret.versions[currentVersionId].stages.emplace_back(Dto::SecretsManager::VersionStateToString(Dto::SecretsManager::VersionStateType::AWSPREVIOUS));

            Database::Entity::SecretsManager::SecretVersion newVersion;
            newVersion.stages.emplace_back(Dto::SecretsManager::VersionStateToString(Dto::SecretsManager::VersionStateType::AWSCURRENT));

            if (!secret.kmsKeyId.empty()) {
                Dto::KMS::EncryptRequest encryptRequest;
                encryptRequest.keyId = secret.kmsKeyId;
                encryptRequest.plaintext = request.secretString;
                Dto::KMS::EncryptResponse kmsResponse = _kmsService.Encrypt(encryptRequest);
                newVersion.secretString = Core::Crypto::Base64Decode(kmsResponse.ciphertext);
            } else if (!request.secretBinary.empty()) {
                Dto::KMS::EncryptRequest encryptRequest;
                encryptRequest.keyId = secret.kmsKeyId;
                encryptRequest.plaintext = newVersion.secretBinary;
                Dto::KMS::EncryptResponse kmsResponse = _kmsService.Encrypt(encryptRequest);
                newVersion.secretBinary = kmsResponse.ciphertext;
            } else {
                log_warning << "Neither string nor binary, secretId: " << request.secretId;
            }
            secret.versions[request.clientRequestToken] = newVersion;

            // Convert to DTO
            response.arn = secret.arn;
            response.name = secret.name;
            response.versionId = request.clientRequestToken;
            response.versionStages.emplace_back(Dto::SecretsManager::VersionStateToString(Dto::SecretsManager::VersionStateType::AWSCURRENT));

            log_debug << "Put secret value, secretId: " << request.secretId;

            // Update database
            secret.lastAccessedDate = system_clock::now();
            secret = _secretsManagerDatabase.UpdateSecret(secret);
            log_trace << "Secret updated, secretId: " << secret.oid;

            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "Secret describe secret failed, message: " + exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SecretsManager::ListSecretsResponse SecretsManagerService::ListSecrets(const Dto::SecretsManager::ListSecretsRequest &request) const {
        Monitoring::MonitoringTimer measure(SECRETSMANAGER_SERVICE_TIMER, "action", "list_secrets");
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

    Dto::SecretsManager::ListSecretVersionIdsResponse SecretsManagerService::ListSecretVersionIds(const Dto::SecretsManager::ListSecretVersionIdsRequest &request) const {
        Monitoring::MonitoringTimer measure(SECRETSMANAGER_SERVICE_TIMER, "action", "list_secret_versions");
        Monitoring::MetricService::instance().IncrementCounter(SECRETSMANAGER_SERVICE_TIMER, "action", "list_secret_versions");
        log_trace << "List secret version Ids request: " << request;

        // Get the arn
        std::string arn = Core::StringUtils::Contains(request.secretId, ":") ? request.secretId : Core::AwsUtils::CreateSecretArn(request.region, _accountId, request.secretId);

        // Check bucket existence
        if (!_secretsManagerDatabase.SecretExistsByArn(arn)) {
            log_warning << "Secret does not exist, secretId: " << request.secretId;
            throw Core::ServiceException("Secret does not exist, secretId: " + request.secretId);
        }
        try {

            Dto::SecretsManager::ListSecretVersionIdsResponse response;

            // Get the object from the database
            Database::Entity::SecretsManager::Secret secret = _secretsManagerDatabase.GetSecretByArn(arn);

            // Get version
            for (const auto &[fst, snd]: secret.versions) {
                Dto::SecretsManager::SecretVersion secretVersion;
                secretVersion.versionId = fst;
                secretVersion.created = snd.created;
                secretVersion.lastAccessed = snd.lastAccessed;
                secretVersion.kmsKeyIds.emplace_back(secret.kmsKeyId);
                secretVersion.versionStages = snd.stages;

                response.versions.emplace_back(secretVersion);
            }

            response.arn = secret.arn;
            response.name = secret.name;

            // Convert to DTO
            log_debug << "Database list secret versions, region: " << request.region;
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "List secret versions failed, message: " + exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SecretsManager::ListSecretCountersResponse SecretsManagerService::ListSecretCounters(const Dto::SecretsManager::ListSecretCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(SECRETSMANAGER_SERVICE_TIMER, "action", "list_secrets");
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
        Monitoring::MonitoringTimer measure(SECRETSMANAGER_SERVICE_TIMER, "action", "list_secret_versions");
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
        Monitoring::MonitoringTimer measure(SECRETSMANAGER_SERVICE_TIMER, "action", "get_secret");
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

            // Convert to DTO
            log_debug << "Get secret details, secretId: " << request.secretId;
            return Dto::SecretsManager::Mapper::map(secret, GetSecretString(secret));

        } catch (Core::DatabaseException &exc) {
            log_error << "Get secret details failed, message: " + exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SecretsManager::UpdateSecretResponse SecretsManagerService::UpdateSecret(const Dto::SecretsManager::UpdateSecretRequest &request) const {
        Monitoring::MonitoringTimer measure(SECRETSMANAGER_SERVICE_TIMER, "action", "update_secrets");
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
            if (!secret.kmsKeyId.empty()) {
                Dto::KMS::EncryptRequest encryptRequest;
                encryptRequest.keyId = secret.kmsKeyId;
                encryptRequest.plaintext = request.secretString;
                Dto::KMS::EncryptResponse kmsResponse = _kmsService.Encrypt(encryptRequest);
                version.secretString = kmsResponse.ciphertext;
            } else {
                version.secretString = request.secretString;
            }
            version.secretBinary = request.secretBinary;
            version.stages.emplace_back(Dto::SecretsManager::VersionStateToString(Dto::SecretsManager::VersionStateType::AWSCURRENT));

            secret.versions[versionId] = version;
            secret.description = request.description;
            secret.lastChangedDate = system_clock::now();
            secret.ResetVersions(versionId);
            secret = _secretsManagerDatabase.UpdateSecret(secret);

            // Convert to DTO
            log_debug << "Database secret updated, secretId: " << request.secretId;
            Dto::SecretsManager::UpdateSecretResponse response;
            response.region = secret.region;
            response.name = secret.name;
            response.arn = secret.arn;
            response.versionId = versionId;
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "Secret update failed, message: " + exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SecretsManager::UpdateSecretDetailsResponse SecretsManagerService::UpdateSecretDetails(const Dto::SecretsManager::UpdateSecretDetailsRequest &request) const {
        Monitoring::MonitoringTimer measure(SECRETSMANAGER_SERVICE_TIMER, "action", "update_secrets");
        Monitoring::MetricService::instance().IncrementCounter(SECRETSMANAGER_SERVICE_TIMER, "action", "update_secrets");
        log_trace << "Update secret details request: " << request;

        // Check bucket existence
        if (!_secretsManagerDatabase.SecretExists(request.secretDetails.secretId)) {
            log_warning << "Secret does not exist, secretId: " << request.secretDetails.secretId;
            throw Core::ServiceException("Secret does not exist, secretId: " + request.secretDetails.secretId);
        }

        try {

            // Get the object from the database
            Database::Entity::SecretsManager::Secret secret = _secretsManagerDatabase.GetSecretBySecretId(request.secretDetails.secretId);

            // Updates are only possible on certain fields
            secret.rotationRules = Dto::SecretsManager::Mapper::map(request.secretDetails.rotationRules);
            secret.rotationLambdaARN = request.secretDetails.rotationLambdaARN;
            if (!request.secretDetails.secretString.empty()) {
                Database::Entity::SecretsManager::SecretVersion version = secret.GetVersion(secret.GetCurrentVersionId());
                EncryptSecret(version, secret.kmsKeyId, request.secretDetails.secretString);
                secret.versions[secret.GetCurrentVersionId()] = version;
            }
            secret = _secretsManagerDatabase.UpdateSecret(secret);

            // Convert to DTO
            log_debug << "Database secret updated, secretId: " << request.secretDetails.secretId;
            return Dto::SecretsManager::Mapper::mapUpdate(secret, GetSecretString(secret));

        } catch (Core::DatabaseException &exc) {
            log_error << "Secret describe secret failed, message: " + exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::SecretsManager::RotateSecretResponse SecretsManagerService::RotateSecret(const Dto::SecretsManager::RotateSecretRequest &request) const {
        Monitoring::MonitoringTimer measure(SECRETSMANAGER_SERVICE_TIMER, "action", "rotate_secrets");
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
            secret.lastRotatedDate = system_clock::now();
            secret.rotationLambdaARN = request.rotationLambdaARN;
            secret.rotationEnabled = true;

            // Create a copy of the current
            std::string versionId = secret.GetCurrentVersionId();
            Database::Entity::SecretsManager::SecretVersion version = secret.versions[versionId];
            version.stages.emplace_back(Dto::SecretsManager::VersionStateToString(Dto::SecretsManager::VersionStateType::AWSPENDING));
            secret.versions[request.clientRequestToken] = version;
            secret = _secretsManagerDatabase.UpdateSecret(secret);

            if (request.rotateImmediately) {
                if (!secret.rotationLambdaARN.empty()) {

                    SecretRotation secretRotation;
                    boost::thread t(boost::ref(secretRotation), secret, request.clientRequestToken);
                    t.detach();
                    log_debug << "Secret rotation started, secretId: " << request.secretId;
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
        Monitoring::MonitoringTimer measure(SECRETSMANAGER_SERVICE_TIMER, "action", "delete_secrets");
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
            response.deletionDate = system_clock::now();
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "SecretManager delete secret failed, message: " + exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    void SecretsManagerService::CreateSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) {

        // Sent create request to lambda function
        Dto::SecretsManager::LambdaInvocationRequest invocationRequest;
        invocationRequest.secretId = secret.secretId;
        invocationRequest.region = secret.region;
        invocationRequest.clientRequestToken = clientRequestToken;
        invocationRequest.requestId = clientRequestToken;
        invocationRequest.step = TaskTypeToString(createSecret);
        SendLambdaInvocationRequest(lambda, invocationRequest.ToJson());
    }

    void SecretsManagerService::SetSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) {

        // Sent create request to lambda function
        Dto::SecretsManager::LambdaInvocationRequest invocationRequest;
        invocationRequest.secretId = secret.secretId;
        invocationRequest.region = secret.region;
        invocationRequest.clientRequestToken = clientRequestToken;
        invocationRequest.requestId = clientRequestToken;
        invocationRequest.step = TaskTypeToString(setSecret);
        SendLambdaInvocationRequest(lambda, invocationRequest.ToJson());
    }

    void SecretsManagerService::TestSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) {

        // Sent create request to lambda function
        Dto::SecretsManager::LambdaInvocationRequest invocationRequest;
        invocationRequest.secretId = secret.secretId;
        invocationRequest.region = secret.region;
        invocationRequest.clientRequestToken = clientRequestToken;
        invocationRequest.requestId = clientRequestToken;
        invocationRequest.step = TaskTypeToString(testSecret);
        SendLambdaInvocationRequest(lambda, invocationRequest.ToJson());
    }

    void SecretsManagerService::FinishSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) {

        // Sent create request to lambda function
        Dto::SecretsManager::LambdaInvocationRequest invocationRequest;
        invocationRequest.secretId = secret.secretId;
        invocationRequest.region = secret.region;
        invocationRequest.clientRequestToken = clientRequestToken;
        invocationRequest.requestId = clientRequestToken;
        invocationRequest.step = TaskTypeToString(finishSecret);
        SendLambdaInvocationRequest(lambda, invocationRequest.ToJson());
    }

    void SecretsManagerService::SendLambdaInvocationRequest(const Database::Entity::Lambda::Lambda &lambda, const std::string &body) {
        log_debug << "Invoke lambda function request, function: " << lambda.function << " body: " << body;

        const auto region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");
        std::string payload = body;
        Dto::Lambda::LambdaResult result = _lambdaService.InvokeLambdaFunction(region, lambda.function, payload, Dto::Lambda::LambdaInvocationType::EVENT);
        log_debug << "Lambda send invocation request finished, function: " << lambda.function;
    }

    void SecretsManagerService::CreateKmsKey(Database::Entity::SecretsManager::Secret &secret) const {
        Dto::KMS::CreateKeyRequest kmsRequest;
        kmsRequest.description = "KMS key for secret " + secret.name;
        kmsRequest.keySpec = Dto::KMS::KeySpec::SYMMETRIC_DEFAULT;
        kmsRequest.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;
        const Dto::KMS::CreateKeyResponse kmsResponse = _kmsService.CreateKey(kmsRequest);
        _kmsService.WaitForAesKey(kmsResponse.key.keyId, 5);
        secret.kmsKeyId = kmsResponse.key.keyId;
    }

    void SecretsManagerService::EncryptSecret(Database::Entity::SecretsManager::SecretVersion &version, const std::string &kmsKeyId, const std::string &secretString) const {
        Dto::KMS::EncryptRequest encryptRequest;
        encryptRequest.keyId = kmsKeyId;
        encryptRequest.plaintext = Core::Crypto::Base64Encode(secretString);
        const Dto::KMS::EncryptResponse encryptResponse = _kmsService.Encrypt(encryptRequest);
        version.secretString = encryptResponse.ciphertext;
    }

    std::string SecretsManagerService::DecryptSecret(Database::Entity::SecretsManager::SecretVersion &version, const std::string &kmsKeyId, const std::string &secretString) const {
        Dto::KMS::DecryptRequest decryptRequest;
        decryptRequest.keyId = kmsKeyId;
        decryptRequest.ciphertext = secretString;
        const Dto::KMS::DecryptResponse decryptResponse = _kmsService.Decrypt(decryptRequest);
        log_trace << "Decrypt secret, secretString: " << Core::Crypto::Base64Decode(decryptResponse.plaintext);
        return Core::Crypto::Base64Decode(decryptResponse.plaintext);
    }

    std::string SecretsManagerService::GetSecretString(Database::Entity::SecretsManager::Secret &secret) const {
        const std::string versionId = secret.GetCurrentVersionId();
        const Database::Entity::SecretsManager::SecretVersion version = secret.versions[versionId];

        Dto::KMS::DecryptRequest decryptRequest;
        decryptRequest.keyId = secret.kmsKeyId;
        decryptRequest.ciphertext = version.secretString;
        const Dto::KMS::DecryptResponse decryptResponse = _kmsService.Decrypt(decryptRequest);
        log_trace << "GetSecretString secret, secretString: " << Core::Crypto::Base64Decode(decryptResponse.plaintext);
        return Core::Crypto::Base64Decode(decryptResponse.plaintext);
    }
}// namespace AwsMock::Service
