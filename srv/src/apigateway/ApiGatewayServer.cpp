//
// Created by vogje01 on 04/01/2023.
//

#include <awsmock/service/apigateway/ApiGatewayServer.h>

namespace Awsmock::Service {

    ApiGatewayServer::ApiGatewayServer() : AbstractServer("api-gateway") {

        // Get HTTP configuration values
        const int monitoringPeriod = Core::Configuration::instance().get<int>("awsmock.modules.api-gateway.monitoring-period");
        const bool backupActive = Core::Configuration::instance().get<bool>("awsmock.modules.api-gateway.backup.active");
        const auto backupCron = Core::Configuration::instance().get<std::string>("awsmock.modules.api-gateway.backup.cron");

        // Start api gateway monitoring update counters
        Core::Scheduler::instance().AddTask("api-gateway-monitoring", [this] { this->UpdateCounter(); }, monitoringPeriod);
        Core::Scheduler::instance().AddOneTimeTask("api-gateway-servers", [this] { this->StartRestApis(); });

        // Start backup
        if (backupActive) {
            Core::Scheduler::instance().AddTask("api-gateway-backup", [] { BackupApiGateway(); }, backupCron);
        }

        // Connect stop signal
        Core::EventBus::instance().sigShutdown.connect(boost::signals2::signal<void()>::slot_type(&ApiGatewayServer::Shutdown, this));

        log_info << "ApiGateway server started";
    }

    void ApiGatewayServer::UpdateCounter() const {
        log_trace << "ApiGateway monitoring starting";

        //
        // const long users = _apiGatewayDatabase->countUsers({}, {}, {});
        // const long userPools = _apiGatewayDatabase->countUserPools({});
        // Core::EventBus::instance().sigMetricGauge(APIGATEWAY_USER_COUNT, {}, {}, static_cast<double>(users));
        // Core::EventBus::instance().sigMetricGauge(APIGATEWAY_USERPOOL_COUNT, {}, {}, static_cast<double>(userPools));
        //
        // // Count users per user pool
        // for (auto &userPool: _apiGatewayDatabase->listUserPools({}, {}, 0, 0, {})) {
        //     const long usersPerUserPool = _apiGatewayDatabase->countUsers(userPool.region, userPool.userPoolId, {});
        //     Core::EventBus::instance().sigMetricGauge(APIGATEWAY_USER_BY_USERPOOL_COUNT, "userPool", userPool.name, static_cast<double>(usersPerUserPool));
        // }
        //
        // // Count users per user group
        // for (auto &group: _apiGatewayDatabase->listGroups({}, {})) {
        //     const long usersPerGroup = _apiGatewayDatabase->countUsers(group.region, group.userPoolId, group.groupName);
        //     Core::EventBus::instance().sigMetricGauge(APIGATEWAY_USER_BY_GROUP_COUNT, "group", group.groupName, static_cast<double>(usersPerGroup));
        // }
        log_trace << "ApiGateway monitoring finished";
    }

    void ApiGatewayServer::StartRestApis() {
        const auto region = Core::Configuration::instance().get<std::string>("awsmock.region");
        const auto restApis = _apiGatewayDatabase->listRestApis(region);
        if (restApis.empty()) return;

        const auto executable = Core::Configuration::instance().getOr<std::string>("awsmock.modules.api-gateway.proxy.executable", "awsmock-agw");
        const int threads = Core::Configuration::instance().getOr<int>("awsmock.modules.api-gateway.proxy.threads", 4);
        const int targetPort = Core::Configuration::instance().getOr<int>("awsmock.modules.api-gateway.proxy.target-port", 4566);
        const auto targetHost = Core::Configuration::instance().getOr<std::string>("awsmock.modules.api-gateway.proxy.target-host", "localhost");

        for (const auto &restApi: restApis) {
            const int listenPort = Core::SystemUtils::GetNextFreePort();
            const std::string socketPath = "/tmp/awsmock-agw-" + restApi.name + ".sock";

            Dto::ProcessConfig cfg;
            cfg.name = restApi.name;
            cfg.executable = executable;
            cfg.socketPath = socketPath;
            cfg.args = {
                    "--name",
                    restApi.name,
                    "--port",
                    std::to_string(listenPort),
                    "--target-host",
                    targetHost,
                    "--target-port",
                    std::to_string(targetPort),
                    "--socket-path",
                    socketPath,
                    "--threads",
                    std::to_string(threads),
            };

            _controller.registerModule(cfg);
            if (_controller.start(restApi.name)) {
                log_info << "Started restApi proxy, name: " << restApi.name << ", port: " << listenPort;
            } else {
                log_error << "Failed to start restApi proxy, name: " << restApi.name;
            }
        }
    }

    void ApiGatewayServer::BackupApiGateway() {
        ModuleService{}.BackupModule("apiGateway", Dto::Module::ExportType::INFRA_STRUCTURE);
    }

    void ApiGatewayServer::Shutdown() {
        Core::Scheduler::instance().Shutdown("api-gateway-monitoring");
        Core::Scheduler::instance().Shutdown("api-gateway-backup");
        _controller.stopAll();
        log_info << "ApiGateway server stopped";
    }
}// namespace Awsmock::Service
