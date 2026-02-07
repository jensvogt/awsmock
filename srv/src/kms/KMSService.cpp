//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/kms/KMSService.h>
#include <thread>

namespace AwsMock::Service {

    template<typename K>
    void CallAsyncCreateKey(K &&k) {
        boost::thread t(&KMSCreator::CreateKmsKey, k);
        t.detach();
    }

    KMSService::KMSService() : _kmsDatabase(Database::KMSDatabase::instance()) {

        // Initialize environment
        _accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
    }

    Dto::KMS::ListKeysResponse KMSService::ListKeys(const Dto::KMS::ListKeysRequest &request) const {
        Monitoring::MonitoringTimer measure(KMS_SERVICE_TIMER, KMS_SERVICE_COUNTER, "method", "list_keys");
        log_trace << "List keys request: " << request;

        try {

            const Database::Entity::KMS::KeyList keyList = _kmsDatabase.ListKeys();
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
            const Database::Entity::KMS::KeyList keyList = _kmsDatabase.ListKeys(request.region, request.prefix, request.pageSize, request.pageIndex, Dto::Common::Mapper::map(request.sortColumns));
            listKeyCountersResponse.total = _kmsDatabase.CountKeys();

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
            const Database::Entity::KMS::KeyList keyList = _kmsDatabase.ListKeys();

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
            Database::Entity::KMS::Key keyEntity = {
                    .region = request.region,
                    .keyId = keyId,
                    .keyUsage = KeyUsageToString(request.keyUsage),
                    .keySpec = KeySpecToString(request.keySpec),
                    .arn = arn,
                    .description = request.description,
                    .tags = request.tags};

            // Store in a database
            keyEntity = _kmsDatabase.CreateKey(keyEntity);
            log_trace << "KMS keyEntity created: " << keyEntity;

            // Create keyEntity material asynchronously
            CallAsyncCreateKey<std::string &>(keyId);
            log_debug << "KMS keyEntity creation started, keyId: " << keyId;

            Dto::KMS::Key key;
            key.keyId = keyEntity.keyId;
            key.arn = keyEntity.arn;
            key.keySpec = Dto::KMS::KeySpecFromString(keyEntity.keySpec);
            key.keyUsage = Dto::KMS::KeyUsageFromString(keyEntity.keyUsage);
            key.keyState = Dto::KMS::KeyStateFromString(keyEntity.keyState);
            key.description = keyEntity.description;
            Dto::KMS::CreateKeyResponse response;
            response.key = key;
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "KMS create key failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    void KMSService::WaitForRsaKey(const std::string &keyId, const int maxSeconds) const {

        int i = 0;
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            if (Database::Entity::KMS::Key key = _kmsDatabase.GetKeyByKeyId(keyId); !key.rsaPrivateKey.empty() || i > maxSeconds * 2) {
                break;
            }
            i++;
        }
    }

