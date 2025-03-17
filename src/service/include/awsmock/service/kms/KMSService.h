//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_KMS_SERVICE_H
#define AWSMOCK_SERVICE_KMS_SERVICE_H

// C++ standard includes
#include <chrono>
#include <ctime>
#include <string>
#include <thread>

// Boost includes
#include <boost/thread.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/dto/kms/CreateKeyRequest.h>
#include <awsmock/dto/kms/CreateKeyResponse.h>
#include <awsmock/dto/kms/DecryptRequest.h>
#include <awsmock/dto/kms/DecryptResponse.h>
#include <awsmock/dto/kms/DescribeKeyRequest.h>
#include <awsmock/dto/kms/DescribeKeyResponse.h>
#include <awsmock/dto/kms/EncryptRequest.h>
#include <awsmock/dto/kms/EncryptResponse.h>
#include <awsmock/dto/kms/ListKeysRequest.h>
#include <awsmock/dto/kms/ListKeysResponse.h>
#include <awsmock/dto/kms/ScheduleKeyDeletionRequest.h>
#include <awsmock/dto/kms/ScheduleKeyDeletionResponse.h>
#include <awsmock/dto/kms/model/Key.h>
#include <awsmock/dto/kms/model/ListKey.h>
#include <awsmock/repository/KMSDatabase.h>
#include <awsmock/service/kms/KMSCreator.h>
#include <awsmock/service/monitoring/MetricDefinition.h>
#include <awsmock/service/monitoring/MetricService.h>
#include <awsmock/service/monitoring/MetricServiceTimer.h>

#define DEFAULT_KMS_ACCOUNT_ID "000000000000"

namespace AwsMock::Service {

    using std::chrono::system_clock;

    /**
     * @brief KMS service
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class KMSService {

      public:

        /**
         * @brief Constructor
         */
        explicit KMSService();

        /**
         * @brief List all keys
         *
         * @param request list queue request
         * @return ListKeysResponse
         * @throws Core::DatabaseException
         * @see Dto::KMS::ListKeysRequest
         * @see Dto::KMS::ListKeysResponse
         */
        Dto::KMS::ListKeysResponse ListKeys(const Dto::KMS::ListKeysRequest &request) const;

        /**
         * @brief Creates a new key
         *
         * @param request create key request
         * @return CreateKeyResponse
         * @see Dto::KMS::CreateKeyRequest
         * @see Dto::KMS::CreateKeyResponse
         */
        Dto::KMS::CreateKeyResponse CreateKey(const Dto::KMS::CreateKeyRequest &request) const;

        /**
         * @brief Wait for the asynchronous key creation
         *
         * @param keyId key ID
         * @param maxSeconds maximal seconds
         */
        void WaitForRsaKey(const std::string &keyId, int maxSeconds) const;

        /**
         * @brief Wait for the asynchronous key creation
         *
         * @param keyId key ID
         * @param maxSeconds maximal seconds
         */
        void WaitForAesKey(const std::string &keyId, int maxSeconds) const;

        /**
         * @brief Schedules a key deletion
         *
         * @param request create queue request
         * @return ScheduleKeyDeletionRequest
         * @throws Core::DatabaseException
         * @see Dto::KMS::ScheduledKeyDeletionResponse
         */
        Dto::KMS::ScheduledKeyDeletionResponse ScheduleKeyDeletion(const Dto::KMS::ScheduleKeyDeletionRequest &request) const;

        /**
         * @brief Describe a key
         *
         * @param request describe key request
         * @return DescribeKeyResponse
         * @throws Core::DatabaseException
         * @see Dto::KMS::DescribeKeyRequest
         * @see Dto::KMS::DescribeKeyResponse
         */
        [[nodiscard]] Dto::KMS::DescribeKeyResponse DescribeKey(const Dto::KMS::DescribeKeyRequest &request) const;

        /**
         * @brief Encrypt a plain text using a given algorithm
         *
         * @param request encrypt request
         * @return EncryptResponse
         * @throws Core::DatabaseException, Core::ServiceException
         * @see Dto::KMS::EncryptRequest
         * @see Dto::KMS::EncryptResponse
         */
        [[nodiscard]] Dto::KMS::EncryptResponse Encrypt(const Dto::KMS::EncryptRequest &request) const;

        /**
         * @brief Decrypts a cipher text using a given algorithm
         *
         * @param request decrypt request
         * @return DecryptResponse
         * @throws Core::DatabaseException, Core::ServiceException
         * @see Dto::KMS::DecryptRequest
         * @see Dto::KMS::DecryptResponse
         */
        Dto::KMS::DecryptResponse Decrypt(const Dto::KMS::DecryptRequest &request) const;

      private:

        /**
         * @brief Encrypt a plaintext.
         *
         * @param key KMS key
         * @param plainText plain text
         * @return ciphertext
         */
        static std::string EncryptPlaintext(const Database::Entity::KMS::Key &key, const std::string &plainText);

        /**
         * @brief Decrypt a ciphertext.
         *
         * @param key KMS key
         * @param ciphertext cipher text
         * @return plaintext
         */
        static std::string DecryptPlaintext(const Database::Entity::KMS::Key &key, const std::string &ciphertext);

        /**
         * Account ID
         */
        std::string _accountId;

        /**
         * Database connection
         */
        Database::KMSDatabase &_kmsDatabase;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_KMS_SERVICE_H
