//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_MONITORING_SERVER_H
#define AWSMOCK_SERVICE_MONITORING_SERVER_H

// AwsMock includes
#include <awsmock/core/scheduler/PeriodicScheduler.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/monitoring/MetricSystemCollector.h>
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
         */
        explicit MonitoringServer(Core::PeriodicScheduler &scheduler);

        /**
       * @brief Delete monitoring data older than the retention period.
       *
       * @par
       * Delete al monitoring data oder than the retention period. Default is 3 days, which means monitoring data older than 3 days will be deleted.
       */
        [[maybe_unused]] void DeleteMonitoringData() const;

      private:

        /**
         * Monitoring system collector
         */
        Monitoring::MetricSystemCollector _metricSystemCollector;

        /**
         * Database connection
         */
        Database::MonitoringDatabase &_monitoringDatabase = Database::MonitoringDatabase::instance();
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_MONITORING_SERVER_H
