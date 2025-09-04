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

    Entity::ApiGateway::Key ApiGatewayDatabase::CreateKey(Entity::ApiGateway::Key &key) const {

        key.created = system_clock::now();
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _applicationCollection = client->database(_databaseName)[_apiKeyCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = _applicationCollection.insert_one(key.ToDocument());
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

}// namespace AwsMock::Database
