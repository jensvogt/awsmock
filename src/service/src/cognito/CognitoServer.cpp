//
// Created by vogje01 on 04/01/2023.
//

#include <awsmock/service/cognito/CognitoServer.h>

namespace AwsMock::Service {

    CognitoServer::CognitoServer(Core::PeriodicScheduler &scheduler) : AbstractServer("cognito"), _module("cognito"), _scheduler(scheduler) {

        // Get HTTP configuration values
        _monitoringPeriod = Core::Configuration::instance().GetValueInt("awsmock.modules.cognito.monitoring.period");

        // Check module active
        if (!IsActive("cognito")) {
            log_info << "Cognito module inactive";
            return;
        }
        log_info << "Cognito module starting";

        // Start DynamoDB monitoring update counters
        _scheduler.AddTask("monitoring-dynamodb-counters", [this] { this->UpdateCounter(); }, _monitoringPeriod);

        // Set running
        SetRunning();
        log_debug << "Cognito server started";
    }

    void CognitoServer::Shutdown() {
        _scheduler.Shutdown();
        log_info << "Cognito scheduler shutdown";
    }

    void CognitoServer::UpdateCounter() const {
        log_trace << "Cognito monitoring starting";

        const long users = _cognitoDatabase.CountUsers();
        const long userPools = _cognitoDatabase.CountUserPools();
        _metricService.SetGauge(COGNITO_USER_COUNT, users);
        _metricService.SetGauge(COGNITO_USERPOOL_COUNT, userPools);

        // Count users per user pool
        for (auto &userPool: _cognitoDatabase.ListUserPools()) {
            std::string labelValue = userPool.name;
            Core::StringUtils::Replace(labelValue, "-", "_");
            const long usersPerUserPool = _cognitoDatabase.CountUsers(userPool.region, userPool.userPoolId);
            _metricService.SetGauge(COGNITO_USER_BY_USERPOOL_COUNT, "userPool", labelValue, usersPerUserPool);
        }
        log_trace << "Cognito monitoring finished";
    }
}// namespace AwsMock::Service
