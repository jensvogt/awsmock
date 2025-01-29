//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/KMSDatabase.h>

namespace AwsMock::Database {

    bool KMSDatabase::KeyExists(const std::string &keyId) {

        if (HasDatabase()) {

            try {

                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _keyCollection = (*client)[_databaseName][_keyCollectionName];
                int64_t count = _keyCollection.count_documents(make_document(kvp("keyId", keyId)));
                log_trace << "Topic exists: " << std::boolalpha << count;
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

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_keyCollectionName];
            std::optional<bsoncxx::document::value> mResult = _topicCollection.find_one(make_document(kvp("_id", oid)));
            if (mResult->empty()) {
                log_error << "KMS key not found, oid" << oid.to_string();
                throw Core::DatabaseException("KMS key not found, oid" + oid.to_string());
            }

            Entity::KMS::Key result;
            result.FromDocument(mResult->view());
            return result;


        } catch (const mongocxx::exception &exc) {
            log_error << "KMS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::KMS::Key KMSDatabase::GetKeyById(const std::string &oid) {

        if (HasDatabase()) {

            return GetKeyById(bsoncxx::oid(oid));

        } else {

            return _memoryDb.GetKeyById(oid);
        }
    }

    Entity::KMS::Key KMSDatabase::GetKeyByKeyId(const std::string &keyId) {

        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _keyCollection = (*client)[_databaseName][_keyCollectionName];

            try {

                std::optional<bsoncxx::document::value> mResult = _keyCollection.find_one(make_document(kvp("keyId", keyId)));
                if (mResult->empty()) {
                    log_error << "KMS key not found, keyId: " << keyId;
                    throw Core::DatabaseException("KMS key not found, keyId" + keyId);
                }

                Entity::KMS::Key result;
                result.FromDocument(mResult->view());
                return result;


            } catch (const mongocxx::exception &exc) {
                log_error << "KMS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        } else {

            return _memoryDb.GetKeyByKeyId(keyId);
        }
    }

    Entity::KMS::KeyList KMSDatabase::ListKeys(const std::string &region) const {

        Entity::KMS::KeyList keyList;
        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
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

    long KMSDatabase::CountKeys() {

        if (HasDatabase()) {

            try {
                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _keyCollection = (*client)[_databaseName][_keyCollectionName];

                long count = _keyCollection.count_documents(make_document());
                log_trace << "Key count: " << count;
                return count;

            } catch (mongocxx::exception::system_error &e) {
                log_error << "Key count failed, error: " << e.what();
            }

        } else {

            return _memoryDb.CountKeys();
        }
        return -1;
    }

    Entity::KMS::Key KMSDatabase::CreateKey(const Entity::KMS::Key &key) {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
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
        }
        return _memoryDb.CreateKey(key);
    }

    Entity::KMS::Key KMSDatabase::UpdateKey(const Entity::KMS::Key &key) {

        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _keyCollection = (*client)[_databaseName][_keyCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                auto result = _keyCollection.replace_one(make_document(kvp("keyId", key.keyId)), key.ToDocument());
                log_trace << "Key updated: " << key.ToString();
                session.commit_transaction();
                return GetKeyByKeyId(key.keyId);

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "KMS Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.UpdateKey(key);
    }

    Entity::KMS::Key KMSDatabase::UpsertKey(const Entity::KMS::Key &key) {

        if (KeyExists(key.keyId)) {

            return UpdateKey(key);
        }
        return CreateKey(key);
    }

    void KMSDatabase::DeleteKey(const Entity::KMS::Key &key) const {

        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
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
                throw Core::DatabaseException(exc.what());
            }

        } else {

            _memoryDb.DeleteKey(key);
        }
    }

    void KMSDatabase::DeleteAllKeys() const {

        if (HasDatabase()) {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_keyCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                auto delete_many_result = _bucketCollection.delete_many({});
                session.commit_transaction();
                log_debug << "All KMS keys deleted, count: " << delete_many_result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            _memoryDb.DeleteAllKeys();
        }
    }

}// namespace AwsMock::Database
