//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/kms/KMSMongoRepository.h>

namespace Awsmock::Database {

    bool KMSMongoRepository::keyExists(const std::string &keyId) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _keyCollection = (*client)[_databaseName][_keyCollectionName];
            const int64_t count = _keyCollection.count_documents(make_document(kvp("keyId", keyId)));
            log_trace << "Topic exists: " << std::boolalpha << count;
            return count > 0;

        } catch (const mongocxx::exception &exc) {
            log_error << "KMS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::KMS::Key KMSMongoRepository::getKeyById(const bsoncxx::oid &oid) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_keyCollectionName];
            const auto mResult = _topicCollection.find_one(make_document(kvp("_id", oid)));
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

    Entity::KMS::Key KMSMongoRepository::getKeyById(const std::string &oid) const {
        return getKeyById(bsoncxx::oid(oid));
    }

    Entity::KMS::Key KMSMongoRepository::getKeyByKeyId(const std::string &keyId) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _keyCollection = (*client)[_databaseName][_keyCollectionName];

        try {

            const auto mResult = _keyCollection.find_one(make_document(kvp("keyId", keyId)));
            if (!mResult) {
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
    }

    Entity::KMS::KeyList KMSMongoRepository::listKeys(const std::string &region, const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {


        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _keyCollection = (*client)[_databaseName][_keyCollectionName];

        mongocxx::options::find opts;
        if (!sortColumns.empty()) {
            bsoncxx::builder::basic::document sort = {};
            for (const auto &sortColumn: sortColumns) {
                sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
            }
            opts.sort(sort.extract());
        }
        if (pageSize > 0) {
            opts.skip(pageSize * pageIndex);
        }
        if (pageSize > 0) {
            opts.limit(pageSize);
        }

        document query = {};
        if (!region.empty()) {
            query.append(kvp("region", region));
        }
        if (!prefix.empty()) {
            query.append(kvp("keyId", make_document(kvp("$regex", "^" + prefix))));
        }

        Entity::KMS::KeyList keyList;
        for (auto keyCursor = _keyCollection.find(query.extract(), opts); const auto &key: keyCursor) {
            Entity::KMS::Key result;
            result.FromDocument(key);
            keyList.push_back(result);
        }
        return keyList;
    }

    long KMSMongoRepository::countKeys() const {

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _keyCollection = (*client)[_databaseName][_keyCollectionName];

            const long count = static_cast<long>(_keyCollection.count_documents(make_document()));
            log_trace << "Key count: " << count;
            return count;

        } catch (mongocxx::exception::system_error &e) {
            log_error << "Key count failed, error: " << e.what();
        }
        return -1;
    }

    Entity::KMS::Key KMSMongoRepository::createKey(Entity::KMS::Key &key) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _keyCollection = (*client)[_databaseName][_keyCollectionName];

        try {

            const auto result = _keyCollection.insert_one(key.ToDocument());
            log_trace << "Key created, oid: " << result->inserted_id().get_oid().value.to_string();
            key.oid = result->inserted_id().get_oid().value.to_string();
            return key;

        } catch (const mongocxx::exception &exc) {
            log_error << "KMS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::KMS::Key KMSMongoRepository::updateKey(Entity::KMS::Key &key) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _keyCollection = (*client)[_databaseName][_keyCollectionName];

        try {
            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            const auto mResult = _keyCollection.find_one_and_update(make_document(kvp("keyId", key.keyId)), key.ToDocument(), opts);
            log_trace << "Key updated: " << key;

            if (mResult && !mResult->empty()) {
                key.FromDocument(mResult->view());
                return key;
            }
            return key;

        } catch (const mongocxx::exception &exc) {
            log_error << "KMS Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::KMS::Key KMSMongoRepository::upsertKey(Entity::KMS::Key &key) const {
        if (keyExists(key.keyId)) {
            return updateKey(key);
        }
        return createKey(key);
    }

    void KMSMongoRepository::deleteKey(const Entity::KMS::Key &key) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _keyCollection = (*client)[_databaseName][_keyCollectionName];

        try {

            const auto delete_many_result = _keyCollection.delete_one(make_document(kvp("keyId", key.keyId)));
            log_debug << "KMS key deleted, count: " << delete_many_result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    long KMSMongoRepository::deleteAllKeys() const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _keyCollection = (*client)[_databaseName][_keyCollectionName];

        try {

            const auto result = _keyCollection.delete_many({});
            return result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

} // namespace Awsmock::Database
