//
// Created by vogje01 on 06/07/2025
//

#include <awsmock/repository/application/ApplicationMongoRepository.h>

namespace Awsmock::Database {

    bool ApplicationMongoRepository::applicationExists(const std::string &region, const std::string &name) const {
        Monitoring::MonitoringTimer measure(APPLICATION_DATABASE_TIMER, APPLICATION_DATABASE_COUNTER, "action", "application_exists");

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection applicationCollection = client->database(_databaseName)[_applicationCollectionName];

            // Set limit to 1 (Very important for performance!)
            mongocxx::options::count options;
            options.limit(1);

            return applicationCollection.count_documents(make_document(kvp("region", region), kvp("name", name)), options) > 0;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::Apps::Application ApplicationMongoRepository::createApplication(Entity::Apps::Application &application) const {
        Monitoring::MonitoringTimer measure(APPLICATION_DATABASE_TIMER, APPLICATION_DATABASE_COUNTER, "action", "create_application");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection applicationCollection = client->database(_databaseName)[_applicationCollectionName];

        try {

            const auto result = applicationCollection.insert_one(application.ToDocument());
            log_trace << "Application created, oid: " << result->inserted_id().get_oid().value.to_string();
            application.oid = result->inserted_id().get_oid().value.to_string();
            return application;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::Apps::Application ApplicationMongoRepository::importApplication(Entity::Apps::Application &application) const {
        Monitoring::MonitoringTimer measure(APPLICATION_DATABASE_TIMER, APPLICATION_DATABASE_COUNTER, "action", "import_application");

        application.containerId = "";
        application.containerName = "";
        if (applicationExists(application.region, application.name)) {
            return updateApplication(application);
        }
        return createApplication(application);
    }

    Entity::Apps::Application ApplicationMongoRepository::getApplication(const std::string &region, const std::string &name) const {
        Monitoring::MonitoringTimer measure(APPLICATION_DATABASE_TIMER, APPLICATION_DATABASE_COUNTER, "action", "get_application");

        try {

            if (const auto client = ConnectionPool::instance().GetConnection()) {
                mongocxx::collection applicationCollection = client->database(_databaseName)[_applicationCollectionName];
                const auto result = applicationCollection.find_one(make_document(kvp("region", region), kvp("name", name)));
                log_trace << "Application retrieved, region: " << region << ", name: " << name;
                if (result) {
                    Entity::Apps::Application application;
                    application.FromDocument(result->view());
                    return application;
                }
            }
            return {};

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    std::vector<Entity::Apps::Application> ApplicationMongoRepository::listApplications(const std::string &region, const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {
        Monitoring::MonitoringTimer measure(APPLICATION_DATABASE_TIMER, APPLICATION_DATABASE_COUNTER, "action", "list_applications");

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection applicationCollection = client->database(_databaseName)[_applicationCollectionName];

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
                for (const auto &sortColumn: sortColumns) {
                    sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
                }
                opts.sort(sort.extract());
            }
            if (pageSize > 0) {
                opts.skip(pageSize * pageIndex);
                opts.limit(pageSize);
            }

            std::vector<Entity::Apps::Application> applications;
            for (auto applicationCursor = applicationCollection.find(query.extract(), opts); auto application: applicationCursor) {
                Entity::Apps::Application result;
                result.FromDocument(application);
                applications.push_back(result);
            }
            return applications;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    long ApplicationMongoRepository::countApplications(const std::string &region, const std::string &prefix) const {
        Monitoring::MonitoringTimer measure(APPLICATION_DATABASE_TIMER, APPLICATION_DATABASE_COUNTER, "action", "count_applications");

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection applicationCollection = client->database(_databaseName)[_applicationCollectionName];

            document query = {};

            if (!region.empty()) {
                query.append(kvp("region", region));
            }
            if (!prefix.empty()) {
                query.append(kvp("name", make_document(kvp("$regex", "^" + prefix))));
            }

            const long count = applicationCollection.count_documents(query.view());
            log_trace << "Count applications, result: " << count;
            return count;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::Apps::Application ApplicationMongoRepository::updateApplication(Entity::Apps::Application &application) const {
        Monitoring::MonitoringTimer measure(APPLICATION_DATABASE_TIMER, APPLICATION_DATABASE_COUNTER, "action", "update_applications");

        application.modified = system_clock::now();

        mongocxx::options::find_one_and_update opts{};
        opts.return_document(mongocxx::options::return_document::k_after);

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection applicationCollection = client->database(_databaseName)[_applicationCollectionName];

        try {

            const auto mResult = applicationCollection.find_one_and_update(make_document(kvp("region", application.region), kvp("name", application.name)), application.ToDocument(), opts);

            if (mResult) {
                log_trace << "Application updated: " << application;
                application.FromDocument(mResult.value());
                return application;
            }
            return {};

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    void ApplicationMongoRepository::setEnabled(const std::string &region, const std::string &name, const bool enabled) const {
        Monitoring::MonitoringTimer measure(APPLICATION_DATABASE_TIMER, APPLICATION_DATABASE_COUNTER, "action", "set_enabled");

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection applicationCollection = client->database(_databaseName)[_applicationCollectionName];
            applicationCollection.update_one(
                    make_document(kvp("region", region), kvp("name", name)),
                    make_document(kvp("$set", make_document(
                                                      kvp("enabled", enabled),
                                                      kvp("modified", bsoncxx::types::b_date(system_clock::now()))))));
            log_debug << "Application enabled flag set, region: " << region << ", name: " << name << ", enabled: " << enabled;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    long ApplicationMongoRepository::deleteApplication(const std::string &region, const std::string &name) const {
        Monitoring::MonitoringTimer measure(APPLICATION_DATABASE_TIMER, APPLICATION_DATABASE_COUNTER, "action", "delete_applications");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection applicationCollection = client->database(_databaseName)[_applicationCollectionName];

        try {

            const auto result = applicationCollection.delete_many(make_document(kvp("region", region), kvp("name", name)));
            log_trace << "Application deleted: " << result.value().deleted_count();
            return result.value().deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    long ApplicationMongoRepository::deleteAllApplications() const {
        Monitoring::MonitoringTimer measure(APPLICATION_DATABASE_TIMER, APPLICATION_DATABASE_COUNTER, "action", "delete_applications");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _groupCollection = client->database(_databaseName)[_applicationCollectionName];

        try {

            const auto result = _groupCollection.delete_many({});
            log_debug << "All applications deleted, count: " << result->deleted_count();
            return result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

}// namespace Awsmock::Database
