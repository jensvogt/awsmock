//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/kms/KMSCreator.h>

namespace AwsMock::Service {

    void KMSCreator::CreateKmsKey(const std::string &keyId) {

        log_debug << "Start creating KMS key, keyId: " << keyId;

        // Get database connection
        const Database::KMSDatabase &kmsDatabase = Database::KMSDatabase::instance();

        // Make local copy
        Database::Entity::KMS::Key key = kmsDatabase.GetKeyByKeyId(keyId);
        key.keyState = KeyStateToString(Dto::KMS::KeyState::CREATING);

        switch (Dto::KMS::KeySpecFromString(key.keySpec)) {

            case Dto::KMS::KeySpec::SYMMETRIC_DEFAULT: {
                GenerateAes256Key(key);
                break;
            }

            case Dto::KMS::KeySpec::RSA_2048: {
                GenerateRsaKeyPair(key, 2048);
                break;
            }

            case Dto::KMS::KeySpec::RSA_3072: {
                GenerateRsaKeyPair(key, 3072);
                break;
            }

            case Dto::KMS::KeySpec::RSA_4096: {
                GenerateRsaKeyPair(key, 4096);
                break;
            }

            case Dto::KMS::KeySpec::HMAC_224:
                CreateHmacKey(key, 224);
                break;

            case Dto::KMS::KeySpec::HMAC_256:
                CreateHmacKey(key, 256);
                break;

            case Dto::KMS::KeySpec::HMAC_384:
                CreateHmacKey(key, 384);
                break;

            case Dto::KMS::KeySpec::HMAC_512:
                CreateHmacKey(key, 512);
                break;

            case Dto::KMS::KeySpec::ECC_NIST_P256:
            case Dto::KMS::KeySpec::ECC_NIST_P384:
            case Dto::KMS::KeySpec::ECC_NIST_P521:
            case Dto::KMS::KeySpec::ECC_SECG_P256K1: {
                log_warning << "Encryption algorithm not yet implemented, name: " << key.keySpec;
                break;
            }
        }

        // Save to database
        key.keyState = KeyStateToString(Dto::KMS::KeyState::ENABLED);
        key = kmsDatabase.UpdateKey(key);

        log_debug << "KMS key created, keyId: " << key.keyId;
    }

    void KMSCreator::GenerateAes256Key(Database::Entity::KMS::Key &key) {
        log_debug << "Start creating AES256 KMS key, keyId: " << key.keyId;

        unsigned char keyMaterial[32], iv[32];
        Core::Crypto::CreateAes256Key(keyMaterial, iv);

        // Base64 hashing
        const std::string hexKey = Core::Crypto::HexEncode(keyMaterial, 32);
        const std::string hexIv = Core::Crypto::HexEncode(iv, 16);
        log_debug << "KMS keys created";

        key.aes256Key = hexKey;
        key.aes256Iv = hexIv;

        log_debug << "AES256 KMS key created, keyId: " << key.keyId;
    }

    void KMSCreator::CreateHmacKey(Database::Entity::KMS::Key &key, const int length) {
        log_debug << "Start creating HMAC KMS key, keyId: " << key.keyId << " length: " << length;

        unsigned char keyMaterial[512];
        Core::Crypto::CreateHmacKey(keyMaterial, length);

        // Base64 hashing
        const std::string hexKey = Core::Crypto::HexEncode(keyMaterial, length);
        log_debug << "HMAC KMS keys created, length: " << length;

        if (length == 224) {
            key.hmac224Key = hexKey;
        } else if (length == 256) {
            key.hmac256Key = hexKey;
        } else if (length == 384) {
            key.hmac384Key = hexKey;
        } else if (length == 512) {
            key.hmac512Key = hexKey;
        }
        log_debug << "HMAC KMS key created, keyId: " << key.keyId;
    }

    void KMSCreator::GenerateRsaKeyPair(Database::Entity::KMS::Key &key, const int length) {
        log_debug << "Start creating RSA KMS key pair, keyId: " << key.keyId << " length: " << length;

        EVP_PKEY *keyMaterial = Core::Crypto::GenerateRsaKeys(length);
        key.rsaPublicKey = Core::Crypto::GetRsaPublicKey(keyMaterial);
        key.rsaPrivateKey = Core::Crypto::GetRsaPrivateKey(keyMaterial);

        log_debug << "AES256 KMS key created, keyId: " << key.keyId;
    }

}// namespace AwsMock::Service
