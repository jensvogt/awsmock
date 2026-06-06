//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/repository/application/ApplicationMemoryRepository.h>

namespace Awsmock::Database {

    boost::mutex ApplicationMemoryRepository::_applicationMutex;

    bool ApplicationMemoryRepository::applicationExists(const std::string &region, const std::string &name) const {

        return std::ranges::find_if(_applications,
                                    [region, name](const std::pair<std::string, Entity::Apps::Application> &application) {
                                        return application.second.region == region && application.second.name == name;
                                    }) != _applications.end();
    }

    Entity::Apps::Application ApplicationMemoryRepository::getApplication(const std::string &region, const std::string &name) const {

        const auto it = std::ranges::find_if(_applications,
                                             [region, name](const std::pair<std::string, Entity::Apps::Application> &application) {
                                                 return application.second.region == region && application.second.name == name;
                                             });

        if (it == _applications.end()) {
            log_error << "Get application by name failed, region: " << region << ", name: " << name;
            throw Core::DatabaseException("Get application by name failed, region: " + region + ", name: " + name);
        }
        return it->second;
    }

    Entity::Apps::Application ApplicationMemoryRepository::createApplication(Entity::Apps::Application &application) const {
        boost::mutex::scoped_lock lock(_applicationMutex);

        application.oid = Core::StringUtils::CreateRandomUuid();
        _applications[application.oid] = application;
        log_trace << "Application created, oid: " << application.oid;
        return _applications[application.oid];
    }

    Entity::Apps::Application ApplicationMemoryRepository::updateApplication(Entity::Apps::Application &application) const {
        boost::mutex::scoped_lock lock(_applicationMutex);

        const auto it = std::ranges::find_if(_applications,
                                             [application](const std::pair<std::string, Entity::Apps::Application> &a) {
                                                 return a.second.region == application.region && a.second.name == application.name;
                                             });

        if (it == _applications.end()) {
            log_error << "Update user pool failed, region: " << application.region << " name: " << application.name;
            throw Core::DatabaseException("Update cognito user pool failed, region: " + application.region + " name: " + application.name);
        }
        _applications[it->first] = application;
        return _applications[it->first];
    }

    void ApplicationMemoryRepository::setEnabled(const std::string &region, const std::string &name, const bool enabled) const {
        boost::mutex::scoped_lock lock(_applicationMutex);

        const auto it = std::ranges::find_if(_applications, [&region, &name](const std::pair<std::string, Entity::Apps::Application> &a) {
            return a.second.region == region && a.second.name == name;
        });

        if (it == _applications.end()) {
            log_error << "setEnabled failed, region: " << region << " name: " << name;
            throw Core::DatabaseException("setEnabled failed, region: " + region + " name: " + name);
        }
        it->second.enabled = enabled;
        it->second.modified = system_clock::now();
    }

    std::vector<Entity::Apps::Application> ApplicationMemoryRepository::listApplications(const std::string &region, const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        auto q = Core::from(Core::NumberUtils::toVector(_applications));
        if (!region.empty()) {
            q = q.where([region](const Entity::Apps::Application &application) { return application.region == region; });
        }
        if (!prefix.empty()) {
            q = q.where([prefix](const Entity::Apps::Application &application) { return Core::StringUtils::StartsWith(application.name, prefix); });
        }
        if (!sortColumns.empty()) {
            std::ranges::sort(q.to_vector(), [sortColumns](const Entity::Apps::Application &a, const Entity::Apps::Application &b) {
                for (const auto &sc: sortColumns) {
                    if (sc.column == "name") {
                        return sc.sortDirection == 1 ? a.name < b.name : b.name < a.name;
                    }
                }
                return false;
            });
        }

        // Create page iterators
        return Core::PageVector<Entity::Apps::Application>(q.to_vector(), pageSize, pageIndex);
    }

    long ApplicationMemoryRepository::countApplications(const std::string &region, const std::string &prefix) const {

        auto q = Core::from(Core::NumberUtils::toVector(_applications));
        if (!region.empty()) {
            q = q.where([region](const Entity::Apps::Application &application) { return application.region == region; });
        }
        if (!prefix.empty()) {
            q = q.where([prefix](const Entity::Apps::Application &application) { return Core::StringUtils::StartsWith(application.region, prefix); });
        }
        return static_cast<long>(q.count());
    }

    Entity::Apps::Application ApplicationMemoryRepository::importApplication(Entity::Apps::Application &application) const {
        if (applicationExists(application.region, application.name)) {
            return updateApplication(application);
        }
        return createApplication(application);
    }

    long ApplicationMemoryRepository::deleteApplication(const std::string &region, const std::string &name) const {
        boost::mutex::scoped_lock lock(_applicationMutex);

        const auto count = std::erase_if(_applications, [region, name](const std::pair<std::string, Entity::Apps::Application> &a) {
            return a.second.region == region && a.second.name == name;
        });
        log_debug << "Application deleted, name: " << name << " count: " << count;
        return static_cast<long>(count);
    }

    long ApplicationMemoryRepository::deleteAllApplications() const {
        boost::mutex::scoped_lock lock(_applicationMutex);

        const long count = static_cast<long>(_applications.size());
        _applications.clear();
        log_debug << "All applications deleted";
        return count;
    }
} // namespace Awsmock::Database
