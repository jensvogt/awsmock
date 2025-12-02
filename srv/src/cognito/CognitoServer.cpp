//
// Created by vogje01 on 04/01/2023.
//

#include <awsmock/service/cognito/CognitoServer.h>

namespace AwsMock::Service {

    CognitoServer::CognitoServer(Core::Scheduler &scheduler) : AbstractServer("cognito"), _module("cognito"), _scheduler(scheduler) {

        // Get HTTP configuration values
        _monitoringPeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.cognito.monitoring.period");
        _backupActive = Core::Configuration::instance().GetValue<bool>("awsmock.modules.cognito.backup.active");
        _backupCron = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.cognito.backup.cron");

        // Check module active
        if (!IsActive("cognito")) {
            log_info << "Cognito module inactive";
            return;
        }
        log_info << "Cognito module starting";

        // Start DynamoDB monitoring update counters
        _scheduler.AddTask("cognito-monitoring", [this] { this->UpdateCounter(); }, _monitoringPeriod);

        // Start backup
        if (_backupActive) {
            scheduler.AddTask("cognito-backup", [this] { BackupCognito(); }, _backupCron);
        }

        // Set running
        SetRunning();
        log_debug << "Cognito server started";
    }

    void CognitoServer::UpdateCounter() const {
        log_trace << "Cognito monitoring starting";

        const long users = _cognitoDatabase.CountUsers();
        const long userPools = _cognitoDatabase.CountUserPools();
        _metricService.SetGauge(COGNITO_USER_COUNT, {}, {}, static_cast<double>(users));
        _metricService.SetGauge(COGNITO_USERPOOL_COUNT, {}, {}, static_cast<double>(userPools));

        // Count users per user pool
        for (auto &userPool: _cognitoDatabase.ListUserPools()) {
            const long usersPerUserPool = _cognitoDatabase.CountUsers(userPool.region, userPool.userPoolId);
            _metricService.SetGauge(COGNITO_USER_BY_USERPOOL_COUNT, "userPool", userPool.name, static_cast<double>(usersPerUserPool));
        }

        // Count users per user group
        for (auto &group: _cognitoDatabase.ListGroups()) {
            const long usersPerGroup = _cognitoDatabase.CountUsers(group.region, group.userPoolId, group.groupName);
            _metricService.SetGauge(COGNITO_USER_BY_GROUP_COUNT, "group", group.groupName, static_cast<double>(usersPerGroup));
        }
        log_trace << "Cognito monitoring finished";
    }

    void CognitoServer::BackupCognito() {
        ModuleService::BackupModule("cognito", true);
    }

}// namespace AwsMock::Service
