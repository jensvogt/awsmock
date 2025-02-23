//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/kms/KMSService.h>

namespace AwsMock::Service {

    template<typename K>
    void CallAsyncCreateKey(K &&k) {
        boost::thread t(&KMSCreator::CreateKmsKey, k);
        t.detach();
    }

    KMSService::KMSService() : _kmsDatabase(Database::KMSDatabase::instance()) {

        // Initialize environment
        _accountId = Core::Configuration::instance().GetValueString("awsmock.access.account-id");
    }

    Dto::KMS::ListKeysResponse KMSService::ListKeys(const Dto::KMS::ListKeysRequest &request) const {
        Monitoring::MetricServiceTimer measure(KMS_SERVICE_TIMER, "method", "list_keys");
        log_trace << "List keys request: " << request.ToString();

        try {

            Database::Entity::KMS::KeyList keyList = _kmsDatabase.ListKeys();
            Dto::KMS::ListKeysResponse listKeysResponse;
            for (const auto &k: keyList) {
                Dto::KMS::ListKey key = {.keyId = k.keyId, .keyArn = k.arn};
                listKeysResponse.keys.emplace_back(key);
            }
            log_debug << "List all keys, size: " << keyList.size();

            return listKeysResponse;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::KMS::CreateKeyResponse KMSService::CreateKey(const Dto::KMS::CreateKeyRequest &request) const {
        Monitoring::MetricServiceTimer measure(KMS_SERVICE_TIMER, "method", "create_key");
        log_trace << "Create key request: " << request.ToString();

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

            // Store in database
            keyEntity = _kmsDatabase.CreateKey(keyEntity);
            log_trace << "KMS keyEntity created: " << keyEntity.ToString();

            // Create keyEntity material asynchronously
            CallAsyncCreateKey<std::string &>(keyId);
            log_debug << "KMS keyEntity creation started, keyId: " << keyId;

            Dto::KMS::Key key = {
                    .keyId = keyEntity.keyId,
                    .arn = keyEntity.arn,
                    .keySpec = Dto::KMS::KeySpecFromString(keyEntity.keySpec),
                    .keyUsage = Dto::KMS::KeyUsageFromString(keyEntity.keyUsage),
                    .keyState = Dto::KMS::KeyStateFromString(keyEntity.keyState),
                    .description = keyEntity.description};
            Dto::KMS::CreateKeyResponse response = {.key = key};
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
        Monitoring::MetricServiceTimer measure(KMS_SERVICE_TIMER, "method", "schedule_key_deletion");
        log_trace << "Schedule key deletion request: " << request.ToString();

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

            // Store in database
            key = _kmsDatabase.UpdateKey(key);
            log_trace << "KMS key updated: " << key.ToString();

            return {
                    .keyId = request.keyId,
                    .keyState = key.keyState,
                    .deletionDate = key.scheduledDeletion.time_since_epoch().count(),
                    .pendingWindowInDays = request.pendingWindowInDays,
            };

        } catch (Core::DatabaseException &exc) {
            log_error << "KMS create key failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::KMS::DescribeKeyResponse KMSService::DescribeKey(const Dto::KMS::DescribeKeyRequest &request) const {
        Monitoring::MetricServiceTimer measure(KMS_SERVICE_TIMER, "method", "describe_key");
        log_trace << "Create key request: " << request.ToString();

        if (!_kmsDatabase.KeyExists(request.keyId)) {
            log_error << "Key not found, keyId: " << request.keyId;
            throw Core::ServiceException("Key not found, keyId: " + request.keyId);
        }

        try {

            Database::Entity::KMS::Key keyEntity = _kmsDatabase.GetKeyByKeyId(request.keyId);
            log_trace << "KMS key entity received: " << keyEntity.ToString();

            Dto::KMS::Key key = {
                    .keyId = keyEntity.keyId,
                    .arn = keyEntity.arn,
                    .keySpec = Dto::KMS::KeySpecFromString(keyEntity.keySpec),
                    .keyUsage = Dto::KMS::KeyUsageFromString(keyEntity.keyUsage),
                    .keyState = Dto::KMS::KeyStateFromString(keyEntity.keyState),
                    .description = keyEntity.description,
                    .creationDate = keyEntity.created.time_since_epoch().count(),
                    .deletionDate = keyEntity.scheduledDeletion.time_since_epoch().count(),
                    .enabled = Core::StringUtils::Equals(keyEntity.keyState, Dto::KMS::KeyStateToString(Dto::KMS::KeyState::ENABLED))};
            Dto::KMS::DescribeKeyResponse response = {.key = key};
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "KMS describe key failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::KMS::EncryptResponse KMSService::Encrypt(const Dto::KMS::EncryptRequest &request) const {
        Monitoring::MetricServiceTimer measure(KMS_SERVICE_TIMER, "method", "encrypt");
        log_trace << "Encrypt plaintext request: " << request.ToString();

        if (!_kmsDatabase.KeyExists(request.keyId)) {
            log_error << "Key not found, keyId: " << request.keyId;
            throw Core::ServiceException("Key not found, keyId: " + request.keyId);
        }

        try {

            const Database::Entity::KMS::Key keyEntity = _kmsDatabase.GetKeyByKeyId(request.keyId);
            log_trace << "KMS key entity received: " << keyEntity.ToString();

            const std::string cipherText = EncryptPlaintext(keyEntity, request.plainText);

            Dto::KMS::EncryptResponse response = {{.region = request.region}, keyEntity.keyId, request.encryptionAlgorithm, cipherText};
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "KMS encrypt failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::KMS::DecryptResponse KMSService::Decrypt(const Dto::KMS::DecryptRequest &request) const {
        Monitoring::MetricServiceTimer measure(KMS_SERVICE_TIMER, "method", "decrypt");
        log_trace << "Decrypt plaintext request: " << request.ToString();

        if (!_kmsDatabase.KeyExists(request.keyId)) {
            log_error << "Key not found, keyId: " << request.keyId;
            throw Core::ServiceException("Key not found, keyId: " + request.keyId);
        }

        try {

            Database::Entity::KMS::Key keyEntity = _kmsDatabase.GetKeyByKeyId(request.keyId);
            log_trace << "KMS key entity received: " << keyEntity.ToString();

            std::string plainText = DecryptPlaintext(keyEntity, request.ciphertext);

            Dto::KMS::DecryptResponse response = {{.region = request.region}, keyEntity.keyId, request.encryptionAlgorithm, plainText};
            return response;

        } catch (Core::DatabaseException &exc) {
            log_error << "KMS decrypt failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    std::string KMSService::EncryptPlaintext(const Database::Entity::KMS::Key &key, const std::string &plainText) {

        switch (Dto::KMS::KeySpecFromString(key.keySpec)) {

            case Dto::KMS::KeySpec::SYMMETRIC_DEFAULT: {

                // Preparation
                unsigned char *rawKey = Core::Crypto::HexDecode(key.aes256Key);
                const std::string rawPlaintext = Core::Crypto::Base64Decode(plainText);
                int plaintextLen = static_cast<int>(rawPlaintext.length());

                // Encryption
                unsigned char *rawCiphertext = Core::Crypto::Aes256EncryptString((unsigned char *) rawPlaintext.c_str(), &plaintextLen, rawKey);
                log_debug << "Encrypted plaintext, length: " << plaintextLen;
                return Core::Crypto::Base64Encode({reinterpret_cast<char *>(rawCiphertext), static_cast<size_t>(plaintextLen)});
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
                unsigned char *rawKey = Core::Crypto::HexDecode(key.aes256Key);
                const std::string rawCiphertext = Core::Crypto::Base64Decode(ciphertext);
                int ciphertextLenLen = static_cast<int>(rawCiphertext.length());

                // Description
                unsigned char *rawPlaintext = Core::Crypto::Aes256DecryptString((unsigned char *) rawCiphertext.c_str(), &ciphertextLenLen, rawKey);
                log_debug << "Decrypted plaintext, length: " << ciphertextLenLen;

                return Core::Crypto::Base64Encode({reinterpret_cast<char *>(rawPlaintext)});
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