//
// Created by vogje01 on 06/10/2023.
//

#ifndef AWSMOCK_SERVICE_SSM_SERVER_H
#define AWSMOCK_SERVICE_SSM_SERVER_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/scheduler/PeriodicScheduler.h>
#include <awsmock/core/scheduler/PeriodicTask.h>
#include <awsmock/repository/SSMDatabase.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/monitoring/MetricDefinition.h>
#include <awsmock/service/monitoring/MetricService.h>

namespace AwsMock::Service {

    /**
     * @brief SSM module server
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SSMServer final : public AbstractServer {

      public:

        /**
         * @brief Constructor
         */
        explicit SSMServer(Core::PeriodicScheduler &scheduler);

      private:

        /**
         * @brief Update counters
         */
        void UpdateCounter() const;

        /**
         * @brief Metric service
         */
        Monitoring::MetricService &_metricService = Monitoring::MetricService::instance();

        /**
         * @brief Database connection
         */
        Database::SSMDatabase &_ssmDatabase = Database::SSMDatabase::instance();

        /**
         * @brief SSM server period
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

#endif// AWSMOCK_SERVICE_KMS_SERVER_H
