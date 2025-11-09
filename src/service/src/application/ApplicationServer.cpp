//
// Created by vogje01 on 04/01/2023.
//

#include <awsmock/service/apps/ApplicationServer.h>

namespace AwsMock::Service {

    ApplicationServer::ApplicationServer(Core::Scheduler &scheduler, boost::asio::io_context &ioc) : AbstractServer("application"), _applicationService(ioc), _module("application"), _scheduler(scheduler) {

        // Get configuration values
        _monitoringPeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.application.monitoring-period");
        _watchdogPeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.application.watchdog-period");
        _backupActive = Core::Configuration::instance().GetValue<bool>("awsmock.modules.application.backup.active");
        _backupCron = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.application.backup.cron");
        _logServer = Core::Configuration::instance().GetValue<bool>("awsmock.modules.application.log-server");
        _logServerPort = Core::Configuration::instance().GetValue<int>("awsmock.modules.application.log-server-port");

        // Check module active
        if (!IsActive("application")) {
            log_info << "Application module inactive";
            return;
        }
        log_info << "Application module starting";

        // Start application background threads
        _scheduler.AddTask("application-monitoring", [this] { this->UpdateCounter(); }, _monitoringPeriod);
        _scheduler.AddTask("application-restart", [this] { this->RestartApplications(); }, -1);
        _scheduler.AddTask("application-watchdog", [this] { this->WatchdogApplications(); }, _watchdogPeriod, _watchdogPeriod);

        // Start backup
        if (_backupActive) {
            scheduler.AddTask("application-backup", [] { BackupApplication(); }, _backupCron);
        }

        // Start the application log server (websocket)
        if (_logServer) {
            StartApplicationLogServer();
        }

        // Set running
        SetRunning();
        log_debug << "Application server started";
    }

    void ApplicationServer::UpdateCounter() const {
        log_trace << "Application Monitoring starting";

        // Total count
        _metricService.SetGauge(APPLICATION_COUNT, {}, {}, static_cast<double>(_applicationDatabase.CountApplications()));

        // CPU / memory usage
        for (auto &application: _applicationDatabase.ListApplications()) {

            if (!application.containerId.empty() && ContainerService::instance().ContainerExists(application.containerId)) {
                const Dto::Docker::ContainerStat containerStat = ContainerService::instance().GetContainerStats(application.containerId);
                const auto cpuDelta = static_cast<double>(containerStat.cpuStats.cpuUsage.total - containerStat.preCpuStats.cpuUsage.total);
                const auto systemCpuDelta = static_cast<double>(containerStat.cpuStats.cpuUsage.system - containerStat.preCpuStats.cpuUsage.system);
                const auto numberCpus = static_cast<double>(containerStat.cpuStats.onlineCpus);
                if (const auto cpuPercentages = cpuDelta / systemCpuDelta / numberCpus * 100; std::isfinite(cpuPercentages) && cpuPercentages >= 0 && cpuPercentages <= 100) {
                    _metricService.SetGauge(APPLICATION_CPU_USAGE, "application", application.name, cpuPercentages);
                }
                const auto usedMemory = static_cast<double>(containerStat.memoryStats.usage - containerStat.memoryStats.stats.cache);
                const auto totalMemory = static_cast<double>(containerStat.memoryStats.limit);
                if (const double memoryPercentages = usedMemory / totalMemory * 100; std::isfinite(memoryPercentages) && memoryPercentages >= 0 && memoryPercentages <= 100) {
                    _metricService.SetGauge(APPLICATION_MEMORY_USAGE, "application", application.name, memoryPercentages);
                }
            }
        }
        log_debug << "Application monitoring finished, freeShmSize: " << _segment.get_free_memory();
    }

