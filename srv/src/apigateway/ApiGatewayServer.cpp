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
        log_trace << "ApiGateway monitoring finished";
    }

    void ApiGatewayServer::StartRestApis() {
        const auto region = Core::Configuration::instance().get<std::string>("awsmock.region");
        const auto restApis = _apiGatewayDatabase->listRestApis(region);
        if (restApis.empty()) return;

        const int targetPort = Core::Configuration::instance().getOr<int>("awsmock.modules.api-gateway.proxy.target-port", 4566);
        const auto targetHost = Core::Configuration::instance().getOr<std::string>("awsmock.modules.api-gateway.proxy.target-host", "localhost");

        for (const auto &restApi: restApis) {
            const int listenPort = Core::SystemUtils::GetNextFreePort();

            Dto::ApiGateway::ProxyConfig cfg;
            cfg.name = restApi.name;
            cfg.listenPort = static_cast<unsigned short>(listenPort);
            cfg.targetHost = targetHost;
            cfg.targetPort = static_cast<unsigned short>(targetPort);

            auto listener = std::make_shared<Agw::ProxyListener>(_ioc, cfg);
            listener->Run();
            _listeners[restApi.name] = listener;

            log_info << "Started REST API proxy, name: " << restApi.name << ", listenPort: " << listenPort << ", target: " << targetHost << ":" << targetPort;
        }
    }

    void ApiGatewayServer::BackupApiGateway() {
        ModuleService{}.BackupModule("api-gateway", Dto::Module::ExportType::INFRA_STRUCTURE);
    }

    void ApiGatewayServer::Shutdown() {
        Core::Scheduler::instance().Shutdown("api-gateway-monitoring");
        Core::Scheduler::instance().Shutdown("api-gateway-backup");
        for (auto &[name, listener]: _listeners) {
            listener->Stop();
            log_info << "Stopped REST API proxy, name: " << name;
        }
        _listeners.clear();
        log_info << "ApiGateway server stopped";
    }
}// namespace Awsmock::Service
