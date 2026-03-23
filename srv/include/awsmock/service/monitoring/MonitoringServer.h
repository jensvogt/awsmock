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

      private:

        /**
         * @brief Shutdown the server
         */
        void Shutdown() override;

        /**
         * Docker counter collector
         */
        static void CollectDockerCounter();

        /**
         * Asynchronous task scheduler
         */
        Core::Scheduler &_scheduler;

        /**
         * Monitoring system collector
         */
        Monitoring::MetricSystemCollector _metricSystemCollector;

        /**
         * Monitoring docker collector
         */
        Monitoring::MetricDockerCollector _metricDockerCollector;

        /**
         * Database connection
         */
        Database::MonitoringDatabase &_monitoringDatabase = Database::MonitoringDatabase::instance();

        /**
         * Database connection
         */
        Monitoring::MonitoringCollector _monitoringCollector;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_MONITORING_SERVER_H
