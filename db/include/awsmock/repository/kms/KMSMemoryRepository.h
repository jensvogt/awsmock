//
// Created by vogje01 on 11/19/23.
//

#pragma once

// C++ includes
#include <string>

// Boost includes
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/kms/Key.h>
#include <awsmock/repository/Database.h>
#include <awsmock/repository/kms/IKMSRepository.h>

namespace Awsmock::Database {

    /**
     * KMS in-memory database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class KMSMemoryRepository final : public IKMSRepository {

    public:
        /**
         * Constructor
         */
        KMSMemoryRepository() = default;

        /**
         * Singleton instance
         */
        static KMSMemoryRepository &instance() {
            static KMSMemoryRepository instance;
            return instance;
        }

        /**
         * @brief Check existence of key by keyId
         *
         * @param keyId key ID
         * @return true if key already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool keyExists(const std::string &keyId) const override;

        /**
         * @brief Returns a KMS key by primary key
         *
         * @param oid key primary key
         * @return key entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::KMS::Key getKeyById(const std::string &oid) const override;

        /**
         * @brief Returns a KMS key by primary key
         *
         * @param oid key primary key
         * @return key entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::KMS::Key getKeyById(const bsoncxx::oid &oid) const override;

        /**
         * @brief Returns a KMS key by key ID
         *
         * @param keyId key ID
         * @return key entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::KMS::Key getKeyByKeyId(const std::string &keyId) const override;

        /**
         * @brief List all keys
         *
         * @param region AWS region
         * @param prefix key prefix
         * @param pageSize page size
         * @param pageIndex page index
         * @param sortColumns sorting
         * @return KeyList
         */
        [[nodiscard]]
        Entity::KMS::KeyList listKeys(const std::string &region, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const override;

        /**
         * Returns the total number of keys
         *
         * @return total number of keys
         * @throws DatabaseException
         */
        [[nodiscard]]
        long countKeys() const override;

        /**
         * @brief Create a new key in the KMS key table
         *
         * @param key key entity
         * @return created KMS key entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::KMS::Key createKey(Entity::KMS::Key &key) const override;

        /**
         * @brief Updates a key
         *
         * @param key key entity
         * @return created key entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::KMS::Key updateKey(Entity::KMS::Key &key) const override;

        /**
         * @brief Creates a new key or updates an existing key
         *
         * @param key key entity
         * @return created or updated key entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::KMS::Key upsertKey(Entity::KMS::Key &key) const override;

        /**
         * @brief Delete a key
         *
         * @param key key entity
         * @throws DatabaseException
         */
        void deleteKey(const Entity::KMS::Key &key) const override;

        /**
         * @brief Delete all keys
         *
         * @return number of entities deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        long deleteAllKeys() const override;

    private:
        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "KMS"};

        /**
         * KMS topic vector, when running without database
         */
        mutable std::map<std::string, Entity::KMS::Key> _keys;

        /**
         * Key mutex
         */
        static boost::mutex _keyMutex;
    };

} // namespace Awsmock::Database
