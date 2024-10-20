//
// Created by vogje01 on 06/10/2023.
//

#ifndef AWSMOCK_SERVICE_SNS_SERVER_H
#define AWSMOCK_SERVICE_SNS_SERVER_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/core/scheduler/PeriodicScheduler.h>
#include <awsmock/core/scheduler/PeriodicTask.h>
#include <awsmock/repository/SNSDatabase.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/monitoring/MetricDefinition.h>
#include <awsmock/service/monitoring/MetricService.h>

#define SNS_DEFAULT_WORKER_PERIOD 300
#define SNS_DEFAULT_MONITORING_PERIOD 300
#define SNS_DEFAULT_MESSAGE_TIMEOUT 15

namespace AwsMock::Service {

    /**
     * @brief SNS module server
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SNSServer : public AbstractServer {

      public:

        /**
         * @brief Constructor
         */
        explicit SNSServer(Core::PeriodicScheduler &scheduler);

      private:

        /**
         * @brief Delete resources, which are over the retention period.
         */
        void DeleteOldMessages();

        /**
         * @brief Synchronizes the topic available messages counters.
         */
        void SychronizeCounters();

        /**
         * @brief Update counters
         */
        void UpdateCounter();

        /**
         * @brief Database connection
         */
        Database::SNSDatabase &_snsDatabase = Database::SNSDatabase::instance();

        /**
         * Metric service
         */
        Monitoring::MetricService &_metricService = Monitoring::MetricService::instance();

        /**
         * @brief SNS server period
         *
         * <p>
         * Used for the background threads (cleanup, reset, retention, etc.)
         * </p>
         */
        int _workerPeriod;

        /**
         * @brief SNS monitoring period
         */
        int _monitoringPeriod;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_SNSSERVER_H
