//
// Created by vogje01 on 04/01/2023.
//

#include <awsmock/service/apigateway/ApiGatewayServer.h>

namespace Awsmock::Service {

    ApiGatewayServer::ApiGatewayServer(boost::asio::io_context &ioc) : AbstractServer("api-gateway"), _ioc(ioc) {

        // Get HTTP configuration values
        const int monitoringPeriod = Core::Configuration::instance().get<int>("awsmock.modules.api-gateway.monitoring-period");
        const bool backupActive = Core::Configuration::instance().get<bool>("awsmock.modules.api-gateway.backup.active");
        const auto backupCron = Core::Configuration::instance().get<std::string>("awsmock.modules.api-gateway.backup.cron");

        // Start api gateway monitoring update counters
        Core::Scheduler::instance().AddTask("api-gateway-monitoring", [this] { this->updateCounter(); }, monitoringPeriod);

        // Start backup
        if (backupActive) {
            Core::Scheduler::instance().AddTask("api-gateway-backup", [] { backupApiGateway(); }, backupCron);
        }

        // Connect stop signal
        Core::EventBus::instance().sigShutdown.connect(boost::signals2::signal<void()>::slot_type(&ApiGatewayServer::shutdown, this));

        log_info << "ApiGateway server started";
    }

    void ApiGatewayServer::updateCounter() const {
        log_trace << "ApiGateway monitoring starting";
        log_trace << "ApiGateway monitoring finished";
    }

    void ApiGatewayServer::backupApiGateway() {
        ModuleService{}.BackupModule("api-gateway", Dto::Module::ExportType::INFRA_STRUCTURE);
    }

    void ApiGatewayServer::shutdown() {
        Core::Scheduler::instance().Shutdown("api-gateway-monitoring");
        Core::Scheduler::instance().Shutdown("api-gateway-backup");
        log_info << "ApiGateway server stopped";
    }
}// namespace Awsmock::Service
