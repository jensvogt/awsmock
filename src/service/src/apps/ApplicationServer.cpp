//
// Created by vogje01 on 04/01/2023.
//

#include "awsmock/dto/apps/model/Status.h"


#include <awsmock/service/apps/ApplicationServer.h>

namespace AwsMock::Service {

    ApplicationServer::ApplicationServer(Core::Scheduler &scheduler) : AbstractServer("application"), _module("application"), _scheduler(scheduler) {

        // Get HTTP configuration values
        _monitoringPeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.application.monitoring-period");
        _backupActive = Core::Configuration::instance().GetValue<bool>("awsmock.modules.application.backup.active");
        _backupCron = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.application.backup.cron");

        // Check module active
        if (!IsActive("application")) {
            log_info << "Application module inactive";
            return;
        }
        log_info << "Application module starting";

        // Start application monitoring update counters
        _scheduler.AddTask("application-monitoring", [this] { this->UpdateCounter(); }, _monitoringPeriod);
        _scheduler.AddTask("application-updates", [this] { this->UpdateApplications(); }, _monitoringPeriod);

        // Start backup
        if (_backupActive) {
            scheduler.AddTask("application-backup", [this] { BackupApplication(); }, _backupCron);
        }

        // Create a test app
        CreateTestApplication();

        // Set running
        SetRunning();
        log_debug << "Application server started";
    }

    void ApplicationServer::UpdateCounter() const {
        log_trace << "Application monitoring starting";

        /* const long users = _applicationDatabase.CountUsers();
        const long userPools = _applicationDatabase.CountUserPools();
        _metricService.SetGauge(APPLICATION_USER_COUNT, {}, {}, static_cast<double>(users));
        _metricService.SetGauge(APPLICATION_USERPOOL_COUNT, {}, {}, static_cast<double>(userPools));

        // Count users per user pool
        for (auto &userPool: _applicationDatabase.ListUserPools()) {
            const long usersPerUserPool = _applicationDatabase.CountUsers(userPool.region, userPool.userPoolId);
            _metricService.SetGauge(APPLICATION_USER_BY_USERPOOL_COUNT, "userPool", userPool.name, static_cast<double>(usersPerUserPool));
        }

        // Count users per user group
        for (auto &group: _applicationDatabase.ListGroups()) {
            const long usersPerGroup = _applicationDatabase.CountUsers(group.region, group.userPoolId, group.groupName);
            _metricService.SetGauge(APPLICATION_USER_BY_GROUP_COUNT, "group", group.groupName, static_cast<double>(usersPerGroup));
        }*/
        log_trace << "Application monitoring finished";
    }

    void ApplicationServer::UpdateApplications() const {
        for (auto &application: _applicationDatabase.ListApplications()) {
        }
    }

    void ApplicationServer::BackupApplication() {
        ModuleService::BackupModule("application", true);
    }

    void ApplicationServer::CreateTestApplication() const {
        Database::Entity::Apps::Application application;
        application.name = "test-application";
        application.runtime = "Java21";
        application.region = "eu-central-1";
        application.status = Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::PENDING);
        application.enabled = true;
        application.environment["key"] = "value";
        application.options["key"] = "value";
        application.tags["key"] = "value";
        _applicationDatabase.CreateApplication(application);
        log_debug << "Test application created";
    }
}// namespace AwsMock::Service
