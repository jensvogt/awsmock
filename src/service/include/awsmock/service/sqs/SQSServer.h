//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_SERVICE_SQS_SERVER_H
#define AWSMOCK_SERVICE_SQS_SERVER_H

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/scheduler/PeriodicScheduler.h>
#include <awsmock/core/scheduler/PeriodicTask.h>
#include <awsmock/repository/SQSDatabase.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/monitoring/MetricDefinition.h>
#include <awsmock/service/monitoring/MetricService.h>

namespace AwsMock::Service {

    /**
     * @brief SQS module server
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SQSServer : public AbstractServer {

      public:

        /**
         * @brief Constructor
         */
        explicit SQSServer(Core::PeriodicScheduler &scheduler);

      private:

        /**
         * @brief Reset resources
         *
         * @par
         * Loops over all SQS queues and sets the state to INITIAL in case the visibilityTimeout timeout has been reached. Also, the retry count is increased by one.
         *
         * @par
         * Checks also the expiration date and removed the resources, which are older than the max retention period.
         */
        void ResetMessages() const;

        /**
         * @brief Collect waiting time statistics
         *
         * @par
         * Collects the average waiting time for messages in that queue.
         */
        void CollectWaitingTimeStatistics() const;

        /**
         * @brief Update counters
         */
        void UpdateCounter() const;

        /**
         * @brief Sets the DLQ flags
         */
        void SetDlq() const;

        /**
         * Metric service
         */
        Monitoring::MetricService &_metricService = Monitoring::MetricService::instance();

        /**
         * SQS database
         */
        Database::SQSDatabase _sqsDatabase;

        /**
         * SQS monitoring period
         */
        int _monitoringPeriod;

        /**
         * SQS reset period
         */
        int _resetPeriod;

        /**
         * SQS adjust counter period
         */
        int _counterPeriod;

        /**
         * Shared memory segment
         */
        boost::interprocess::managed_shared_memory _segment;

        /**
         * Counter map in a shared memory segment
         */
        Database::SqsCounterMapType *_sqsCounterMap{};
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_SQS_SERVER_H
