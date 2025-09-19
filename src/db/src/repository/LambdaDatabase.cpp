//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/LambdaDatabase.h>

namespace AwsMock::Database {

    LambdaDatabase::LambdaDatabase() : _databaseName(GetDatabaseName()), _lambdaCollectionName("lambda"), _lambdaResultCollectionName("lambda_result"), _memoryDb(LambdaMemoryDb::instance()) {}

    bool LambdaDatabase::LambdaExists(const std::string &region, const std::string &function, const std::string &runtime) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];
                const int64_t count = _lambdaCollection.count_documents(make_document(kvp("region", region), kvp("function", function), kvp("runtime", runtime)));
                log_trace << "Lambda function exists: " << std::boolalpha << count;
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

    long LambdaDatabase::LambdaCount(const std::string &region) const {

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

    Entity::Lambda::Lambda LambdaDatabase::CreateLambda(Entity::Lambda::Lambda &lambda) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];
            try {


                const auto result = _lambdaCollection.insert_one(lambda.ToDocument());
                log_trace << "Lambda created, oid: " << result->inserted_id().get_oid().value.to_string();
                lambda.oid = result->inserted_id().get_oid().value.to_string();
                return lambda;
            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.CreateLambda(lambda);
    }

    Entity::Lambda::Lambda LambdaDatabase::GetLambdaById(bsoncxx::oid oid) const {

        if (HasDatabase()) {
            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];
                const auto mResult = _lambdaCollection.find_one(make_document(kvp("_id", oid)));
                if (!mResult) {
                    log_error << "Database exception: Lambda not found ";
                    throw Core::DatabaseException("Database exception, Lambda not found ");
                }

                Entity::Lambda::Lambda result;
                result.FromDocument(mResult.value());
                return result;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.GetLambdaById(oid.to_string());
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

        }
        return _memoryDb.GetLambdaByArn(arn);
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
        }
        return _memoryDb.GetLambdaByName(region, name);
    }

    Entity::Lambda::Lambda LambdaDatabase::UpdateLambda(Entity::Lambda::Lambda &lambda) const {

        lambda.modified = system_clock::now();
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];
            auto session = client->start_session();

            try {

                mongocxx::options::find_one_and_update opts{};
                opts.return_document(mongocxx::options::return_document::k_after);

                session.start_transaction();
                auto mResult = _lambdaCollection.find_one_and_update(make_document(kvp("region", lambda.region), kvp("function", lambda.function), kvp("runtime", lambda.runtime)), lambda.ToDocument(), opts);
                session.commit_transaction();
                log_trace << "Lambda updated: " << lambda.ToString();
                if (mResult.has_value()) {
                    lambda.FromDocument(mResult.value());
                    return lambda;
                }
                return {};

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.UpdateLambda(lambda);
    }

    Entity::Lambda::Lambda LambdaDatabase::CreateOrUpdateLambda(Entity::Lambda::Lambda &lambda) const {

        if (LambdaExists(lambda)) {
            return UpdateLambda(lambda);
        }
        return CreateLambda(lambda);
    }

    Entity::Lambda::Lambda LambdaDatabase::ImportLambda(Entity::Lambda::Lambda &lambda) const {

        if (LambdaExists(lambda)) {
            const Entity::Lambda::Lambda existing = GetLambdaByArn(lambda.arn);
            lambda.modified = system_clock::now();
            lambda.instances = existing.instances;
        }
        return CreateOrUpdateLambda(lambda);
    }


    void LambdaDatabase::SetInstanceValues(const std::string &containerId, const Entity::Lambda::LambdaInstanceStatus &status) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                _lambdaCollection.update_one(make_document(kvp("instances.containerId", containerId)),
                                             make_document(kvp("$set", make_document(
                                                                               kvp("instances.$.status", LambdaInstanceStatusToString(status)),
                                                                               kvp("instances.$.lastInvocation", bsoncxx::types::b_date(system_clock::now()))))));
                session.commit_transaction();

            } catch (mongocxx::exception::system_error &e) {
                session.abort_transaction();
                log_error << "Get lambda by ARN failed, error: " << e.what();
            }

        } else {

            _memoryDb.SetInstanceValues(containerId, status);
        }
    }

    void LambdaDatabase::SetLambdaValues(const Entity::Lambda::Lambda &lambda, long invocations, long avgRuntime) const {
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                _lambdaCollection.update_one(make_document(kvp("_id", bsoncxx::oid(lambda.oid))),
                                             make_document(kvp("$set", make_document(kvp("invocations", bsoncxx::types::b_int64(invocations)), kvp("averageRuntime", bsoncxx::types::b_int64(avgRuntime))))));
                session.commit_transaction();

            } catch (mongocxx::exception::system_error &e) {
                log_error << "Set last invocation failed, error: " << e.what();
            }

        } else {

            _memoryDb.SetLambdaValues(lambda, invocations, avgRuntime);
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

    std::vector<Entity::Lambda::Lambda> LambdaDatabase::ListLambdaCounters(const std::string &region, const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];

                mongocxx::options::find opts;
                if (!sortColumns.empty()) {
                    document sort = {};
                    for (const auto &[column, sortDirection]: sortColumns) {
                        sort.append(kvp(column, sortDirection));
                    }
                    opts.sort(sort.extract());
                }
                if (pageIndex > 0) {
                    opts.skip(pageIndex);
                }
                if (pageSize > 0) {
                    opts.limit(pageSize);
                }

                document query = {};
                if (!region.empty()) {
                    query.append(kvp("region", region));
                }
                if (!prefix.empty()) {
                    query.append(kvp("functionName", make_document(kvp("$regex", "^" + prefix))));
                }

                std::vector<Entity::Lambda::Lambda> lambdas;
                for (auto lambdaCursor = _lambdaCollection.find(query.extract(), opts); auto lambda: lambdaCursor) {
                    Entity::Lambda::Lambda result;
                    result.FromDocument(lambda);
                    lambdas.push_back(result);
                }
                log_trace << "Got lambda counter list, size:" << lambdas.size();
                return lambdas;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }

        }
        return _memoryDb.ListLambdaCounters(region, prefix, pageSize, pageIndex, sortColumns);
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

    bool LambdaDatabase::LambdaResultExists(const std::string &oid) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaResultsCollection = (*client)[_databaseName][_lambdaResultCollectionName];

                const auto result = _lambdaResultsCollection.count_documents(make_document(kvp("_id", bsoncxx::oid(oid))));
                log_trace << "Lambda result exists, oid: " << oid << ", result: " << std::boolalpha << (result > 0);
                return result > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.LambdaResultExists(oid);
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
        return _memoryDb.DeleteResultsCounter(oid);
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
        return _memoryDb.DeleteResultsCounters(lambdaArn);
    }

    long LambdaDatabase::DeleteAllResultsCounters() const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _lambdaResultCollection = (*client)[_databaseName][_lambdaResultCollectionName];
                const auto result = _lambdaResultCollection.delete_many({});
                log_trace << "All lambda results deleted, size:" << result->deleted_count();
                return result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.DeleteAllResultsCounters();
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
