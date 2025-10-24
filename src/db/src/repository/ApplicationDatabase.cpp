//
// Created by vogje01 on 06/07/2025
//

#include <awsmock/repository/ApplicationDatabase.h>

namespace AwsMock::Database {

    ApplicationDatabase::ApplicationDatabase() : _databaseName(GetDatabaseName()), _applicationCollectionName("apps_application"), _memoryDb(ApplicationMemoryDb::instance()) {}

    bool ApplicationDatabase::ApplicationExists(const std::string &region, const std::string &name) const {
       Monitoring::MonitoringTimer measure(APPLICATION_DATABASE_TIMER, APPLICATION_DATABASE_COUNTER, "action", "application_exists");

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _applicationCollection = client->database(_databaseName)[_applicationCollectionName];
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
        Monitoring::MonitoringTimer measure(APPLICATION_DATABASE_TIMER, APPLICATION_DATABASE_COUNTER, "action", "create_application");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _applicationCollection = client->database(_databaseName)[_applicationCollectionName];
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

    Entity::Apps::Application ApplicationDatabase::ImportApplication(Entity::Apps::Application &application) const {
        Monitoring::MonitoringTimer measure(APPLICATION_DATABASE_TIMER, APPLICATION_DATABASE_COUNTER, "action", "import_application");

        application.containerId = "";
        application.containerName = "";
        if (ApplicationExists(application.region, application.name)) {
            return UpdateApplication(application);
        }
        return CreateApplication(application);
    }

    Entity::Apps::Application ApplicationDatabase::GetApplication(const std::string &region, const std::string &name) const {
        Monitoring::MonitoringTimer measure(APPLICATION_DATABASE_TIMER, APPLICATION_DATABASE_COUNTER, "action", "get_application");

        if (HasDatabase()) {

            try {

                if (const auto client = ConnectionPool::instance().GetConnection()) {
                    mongocxx::collection _applicationCollection = client->database(_databaseName)[_applicationCollectionName];
                    const auto result = _applicationCollection.find_one(make_document(kvp("region", region), kvp("name", name)));
                    log_trace << "Application retrieved, region: " << region << ", name: " << name;
                    if (result) {
                        Entity::Apps::Application application;
                        application.FromDocument(result->view());
                        return application;
                    }
                }
            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.GetApplication(region, name);
    }

    std::vector<Entity::Apps::Application> ApplicationDatabase::ListApplications(const std::string &region, const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {
        Monitoring::MonitoringTimer measure(APPLICATION_DATABASE_TIMER, APPLICATION_DATABASE_COUNTER, "action", "list_applications");

        std::vector<Entity::Apps::Application> applications;
        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _applicationCollection = client->database(_databaseName)[_applicationCollectionName];

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

    long ApplicationDatabase::CountApplications(const std::string &region, const std::string &prefix) const {
        Monitoring::MonitoringTimer measure(APPLICATION_DATABASE_TIMER, APPLICATION_DATABASE_COUNTER, "action", "count_applications");

        if (HasDatabase()) {

            try {
                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _topicCollection = client->database(_databaseName)[_applicationCollectionName];

                document query = {};

                if (!region.empty()) {
                    query.append(kvp("region", region));
                }
                if (!prefix.empty()) {
                    query.append(kvp("name", make_document(kvp("$regex", "^" + prefix))));
                }

                const long count = _topicCollection.count_documents(query.extract());
                log_trace << "Count applications, result: " << count;
                return count;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.CountApplications(region, prefix);
    }

    Entity::Apps::Application ApplicationDatabase::UpdateApplication(Entity::Apps::Application &application) const {
        Monitoring::MonitoringTimer measure(APPLICATION_DATABASE_TIMER, APPLICATION_DATABASE_COUNTER, "action", "update_applications");

        application.modified = system_clock::now();
        if (HasDatabase()) {

            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _applicationCollection = client->database(_databaseName)[_applicationCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto mResult = _applicationCollection.find_one_and_update(make_document(kvp("region", application.region), kvp("name", application.name)), application.ToDocument(), opts);
                session.commit_transaction();

                if (mResult) {
                    log_trace << "Application updated: " << application;
                    application.FromDocument(mResult.value());
                    return application;
                }
                return {};

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.UpdateApplication(application);
    }

    long ApplicationDatabase::DeleteApplication(const std::string &region, const std::string &name) const {
        Monitoring::MonitoringTimer measure(APPLICATION_DATABASE_TIMER, APPLICATION_DATABASE_COUNTER, "action", "delete_applications");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            auto session = client->start_session();

            try {

                session.start_transaction();
                mongocxx::collection _applicationCollection = client->database(_databaseName)[_applicationCollectionName];
                const auto result = _applicationCollection.delete_many(make_document(kvp("region", region), kvp("name", name)));
                session.commit_transaction();
                log_trace << "Application deleted: " << result.value().deleted_count();
                return result.value().deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.ApplicationExists(region, name);
    }

    long ApplicationDatabase::DeleteAllApplications() const {
        Monitoring::MonitoringTimer measure(APPLICATION_DATABASE_TIMER, APPLICATION_DATABASE_COUNTER, "action", "delete_applications");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _groupCollection = client->database(_databaseName)[_applicationCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = _groupCollection.delete_many({});
                log_debug << "All groups deleted, count: " << result->deleted_count();
                session.commit_transaction();
                return result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.DeleteAllApplications();
    }

}// namespace AwsMock::Database
