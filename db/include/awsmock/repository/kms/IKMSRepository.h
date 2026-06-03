//
// Created by vogje01 on 5/24/26.
//

#pragma once

// C++ includes
#include <string>
#include <vector>

// Awsmock includes
#include <awsmock/entity/monitoring/Counter.h>
#include <awsmock/entity/sns/Message.h>
#include <awsmock/entity/sns/Topic.h>
#include <awsmock/utils/SortColumn.h>

#include "awsmock/entity/kms/Key.h"

namespace Awsmock::Database {

    /**
     * @brief Interface for KMS repository operations.
     *
     * Provides an abstraction for storing, retrieving, and managing
     * KMS-related data.
     */
    class IKMSRepository {

    public:
        /**
         * @brief Virtual destructor for the ISQSRepository interface.
         *
         * Ensures derived classes' destructor is invoked correctly
         * during object destruction to release resources.
         */
        virtual ~IKMSRepository() = default;


        /**
         * @brief Check existence of key by keyId
         *
         * @param keyId key ID
         * @return true if key already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool keyExists(const std::string &keyId) const = 0;

        /**
         * @brief Returns a KMS key by primary key
         *
         * @param oid key primary key
         * @return key entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::KMS::Key getKeyById(const std::string &oid) const = 0;

        /**
         * @brief Returns a KMS key by primary key
         *
         * @param oid key primary key
         * @return key entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::KMS::Key getKeyById(const bsoncxx::oid &oid) const = 0;

        /**
         * @brief Returns a KMS key by key ID
         *
         * @param keyId key ID
         * @return key entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::KMS::Key getKeyByKeyId(const std::string &keyId) const = 0;

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
        virtual Entity::KMS::KeyList listKeys(const std::string &region, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const = 0;

        /**
         * @brief Returns the total number of keys
         *
         * @return total number of keys
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long countKeys() const = 0;

        /**
         * @brief Create a new key in the KMS key table
         *
         * @param key key entity
         * @return created KMS key entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::KMS::Key createKey(Entity::KMS::Key &key) const = 0;

        /**
         * @brief Create or update a key in the KMS key table
         *
         * @param key key entity
         * @return created or updated KMS key entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::KMS::Key upsertKey(Entity::KMS::Key &key) const = 0;

        /**
         * @brief Updates a key
         *
         * @param key key entity
         * @return created key entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::KMS::Key updateKey(Entity::KMS::Key &key) const = 0;

        /**
         * @brief Delete a key
         *
         * @param key key entity
         * @throws DatabaseException
         */
        virtual void deleteKey(const Entity::KMS::Key &key) const = 0;

        /**
         * @brief Delete all keys
         *
         * @return number of entities deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long deleteAllKeys() const = 0;
    };

} // namespace Awsmock::Database
