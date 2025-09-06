//
// Created by vogje01 on 06/07/2025
//

#include <awsmock/repository/ApiGatewayDatabase.h>

namespace AwsMock::Database {
    ApiGatewayDatabase::ApiGatewayDatabase() : _databaseName(GetDatabaseName()), _apiKeyCollectionName("apigateway_key"), _memoryDb(ApiGatewayMemoryDb::instance()) {}

    bool ApiGatewayDatabase::ApiKeyExists(const std::string &region, const std::string &name) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _apiKeyCollection = client->database(_databaseName)[_apiKeyCollectionName];
                const int64_t count = _apiKeyCollection.count_documents(make_document(kvp("region", region), kvp("name", name)));
                log_trace << "API key exists: " << std::boolalpha << count;
                return count > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.ApiKeyExists(region, name);
    }

    bool ApiGatewayDatabase::ApiKeyExists(const std::string &id) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _apiKeyCollection = client->database(_databaseName)[_apiKeyCollectionName];
                const int64_t count = _apiKeyCollection.count_documents(make_document(kvp("id", id)));
                log_trace << "API key exists: " << std::boolalpha << count;
                return count > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.ApiKeyExists(id);
    }

    Entity::ApiGateway::Key ApiGatewayDatabase::CreateKey(Entity::ApiGateway::Key &key) const {

        key.created = system_clock::now();
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection apiKeyCollection = client->database(_databaseName)[_apiKeyCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = apiKeyCollection.insert_one(key.ToDocument());
                session.commit_transaction();
                log_trace << "Key created, oid: " << result->inserted_id().get_oid().value.to_string();
                key.oid = result->inserted_id().get_oid().value.to_string();
                return key;

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.CreateKey(key);
    }

    std::vector<Entity::ApiGateway::Key> ApiGatewayDatabase::GetApiKeys(const std::string &nameQuery, const std::string &customerId, const std::string &position, const long limit) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection apiKeyCollection = client->database(_databaseName)[_apiKeyCollectionName];
            auto session = client->start_session();

            try {
                std::vector<Entity::ApiGateway::Key> apiKeyList;

                mongocxx::options::find opts;
                document sort = {};
                sort.append(kvp("id", 1));
                opts.sort(sort.extract());

                if (limit > 0) {
                    opts.limit(limit);
                }

                document query = {};
                if (!nameQuery.empty()) {
                    query.append(kvp("name", make_document(kvp("$regex", "^" + nameQuery))));
                }

                if (!position.empty()) {
                    query.append(kvp("id", make_document(kvp("$gt", position))));
                }

                for (auto bucketCursor = apiKeyCollection.find(query.extract(), opts); const auto &bucket: bucketCursor) {
                    Entity::ApiGateway::Key result;
                    result.FromDocument(bucket);
                    apiKeyList.push_back(result);
                }
                return apiKeyList;

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.GetApiKeys(nameQuery, customerId, position, limit);
    }

    Entity::ApiGateway::Key ApiGatewayDatabase::GetApiKeyById(const std::string &id) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection apiKeyCollection = client->database(_databaseName)[_apiKeyCollectionName];
            auto session = client->start_session();

            try {
                std::vector<Entity::ApiGateway::Key> apiKeyList;

                document query = {};
                if (!id.empty()) {
                    query.append(kvp("id", id));
                }
                if (const auto result = apiKeyCollection.find_one(query.extract()); result.has_value()) {
                    Entity::ApiGateway::Key apiKey;
                    apiKey.FromDocument(*result);
                    return apiKey;
                }

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.GetApiKeyById(id);
    }

    Entity::ApiGateway::Key ApiGatewayDatabase::UpdateApiKey(Entity::ApiGateway::Key &key) const {

        key.modified = system_clock::now();
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection apiKeyCollection = (*client)[_databaseName][_apiKeyCollectionName];
            auto session = client->start_session();

            try {

                mongocxx::options::find_one_and_update opts{};
                opts.return_document(mongocxx::options::return_document::k_after);

                document query = {};
                query.append(kvp("id", key.id));

                session.start_transaction();
                const auto mResult = apiKeyCollection.find_one_and_update(query.extract(), key.ToDocument(), opts);
                session.commit_transaction();

                if (mResult) {
                    key.FromDocument(mResult->view());
                    log_trace << "API key updated, id: " << key.id;
                    return key;
                }
                return {};

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.UpdateApiKey(key);
    }

    void ApiGatewayDatabase::ImportApiKey(Entity::ApiGateway::Key &key) const {

        if (ApiKeyExists(key.id)) {
            key = UpdateApiKey(key);
        }
        key = CreateKey(key);
    }

    long ApiGatewayDatabase::CountApiKeys() const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection apiKeyCollection = (*client)[_databaseName][_apiKeyCollectionName];
                const int64_t count = apiKeyCollection.count_documents({});
                log_trace << "API key count: " << count;
                return count;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.CountApiKeys();
    }

    void ApiGatewayDatabase::DeleteKey(const std::string &id) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection apiKeyCollection = client->database(_databaseName)[_apiKeyCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = apiKeyCollection.delete_one(make_document(kvp("id", id)));
                session.commit_transaction();
                log_trace << "Key deleted, count: " << result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.DeleteKey(id);
    }

    std::vector<Entity::ApiGateway::Key> ApiGatewayDatabase::ListApiKeyCounters(const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection apiKeyCollection = client->database(_databaseName)[_apiKeyCollectionName];
            auto session = client->start_session();

            try {
                std::vector<Entity::ApiGateway::Key> apiKeyList;

                mongocxx::options::find opts;
                if (!sortColumns.empty()) {
                    document sort = {};
                    for (const auto &[column, sortDirection]: sortColumns) {
                        sort.append(kvp(column, sortDirection));
                    }
                    opts.sort(sort.extract());
                }
                if (pageIndex > 0) {
                    opts.skip(pageSize * pageIndex);
                }
                if (pageSize > 0) {
                    opts.limit(pageSize);
                }

                document query = {};
                if (!prefix.empty()) {
                    query.append(kvp("functionName", make_document(kvp("$regex", "^" + prefix))));
                }

                for (auto lambdaCursor = apiKeyCollection.find(query.extract(), opts); auto lambda: lambdaCursor) {
                    Entity::ApiGateway::Key result;
                    result.FromDocument(lambda);
                    apiKeyList.push_back(result);
                }
                return apiKeyList;

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.ListApiKeyCounters(prefix, pageSize, pageIndex, sortColumns);
    }

}// namespace AwsMock::Database
