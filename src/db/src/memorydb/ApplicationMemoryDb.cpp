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

        std::string region = application.region;
        std::string name = application.name;
        const auto it = std::ranges::find_if(_applications,
                                             [region, name](const std::pair<std::string, Entity::Apps::Application> &application) {
                                                 return application.second.region == region && application.second.name == name;
                                             });

        if (it == _applications.end()) {
            log_error << "Update user pool failed, region: " << application.region << " name: " << application.name;
            throw Core::DatabaseException("Update cognito user pool failed, region: " + application.region + " name: " + application.name);
        }
        _applications[it->first] = application;
        return _applications[it->first];
    }

    std::vector<Entity::Apps::Application> ApplicationMemoryDb::ListApplications(const std::string &region, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) {

        std::vector<Entity::Apps::Application> applicationList;
        if (!region.empty()) {

            for (const auto &val: _applications | std::views::values) {
                if (val.region == region) {
                    applicationList.emplace_back(val);
                }
            }

        } else {

            for (const auto &val: _applications | std::views::values) {
                applicationList.emplace_back(val);
            }
        }

        log_trace << "Got application list, size: " << applicationList.size();
        return applicationList;
    }

    long ApplicationMemoryDb::CountApplications(const std::string &region, const std::string &prefix) const {

        long count = 0;
        if (region.empty()) {

            count = static_cast<long>(_applications.size());

        } else {

            for (const auto &val: _applications | std::views::values) {
                if (val.region == region) {
                    count++;
                }
            }
        }
        return count;
    }

    long ApplicationMemoryDb::DeleteApplication(const std::string &region, const std::string &name) {
        boost::mutex::scoped_lock lock(_applicationMutex);

        const auto count = std::erase_if(_applications, [region, name](const std::pair<std::string, Entity::Apps::Application> &a) {
            return a.second.region == region && a.second.name == name;
        });
        log_debug << "Application deleted, name: " << name << " count: " << count;
        return count;
    }

    long ApplicationMemoryDb::DeleteAllApplications() {
        boost::mutex::scoped_lock lock(_applicationMutex);

        const long count = _applications.size();
        _applications.clear();
        log_debug << "All applications deleted";
        return count;
    }
}// namespace AwsMock::Database