    void ApplicationServer::StartApplications() const {
        for (auto &application: _applicationDatabase.ListApplications()) {
            if (application.enabled) {
                StartApplication(application);
            } else {
                application.status = Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::STOPPED);
                application = _applicationDatabase.UpdateApplication(application);
                log_debug << "Application stopped, name: " << application.name;
            }
        }
    }

    void ApplicationServer::StartApplicationLogServer() const {
        log_info << "Starting application log server";
        ApplicationLogServer applicationLogServer;
        boost::thread t(boost::ref(applicationLogServer), "0.0.0.0", _logServerPort);
        t.detach();
        log_info << "Application log server started";
    }

    auto ApplicationServer::StartApplication(const Database::Entity::Apps::Application &application) const -> void {

        if (!application.dependencies.empty()) {

            // Start dependent application
            for (const auto &dependency: application.dependencies) {
                StartApplication(_applicationDatabase.GetApplication(application.region, dependency));
            }
        }

        // Start actual application
        Dto::Apps::StartApplicationRequest request;
        request.application = Dto::Apps::Mapper::map(application);
        request.region = application.region;
        _applicationService.StartApplication(request);
        log_info << "Application started, name: " << request.application.name;
    }

    auto ApplicationServer::StopApplication(const Database::Entity::Apps::Application &application) const -> void {

        if (!application.dependencies.empty()) {

            // Start dependent application
            for (const auto &dependency: application.dependencies) {
                StopApplication(_applicationDatabase.GetApplication(application.region, dependency));
            }
        }

        // Start actual application
        Dto::Apps::StopApplicationRequest request;
        request.application = Dto::Apps::Mapper::map(application);
        request.region = application.region;
        _applicationService.StopApplication(request);
        log_info << "Application started, name: " << request.application.name;
    }

    void ApplicationServer::BackupApplication() {
        ModuleService::BackupModule("application", true);
    }

    void ApplicationServer::RestartApplications() const {

        // Synchronize containers
        SyncContainers();

        const long restarted = _applicationService.RestartAllApplications();
        log_info << "Applications restarted, count: " << restarted;
    }

    void ApplicationServer::WatchdogApplications() const {

        // Synchronize containers
        SyncContainers();

        // Check status
        for (auto &application: _applicationDatabase.ListApplications()) {

            // If containerId is empty and the application is enabled, start it
            if (application.containerId.empty() && application.enabled) {
                StartApplication(application);
                log_info << "Application started, name: " << application.name;
                continue;
            }

            // If containerId is not empty and the application is disabled, stop it
            if (!application.containerId.empty() && !application.enabled) {
                StopApplication(application);
                log_info << "Application stopped, name: " << application.name;
                continue;
            }

            if (Dto::Docker::Container response = ContainerService::instance().GetContainerByName(application.containerName); response.id.empty()) {
                application.status = Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::STOPPED);
                application.containerId = "";
                application.containerName = "";
                application = _applicationDatabase.UpdateApplication(application);
            }
        }
    }

    void ApplicationServer::SyncContainers() const {

        // Sync docker container with the database
        const auto region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");
        for (const auto &container: ContainerService::instance().ListContainers()) {

            std::string name{}, tag{};
            if (std::vector<std::string> parts = Core::StringUtils::Split(container.image, ":"); parts.size() == 1) {
                name = parts[0];
            } else if (parts.size() == 2) {
                name = parts[0];
            }
            if (_applicationDatabase.ApplicationExists(region, name)) {
                Database::Entity::Apps::Application application = _applicationDatabase.GetApplication(region, name);
                application.region = region;
                application.containerId = container.id;
                application.containerName = container.GetContainerName();
                application.imageName = container.image;
                application.imageId = container.imageId;
                application.status = container.state.running ? Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::RUNNING) : Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::STOPPED);
                application = _applicationDatabase.UpdateApplication(application);
                log_debug << "Application updated, imageName: " << application.imageName;
            }
        }
    }

    void ApplicationServer::Shutdown() {
        log_debug << "Application server shutdown";

        for (std::vector<Database::Entity::Apps::Application> applications = _applicationDatabase.ListApplications(); auto &application: applications) {

            ContainerService::instance().KillContainer(application.containerId);
            log_info << "Application stopped, name: " << application.name;
        }
    }
}// namespace AwsMock::Service
