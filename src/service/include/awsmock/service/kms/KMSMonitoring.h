//
// Created by vogje01 on 4/21/24.
//

#ifndef AWSMOCK_SERVICE_KMS_MONITORING_H
#define AWSMOCK_SERVICE_KMS_MONITORING_H

// AwsMock includes
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/repository/KMSDatabase.h>
#include <awsmock/service/monitoring/MetricService.h>

namespace AwsMock::Service {

    /**
     * @brief KMS monitoring thread
     *
     * @par
     * The KMS monitoring is 'self-starting', which means the constructor start itself as a background process.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class KMSMonitoring {
      public:

        /**
         * @brief Constructor
         */
        explicit KMSMonitoring() = default;

        /**
         * @brief Update counters
         */
        void UpdateCounter();

      private:

        /**
         * @brief Metric service
         */
        Monitoring::MetricService &_metricService = Monitoring::MetricService::instance();

        /**
         * @brief Database connection
         */
        Database::KMSDatabase &_kmsDatabase = Database::KMSDatabase::instance();
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_KMS_MONITORING_H
