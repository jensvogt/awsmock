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

        // Start DynamoDB monitoring update counters
        _scheduler.AddTask("cognito-monitoring", [this] { this->UpdateCounter(); }, _monitoringPeriod);

        // Start backup
        if (_backupActive) {
            scheduler.AddTask("cognito-backup", [] { BackupCognito(); }, _backupCron);
        }

        // Connect stop signal
        Core::EventBus::instance().sigShutdown.connect(boost::signals2::signal<void()>::slot_type(&CognitoServer::Shutdown, this));

        log_info << "Cognito server started";
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

    void CognitoServer::Shutdown() {
        _scheduler.Shutdown("cognito-monitoring");
        _scheduler.Shutdown("cognito-backup");
        log_info << "Cognito server stopped";
    }
}// namespace AwsMock::Service
