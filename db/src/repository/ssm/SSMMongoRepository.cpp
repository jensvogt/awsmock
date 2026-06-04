//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/ssm/SSMMongoRepository.h>

namespace Awsmock::Database {

    bool SSMMongoRepository::parameterExists(const std::string &name) const {

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

    Entity::SSM::Parameter SSMMongoRepository::getParameterById(const bsoncxx::oid &oid) const {

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

    Entity::SSM::Parameter SSMMongoRepository::getParameterById(const std::string &oid) const {
        return getParameterById(bsoncxx::oid(oid));
    }

    Entity::SSM::Parameter SSMMongoRepository::getParameterByName(const std::string &name) const {

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

    Entity::SSM::ParameterList SSMMongoRepository::listParameters(const std::string &region, const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {

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
            document sort;
            for (const auto &sortColumn: sortColumns) {
                sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
            }
            opts.sort(sort.extract());
        }

        document query = {};
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

    long SSMMongoRepository::countParameters(const std::string &region, const std::string &prefix) const {

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _parameterCollection = (*client)[_databaseName][_parameterCollectionName];

            document query = {};
            if (!prefix.empty()) {
                query.append(kvp("name", make_document(kvp("$regex", "^" + prefix))));
            }
            if (!region.empty()) {
                query.append(kvp("region", region));
            }

            const long count = _parameterCollection.count_documents(query.view());
            log_trace << "Parameter count: " << count;
            return count;

        } catch (mongocxx::exception::system_error &e) {
            log_error << "Parameter count failed, error: " << e.what();
            throw Core::DatabaseException(e.what());
        }
    }

    Entity::SSM::Parameter SSMMongoRepository::createParameter(Entity::SSM::Parameter &parameter) const {

        parameter.created = system_clock::now();

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _parameterCollection = (*client)[_databaseName][_parameterCollectionName];

        try {

            const auto result = _parameterCollection.insert_one(parameter.ToDocument());
            log_trace << "Parameter created, oid: " << result->inserted_id().get_oid().value.to_string();

            parameter.oid = result->inserted_id().get_oid().value.to_string();
            return parameter;

        } catch (const mongocxx::exception &exc) {
            log_error << "SSM database exception: " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SSM::Parameter SSMMongoRepository::updateParameter(Entity::SSM::Parameter &parameter) const {

        parameter.modified = system_clock::now();

        mongocxx::options::find_one_and_update opts{};
        opts.return_document(mongocxx::options::return_document::k_after);

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _parameterCollection = (*client)[_databaseName][_parameterCollectionName];

        try {

            const auto mResult = _parameterCollection.find_one_and_update(make_document(kvp("name", parameter.parameterName)), parameter.ToDocument(), opts);
            log_trace << "Parameter updated: " << parameter.ToString();

            if (mResult) {
                parameter.FromDocument(mResult->view());
                return parameter;
            }
            return {};

        } catch (const mongocxx::exception &exc) {
            log_error << "SSM database exception: " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SSM::Parameter SSMMongoRepository::importParameter(Entity::SSM::Parameter &parameter) const {

        if (parameterExists(parameter.parameterName)) {
            return updateParameter(parameter);
        }
        return createParameter(parameter);
    }

    long SSMMongoRepository::deleteParameter(const Entity::SSM::Parameter &parameter) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _parameterCollection = (*client)[_databaseName][_parameterCollectionName];

        try {

            const auto delete_many_result = _parameterCollection.delete_one(make_document(kvp("name", parameter.parameterName)));
            log_debug << "SSM parameter deleted, count: " << delete_many_result->deleted_count();
            return delete_many_result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "SSM database exception: " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    long SSMMongoRepository::deleteAllParameters() const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _parameterCollection = (*client)[_databaseName][_parameterCollectionName];

        try {

            const auto delete_many_result = _parameterCollection.delete_many({});
            log_debug << "All ssm parameters deleted, count: " << delete_many_result->deleted_count();
            return delete_many_result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "SSM database exception: " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

}// namespace Awsmock::Database
