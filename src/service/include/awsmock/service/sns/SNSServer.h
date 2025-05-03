//
// Created by vogje01 on 06/10/2023.
//

#ifndef AWSMOCK_SERVICE_SNS_SERVER_H
#define AWSMOCK_SERVICE_SNS_SERVER_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/scheduler/PeriodicScheduler.h>
#include <awsmock/core/scheduler/PeriodicTask.h>
#include <awsmock/repository/SNSDatabase.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/monitoring/MetricDefinition.h>
#include <awsmock/service/monitoring/MetricService.h>

namespace AwsMock::Service {

    /**
     * @brief SNS module server
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SNSServer final : public AbstractServer {

      public:

        /**
         * @brief Constructor
         */
        explicit SNSServer(Core::PeriodicScheduler &scheduler);

      private:

        /**
         * @brief Delete resources, which are over the retention period.
         */
        void DeleteOldMessages() const;

        /**
         * @brief Update counters
         */
        void UpdateCounter() const;

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
        int _deletePeriod;

        /**
         * @brief SNS server period
         *
         * <p>
         * Used for the background threads (cleanup, reset, retention, etc.)
         * </p>
         */
        int _counterPeriod;

        /**
         * @brief SNS monitoring period
         */
        int _monitoringPeriod;

        /**
         * Shared memory segment
         */
        boost::interprocess::managed_shared_memory _segment;

        /**
         * Counter map in a shared memory segment
         */
        Database::SnsCounterMapType *_snsCounterMap{};
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_SNSSERVER_H
