//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_MONITORING_SERVER_H
#define AWSMOCK_SERVICE_MONITORING_SERVER_H

// AwsMock includes
#include <awsmock/core/EventBus.h>
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/service/apps/ApplicationService.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/container/ContainerService.h>
#include <awsmock/service/monitoring/MetricDockerCollector.h>
#include <awsmock/service/monitoring/MetricSystemCollector.h>
#include <awsmock/service/monitoring/MonitoringCollector.h>
#include <awsmock/service/monitoring/MonitoringService.h>

namespace AwsMock::Service {

    /**
     * @brief Monitoring module server
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class MonitoringServer final : public AbstractServer {

      public:

        /**
         * @brief Constructor
         *
         * @param scheduler boost periodic scheduler
         * @param ioc boost io context boost periodic scheduler
         */
        explicit MonitoringServer(Core::Scheduler &scheduler, boost::asio::io_context &ioc);

        /**
         * @brief Delete monitoring data older than the retention period.
         *
         * @par
         * Delete all monitoring data older than the retention period. Default is 3 days, which means monitoring data older than 3 days will be deleted.
         */
        [[maybe_unused]] void DeleteMonitoringData() const;

        /**
         * @brief Shutdown the server
         */
        void Shutdown() override;

      private:

        /**
         * @brief Checks monitoring exclusions.
         *
         * @param name metric name
         * @param labelName metric label name
         * @param labelValue metric label value
         * @return true if supplied values are not in exclusion list
         */
        bool CheckExclusions(const std::string &name, const std::string &labelName, const std::string &labelValue) const;

        /**
         * @brief Asynchronous task scheduler
         */
        Core::Scheduler &_scheduler;

        /**
         * @brief Monitoring system collector
         */
        Monitoring::MetricSystemCollector _metricSystemCollector;

        /**
         * @brief Monitoring docker collector
         */
        Monitoring::MetricDockerCollector _metricDockerCollector;

        /**
         * @brief Database connection
         */
        Database::MonitoringDatabase &_monitoringDatabase = Database::MonitoringDatabase::instance();

        /**
         * @brief Database connection
         */
        Monitoring::MonitoringCollector _monitoringCollector;

        /**
         * @brief Exclusion list
         *
         * @par
         * The exclusion list is a vector of string in format name::labelName::labelValue.
         */
        std::vector<std::string> _exclusions;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_MONITORING_SERVER_H
