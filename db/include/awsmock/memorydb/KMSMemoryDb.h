//
// Created by vogje01 on 11/19/23.
//

#ifndef AWSMOCK_REPOSITORY_KMS_MEMORYDB_H
#define AWSMOCK_REPOSITORY_KMS_MEMORYDB_H

// C++ includes
#include <string>

// Boost includes
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/kms/Key.h>
#include <awsmock/repository/Database.h>

namespace AwsMock::Database {

    /**
     * KMS in-memory database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class KMSMemoryDb {

      public:

        /**
         * Constructor
         */
        KMSMemoryDb() = default;

        /**
         * Singleton instance
         */
        static KMSMemoryDb &instance() {
            static KMSMemoryDb kmsMemoryDb;
            return kmsMemoryDb;
        }

        /**
         * @brief Check existence of key by keyId
         *
         * @param keyId key ID
         * @return true if key already exists
         * @throws DatabaseException
         */
        bool KeyExists(const std::string &keyId);

        /**
         * @brief Returns a KMS key by primary key
         *
         * @param oid key primary key
         * @return key entity
         * @throws DatabaseException
         */
        Entity::KMS::Key GetKeyById(const std::string &oid);

        /**
         * @brief Returns a KMS key by key ID
         *
         * @param keyId key ID
         * @return key entity
         * @throws DatabaseException
         */
        Entity::KMS::Key GetKeyByKeyId(const std::string &keyId);

        /**
         * @brief List all keys
         *
         * @param region AWS region
         * @return KeyList
         * @see Database::Entity::KMS::Key
         */
        Entity::KMS::KeyList ListKeys(const std::string &region = {}) const;

        /**
         * Returns the total number of keys
         *
         * @return total number of keys
         * @throws DatabaseException
         */
        long CountKeys() const;

        /**
         * @brief Create a new key in the KMS key table
         *
         * @param key key entity
         * @return created KMS key entity
         * @throws DatabaseException
         */
        Entity::KMS::Key CreateKey(const Entity::KMS::Key &key);

        /**
         * @brief Updates a key
         *
         * @param key key entity
         * @return created key entity
         * @throws DatabaseException
         */
        Entity::KMS::Key UpdateKey(const Entity::KMS::Key &key);

        /**
         * @brief Delete a key
         *
         * @param key key entity
         * @throws DatabaseException
         */
        void DeleteKey(const Entity::KMS::Key &key);

        /**
         * @brief Delete all keys
         *
         * @return number of entities deleted
         * @throws DatabaseException
         */
        long DeleteAllKeys();

      private:

        /**
         * KMS topic vector, when running without database
         */
        std::map<std::string, Entity::KMS::Key> _keys;

        /**
         * Key mutex
         */
        static boost::mutex _keyMutex;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_KMS_MEMORYDB_H
