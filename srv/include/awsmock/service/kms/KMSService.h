//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_KMS_SERVICE_H
#define AWSMOCK_SERVICE_KMS_SERVICE_H

// C++ standard includes
#include <chrono>
#include <ctime>
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/monitoring/MonitoringTimer.h>
#include <awsmock/dto/common/mapper/Mapper.h>
#include <awsmock/dto/kms/CreateKeyRequest.h>
#include <awsmock/dto/kms/CreateKeyResponse.h>
#include <awsmock/dto/kms/DecryptRequest.h>
#include <awsmock/dto/kms/DecryptResponse.h>
#include <awsmock/dto/kms/DescribeKeyRequest.h>
#include <awsmock/dto/kms/DescribeKeyResponse.h>
#include <awsmock/dto/kms/EncryptRequest.h>
#include <awsmock/dto/kms/EncryptResponse.h>
#include <awsmock/dto/kms/GetKeyCounterRequest.h>
#include <awsmock/dto/kms/GetKeyCounterResponse.h>
#include <awsmock/dto/kms/ListKeysRequest.h>
#include <awsmock/dto/kms/ListKeysResponse.h>
#include <awsmock/dto/kms/ScheduleKeyDeletionRequest.h>
#include <awsmock/dto/kms/ScheduleKeyDeletionResponse.h>
#include <awsmock/dto/kms/UpdateKeyCounterRequest.h>
#include <awsmock/dto/kms/internal/DeleteKeyRequest.h>
#include <awsmock/dto/kms/internal/ListKeyArnsResponse.h>
#include <awsmock/dto/kms/internal/ListKeyCountersRequest.h>
#include <awsmock/dto/kms/internal/ListKeyCountersResponse.h>
#include <awsmock/dto/kms/model/Key.h>
#include <awsmock/dto/kms/model/KeyCounter.h>
#include <awsmock/repository/KMSDatabase.h>
#include <awsmock/service/kms/KMSCreator.h>
#include <awsmock/service/monitoring/MetricService.h>

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
         * @param request list keys request
         * @return ListKeysResponse
         * @throws Core::DatabaseException
         * @see Dto::KMS::ListKeysRequest
         * @see Dto::KMS::ListKeysResponse
         */
        [[nodiscard]] Dto::KMS::ListKeysResponse ListKeys(const Dto::KMS::ListKeysRequest &request) const;

        /**
         * @brief List all key counters
         *
         * @param request list key counters request
         * @return ListKeysResponse
         * @throws Core::DatabaseException
         * @see Dto::KMS::ListKeysRequest
         * @see Dto::KMS::ListKeysResponse
         */
        [[nodiscard]] Dto::KMS::ListKeyCountersResponse ListKeyCounters(const Dto::KMS::ListKeyCountersRequest &request) const;

        /**
         * @brief List all key ARNs
         *
         * @return ListKeyArnsResponse
         * @throws Core::DatabaseException
         * @see Dto::KMS::ListKeyArnsResponse
         */
        [[nodiscard]] Dto::KMS::ListKeyArnsResponse ListKeyArns() const;

        /**
         * @brief Creates a new key
         *
         * @param request create a key request
         * @return CreateKeyResponse
         * @see Dto::KMS::CreateKeyRequest
         * @see Dto::KMS::CreateKeyResponse
         */
        [[nodiscard]] Dto::KMS::CreateKeyResponse CreateKey(const Dto::KMS::CreateKeyRequest &request) const;

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
        [[nodiscard]] Dto::KMS::ScheduledKeyDeletionResponse ScheduleKeyDeletion(const Dto::KMS::ScheduleKeyDeletionRequest &request) const;

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
         * @brief Get a key
         *
         * @param request get key counter request
         * @return GetKeyCounterResponse
         * @throws Core::DatabaseException
         * @see Dto::KMS::GetKeyCounterRequest
         * @see Dto::KMS::GetKeyCounterResponse
         */
        [[nodiscard]] Dto::KMS::GetKeyCounterResponse GetKeyCounter(const Dto::KMS::GetKeyCounterRequest &request) const;

        /**
         * @brief Update a key
         *
         * @param request update key counter request
         * @throws Core::DatabaseException
         * @see Dto::KMS::UpdateKeyCounterRequest
         */
        void UpdateKeyCounter(const Dto::KMS::UpdateKeyCounterRequest &request) const;

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
        [[nodiscard]] Dto::KMS::DecryptResponse Decrypt(const Dto::KMS::DecryptRequest &request) const;

        /**
         * @brief Deletes a key
         *
         * @param request delete key request
         * @throws Core::DatabaseException, Core::ServiceException
         * @see Dto::KMS::DecryptRequest
         */
        void DeleteKey(const Dto::KMS::DeleteKeyRequest &request) const;

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
