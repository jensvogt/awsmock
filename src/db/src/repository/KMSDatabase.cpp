//
// Created by vogje01 on 29/05/2023.
//

#include "awsmock/entity/s3/Object.h"
#include <awsmock/repository/KMSDatabase.h>

namespace AwsMock::Database {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::builder::stream::document;

    KMSDatabase::KMSDatabase() : _memoryDb(KMSMemoryDb::instance()), _useDatabase(HasDatabase()), _databaseName(GetDatabaseName()), _keyCollectionName("kms_key") {}

    bool KMSDatabase::KeyExists(const std::string &keyId) {

        if (_useDatabase) {

            try {

                auto client = GetClient();
                mongocxx::collection _keyCollection = (*client)[_databaseName][_keyCollectionName];
                int64_t count = _keyCollection.count_documents(make_document(kvp("keyId", keyId)));
                log_trace << "Topic exists: " << (count > 0 ? "true" : "false");
                return count > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "KMS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            return _memoryDb.KeyExists(keyId);
        }
    }

    Entity::KMS::Key KMSDatabase::GetKeyById(bsoncxx::oid oid) {

        try {

            auto client = GetClient();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_keyCollectionName];
            mongocxx::stdx::optional<bsoncxx::document::value> mResult = _topicCollection.find_one(make_document(kvp("_id", oid)));
            if (!mResult.has_value()) {
                log_error << "KMS key not found, oid" << oid.to_string();
                throw Core::DatabaseException("KMS key not found, oid" + oid.to_string());
            }

            Entity::KMS::Key result;
            result.FromDocument(mResult);
            return result;


        } catch (const mongocxx::exception &exc) {
            log_error << "KMS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::KMS::Key KMSDatabase::GetKeyById(const std::string &oid) {

        if (_useDatabase) {

            return GetKeyById(bsoncxx::oid(oid));

        } else {

            return _memoryDb.GetKeyById(oid);
        }
    }

    Entity::KMS::Key KMSDatabase::GetKeyByKeyId(const std::string &keyId) {

        if (_useDatabase) {

            auto client = GetClient();
            mongocxx::collection _keyCollection = (*client)[_databaseName][_keyCollectionName];

            try {

                mongocxx::stdx::optional<bsoncxx::document::value> mResult = _keyCollection.find_one(make_document(kvp("keyId", keyId)));
                if (!mResult.has_value()) {
                    log_error << "KMS key not found, keyId: " << keyId;
                    throw Core::DatabaseException("KMS key not found, keyId" + keyId);
                }

                Entity::KMS::Key result;
                result.FromDocument(mResult);
                return result;


            } catch (const mongocxx::exception &exc) {
                log_error << "KMS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {

            return _memoryDb.GetKeyByKeyId(keyId);
        }
    }

    Entity::KMS::KeyList KMSDatabase::ListKeys(const std::string &region) {

        Entity::KMS::KeyList keyList;
        if (_useDatabase) {

            auto client = GetClient();
            mongocxx::collection _keyCollection = (*client)[_databaseName][_keyCollectionName];

            if (region.empty()) {

                auto keyCursor = _keyCollection.find({});
                for (const auto &key: keyCursor) {
                    Entity::KMS::Key result;
                    result.FromDocument(key);
                    keyList.push_back(result);
                }
            } else {

                auto keyCursor = _keyCollection.find(make_document(kvp("region", region)));
                for (const auto &key: keyCursor) {
                    Entity::KMS::Key result;
                    result.FromDocument(key);
                    keyList.push_back(result);
                }
            }

        } else {

            keyList = _memoryDb.ListKeys(region);
        }

        log_trace << "Got key list, size:" << keyList.size();
        return keyList;
    }

    Entity::KMS::Key KMSDatabase::CreateKey(const Entity::KMS::Key &key) {

        if (_useDatabase) {

            auto client = GetClient();
            mongocxx::collection _keyCollection = (*client)[_databaseName][_keyCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                auto result = _keyCollection.insert_one(key.ToDocument());
                session.commit_transaction();
                log_trace << "Key created, oid: " << result->inserted_id().get_oid().value.to_string();

                return GetKeyById(result->inserted_id().get_oid().value);

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "KMS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            return _memoryDb.CreateKey(key);
        }
    }

    Entity::KMS::Key KMSDatabase::UpdateKey(const Entity::KMS::Key &key) {

        if (_useDatabase) {

            auto client = GetClient();
            mongocxx::collection _keyCollection = (*client)[_databaseName][_keyCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                auto result = _keyCollection.replace_one(make_document(kvp("keyId", key.keyId)), key.ToDocument());
                log_trace << "Bucket updated: " << key.ToString();
                session.commit_transaction();
                return GetKeyByKeyId(key.keyId);

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what(), 500);
            }

        } else {

            return _memoryDb.UpdateKey(key);
        }
    }

    void KMSDatabase::DeleteKey(const Entity::KMS::Key &key) {

        if (_useDatabase) {

            auto client = GetClient();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_keyCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                auto delete_many_result = _bucketCollection.delete_one(make_document(kvp("name", key.keyId)));
                session.commit_transaction();
                log_debug << "KMS key deleted, count: " << delete_many_result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what(), 500);
            }

        } else {

            _memoryDb.DeleteKey(key);
        }
    }
}// namespace AwsMock::Database