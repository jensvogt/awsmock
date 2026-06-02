//
// Created by vogje01 on 30/05/2023.
//

#pragma once

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

namespace Awsmock::Service {

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
        [[maybe_unused]]
        explicit KMSCreator() = default;

        /**
         * @brief Create new KMS key
         *
         * @param keyId ID of the key
         */
        void CreateKmsKey(const std::string &keyId) const;

      private:

        mutable logger_t _logger{boost::log::keywords::channel = "KMS"};

        /**
         * @brief Generate AES 256 key
         *
         * @par
         * Key material will be hex-encoded.
         *
         * @param key key entity
         */
        [[maybe_unused]]
        void GenerateAes256Key(Database::Entity::KMS::Key &key) const;

        /**
         * @brief Generate HMAC key.
         *
         * @par
         * Key material will be hex-encoded.
         *
         * @param key key entity
         * @param length key length
         */
        [[maybe_unused]]
        void CreateHmacKey(Database::Entity::KMS::Key &key, int length) const;

        /**
         * @brief Generate a public/private key pair
         *
         * @par
         * Key material will be hex-encoded.
         *
         * @param key key
         * @param length length
         * @see AwsMock::Core::CryptoUtils
         */
        [[maybe_unused]]
        void GenerateRsaKeyPair(Database::Entity::KMS::Key &key, int length) const;
    };

}// namespace Awsmock::Service
