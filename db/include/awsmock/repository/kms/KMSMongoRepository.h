//
// Created by vogje01 on 29/05/2023.
//

#pragma once

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/kms/Key.h>
#include <awsmock/entity/s3/Object.h>
#include <awsmock/repository/Database.h>
#include <awsmock/repository/DatabaseBase.h>
#include <awsmock/utils/SortColumn.h>

#include "IKMSRepository.h"

namespace Awsmock::Database {

    /**
     * @brief KMS MongoDB database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class KMSMongoRepository final : public IKMSRepository {

    public:
        /**
         * @brief Constructor
         */
        explicit KMSMongoRepository() = default;

        /**
         * @brief Singleton instance
         */
        static KMSMongoRepository &instance() {
            static KMSMongoRepository instance;
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
         * @brief Returns the total number of keys
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
         * @brief Create or update a key in the KMS key table
         *
         * @param key key entity
         * @return created or updated KMS key entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::KMS::Key upsertKey(Entity::KMS::Key &key) const override;

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
         * @brfief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "KMS"};

        /**
         * Database name
         */
        static constexpr auto _databaseName = "awsmock";

        /**
         * Key collection name
         */
        static constexpr auto _keyCollectionName = "kms_key";
    };

} // namespace Awsmock::Database
