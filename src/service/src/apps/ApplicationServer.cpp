//
// Created by vogje01 on 04/01/2023.
//

#include "awsmock/service/apps/ApplicationLogServer.h"


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

        // Initialize shared memory
        _segment = boost::interprocess::managed_shared_memory(boost::interprocess::open_only, SHARED_MEMORY_SEGMENT_NAME);
        _applicationCounterMap = _segment.find<Database::ApplicationCounterMapType>(Database::APPLICATION_COUNTER_MAP_NAME).first;

        // Start application monitoring update counters
        _scheduler.AddTask("application-monitoring", [this] { this->UpdateCounter(); }, _monitoringPeriod);
        _scheduler.AddTask("application-updates", [this] { this->UpdateApplications(); }, _monitoringPeriod, _monitoringPeriod);

        // Start backup
        if (_backupActive) {
            scheduler.AddTask("application-backup", [this] { BackupApplication(); }, _backupCron);
        }

        // Start the application
        StartApplicationLogServer();

        // Start the application
        StartApplications();

        // Set running
        SetRunning();
        log_debug << "Application server started";
    }

    void ApplicationServer::UpdateCounter() const {
        log_trace << "Application Monitoring starting";

        if (_applicationCounterMap) {
            _metricService.SetGauge(APPLICATION_COUNT, {}, {}, static_cast<double>(_applicationCounterMap->size()));
        }

        // CPU / memory usage
        for (auto &application: _applicationDatabase.ListApplications()) {

            if (!application.containerId.empty()) {
                const Dto::Docker::ContainerStat containerStat = ContainerService::instance().GetContainerStats(application.containerId);
                const auto cpuDelta = (double) (containerStat.cpuStats.cpuUsage.total - containerStat.preCpuStats.cpuUsage.total);
                const auto systemCpuDelta = (double) (containerStat.cpuStats.cpuUsage.system - containerStat.preCpuStats.cpuUsage.system);
                const auto numberCpus = (double) containerStat.cpuStats.onlineCpus;
                if (const auto cpuPercentages = cpuDelta / systemCpuDelta / numberCpus * 100; std::isfinite(cpuPercentages) && cpuPercentages >= 0 && cpuPercentages <= 100) {
                    _metricService.SetGauge(APPLICATION_CPU_USAGE, "application", application.name, cpuPercentages);
                }
                const auto usedMemory = (double) (containerStat.memoryStats.usage - containerStat.memoryStats.stats.cache);
                const auto totalMemory = (double) containerStat.memoryStats.limit;
                if (const double memoryPercentages = usedMemory / totalMemory * 100; std::isfinite(memoryPercentages) && memoryPercentages >= 0 && memoryPercentages <= 100) {
                    _metricService.SetGauge(APPLICATION_MEMORY_USAGE, "application", application.name, memoryPercentages);
                }
            }
        }
        log_debug << "Application monitoring finished, freeShmSize: " << _segment.get_free_memory();
    }

    void ApplicationServer::UpdateApplications() const {
        for (auto &application: _applicationDatabase.ListApplications()) {
            if (application.enabled) {
                Dto::Docker::Container container = ContainerService::instance().GetFirstContainerByImageName(application.name, application.version);
                if (Dto::Docker::InspectContainerResponse response = ContainerService::instance().InspectContainer(container.id); response.status == http::status::ok) {
                    application.status = response.state.status == "running" ? Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::RUNNING) : Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::STOPPED);
                    application.containerId = response.id;
                    application.containerName = Core::StringUtils::StartsWith(response.name, "/") ? response.name.substr(1) : response.name;
                    application.imageId = response.image;
                    application.imageSize = response.sizeRootFs;
                    application = _applicationDatabase.UpdateApplication(application);
                    log_debug << "Application updated, name: " << application.name << ", status: " << application.status;
                }
            }
        }
    }

    void ApplicationServer::StartApplications() const {
        for (const auto &application: _applicationDatabase.ListApplications()) {
            if (application.enabled) {
                DoAddApplication(application);
            }
        }
    }

    void ApplicationServer::StartApplicationLogServer() {
        log_info << "Starting application log server";
        ApplicationLogServer applicationLogServer;
        boost::thread t(boost::ref(applicationLogServer), "0.0.0.0", 4568);
        t.detach();
        log_info << "Application log server started";
    }

    auto ApplicationServer::DoAddApplication(const Database::Entity::Apps::Application &application) const -> void {
        if (!application.dependencies.empty()) {
            for (const auto &d: application.dependencies) {
                Database::Entity::Apps::Application dependency = _applicationDatabase.GetApplication(application.region, d);
                DoAddApplication(dependency);
            }
        }
        Dto::Apps::StartApplicationRequest request;
        request.application = Dto::Apps::Mapper::map(application);
        request.region = application.region;
        _applicationService.StartApplication(request);
        log_info << "Application started, name: " << request.application.name;
    }

    void ApplicationServer::BackupApplication() {
        ModuleService::BackupModule("application", true);
    }

    void ApplicationServer::Shutdown() {
        log_debug << "Application server shutdown";

        for (std::vector<Database::Entity::Apps::Application> applications = _applicationDatabase.ListApplications(); auto &application: applications) {

            ContainerService::instance().StopContainer(application.containerId);
            ContainerService::instance().DeleteContainer(application.containerId);
            application = _applicationDatabase.UpdateApplication(application);
            log_info << "Application stopped, name: " << application.name;
        }
    }

}// namespace AwsMock::Service
