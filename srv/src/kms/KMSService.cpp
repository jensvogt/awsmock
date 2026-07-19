//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/kms/KMSService.h>

#include <awsmock/core/scheduler/Scheduler.h>

namespace Awsmock::Service {

    KMSService::KMSService() {

        // Initialize environment
        _accountId = Core::Configuration::instance().get<std::string>("awsmock.access.account-id");
    }

    Dto::KMS::ListKeysResponse KMSService::ListKeys(const Dto::KMS::ListKeysRequest &request) const {
        Monitoring::MonitoringTimer measure(KMS_SERVICE_TIMER, KMS_SERVICE_COUNTER, "method", "list_keys");
        log_trace << "List keys request: " << request;

        try {

            const Database::Entity::KMS::KeyList keyList = _kmsDatabase->listKeys({}, {}, 0, 0, {});
            Dto::KMS::ListKeysResponse listKeysResponse;
            for (const auto &k: keyList) {
                Dto::KMS::Key key;
                key.keyId = k.keyId;
                key.arn = k.arn;
                listKeysResponse.keys.emplace_back(key);
            }
            log_debug << "List all keys, size: " << keyList.size();

            return listKeysResponse;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::KMS::ListKeyCountersResponse KMSService::ListKeyCounters(const Dto::KMS::ListKeyCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(KMS_SERVICE_TIMER, KMS_SERVICE_COUNTER, "method", "list_key_counters");
        log_trace << "List key counters request: " << request;

        try {
            Dto::KMS::ListKeyCountersResponse listKeyCountersResponse;
            const Database::Entity::KMS::KeyList keyList = _kmsDatabase->listKeys(request.region, request.prefix, request.pageSize, request.pageIndex, Dto::Common::SortColumnMapper::map(request.sortColumns));
            listKeyCountersResponse.total = _kmsDatabase->countKeys();

            for (const auto &k: keyList) {
                Dto::KMS::KeyCounter key;
                key.region = request.region;
                key.keyId = k.keyId;
                key.arn = k.arn;
                key.keyUsage = Dto::KMS::KeyUsageFromString(k.keyUsage);
                key.keySpec = Dto::KMS::KeySpecFromString(k.keySpec);
                key.keyState = Dto::KMS::KeyStateFromString(k.keyState);
                key.created = k.created;
                key.modified = k.modified;
                listKeyCountersResponse.keyCounters.emplace_back(key);
            }
            log_debug << "List all key counters, size: " << keyList.size();

            return listKeyCountersResponse;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::KMS::ListKeyArnsResponse KMSService::ListKeyArns() const {
        Monitoring::MonitoringTimer measure(KMS_SERVICE_TIMER, KMS_SERVICE_COUNTER, "method", "list_key_arns");
        log_trace << "List key ARNs request";

        try {
            Dto::KMS::ListKeyArnsResponse listKeyArnsResponse;
            const Database::Entity::KMS::KeyList keyList = _kmsDatabase->listKeys({}, {}, 0, 0, {});

            for (const auto &k: keyList) {
                listKeyArnsResponse.keyArns.emplace_back(k.arn);
            }
            log_debug << "List all key counters, size: " << keyList.size();

            return listKeyArnsResponse;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::KMS::CreateKeyResponse KMSService::CreateKey(const Dto::KMS::CreateKeyRequest &request) const {
        Monitoring::MonitoringTimer measure(KMS_SERVICE_TIMER, KMS_SERVICE_COUNTER, "method", "create_key");
        log_trace << "Create key request: " << request;

        try {
            // Update database
            std::string keyId = Core::StringUtils::CreateRandomUuid();

            std::string arn = Core::AwsUtils::CreateKMSKeyArn(request.region, _accountId, keyId);
            Database::Entity::KMS::Key keyEntity;
            keyEntity.region = request.region;
            keyEntity.keyId = keyId;
            keyEntity.keyUsage = KeyUsageToString(request.keyUsage);
            keyEntity.keySpec = KeySpecToString(request.keySpec);
            keyEntity.arn = arn;
            keyEntity.description = request.description;
            keyEntity.tags = request.tags;

            // Store in a database
            keyEntity = _kmsDatabase->createKey(keyEntity);
            log_trace << "KMS keyEntity lastStarted: " << keyEntity;

            // Create key material asynchronously via scheduler; callers use WaitForAesKey/WaitForRsaKey to synchronize
            const std::string taskName = "create-kms-key-" + keyId;
            auto future = Core::Scheduler::instance().AddWaitableOneTimeTask(taskName, [keyId]() {
                KMSCreator{}.CreateKmsKey(keyId);
            });
            std::lock_guard lock(_futureMutex);
            _keyCreationFutures[keyId] = std::move(future);
            log_debug << "KMS key creation started asynchronously, keyId: " << keyId;

            Dto::KMS::Key key;
            key.keyId = keyEntity.keyId;
            key.arn = keyEntity.arn;
            key.keySpec = Dto::KMS::KeySpecFromString(keyEntity.keySpec);
            key.keyUsage = Dto::KMS::KeyUsageFromString(keyEntity.keyUsage);
            key.keyState = Dto::KMS::KeyStateFromString(keyEntity.keyState);
            key.description = keyEntity.description;
            Dto::KMS::CreateKeyResponse response;
            response.keyMetadata = key;
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "KMS create key failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    void KMSService::WaitForKey(const std::string &keyId, const int maxSeconds) const {
        std::shared_future<void> future;
        {
            std::lock_guard lock(_futureMutex);
            if (const auto it = _keyCreationFutures.find(keyId); it != _keyCreationFutures.end()) {
                future = it->second;
                _keyCreationFutures.erase(it);
            }
        }
        if (future.valid()) {
            if (future.wait_for(std::chrono::seconds(maxSeconds)) == std::future_status::timeout) {
                log_warning << "Timed out waiting for KMS key material, keyId: " << keyId;
            }
        }
    }

    void KMSService::WaitForRsaKey(const std::string &keyId, const int maxSeconds) const {
        WaitForKey(keyId, maxSeconds);
    }

    void KMSService::WaitForAesKey(const std::string &keyId, const int maxSeconds) const {
        WaitForKey(keyId, maxSeconds);
    }

    Dto::KMS::ScheduledKeyDeletionResponse KMSService::ScheduleKeyDeletion(const Dto::KMS::ScheduleKeyDeletionRequest &request) const {
        Monitoring::MonitoringTimer measure(KMS_SERVICE_TIMER, KMS_SERVICE_COUNTER, "method", "schedule_key_deletion");
        log_trace << "Schedule key deletion request: " << request;

        if (!_kmsDatabase->keyExists(request.keyId)) {
            log_error << "Key not found, keyId: " << request.keyId;
            throw Core::ServiceException("Key not found, keyId: " + request.keyId);
        }

        try {

            // Get the key
            Database::Entity::KMS::Key key = _kmsDatabase->getKeyByKeyId(request.keyId);

            key.pendingWindowInDays = request.pendingWindowInDays;
            key.scheduledDeletion = system_clock::now() + std::chrono::days(request.pendingWindowInDays);
            key.keyState = Dto::KMS::KeyStateToString(Dto::KMS::KeyState::PENDING_DELETION);

            // Store in a database
            key = _kmsDatabase->updateKey(key);
            log_trace << "KMS key updated: " << key;
            Dto::KMS::ScheduledKeyDeletionResponse response;
            response.keyId = request.keyId;
            response.keyState = key.keyState;
            response.deletionDate = Core::DateTimeUtils::UnixTimestamp(key.scheduledDeletion);
            response.pendingWindowInDays = request.pendingWindowInDays;
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "KMS create key failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::KMS::DescribeKeyResponse KMSService::DescribeKey(const Dto::KMS::DescribeKeyRequest &request) const {
        Monitoring::MonitoringTimer measure(KMS_SERVICE_TIMER, KMS_SERVICE_COUNTER, "method", "describe_key");
        log_trace << "Create key request: " << request;

        if (!_kmsDatabase->keyExists(request.keyId)) {
            log_error << "Key not found, keyId: " << request.keyId;
            throw Core::ServiceException("Key not found, keyId: " + request.keyId);
        }

        try {

            Database::Entity::KMS::Key keyEntity = _kmsDatabase->getKeyByKeyId(request.keyId);
            log_trace << "KMS key entity received: " << keyEntity.ToString();

            Dto::KMS::Key key;
            key.keyId = keyEntity.keyId;
            key.arn = keyEntity.arn;
            key.keySpec = Dto::KMS::KeySpecFromString(keyEntity.keySpec);
            key.keyUsage = Dto::KMS::KeyUsageFromString(keyEntity.keyUsage);
            key.keyState = Dto::KMS::KeyStateFromString(keyEntity.keyState);
            key.description = keyEntity.description;
            key.creationDate = Core::DateTimeUtils::UnixTimestamp(keyEntity.created);
            key.deletionDate = Core::DateTimeUtils::UnixTimestamp(keyEntity.scheduledDeletion);
            key.enabled = Core::StringUtils::Equals(keyEntity.keyState, Dto::KMS::KeyStateToString(Dto::KMS::KeyState::ENABLED));
            Dto::KMS::DescribeKeyResponse response;
            response.requestId = request.requestId;
            response.region = request.region;
            response.user = request.user;
            response.keyMetadata = key;
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "KMS describe key failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::KMS::GetKeyCounterResponse KMSService::GetKeyCounter(const Dto::KMS::GetKeyCounterRequest &request) const {
        Monitoring::MonitoringTimer measure(KMS_SERVICE_TIMER, KMS_SERVICE_COUNTER, "method", "get_key_counter");
        log_trace << "Get key request: " << request;

        if (!_kmsDatabase->keyExists(request.keyId)) {
            log_error << "Key not found, keyId: " << request.keyId;
            throw Core::ServiceException("Key not found, keyId: " + request.keyId);
        }

        try {

            Database::Entity::KMS::Key keyEntity = _kmsDatabase->getKeyByKeyId(request.keyId);
            log_trace << "KMS key entity received: " << keyEntity.ToString();

            // TODO: use mapper
            Dto::KMS::KeyCounter key;
            key.keyId = keyEntity.keyId;
            key.arn = keyEntity.arn;
            key.keySpec = Dto::KMS::KeySpecFromString(keyEntity.keySpec);
            key.keyUsage = Dto::KMS::KeyUsageFromString(keyEntity.keyUsage);
            key.keyState = Dto::KMS::KeyStateFromString(keyEntity.keyState);
            key.origin = Dto::KMS::OriginFromString(keyEntity.origin);
            key.description = keyEntity.description;
            key.scheduledDeletion = keyEntity.scheduledDeletion;
            key.enabled = Core::StringUtils::Equals(keyEntity.keyState, Dto::KMS::KeyStateToString(Dto::KMS::KeyState::ENABLED));
            key.created = keyEntity.created;
            key.modified = keyEntity.modified;

            // Create response
            Dto::KMS::GetKeyCounterResponse response;
            response.requestId = request.requestId;
            response.region = request.region;
            response.user = request.user;
            response.key = key;
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "KMS get key failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    void KMSService::UpdateKeyCounter(const Dto::KMS::UpdateKeyCounterRequest &request) const {
        Monitoring::MonitoringTimer measure(KMS_SERVICE_TIMER, KMS_SERVICE_COUNTER, "method", "update_key_counter");
        log_trace << "Update key request: " << request;

        if (!_kmsDatabase->keyExists(request.keyCounter.keyId)) {
            log_error << "Key not found, keyId: " << request.keyCounter.keyId;
            throw Core::ServiceException("Key not found, keyId: " + request.keyCounter.keyId);
        }

        try {

            Database::Entity::KMS::Key keyEntity = _kmsDatabase->getKeyByKeyId(request.keyCounter.keyId);
            log_trace << "KMS key entity received, key: " << keyEntity;

            keyEntity.keySpec = Dto::KMS::KeySpecToString(request.keyCounter.keySpec);
            keyEntity.keyUsage = Dto::KMS::KeyUsageToString(request.keyCounter.keyUsage);
            keyEntity.keyState = Dto::KMS::KeyStateToString(request.keyCounter.keyState);
            keyEntity.origin = Dto::KMS::OriginToString(request.keyCounter.origin);
            keyEntity.description = request.keyCounter.description;
            keyEntity = _kmsDatabase->updateKey(keyEntity);
            log_trace << "KMS key entity updated, key: " << keyEntity;

        } catch (Core::DatabaseException &exc) {
            log_error << "KMS get key failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::KMS::EncryptResponse KMSService::Encrypt(const Dto::KMS::EncryptRequest &request) const {
        Monitoring::MonitoringTimer measure(KMS_SERVICE_TIMER, KMS_SERVICE_COUNTER, "method", "encrypt");
        log_trace << "Encrypt plaintext request: " << request;

        if (!_kmsDatabase->keyExists(request.keyId)) {
            log_error << "Key not found, keyId: " << request.keyId;
            throw Core::ServiceException("Key not found, keyId: " + request.keyId);
        }

        try {

            const Database::Entity::KMS::Key keyEntity = _kmsDatabase->getKeyByKeyId(request.keyId);
            log_trace << "KMS key entity received: " << keyEntity.ToString();

            const std::string cipherText = EncryptPlaintext(keyEntity, request.plaintext);

            Dto::KMS::EncryptResponse response;
            response.region = request.region;
            response.keyId = keyEntity.keyId;
            response.encryptionAlgorithm = request.encryptionAlgorithm;
            response.ciphertext = cipherText;
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "KMS encrypt failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::KMS::GenerateDataKeyResponse KMSService::GenerateDataKey(const Dto::KMS::GenerateDataKeyRequest &request) const {
        Monitoring::MonitoringTimer measure(KMS_SERVICE_TIMER, KMS_SERVICE_COUNTER, "method", "generate_data_key");
        log_trace << "GenerateDataKey request, keyId: " << request.keyId;

        if (!_kmsDatabase->keyExists(request.keyId)) {
            log_error << "Key not found, keyId: " << request.keyId;
            throw Core::ServiceException("Key not found, keyId: " + request.keyId);
        }

        try {
            const Database::Entity::KMS::Key keyEntity = _kmsDatabase->getKeyByKeyId(request.keyId);

            // Generate random plaintext data key (numberOfBytes bytes)
            const int keyLen = request.numberOfBytes > 0 ? request.numberOfBytes : 32;
            unsigned char plainBytes[64] = {};
            unsigned char ivBytes[32] = {};
            Core::Crypto::CreateAes256Key(plainBytes, ivBytes);

            const std::string plaintextStr(reinterpret_cast<char *>(plainBytes), keyLen);
            const std::string plaintextB64 = Core::Crypto::Base64Encode(plaintextStr);

            // Encrypt the plaintext data key under the KMS key
            const std::string ciphertextB64 = EncryptPlaintext(keyEntity, plaintextB64);

            Dto::KMS::GenerateDataKeyResponse response;
            response.keyId = keyEntity.arn;
            response.plaintext = plaintextB64;
            response.ciphertextBlob = ciphertextB64;
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "GenerateDataKey failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::KMS::DecryptResponse KMSService::Decrypt(const Dto::KMS::DecryptRequest &request) const {
        Monitoring::MonitoringTimer measure(KMS_SERVICE_TIMER, KMS_SERVICE_COUNTER, "method", "decrypt");
        log_trace << "Decrypt plaintext request, keyId: " << request.keyId;

        if (!_kmsDatabase->keyExists(request.keyId)) {
            log_error << "Key not found, keyId: " << request.keyId;
            throw Core::ServiceException("Key not found, keyId: " + request.keyId);
        }

        try {

            const Database::Entity::KMS::Key keyEntity = _kmsDatabase->getKeyByKeyId(request.keyId);
            log_trace << "KMS key entity received: " << keyEntity.ToString();

            const std::string plainText = DecryptPlaintext(keyEntity, request.ciphertext);

            Dto::KMS::DecryptResponse response;
            response.region = request.region;
            response.keyId = keyEntity.keyId;
            response.encryptionAlgorithm = request.encryptionAlgorithm;
            response.plaintext = plainText;
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "KMS decrypt failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    void KMSService::DeleteKey(const Dto::KMS::DeleteKeyRequest &request) const {
        Monitoring::MonitoringTimer measure(KMS_SERVICE_TIMER, KMS_SERVICE_COUNTER, "method", "delete_key");
        log_trace << "Delete key request: " << request;

        if (!_kmsDatabase->keyExists(request.keyId)) {
            log_error << "Key not found, keyId: " << request.keyId;
            throw Core::ServiceException("Key not found, keyId: " + request.keyId);
        }

        try {

            const Database::Entity::KMS::Key keyEntity = _kmsDatabase->getKeyByKeyId(request.keyId);
            log_trace << "KMS key entity received: " << keyEntity;
            _kmsDatabase->deleteKey(keyEntity);

        } catch (Core::DatabaseException &exc) {
            log_error << "Delete KMS key failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    std::string KMSService::EncryptPlaintext(const Database::Entity::KMS::Key &key, const std::string &plainText) const {

        switch (Dto::KMS::KeySpecFromString(key.keySpec)) {

            case Dto::KMS::KeySpec::SYMMETRIC_DEFAULT: {

                // Preparation
                const auto rawKey = static_cast<unsigned char *>(malloc(key.aes256Key.length() * 2));
                Core::Crypto::HexDecode(key.aes256Key, rawKey);
                const std::string rawPlaintext = Core::Crypto::Base64Decode(plainText);
                int plaintextLen = static_cast<int>(rawPlaintext.size());

                // Encryption
                auto *rawCiphertext = static_cast<unsigned char *>(malloc(plaintextLen * 2));
                Core::Crypto::Aes256EncryptString(reinterpret_cast<const unsigned char *>(rawPlaintext.c_str()), &plaintextLen, rawKey, rawCiphertext);
                log_debug << "Encrypted plaintext, length: " << plaintextLen;
                std::string output = Core::Crypto::Base64Encode({reinterpret_cast<char *>(rawCiphertext), static_cast<size_t>(plaintextLen)});
                free(rawCiphertext);
                free(rawKey);
                return output;
            }

            case Dto::KMS::KeySpec::RSA_2048:
            case Dto::KMS::KeySpec::RSA_3072:
            case Dto::KMS::KeySpec::RSA_4096: {

                // Preparation
                const std::string rawPlaintext = Core::Crypto::Base64Decode(plainText);
                EVP_PKEY *publicKey = Core::Crypto::ReadRsaPublicKey(key.rsaPublicKey);

                // Encryption
                const std::string cipherText = Core::Crypto::RsaEncrypt(publicKey, rawPlaintext);
                log_debug << "Encrypted plaintext, length: " << rawPlaintext.length();
                return Core::Crypto::Base64Encode(cipherText);
            }
            case Dto::KMS::KeySpec::ECC_NIST_P256:
            case Dto::KMS::KeySpec::ECC_NIST_P384:
            case Dto::KMS::KeySpec::ECC_NIST_P521:
            case Dto::KMS::KeySpec::ECC_SECG_P256K1:
                break;

            case Dto::KMS::KeySpec::HMAC_224: {

                // Encryption
                const std::string ciphertext = Core::Crypto::GetHmacSha224FromString(key.hmac224Key, Core::Crypto::Base64Decode(plainText));
                log_debug << "Encrypted plaintext, hashedTextLength: " << ciphertext.size();
                return Core::Crypto::Base64Encode({ciphertext.c_str(), ciphertext.size()});
            }

            case Dto::KMS::KeySpec::HMAC_256: {

                // Encryption
                const std::string ciphertext = Core::Crypto::GetHmacSha256FromString(key.hmac256Key, Core::Crypto::Base64Decode(plainText));
                log_debug << "Encrypted plaintext, hashedTextLength: " << ciphertext.size();
                return Core::Crypto::Base64Encode({ciphertext.c_str(), ciphertext.size()});
            }

            case Dto::KMS::KeySpec::HMAC_384: {

                // Encryption
                const std::string ciphertext = Core::Crypto::GetHmacSha384FromString(key.hmac384Key, Core::Crypto::Base64Decode(plainText));
                log_debug << "Encrypted plaintext, hashedTextLength: " << ciphertext.size();
                return Core::Crypto::Base64Encode({ciphertext.c_str(), ciphertext.size()});
            }

            case Dto::KMS::KeySpec::HMAC_512: {

                // Encryption
                const std::string ciphertext = Core::Crypto::GetHmacSha512FromString(key.hmac512Key, Core::Crypto::Base64Decode(plainText));
                log_debug << "Encrypted plaintext, hashedTextLength: " << ciphertext.size();
                return Core::Crypto::Base64Encode({ciphertext.c_str(), ciphertext.size()});
            }
        }
        return {};
    }

    std::string KMSService::DecryptPlaintext(const Database::Entity::KMS::Key &key, const std::string &ciphertext) const {

        switch (Dto::KMS::KeySpecFromString(key.keySpec)) {
            case Dto::KMS::KeySpec::SYMMETRIC_DEFAULT: {

                // Preparation
                const auto rawKey = static_cast<unsigned char *>(malloc(key.aes256Key.length() * 2));
                Core::Crypto::HexDecode(key.aes256Key, rawKey);
                const std::string rawCiphertext = Core::Crypto::Base64Decode(ciphertext);
                int ciphertextLen = static_cast<int>(rawCiphertext.size());

                // Description
                auto *rawPlaintext = static_cast<unsigned char *>(malloc(ciphertextLen * 2));
                Core::Crypto::Aes256DecryptString(reinterpret_cast<const unsigned char *>(rawCiphertext.c_str()), &ciphertextLen, rawKey, rawPlaintext);
                log_debug << "Decrypted plaintext, length: " << ciphertextLen;
                std::string output = Core::Crypto::Base64Encode({reinterpret_cast<char *>(rawPlaintext), static_cast<size_t>(ciphertextLen)});
                free(rawPlaintext);
                free(rawKey);
                return output;
            }

            case Dto::KMS::KeySpec::RSA_2048:
            case Dto::KMS::KeySpec::RSA_3072:
            case Dto::KMS::KeySpec::RSA_4096: {

                // Preparation
                const std::string rawCiphertext = Core::Crypto::Base64Decode(ciphertext);
                EVP_PKEY *privateKey = Core::Crypto::ReadRsaPrivateKey(key.rsaPrivateKey);

                // Description
                const std::string plainText = Core::Crypto::RsaDecrypt(privateKey, rawCiphertext);
                log_debug << "Decrypted plaintext, length: " << plainText.length();
                return Core::Crypto::Base64Encode(plainText);
            }

            case Dto::KMS::KeySpec::ECC_NIST_P256:
            case Dto::KMS::KeySpec::ECC_NIST_P384:
            case Dto::KMS::KeySpec::ECC_NIST_P521:
            case Dto::KMS::KeySpec::ECC_SECG_P256K1:
            case Dto::KMS::KeySpec::HMAC_224:
            case Dto::KMS::KeySpec::HMAC_256:
            case Dto::KMS::KeySpec::HMAC_384:
            case Dto::KMS::KeySpec::HMAC_512:
                break;
        }
        return {};
    }

    Dto::KMS::GetKeyPolicyResponse KMSService::GetKeyPolicy(const Dto::KMS::GetKeyPolicyRequest &request) const {
        log_trace << "GetKeyPolicy request, keyId: " << request.keyId;

        Database::Entity::KMS::Key key = _kmsDatabase->getKeyByKeyId(request.keyId);

        Dto::KMS::GetKeyPolicyResponse response;
        if (key.policy.empty()) {
            // Return a default allow-all policy
            response.policy = R"({"Version":"2012-10-17","Statement":[{"Effect":"Allow","Principal":{"AWS":"arn:aws:iam::000000000000:root"},"Action":"kms:*","Resource":"*"}]})";
        } else {
            response.policy = key.policy;
        }
        return response;
    }

    void KMSService::PutKeyPolicy(const Dto::KMS::PutKeyPolicyRequest &request) const {
        log_trace << "PutKeyPolicy request, keyId: " << request.keyId;

        Database::Entity::KMS::Key key = _kmsDatabase->getKeyByKeyId(request.keyId);
        key.policy = request.policy;
        key.modified = system_clock::now();
        _kmsDatabase->updateKey(key);
        log_debug << "PutKeyPolicy done, keyId: " << request.keyId;
    }

    void KMSService::TagResource(const Dto::KMS::TagResourceRequest &request) const {
        log_trace << "TagResource request, keyId: " << request.keyId;

        Database::Entity::KMS::Key key = _kmsDatabase->getKeyByKeyId(request.keyId);
        for (const auto &[k, v]: request.tags) {
            key.tags[k] = v;
        }
        key.modified = system_clock::now();
        _kmsDatabase->updateKey(key);
        log_debug << "TagResource done, keyId: " << request.keyId << " tags: " << request.tags.size();
    }

    void KMSService::UntagResource(const Dto::KMS::UntagResourceRequest &request) const {
        log_trace << "UntagResource request, keyId: " << request.keyId;

        Database::Entity::KMS::Key key = _kmsDatabase->getKeyByKeyId(request.keyId);
        for (const auto &tagKey: request.tagKeys) {
            key.tags.erase(tagKey);
        }
        key.modified = system_clock::now();
        _kmsDatabase->updateKey(key);
        log_debug << "UntagResource done, keyId: " << request.keyId;
    }

    Dto::KMS::ListResourceTagsResponse KMSService::ListResourceTags(const Dto::KMS::ListResourceTagsRequest &request) const {
        log_trace << "ListResourceTags request, keyId: " << request.keyId;

        Database::Entity::KMS::Key key = _kmsDatabase->getKeyByKeyId(request.keyId);
        Dto::KMS::ListResourceTagsResponse response;
        response.tags = key.tags;
        return response;
    }

    void KMSService::CreateAlias(const Dto::KMS::CreateAliasRequest &request) const {
        log_trace << "CreateAlias request, aliasName: " << request.aliasName << " targetKeyId: " << request.targetKeyId;

        Database::Entity::KMS::Key key = _kmsDatabase->getKeyByKeyId(request.targetKeyId);
        if (std::find(key.aliases.begin(), key.aliases.end(), request.aliasName) == key.aliases.end()) {
            key.aliases.push_back(request.aliasName);
        }
        key.modified = system_clock::now();
        _kmsDatabase->updateKey(key);
        log_debug << "CreateAlias done, aliasName: " << request.aliasName;
    }

    void KMSService::DeleteAlias(const Dto::KMS::DeleteAliasRequest &request) const {
        log_trace << "DeleteAlias request, aliasName: " << request.aliasName;

        // Find the key that owns this alias
        const Database::Entity::KMS::KeyList keys = _kmsDatabase->listKeys({}, {}, 0, 0, {});
        for (auto key: keys) {
            auto it = std::find(key.aliases.begin(), key.aliases.end(), request.aliasName);
            if (it != key.aliases.end()) {
                key.aliases.erase(it);
                key.modified = system_clock::now();
                _kmsDatabase->updateKey(key);
                break;
            }
        }
        log_debug << "DeleteAlias done, aliasName: " << request.aliasName;
    }

    void KMSService::UpdateAlias(const Dto::KMS::UpdateAliasRequest &request) const {
        log_trace << "UpdateAlias request, aliasName: " << request.aliasName << " targetKeyId: " << request.targetKeyId;

        // Remove alias from its current key owner
        const Database::Entity::KMS::KeyList keys = _kmsDatabase->listKeys({}, {}, 0, 0, {});
        for (auto key: keys) {
            auto it = std::find(key.aliases.begin(), key.aliases.end(), request.aliasName);
            if (it != key.aliases.end()) {
                key.aliases.erase(it);
                key.modified = system_clock::now();
                _kmsDatabase->updateKey(key);
                break;
            }
        }

        // Attach alias to new target key
        Database::Entity::KMS::Key newKey = _kmsDatabase->getKeyByKeyId(request.targetKeyId);
        newKey.aliases.push_back(request.aliasName);
        newKey.modified = system_clock::now();
        _kmsDatabase->updateKey(newKey);
        log_debug << "UpdateAlias done, aliasName: " << request.aliasName;
    }

    Dto::KMS::ListAliasesResponse KMSService::ListAliases(const Dto::KMS::ListAliasesRequest &request) const {
        log_trace << "ListAliases request, keyId: " << request.keyId;

        Dto::KMS::ListAliasesResponse response;
        const Database::Entity::KMS::KeyList keys = _kmsDatabase->listKeys({}, {}, 0, 0, {});
        for (const auto &key: keys) {
            if (!request.keyId.empty() && key.keyId != request.keyId) {
                continue;
            }
            for (const auto &aliasName: key.aliases) {
                Dto::KMS::AliasEntry entry;
                entry.aliasName = aliasName;
                entry.targetKeyId = key.keyId;
                entry.aliasArn = "arn:aws:kms:" + key.region + ":000000000000:" + aliasName;
                response.aliases.push_back(entry);
            }
        }
        return response;
    }

}// namespace Awsmock::Service
