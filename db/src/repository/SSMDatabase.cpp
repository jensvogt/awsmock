//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/SSMDatabase.h>

namespace AwsMock::Database {

    bool SSMDatabase::ParameterExists(const std::string &name) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _parameterCollection = (*client)[_databaseName][_parameterCollectionName];
                const int64_t count = _parameterCollection.count_documents(make_document(kvp("name", name)));
                log_trace << "Parameter exists: " << std::boolalpha << (count > 0);
                return count > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "SSM database exception: " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.ParameterExists(name);
    }

    Entity::SSM::Parameter SSMDatabase::GetParameterById(bsoncxx::oid oid) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _topicCollection = (*client)[_databaseName][_parameterCollectionName];
            const auto mResult = _topicCollection.find_one(make_document(kvp("_id", oid)));
            if (mResult->empty()) {
                log_error << "ssm parameter not found, oid: " << oid.to_string();
                throw Core::DatabaseException("ssm parameter not found, oid: " + oid.to_string());
            }

            Entity::SSM::Parameter result;
            result.FromDocument(mResult->view());
            return result;


        } catch (const mongocxx::exception &exc) {
            log_error << "SSM database exception: " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SSM::Parameter SSMDatabase::GetParameterById(const std::string &oid) const {

        if (HasDatabase()) {

            return GetParameterById(bsoncxx::oid(oid));
        }
        return _memoryDb.GetParameterById(oid);
    }

    Entity::SSM::Parameter SSMDatabase::GetParameterByName(const std::string &name) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _parameterCollection = (*client)[_databaseName][_parameterCollectionName];

            try {

                const auto mResult = _parameterCollection.find_one(make_document(kvp("name", name)));
                if (mResult->empty()) {
                    log_error << "SSM parameter not found, name: " << name;
                    throw Core::DatabaseException("ssm parameter not found, name" + name);
                }

                Entity::SSM::Parameter result;
                result.FromDocument(mResult->view());
                return result;


            } catch (const mongocxx::exception &exc) {
                log_error << "SSM database exception: " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.GetParameterByName(name);
    }

    Entity::SSM::ParameterList SSMDatabase::ListParameters(const std::string &region, const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _parameterCollection = (*client)[_databaseName][_parameterCollectionName];

            mongocxx::options::find opts;
            if (pageSize > 0) {
                opts.limit(pageSize);
                if (pageIndex > 0) {
                    opts.skip(pageSize * pageIndex);
                }
            }
            opts.sort(make_document(kvp("_id", 1)));
            if (!sortColumns.empty()) {
                bsoncxx::builder::basic::document sort;
                for (const auto &[column, sortDirection]: sortColumns) {
                    sort.append(kvp(column, sortDirection));
                }
                opts.sort(sort.extract());
            }

            bsoncxx::builder::basic::document query = {};
            if (!prefix.empty()) {
                std::string escapedPrefix = prefix;
                Core::StringUtils::Replace(escapedPrefix, "/", "\\/");
                query.append(kvp("name", make_document(kvp("$regex", "^" + escapedPrefix))));
            }
            if (!region.empty()) {
                query.append(kvp("region", region));
            }

            Entity::SSM::ParameterList parameterList;
            for (auto parameterCursor = _parameterCollection.find(query.extract(), opts); const auto &parameter: parameterCursor) {
                Entity::SSM::Parameter result;
                result.FromDocument(parameter);
                parameterList.push_back(result);
            }
            log_info << "Got parameter list, size: " << parameterList.size();
            return parameterList;
        }
        return _memoryDb.ListParameterCounters(region);
    }

    long SSMDatabase::CountParameters(const std::string &region, const std::string &prefix) const {

        if (HasDatabase()) {

            try {
                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _parameterCollection = (*client)[_databaseName][_parameterCollectionName];

                bsoncxx::builder::basic::document query = {};
                if (!prefix.empty()) {
                    query.append(kvp("name", make_document(kvp("$regex", "^" + prefix))));
                }
                if (!region.empty()) {
                    query.append(kvp("region", region));
                }

                const long count = _parameterCollection.count_documents(query.extract());
                log_trace << "Parameter count: " << count;
                return count;

            } catch (mongocxx::exception::system_error &e) {
                log_error << "Parameter count failed, error: " << e.what();
            }

        } else {

            return _memoryDb.CountParameters();
        }
        return -1;
    }

    Entity::SSM::Parameter SSMDatabase::CreateParameter(Entity::SSM::Parameter &parameter) const {

        parameter.created = system_clock::now();
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _parameterCollection = (*client)[_databaseName][_parameterCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = _parameterCollection.insert_one(parameter.ToDocument());
                session.commit_transaction();
                log_trace << "Parameter created, oid: " << result->inserted_id().get_oid().value.to_string();

                parameter.oid = result->inserted_id().get_oid().value.to_string();
                return parameter;

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "SSM database exception: " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.CreateParameter(parameter);
    }

    Entity::SSM::Parameter SSMDatabase::UpdateParameter(Entity::SSM::Parameter &parameter) const {

        parameter.modified = system_clock::now();
        if (HasDatabase()) {

            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _parameterCollection = (*client)[_databaseName][_parameterCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto mResult = _parameterCollection.find_one_and_update(make_document(kvp("name", parameter.parameterName)), parameter.ToDocument(), opts);
                log_trace << "Parameter updated: " << parameter.ToString();
                session.commit_transaction();

                if (mResult) {
                    parameter.FromDocument(mResult->view());
                    return parameter;
                }
                return {};

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "SSM database exception: " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.UpdateParameter(parameter);
    }

    Entity::SSM::Parameter SSMDatabase::ImportParameter(Entity::SSM::Parameter &parameter) const {

        if (ParameterExists(parameter.parameterName)) {

            return UpdateParameter(parameter);
        }
        return CreateParameter(parameter);
    }

    void SSMDatabase::DeleteParameter(const Entity::SSM::Parameter &parameter) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _parameterCollection = (*client)[_databaseName][_parameterCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto delete_many_result = _parameterCollection.delete_one(make_document(kvp("name", parameter.parameterName)));
                session.commit_transaction();
                log_debug << "ssm parameter deleted, count: " << delete_many_result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "SSM database exception: " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            _memoryDb.DeleteParameter(parameter);
        }
    }

    long SSMDatabase::DeleteAllParameters() const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _parameterCollection = (*client)[_databaseName][_parameterCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto delete_many_result = _parameterCollection.delete_many({});
                session.commit_transaction();
                log_debug << "All ssm parameters deleted, count: " << delete_many_result->deleted_count();
                return delete_many_result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "SSM database exception: " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.DeleteAllParameters();
    }

}// namespace AwsMock::Database
