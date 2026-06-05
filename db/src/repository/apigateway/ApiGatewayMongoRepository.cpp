//
// Created by vogje01 on 06/07/2025
//

#include <awsmock/repository/apigateway/ApiGatewayMongoRepository.h>

namespace Awsmock::Database {

    // ========================================================================================================================
    // API key
    // ========================================================================================================================
    bool ApiGatewayMongoRepository::apiKeyExists(const std::string &region, const std::string &name) const {

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

    bool ApiGatewayMongoRepository::apiKeyExists(const std::string &id) const {

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

    Entity::ApiGateway::ApiKey ApiGatewayMongoRepository::createKey(Entity::ApiGateway::ApiKey &key) const {

        key.created = system_clock::now();

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection apiKeyCollection = client->database(_databaseName)[_apiKeyCollectionName];

        try {

            const auto result = apiKeyCollection.insert_one(key.ToDocument());
            log_trace << "Key created, oid: " << result->inserted_id().get_oid().value.to_string();
            key.oid = result->inserted_id().get_oid().value.to_string();
            return key;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    std::vector<Entity::ApiGateway::ApiKey> ApiGatewayMongoRepository::getApiKeys(const std::string &nameQuery, const std::string &customerId, const std::string &position, const long limit) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection apiKeyCollection = client->database(_databaseName)[_apiKeyCollectionName];

        try {
            std::vector<Entity::ApiGateway::ApiKey> apiKeyList;

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
                Entity::ApiGateway::ApiKey result;
                result.FromDocument(bucket);
                apiKeyList.push_back(result);
            }
            return apiKeyList;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::ApiGateway::ApiKey ApiGatewayMongoRepository::getApiKeyById(const std::string &id) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection apiKeyCollection = client->database(_databaseName)[_apiKeyCollectionName];

        try {
            std::vector<Entity::ApiGateway::ApiKey> apiKeyList;

            document query = {};
            if (!id.empty()) {
                query.append(kvp("id", id));
            }
            if (const auto result = apiKeyCollection.find_one(query.extract()); result.has_value()) {
                Entity::ApiGateway::ApiKey apiKey;
                apiKey.FromDocument(*result);
                return apiKey;
            }
            return {};

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::ApiGateway::ApiKey ApiGatewayMongoRepository::updateApiKey(Entity::ApiGateway::ApiKey &key) const {

        key.modified = system_clock::now();

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection apiKeyCollection = (*client)[_databaseName][_apiKeyCollectionName];

        try {

            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            document query = {};
            query.append(kvp("id", key.id));

            const auto mResult = apiKeyCollection.find_one_and_update(query.extract(), key.ToDocument(), opts);

            if (mResult) {
                key.FromDocument(mResult->view());
                log_trace << "API key updated, id: " << key.id;
                return key;
            }
            return {};

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    void ApiGatewayMongoRepository::importApiKey(Entity::ApiGateway::ApiKey &key) const {

        if (apiKeyExists(key.id)) {
            key = updateApiKey(key);
        }
        key = createKey(key);
    }

    long ApiGatewayMongoRepository::countApiKeys() const {

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

    void ApiGatewayMongoRepository::deleteKey(const std::string &id) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection apiKeyCollection = client->database(_databaseName)[_apiKeyCollectionName];

        try {

            const auto result = apiKeyCollection.delete_one(make_document(kvp("id", id)));
            log_trace << "Key deleted, count: " << result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    long ApiGatewayMongoRepository::deleteAllKeys() const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection apiKeyCollection = client->database(_databaseName)[_apiKeyCollectionName];

        try {

            const auto result = apiKeyCollection.delete_many({});
            log_trace << "Key deleted, count: " << result->deleted_count();
            return result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    // ========================================================================================================================
    // REST API
    // ========================================================================================================================
    bool ApiGatewayMongoRepository::restApiExists(const std::string &region, const std::string &name) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection restApiCollection = client->database(_databaseName)[_restApiCollectionName];
            const int64_t count = restApiCollection.count_documents(make_document(kvp("region", region), kvp("name", name)));
            log_trace << "REST API exists: " << std::boolalpha << count;
            return count > 0;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    bool ApiGatewayMongoRepository::restApiExists(const std::string &oid) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection restApiCollection = client->database(_databaseName)[_restApiCollectionName];
            const int64_t count = restApiCollection.count_documents(make_document(kvp("_id", bsoncxx::oid(oid))));
            log_trace << "REST API exists: " << std::boolalpha << count;
            return count > 0;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::ApiGateway::RestApi ApiGatewayMongoRepository::createRestApi(Entity::ApiGateway::RestApi &restApi) const {

        restApi.created = system_clock::now();
        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection restApiCollection = client->database(_databaseName)[_restApiCollectionName];

        try {

            const auto result = restApiCollection.insert_one(restApi.ToDocument());
            log_trace << "Key created, oid: " << result->inserted_id().get_oid().value.to_string();
            restApi.oid = result->inserted_id().get_oid().value.to_string();
            return restApi;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::ApiGateway::RestApi ApiGatewayMongoRepository::upsertRestApi(Entity::ApiGateway::RestApi &restApi) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection restApiCollection = client->database(_databaseName)[_restApiCollectionName];

        try {

            const auto filter = make_document(kvp("name", restApi.name));
            const auto update = make_document(
                    kvp("$set", restApi.ToDocument()),
                    kvp("$setOnInsert", make_document(
                                                kvp("created", bsoncxx::types::b_date{
                                                                       std::chrono::duration_cast<std::chrono::milliseconds>(
                                                                               restApi.created.time_since_epoch())}))),
                    kvp("$currentDate", make_document(kvp("modified", true))));

            mongocxx::options::find_one_and_update opts;
            opts.upsert(true);
            opts.return_document(mongocxx::options::return_document::k_after);
            const auto mResult = restApiCollection.find_one_and_update(filter.view(), update.view(), opts);
            if (!mResult) {
                throw Core::DatabaseException("find_one_and_update returned no document");
            }
            restApi.FromDocument(mResult->view());
            return restApi;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    // ========================================================================================================================
    // AwsMock internal
    // ========================================================================================================================
    std::vector<Entity::ApiGateway::ApiKey> ApiGatewayMongoRepository::listApiKeyCounters(const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection apiKeyCollection = client->database(_databaseName)[_apiKeyCollectionName];

        try {
            std::vector<Entity::ApiGateway::ApiKey> apiKeyList;

            mongocxx::options::find opts;
            if (!sortColumns.empty()) {
                document sort = {};
                for (const auto &sortColumn: sortColumns) {
                    sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
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
                Entity::ApiGateway::ApiKey result;
                result.FromDocument(lambda);
                apiKeyList.push_back(result);
            }
            return apiKeyList;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    std::vector<Entity::ApiGateway::RestApi> ApiGatewayMongoRepository::listRestApiCounters(const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection restapiCollection = client->database(_databaseName)[_restApiCollectionName];

        try {
            std::vector<Entity::ApiGateway::RestApi> restApiList;

            mongocxx::options::find opts;
            if (!sortColumns.empty()) {
                document sort = {};
                for (const auto &sortColumn: sortColumns) {
                    sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
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

            for (auto lambdaCursor = restapiCollection.find(query.extract(), opts); auto lambda: lambdaCursor) {
                Entity::ApiGateway::RestApi result;
                result.FromDocument(lambda);
                restApiList.push_back(result);
            }
            return restApiList;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

}// namespace Awsmock::Database
