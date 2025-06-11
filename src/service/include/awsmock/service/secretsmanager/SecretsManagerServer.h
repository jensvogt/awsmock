//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_SERVICE_SECRETSMANAGER_SERVER_H
#define AWSMOCK_SERVICE_SECRETSMANAGER_SERVER_H

// C++ standard includes
#include <chrono>
#include <string>

// Boost includes
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/scheduler/PeriodicScheduler.h>
#include <awsmock/core/scheduler/PeriodicTask.h>
#include <awsmock/repository/SecretsManagerDatabase.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/monitoring/MetricService.h>

#define SECRETSMANAGER_DEFAULT_MONITORING_PERIOD 300

namespace AwsMock::Service {

    /**
     * @brief Secret manager module server.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SecretsManagerServer final : public AbstractServer {

      public:

        /**
         * @brief Constructor
         */
        explicit SecretsManagerServer(Core::PeriodicScheduler &scheduler);

      private:

        /**
         * @brief Update counters
         */
        void UpdateCounter() const;

        /**
         * @brief Database connection
         */
        Database::SecretsManagerDatabase &_secretsManagerDatabase = Database::SecretsManagerDatabase::instance();

        /**
         * @brief Metric service
         */
        Monitoring::MetricService &_metricService = Monitoring::MetricService::instance();

        /**
         * Monitoring period
         */
        int _monitoringPeriod;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_SECRETSMANAGER_SERVER_H
