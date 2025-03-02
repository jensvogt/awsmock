//
// Created by vogje01 on 4/21/24.
//

#ifndef AWSMOCK_SERVICE_SECRETSMANAGER_MONITORING_H
#define AWSMOCK_SERVICE_SECRETSMANAGER_MONITORING_H

// AwsMock includes
#include <awsmock/repository/SecretsManagerDatabase.h>
#include <awsmock/service/monitoring/MetricDefinition.h>
#include <awsmock/service/monitoring/MetricService.h>

namespace AwsMock::Service {

    /**
     * @brief Secret manager monitoring thread
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SecretsManagerMonitoring {

      public:

        /**
         * @brief Constructor
         */
        explicit SecretsManagerMonitoring() = default;

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
        Database::SecretsManagerDatabase &_secretsManagerDatabase = Database::SecretsManagerDatabase::instance();
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_SECRETSMANAGER_MONITORING_H
