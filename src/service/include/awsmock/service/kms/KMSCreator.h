//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_KMS_CREATOR_H
#define AWSMOCK_SERVICE_KMS_CREATOR_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/kms/model/KeySpec.h>
#include <awsmock/dto/kms/model/KeyState.h>
#include <awsmock/entity/kms/Key.h>
#include <awsmock/repository/KMSDatabase.h>
#include <awsmock/service/monitoring/MetricService.h>

namespace AwsMock::Service {

    /**
     * @brief KMS symmetric and asymmetric key creator.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class KMSCreator {

      public:

        /**
         * @brief Constructor
         */
        [[maybe_unused]] explicit KMSCreator() = default;

        /**
         * @brief Create new KMS key
         *
         * @param keyId ID of the key
         */
        static void CreateKmsKey(const std::string &keyId);

      private:

        /**
         * @brief Generate AES 256 key
         *
         * @par
         * Key material will be hex-encoded.
         *
         * @param key key entity
         */
        [[maybe_unused]] static void GenerateAes256Key(Database::Entity::KMS::Key &key);

        /**
         * @brief Generate HMAC key.
         *
         * @par
         * Key material will be hex-encoded.
         *
         * @param key key entity
         * @param length key length
         */
        static void CreateHmacKey(Database::Entity::KMS::Key &key, const int length);

        /**
         * @brief Generate a public/private key pair
         *
         * @par
         * Key material will be hex-encoded.
         * @param key key
         * @param length length
         * @see AwsMock::Core::CryptoUtils
         */
        [[maybe_unused]] static void GenerateRsaKeyPair(Database::Entity::KMS::Key &key, int length);
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_KMS_CREATOR_H
