//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/memorydb/ApplicationMemoryDb.h>

namespace AwsMock::Database {

    boost::mutex ApplicationMemoryDb::_applicationMutex;

    bool ApplicationMemoryDb::ApplicationExists(const std::string &region, const std::string &name) {

        return std::ranges::find_if(_applications,
                                    [region, name](const std::pair<std::string, Entity::Apps::Application> &application) {
                                        return application.second.region == region && application.second.name == name;
                                    }) != _applications.end();
    }

    Entity::Apps::Application ApplicationMemoryDb::GetApplicationByOid(const std::string &oid) {

        const auto it = std::ranges::find_if(_applications,
                                             [oid](const std::pair<std::string, Entity::Apps::Application> &application) {
                                                 return application.first == oid;
                                             });

        if (it == _applications.end()) {
            log_error << "Get application by oid failed, oid: " << oid;
            throw Core::DatabaseException("Get application by oid failed, oid: " + oid);
        }

        it->second.oid = oid;
        return it->second;
    }

    Entity::Apps::Application ApplicationMemoryDb::GetApplication(const std::string &region, const std::string &name) const {

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

    Entity::Apps::Application ApplicationMemoryDb::CreateApplication(const Entity::Apps::Application &application) {
        boost::mutex::scoped_lock lock(_applicationMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _applications[oid] = application;
        log_trace << "Application created, oid: " << oid;
        return GetApplicationByOid(oid);
    }

    Entity::Apps::Application ApplicationMemoryDb::UpdateApplication(Entity::Apps::Application &application) {
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

    std::vector<Entity::Apps::Application> ApplicationMemoryDb::ListApplications(const std::string &region, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) {

        auto q = Core::from(_applications | std::views::values | std::ranges::to<std::vector>());
        if (!region.empty()) {
            q = q.where([region](const Entity::Apps::Application &application) { return application.region == region; });
        }
        if (!prefix.empty()) {
            q = q.where([prefix](const Entity::Apps::Application &application) { return Core::StringUtils::StartsWith(application.region, prefix); });
        }
        if (!sortColumns.empty()) {
            std::ranges::sort(q.to_vector(), [sortColumns](const Entity::Apps::Application &a, const Entity::Apps::Application &b) {
                for (const auto &[column, sortDirection]: sortColumns) {
                    if (column == "name") {
                        return sortDirection == 1 ? a.name < b.name : b.name < a.name;
                    }
                }
                return false;
            });
        }

        // Create page iterators
        return Core::PageVector<Entity::Apps::Application>(q.to_vector(), pageSize, pageIndex);
    }

    long ApplicationMemoryDb::CountApplications(const std::string &region, const std::string &prefix) const {

        auto q = Core::from(_applications | std::views::values | std::ranges::to<std::vector>());
        if (!region.empty()) {
            q = q.where([region](const Entity::Apps::Application &application) { return application.region == region; });
        }
        if (!prefix.empty()) {
            q = q.where([prefix](const Entity::Apps::Application &application) { return Core::StringUtils::StartsWith(application.region, prefix); });
        }
        return static_cast<long>(q.count());
    }

    long ApplicationMemoryDb::DeleteApplication(const std::string &region, const std::string &name) {
        boost::mutex::scoped_lock lock(_applicationMutex);

        const auto count = std::erase_if(_applications, [region, name](const std::pair<std::string, Entity::Apps::Application> &a) {
            return a.second.region == region && a.second.name == name;
        });
        log_debug << "Application deleted, name: " << name << " count: " << count;
        return static_cast<long>(count);
    }

    long ApplicationMemoryDb::DeleteAllApplications() {
        boost::mutex::scoped_lock lock(_applicationMutex);

        const long count = _applications.size();
        _applications.clear();
        log_debug << "All applications deleted";
        return count;
    }
} // namespace AwsMock::Database
