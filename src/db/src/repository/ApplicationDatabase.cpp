//
// Created by vogje01 on 06/07/2025
//

#include <awsmock/repository/ApplicationDatabase.h>

namespace AwsMock::Database {

    bool ApplicationDatabase::ApplicationExists(const std::string &region, const std::string &name) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _applicationCollection = (*client)[_databaseName][_applicationCollectionName];
                const int64_t count = _applicationCollection.count_documents(make_document(kvp("region", region), kvp("name", name)));
                log_trace << "Application exists: " << std::boolalpha << count;
                return count > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.ApplicationExists(region, name);
    }

    Entity::Apps::Application ApplicationDatabase::CreateApplication(Entity::Apps::Application &application) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _applicationCollection = (*client)[_databaseName][_applicationCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = _applicationCollection.insert_one(application.ToDocument());
                session.commit_transaction();
                log_trace << "Application created, oid: " << result->inserted_id().get_oid().value.to_string();
                application.oid = result->inserted_id().get_oid().value.to_string();
                return application;

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.CreateApplication(application);
    }

    std::vector<Entity::Apps::Application> ApplicationDatabase::ListApplications(const std::string &region, const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        std::vector<Entity::Apps::Application> applications;
        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _applicationCollection = (*client)[_databaseName][_applicationCollectionName];

                document query;
                if (!region.empty()) {
                    query.append(kvp("region", region));
                }
                if (!prefix.empty()) {
                    query.append(kvp("name", make_document(kvp("$regex", "^" + prefix))));
                }

                mongocxx::options::find opts;
                if (!sortColumns.empty()) {
                    document sort = {};
                    for (const auto &[column, sortDirection]: sortColumns) {
                        sort.append(kvp(column, sortDirection));
                    }
                    opts.sort(sort.extract());
                }
                if (pageSize > 0) {
                    opts.skip(pageSize * pageIndex);
                }
                if (pageSize > 0) {
                    opts.limit(pageSize);
                }

                for (auto applicationCursor = _applicationCollection.find(query.extract(), opts); auto application: applicationCursor) {
                    Entity::Apps::Application result;
                    result.FromDocument(application);
                    applications.push_back(result);
                }

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }

        } else {

            applications = _memoryDb.ListApplications(region, prefix, pageSize, pageIndex, sortColumns);
        }

        log_trace << "Got application list, size:" << applications.size();
        return applications;
    }

}// namespace AwsMock::Database
