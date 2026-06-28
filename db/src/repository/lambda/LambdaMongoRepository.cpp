//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/lambda/LambdaMongoRepository.h>

namespace Awsmock::Database {

    bool LambdaMongoRepository::lambdaExists(const std::string &region, const std::string &function, const std::string &runtime) const {

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];

            // Set limit to 1 (Very important for performance!)
            mongocxx::options::count options;
            options.limit(1);

            return _lambdaCollection.count_documents(make_document(kvp("region", region), kvp("function", function), kvp("runtime", runtime)), options) > 0;
        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    auto LambdaMongoRepository::lambdaExists(const Entity::Lambda::Lambda &lambda) const -> bool {
        return lambdaExists(lambda.region, lambda.function, lambda.runtime);
    }

    bool LambdaMongoRepository::lambdaExists(const std::string &functionName) const {

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];

            // Set limit to 1 (Very important for performance!)
            mongocxx::options::count options;
            options.limit(1);

            return _lambdaCollection.count_documents(make_document(kvp("function", functionName)), options) > 0;
        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    bool LambdaMongoRepository::lambdaExistsByArn(const std::string &arn) const {

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];

            // Set limit to 1 (Very important for performance!)
            mongocxx::options::count options;
            options.limit(1);

            return _lambdaCollection.count_documents(make_document(kvp("arn", arn)), options) > 0;
        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    long LambdaMongoRepository::lambdaCount(const std::string &region) const {
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

    Entity::Lambda::Lambda LambdaMongoRepository::createLambda(Entity::Lambda::Lambda &lambda) const {

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

    Entity::Lambda::Lambda LambdaMongoRepository::getLambdaById(const bsoncxx::oid &oid) const {

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

    Entity::Lambda::Lambda LambdaMongoRepository::getLambdaById(const std::string &oid) const {
        return getLambdaById(bsoncxx::oid(oid));
    }

    Entity::Lambda::Lambda LambdaMongoRepository::getLambdaByArn(const std::string &arn) const {

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
            throw Core::DatabaseException("Database exception " + std::string(e.what()));
        }
    }

    Entity::Lambda::Lambda LambdaMongoRepository::getLambdaByName(const std::string &region, const std::string &name) const {

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
            throw Core::DatabaseException("Database exception " + std::string(e.what()));
        }
    }

    Entity::Lambda::Lambda LambdaMongoRepository::updateLambda(Entity::Lambda::Lambda &lambda) const {
        lambda.modified = system_clock::now();

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];

        try {
            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            auto mResult = _lambdaCollection.find_one_and_update(make_document(kvp("region", lambda.region), kvp("function", lambda.function), kvp("runtime", lambda.runtime)), lambda.ToDocument(), opts);
            log_trace << "Lambda updated: " << lambda.ToString();
            if (mResult.has_value()) {
                lambda.FromDocument(mResult.value());
                return lambda;
            }
            return {};
        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::Lambda::Lambda LambdaMongoRepository::createOrUpdateLambda(Entity::Lambda::Lambda &lambda) const {
        if (lambdaExists(lambda)) {
            return updateLambda(lambda);
        }
        return createLambda(lambda);
    }

    Entity::Lambda::Lambda LambdaMongoRepository::importLambda(Entity::Lambda::Lambda &lambda) const {
        if (lambdaExists(lambda)) {
            const Entity::Lambda::Lambda existing = getLambdaByArn(lambda.arn);
            lambda.modified = system_clock::now();
            lambda.instances = existing.instances;
        }
        return createOrUpdateLambda(lambda);
    }

    std::vector<Entity::Lambda::Lambda> LambdaMongoRepository::listLambdas(const std::string &region) const {

        try {
            std::vector<Entity::Lambda::Lambda> lambdas;

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];

            document query;
            if (!region.empty()) {
                query.append(kvp("region", region));
            }

            for (auto lambdaCursor = _lambdaCollection.find(query.view()); auto lambda: lambdaCursor) {
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

    std::vector<Entity::Lambda::Lambda> LambdaMongoRepository::exportLambdas(const std::vector<SortColumn> &sortColumns) const {
        std::vector<Entity::Lambda::Lambda> lambdas = listLambdaCounters({}, {}, -1, 0, sortColumns);

        // Remove instances, as they will confuse the re-import
        for (auto &lambda: lambdas) {
            lambda.invocations = 0;
            lambda.avgDuration = 0;
            lambda.instances.clear();
        }
        return lambdas;
    }

    std::vector<Entity::Lambda::Lambda> LambdaMongoRepository::listLambdaCounters(const std::string &region, const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];

            mongocxx::options::find opts;
            if (!sortColumns.empty()) {
                document sort = {};
                for (const auto &sortColumn: sortColumns) {
                    sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
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
            for (auto lambdaCursor = _lambdaCollection.find(query.view(), opts); auto lambda: lambdaCursor) {
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

    std::vector<Entity::Lambda::Lambda> LambdaMongoRepository::listLambdasWithEventSource(const std::string &eventSourceArn) const {
        try {
            std::vector<Entity::Lambda::Lambda> lambdas;

            document query;
            query.append(kvp("eventSources", make_document(kvp("$elemMatch", make_document(kvp("eventSourceArn", eventSourceArn))))));

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];
            for (auto lambdaCursor = _lambdaCollection.find(query.view()); auto lambda: lambdaCursor) {
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

    Entity::Lambda::LambdaResult LambdaMongoRepository::createLambdaResult(Entity::Lambda::LambdaResult &lambdaResult) const {
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

    bool LambdaMongoRepository::lambdaResultExists(const std::string &oid) const {

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

    Entity::Lambda::LambdaResult LambdaMongoRepository::getLambdaResultCounter(const std::string &oid) const {

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
            return {};
        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    std::vector<Entity::Lambda::LambdaResult> LambdaMongoRepository::listLambdaResultCounters(const std::string &lambdaArn, const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _lambdaResultCollection = (*client)[_databaseName][_lambdaResultCollectionName];

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
            if (!lambdaArn.empty()) {
                query.append(kvp("arn", lambdaArn));
            }

            std::vector<Entity::Lambda::LambdaResult> lambdaResults;
            for (auto lambdaCursor = _lambdaResultCollection.find(query.extract(), opts); auto lambda: lambdaCursor) {
                Entity::Lambda::LambdaResult result;
                result.FromDocument(lambda);
                lambdaResults.push_back(result);
            }
            return lambdaResults;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    long LambdaMongoRepository::lambdaResultsCount(const std::string &lambdaArn) const {
        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _lambdaResultCollection = (*client)[_databaseName][_lambdaResultCollectionName];

            document query = {};
            if (!lambdaArn.empty()) {
                query.append(kvp("arn", lambdaArn));
            }

            const int64_t count = _lambdaResultCollection.count_documents(query.view());
            log_trace << "Got lambda result count, size:" << count;
            return count;
        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    long LambdaMongoRepository::deleteResultsCounter(const std::string &oid) const {

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

    long LambdaMongoRepository::deleteResultsCounters(const std::string &lambdaArn) const {

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

    long LambdaMongoRepository::deleteAllResultsCounters() const {

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

    long LambdaMongoRepository::removeExpiredLambdaLogs(const system_clock::time_point &cutOff) const {

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

    void LambdaMongoRepository::deleteLambda(const std::string &functionName) const {
        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _lambdaCollection = (*client)[_databaseName][_lambdaCollectionName];
            const auto result = _lambdaCollection.delete_many(make_document(kvp("function", functionName)));
            log_debug << "lambda deleted, function: " << functionName << " count: " << result->deleted_count();
        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    long LambdaMongoRepository::deleteAllLambdas() const {

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

}// namespace Awsmock::Database
