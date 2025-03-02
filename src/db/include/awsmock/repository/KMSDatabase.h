//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_KMS_DATABASE_H
#define AWSMOCK_REPOSITORY_KMS_DATABASE_H

// C++ standard includes
#include <string>
#include <vector>

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/stream/document.hpp>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/entity/s3/Object.h>
#include <awsmock/memorydb/KMSMemoryDb.h>
#include <awsmock/repository/Database.h>

namespace AwsMock::Database {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::builder::stream::document;

    /**
     * @brief KMS MongoDB database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class KMSDatabase : public DatabaseBase {

      public:

        /**
         * @brief Constructor
         */
        explicit KMSDatabase() : _databaseName(GetDatabaseName()), _keyCollectionName("kms_key"), _memoryDb(KMSMemoryDb::instance()) {}

        /**
         * @brief Singleton instance
         */
        static KMSDatabase &instance() {
            static KMSDatabase kmsDatabase;
            return kmsDatabase;
        }

        /**
         * @brief Check existence of key by keyId
         *
         * @param keyId key ID
         * @return true if key already exists
         * @throws DatabaseException
         */
        bool KeyExists(const std::string &keyId) const;

        /**
         * @brief Returns a KMS key by primary key
         *
         * @param oid key primary key
         * @return key entity
         * @throws DatabaseException
         */
        Entity::KMS::Key GetKeyById(const std::string &oid) const;

        /**
         * @brief Returns a KMS key by primary key
         *
         * @param oid key primary key
         * @return key entity
         * @throws DatabaseException
         */
        Entity::KMS::Key GetKeyById(bsoncxx::oid oid) const;

        /**
         * @brief Returns a KMS key by key ID
         *
         * @param keyId key ID
         * @return key entity
         * @throws DatabaseException
         */
        Entity::KMS::Key GetKeyByKeyId(const std::string &keyId) const;

        /**
         * @brief List all keys
         *
         * @param region AWS region
         * @return KeyList
         */
        Entity::KMS::KeyList ListKeys(const std::string &region = {}) const;

        /**
         * @brief Returns the total number of keys
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
        Entity::KMS::Key CreateKey(const Entity::KMS::Key &key) const;

        /**
         * @brief Create or update a key in the KMS key table
         *
         * @param key key entity
         * @return created or updated KMS key entity
         * @throws DatabaseException
         */
        Entity::KMS::Key UpsertKey(const Entity::KMS::Key &key) const;

        /**
         * @brief Updates a key
         *
         * @param key key entity
         * @return created key entity
         * @throws DatabaseException
         */
        Entity::KMS::Key UpdateKey(const Entity::KMS::Key &key) const;

        /**
         * @brief Delete a key
         *
         * @param key key entity
         * @throws DatabaseException
         */
        void DeleteKey(const Entity::KMS::Key &key) const;

        /**
         * @brief Delete a all keys
         *
         * @throws DatabaseException
         */
        void DeleteAllKeys() const;

      private:

        /**
         * Database name
         */
        std::string _databaseName;

        /**
         * Key collection name
         */
        std::string _keyCollectionName;

        /**
         * KMS in-memory database
         */
        KMSMemoryDb &_memoryDb;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_KMS_DATABASE_H
