//
// Created by vogje01 on 11/19/23.
//

#include "awsmock/core/exception/DatabaseException.h"
#include "awsmock/entity/cognito/Group.h"


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
            log_error << "Get cognito user pool by oid failed, oid: " << oid;
            throw Core::DatabaseException("Get cognito user pool by oid failed, oid: " + oid);
        }

        it->second.oid = oid;
        return it->second;
    }

    Entity::Apps::Application ApplicationMemoryDb::CreateApplication(const Entity::Apps::Application &application) {
        boost::mutex::scoped_lock lock(_applicationMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _applications[oid] = application;
        log_trace << "Application created, oid: " << oid;
        return GetApplicationByOid(oid);
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

}// namespace AwsMock::Database