//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/LambdaDatabase.h>

namespace AwsMock::Database {

    LambdaDatabase::LambdaDatabase() : _databaseName(GetDatabaseName()), _lambdaCollectionName("lambda"), _lambdaResultCollectionName("lambdaResult"), _memoryDb(LambdaMemoryDb::instance()) {}

    bool LambdaDatabase::LambdaExists(const std::string &region, const std::string &function, const std::string &runtime) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];
                const int64_t count = _lambdaCollection.count_documents(make_document(kvp("region", region), kvp("function", function), kvp("runtime", runtime)));
                log_trace << "lambda function exists: " << std::boolalpha << count;
                return count > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.LambdaExists(region, function, runtime);
    }

    auto LambdaDatabase::LambdaExists(const Entity::Lambda::Lambda &lambda) const -> bool {

        return LambdaExists(lambda.region, lambda.function, lambda.runtime);
    }

    bool LambdaDatabase::LambdaExists(const std::string &functionName) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];
                const int64_t count = _lambdaCollection.count_documents(make_document(kvp("function", functionName)));
                log_trace << "lambda function exists: " << std::boolalpha << count;
                return count > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.LambdaExists(functionName);
    }

    bool LambdaDatabase::LambdaExistsByArn(const std::string &arn) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];
                const int64_t count = _lambdaCollection.count_documents(make_document(kvp("arn", arn)));
                log_trace << "lambda function exists: " << std::boolalpha << count;
                return count > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.LambdaExistsByArn(arn);
    }

    int LambdaDatabase::LambdaCount(const std::string &region) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];

                long count = 0;
                if (region.empty()) {
                    count = _lambdaCollection.count_documents({});
                } else {
                    count = _lambdaCollection.count_documents(make_document(kvp("region", region)));
                }

                log_trace << "lambda count: " << count;
                return count;

            } catch (mongocxx::exception::system_error &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.LambdaCount(region);
    }

    Entity::Lambda::Lambda LambdaDatabase::CreateLambda(const Entity::Lambda::Lambda &lambda) const {

        if (HasDatabase()) {

            try {

                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];

                auto result = _lambdaCollection.insert_one(lambda.ToDocument());
                log_trace << "Bucket created, oid: " << result->inserted_id().get_oid().value.to_string();
                return GetLambdaById(result->inserted_id().get_oid().value);

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }

        } else {

            return _memoryDb.CreateLambda(lambda);
        }
    }

    Entity::Lambda::Lambda LambdaDatabase::GetLambdaById(bsoncxx::oid oid) const {

        try {

            auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];
            const auto mResult = _lambdaCollection.find_one(make_document(kvp("_id", oid)));
            if (!mResult) {
                log_error << "Database exception: Lambda not found ";
                throw Core::DatabaseException("Database exception, Lambda not found ");
            }

            Entity::Lambda::Lambda result;
            result.FromDocument(mResult->view());
            return result;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::Lambda::Lambda LambdaDatabase::GetLambdaById(const std::string &oid) const {

        if (HasDatabase()) {

            return GetLambdaById(bsoncxx::oid(oid));
        }
        return _memoryDb.GetLambdaById(oid);
    }

    Entity::Lambda::Lambda LambdaDatabase::GetLambdaByArn(const std::string &arn) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];
                const auto mResult = _lambdaCollection.find_one(make_document(kvp("arn", arn)));
                if (!mResult) {
                    log_error << "Database exception: Lambda not found ";
                    throw Core::DatabaseException("Database exception, Lambda not found ");
                }

                Entity::Lambda::Lambda result;
                result.FromDocument(mResult->view());
                return result;

            } catch (mongocxx::exception::system_error &e) {
                log_error << "Get lambda by ARN failed, error: " << e.what();
            }

        } else {

            return _memoryDb.GetLambdaByArn(arn);
        }
        return {};
    }

    Entity::Lambda::Lambda LambdaDatabase::GetLambdaByName(const std::string &region, const std::string &name) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];
                const auto mResult = _lambdaCollection.find_one(make_document(kvp("region", region), kvp("function", name)));
                if (!mResult) {
                    log_error << "Database exception: Lambda not found ";
                    throw Core::DatabaseException("Database exception, Lambda not found ");
                }

                Entity::Lambda::Lambda result;
                result.FromDocument(mResult->view());
                return result;

            } catch (mongocxx::exception::system_error &e) {
                log_error << "Get lambda by ARN failed, error: " << e.what();
            }

        } else {

            return _memoryDb.GetLambdaByName(region, name);
        }
        return {};
    }

    Entity::Lambda::Lambda LambdaDatabase::CreateOrUpdateLambda(const Entity::Lambda::Lambda &lambda) const {

        if (LambdaExists(lambda)) {
            return UpdateLambda(lambda);
        }
        return CreateLambda(lambda);
    }

    Entity::Lambda::Lambda LambdaDatabase::UpdateLambda(const Entity::Lambda::Lambda &lambda) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];
                auto result = _lambdaCollection.find_one_and_update(make_document(kvp("region", lambda.region), kvp("function", lambda.function), kvp("runtime", lambda.runtime)), lambda.ToDocument());
                log_trace << "Lambda updated: " << lambda.ToString();
                return GetLambdaByArn(lambda.arn);

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.UpdateLambda(lambda);
    }

    Entity::Lambda::Lambda LambdaDatabase::ImportLambda(Entity::Lambda::Lambda &lambda) const {

        if (LambdaExists(lambda)) {
            const Entity::Lambda::Lambda existing = GetLambdaByArn(lambda.arn);
            lambda.modified = system_clock::now();
            lambda.instances = existing.instances;
        }
        return CreateOrUpdateLambda(lambda);
    }


    void LambdaDatabase::SetInstanceStatus(const std::string &containerId, const Entity::Lambda::LambdaInstanceStatus &status) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                _lambdaCollection.update_one(make_document(kvp("instances.containerId", containerId)),
                                             make_document(kvp("$set", make_document(kvp("instances.$.status", LambdaInstanceStatusToString(status))))));
                session.commit_transaction();

            } catch (mongocxx::exception::system_error &e) {
                session.abort_transaction();
                log_error << "Get lambda by ARN failed, error: " << e.what();
            }

        } else {

            _memoryDb.SetInstanceStatus(containerId, status);
        }
    }

    void LambdaDatabase::SetLastInvocation(const std::string &oid, const system_clock::time_point &lastInvocation) const {
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                _lambdaCollection.update_one(make_document(kvp("_id", bsoncxx::oid(oid))),
                                             make_document(kvp("$set", make_document(kvp("lastInvocation", bsoncxx::types::b_date(lastInvocation))))));
                session.commit_transaction();

            } catch (mongocxx::exception::system_error &e) {
                log_error << "Set last invocation failed, error: " << e.what();
            }
        } else {

            _memoryDb.SetLastInvocation(oid, lastInvocation);
        }
    }

    void LambdaDatabase::SetAverageRuntime(const std::string &oid, const long millis) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                Entity::Lambda::Lambda lambda = GetLambdaById(oid);
                lambda.invocations++;
                lambda.averageRuntime = static_cast<long>(std::round((lambda.averageRuntime + millis) / lambda.invocations));

                _lambdaCollection.find_one_and_update(make_document(kvp("_id", bsoncxx::oid(oid))), lambda.ToDocument());
                session.commit_transaction();
                log_debug << "Lambda counters updated, oid: " << oid;

            } catch (mongocxx::exception::system_error &e) {
                session.abort_transaction();
                log_error << "Set average function runtime failed, error: " << e.what();
            }

        } else {

            _memoryDb.SetAverageRuntime(oid, millis);
        }
    }

    std::vector<Entity::Lambda::Lambda> LambdaDatabase::ListLambdas(const std::string &region) const {

        if (HasDatabase()) {

            try {
                std::vector<Entity::Lambda::Lambda> lambdas;

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];

                document query;
                if (!region.empty()) {
                    query.append(kvp("region", region));
                }

                for (auto lambdaCursor = _lambdaCollection.find(query.extract()); auto lambda: lambdaCursor) {
                    Entity::Lambda::Lambda result;
                    result.FromDocument(lambda);
                    lambdas.push_back(result);
                }

                log_trace << "Got lambda list, size:" << lambdas.size();
                return lambdas;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.ListLambdas(region);
    }

    std::vector<Entity::Lambda::Lambda> LambdaDatabase::ExportLambdas(const std::vector<SortColumn> &sortColumns) const {

        std::vector<Entity::Lambda::Lambda> lambdas = ListLambdaCounters({}, {}, -1, 0, sortColumns);

        // Remove instances, as they will confuse the re-import
        for (auto &lambda: lambdas) {
            lambda.invocations = 0;
            lambda.averageRuntime = 0;
            lambda.instances.clear();
        }
        return lambdas;
    }

    std::vector<Entity::Lambda::Lambda> LambdaDatabase::ListLambdaCounters(const std::string &region, const std::string &prefix, const long maxResults, const long skip, const std::vector<SortColumn> &sortColumns) const {

        std::vector<Entity::Lambda::Lambda> lambdas;
        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];

                mongocxx::options::find opts;
                if (!sortColumns.empty()) {
                    document sort = {};
                    for (const auto sortColumn: sortColumns) {
                        sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
                    }
                    opts.sort(sort.extract());
                }
                if (skip > 0) {
                    opts.skip(skip);
                }
                if (maxResults > 0) {
                    opts.limit(maxResults);
                }

                document query = {};
                if (!region.empty()) {
                    query.append(kvp("region", region));
                }
                if (!prefix.empty()) {
                    query.append(kvp("functionName", make_document(kvp("$regex", "^" + prefix))));
                }

                for (auto lambdaCursor = _lambdaCollection.find(query.extract(), opts); auto lambda: lambdaCursor) {
                    Entity::Lambda::Lambda result;
                    result.FromDocument(lambda);
                    lambdas.push_back(result);
                }

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }

        log_trace << "Got lambda counter list, size:" << lambdas.size();
        return lambdas;
    }

    std::vector<Entity::Lambda::Lambda> LambdaDatabase::ListLambdasWithEventSource(const std::string &eventSourceArn) const {

        if (HasDatabase()) {

            try {
                std::vector<Entity::Lambda::Lambda> lambdas;

                document query;
                query.append(kvp("eventSources", make_document(kvp("$elemMatch", make_document(kvp("eventSourceArn", eventSourceArn))))));

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];
                for (auto lambdaCursor = _lambdaCollection.find(query.extract()); auto lambda: lambdaCursor) {
                    Entity::Lambda::Lambda result;
                    result.FromDocument(lambda);
                    lambdas.push_back(result);
                }

                log_trace << "Got lambda list, size:" << lambdas.size();
                return lambdas;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.ListLambdasWithEventSource(eventSourceArn);
    }

    Entity::Lambda::LambdaResult LambdaDatabase::CreateLambdaResult(Entity::Lambda::LambdaResult &lambdaResult) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaResultsCollection = (*client)[_databaseName][_lambdaResultCollectionName];

                const auto result = _lambdaResultsCollection.insert_one(lambdaResult.ToDocument());
                log_trace << "Lambda result created, oid: " << result->inserted_id().get_oid().value.to_string();
                lambdaResult.oid = result->inserted_id().get_oid().value.to_string();
                return lambdaResult;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.CreateLambdaResult(lambdaResult);
    }


    Entity::Lambda::LambdaResult LambdaDatabase::GetLambdaResultCounter(const std::string &oid) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaResultsCollection = (*client)[_databaseName][_lambdaResultCollectionName];

                const auto result = _lambdaResultsCollection.find_one(make_document(kvp("_id", bsoncxx::oid(oid))));
                log_trace << "Lambda result found, oid: " << oid;
                if (!result->empty()) {
                    Entity::Lambda::LambdaResult lambdaResult;
                    lambdaResult.FromDocument(result->view());
                    return lambdaResult;
                }

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return {};
    }

    std::vector<Entity::Lambda::LambdaResult> LambdaDatabase::ListLambdaResultCounters(const std::string &lambdaArn, const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        std::vector<Entity::Lambda::LambdaResult> lambdaResults;
        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaResultCollection = (*client)[_databaseName][_lambdaResultCollectionName];

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
                if (!lambdaArn.empty()) {
                    query.append(kvp("arn", lambdaArn));
                }

                for (auto lambdaCursor = _lambdaResultCollection.find(query.extract(), opts); auto lambda: lambdaCursor) {
                    Entity::Lambda::LambdaResult result;
                    result.FromDocument(lambda);
                    lambdaResults.push_back(result);
                }

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }

        log_trace << "Got lambda result counter list, size:" << lambdaResults.size();
        return lambdaResults;
    }

    long LambdaDatabase::LambdaResultsCount(const std::string &lambdaArn) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaResultCollection = (*client)[_databaseName][_lambdaResultCollectionName];

                document query = {};
                if (!lambdaArn.empty()) {
                    query.append(kvp("arn", lambdaArn));
                }

                const int64_t count = _lambdaResultCollection.count_documents(query.extract());
                log_trace << "Got lambda result count, size:" << count;
                return count;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return 0;
    }

    long LambdaDatabase::DeleteResultsCounter(const std::string &oid) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaResultCollection = (*client)[_databaseName][_lambdaResultCollectionName];
                const auto result = _lambdaResultCollection.delete_one(make_document(kvp("_id", bsoncxx::oid(oid))));
                log_trace << "Lambda result deleted, size:" << result->deleted_count();
                return result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return 0;
    }

    long LambdaDatabase::DeleteResultsCounters(const std::string &lambdaArn) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaResultCollection = (*client)[_databaseName][_lambdaResultCollectionName];
                const auto result = _lambdaResultCollection.delete_many(make_document(kvp("arn", lambdaArn)));
                log_trace << "Lambda results deleted, size:" << result->deleted_count();
                return result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return 0;
    }

    long LambdaDatabase::RemoveExpiredLambdaLogs(const system_clock::time_point &cutOff) const {
        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaResultsCollection = (*client)[_databaseName][_lambdaResultCollectionName];

                document query;
                query.append(kvp("timestamp", make_document(kvp("$lt", bsoncxx::types::b_date(cutOff)))));
                const auto result = _lambdaResultsCollection.delete_many(query.extract());
                log_trace << "Lambda results deleted, count: " << result->deleted_count();
                return result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.RemoveExpiredLambdaLogs(cutOff);
    }

    void LambdaDatabase::DeleteLambda(const std::string &functionName) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];
                const auto result = _lambdaCollection.delete_many(make_document(kvp("function", functionName)));
                log_debug << "lambda deleted, function: " << functionName << " count: " << result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }

        } else {

            _memoryDb.DeleteLambda(functionName);
        }
    }

    long LambdaDatabase::DeleteAllLambdas() const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];
                const auto result = _lambdaCollection.delete_many({});
                log_debug << "All lambdas deleted, count: " << result->deleted_count();
                return result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.DeleteAllLambdas();
    }

}// namespace AwsMock::Database