    void KMSService::WaitForAesKey(const std::string &keyId, const int maxSeconds) const {

        int i = 0;
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            if (Database::Entity::KMS::Key key = _kmsDatabase.GetKeyByKeyId(keyId); !key.aes256Key.empty() || i > maxSeconds * 2) {
                break;
            }
            i++;
        }
    }

    Dto::KMS::ScheduledKeyDeletionResponse KMSService::ScheduleKeyDeletion(const Dto::KMS::ScheduleKeyDeletionRequest &request) const {
        Monitoring::MonitoringTimer measure(KMS_SERVICE_TIMER, KMS_SERVICE_COUNTER, "method", "schedule_key_deletion");
        log_trace << "Schedule key deletion request: " << request;

        if (!_kmsDatabase.KeyExists(request.keyId)) {
            log_error << "Key not found, keyId: " << request.keyId;
            throw Core::ServiceException("Key not found, keyId: " + request.keyId);
        }

        try {

            // Get the key
            Database::Entity::KMS::Key key = _kmsDatabase.GetKeyByKeyId(request.keyId);

            key.pendingWindowInDays = request.pendingWindowInDays;
            key.scheduledDeletion = system_clock::now() + std::chrono::days(request.pendingWindowInDays);
            key.keyState = Dto::KMS::KeyStateToString(Dto::KMS::KeyState::PENDING_DELETION);

            // Store in a database
            key = _kmsDatabase.UpdateKey(key);
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

        if (!_kmsDatabase.KeyExists(request.keyId)) {
            log_error << "Key not found, keyId: " << request.keyId;
            throw Core::ServiceException("Key not found, keyId: " + request.keyId);
        }

        try {

            Database::Entity::KMS::Key keyEntity = _kmsDatabase.GetKeyByKeyId(request.keyId);
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
            response.key = key;
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "KMS describe key failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::KMS::GetKeyCounterResponse KMSService::GetKeyCounter(const Dto::KMS::GetKeyCounterRequest &request) const {
        Monitoring::MonitoringTimer measure(KMS_SERVICE_TIMER, KMS_SERVICE_COUNTER, "method", "get_key_counter");
        log_trace << "Get key request: " << request;

        if (!_kmsDatabase.KeyExists(request.keyId)) {
            log_error << "Key not found, keyId: " << request.keyId;
            throw Core::ServiceException("Key not found, keyId: " + request.keyId);
        }

        try {

            Database::Entity::KMS::Key keyEntity = _kmsDatabase.GetKeyByKeyId(request.keyId);
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

        if (!_kmsDatabase.KeyExists(request.keyCounter.keyId)) {
            log_error << "Key not found, keyId: " << request.keyCounter.keyId;
            throw Core::ServiceException("Key not found, keyId: " + request.keyCounter.keyId);
        }

        try {

            Database::Entity::KMS::Key keyEntity = _kmsDatabase.GetKeyByKeyId(request.keyCounter.keyId);
            log_trace << "KMS key entity received, key: " << keyEntity;

            keyEntity.keySpec = Dto::KMS::KeySpecToString(request.keyCounter.keySpec);
            keyEntity.keyUsage = Dto::KMS::KeyUsageToString(request.keyCounter.keyUsage);
            keyEntity.keyState = Dto::KMS::KeyStateToString(request.keyCounter.keyState);
            keyEntity.origin = Dto::KMS::OriginToString(request.keyCounter.origin);
            keyEntity.description = request.keyCounter.description;
            _kmsDatabase.UpdateKey(keyEntity);
            log_trace << "KMS key entity updated, key: " << keyEntity;

        } catch (Core::DatabaseException &exc) {
            log_error << "KMS get key failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::KMS::EncryptResponse KMSService::Encrypt(const Dto::KMS::EncryptRequest &request) const {
        Monitoring::MonitoringTimer measure(KMS_SERVICE_TIMER, KMS_SERVICE_COUNTER, "method", "encrypt");
        log_trace << "Encrypt plaintext request: " << request;

        if (!_kmsDatabase.KeyExists(request.keyId)) {
            log_error << "Key not found, keyId: " << request.keyId;
            throw Core::ServiceException("Key not found, keyId: " + request.keyId);
        }

        try {

            const Database::Entity::KMS::Key keyEntity = _kmsDatabase.GetKeyByKeyId(request.keyId);
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

    Dto::KMS::DecryptResponse KMSService::Decrypt(const Dto::KMS::DecryptRequest &request) const {
        Monitoring::MonitoringTimer measure(KMS_SERVICE_TIMER, KMS_SERVICE_COUNTER, "method", "decrypt");
        log_trace << "Decrypt plaintext request, keyId: " << request.keyId;

        if (!_kmsDatabase.KeyExists(request.keyId)) {
            log_error << "Key not found, keyId: " << request.keyId;
            throw Core::ServiceException("Key not found, keyId: " + request.keyId);
        }

        try {

            const Database::Entity::KMS::Key keyEntity = _kmsDatabase.GetKeyByKeyId(request.keyId);
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

        if (!_kmsDatabase.KeyExists(request.keyId)) {
            log_error << "Key not found, keyId: " << request.keyId;
            throw Core::ServiceException("Key not found, keyId: " + request.keyId);
        }

        try {

            const Database::Entity::KMS::Key keyEntity = _kmsDatabase.GetKeyByKeyId(request.keyId);
            log_trace << "KMS key entity received: " << keyEntity;
            _kmsDatabase.DeleteKey(keyEntity);

        } catch (Core::DatabaseException &exc) {
            log_error << "Delete KMS key failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    std::string KMSService::EncryptPlaintext(const Database::Entity::KMS::Key &key, const std::string &plainText) {

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

                // Preparation
                const std::string rawPlaintext = Core::Crypto::Base64Decode(plainText);
                const int plaintextLen = static_cast<int>(rawPlaintext.length());

                // Encryption
                std::string ciphertext = Core::Crypto::GetHmacSha224FromString(key.hmac224Key, rawPlaintext);
                log_debug << "Encrypted plaintext, length: " << rawPlaintext;
                return Core::Crypto::Base64Encode({ciphertext, static_cast<size_t>(plaintextLen)});
            }

            case Dto::KMS::KeySpec::HMAC_256: {

                // Preparation
                const std::string rawPlaintext = Core::Crypto::Base64Decode(plainText);
                const int plaintextLen = static_cast<int>(rawPlaintext.length());

                // Encryption
                std::string ciphertext = Core::Crypto::GetHmacSha256FromString(key.hmac256Key, rawPlaintext);
                log_debug << "Encrypted plaintext, length: " << rawPlaintext;
                return Core::Crypto::Base64Encode({ciphertext, static_cast<size_t>(plaintextLen)});
            }

            case Dto::KMS::KeySpec::HMAC_384: {

                // Preparation
                std::string ciphertext;
                const std::string rawPlaintext = Core::Crypto::Base64Decode(plainText);
                unsigned int hashLen = 0;

                // Encryption
                ciphertext = Core::Crypto::GetHmacSha384FromString(key.hmac384Key, rawPlaintext, &hashLen);
                log_debug << "Encrypted plaintext, length: " << hashLen;
                return Core::Crypto::Base64Encode({ciphertext.c_str(), static_cast<size_t>(hashLen)});
            }

            case Dto::KMS::KeySpec::HMAC_512: {

                // Preparation
                const std::string rawPlaintext = Core::Crypto::Base64Decode(plainText);
                unsigned int hashLen = 0;

                // Encryption
                const std::string ciphertext = Core::Crypto::GetHmacSha512FromString(key.hmac512Key, rawPlaintext, &hashLen);
                log_debug << "HMAC hashed plaintext, length: " << hashLen;
                std::string tmp = ciphertext.substr(0, hashLen * 2);
                return Core::Crypto::Base64Encode(ciphertext.substr(0, hashLen * 2));
            }
        }
        return {};
    }

    std::string KMSService::DecryptPlaintext(const Database::Entity::KMS::Key &key, const std::string &ciphertext) {

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

}// namespace AwsMock::Service