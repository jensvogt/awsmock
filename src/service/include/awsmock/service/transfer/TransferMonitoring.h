//
// Created by vogje01 on 4/21/24.
//

#ifndef AWSMOCK_SERVICE_TRANSFER_MONITORING_H
#define AWSMOCK_SERVICE_TRANSFER_MONITORING_H

// AwsMock includes
#include <awsmock/core/Timer.h>
#include <awsmock/core/monitoring/MetricDefinition.h>
#include <awsmock/core/monitoring/MetricService.h>
#include <awsmock/repository/TransferDatabase.h>

namespace AwsMock::Service {

    /**
     * @brief Transfer family monitoring thread
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class TransferMonitoring : public Core::Timer {

      public:

        /**
         * Constructor
         *
         * @param timeout monitoring period in seconds.
         */
        explicit TransferMonitoring(int timeout) : Core::Timer("cognito-monitoring", timeout) {}

        /**
         * Initialization
         */
        void Initialize() override;

        /**
         * Main method
         */
        void Run() override;

        /**
         * Shutdown
         */
        void Shutdown() override;

      private:

        /**
         * Update counters
         */
        void UpdateCounter();

        /**
         * Metric service
         */
        Core::MetricService &_metricService = Core::MetricService::instance();

        /**
         * Database connection
         */
        Database::TransferDatabase &_transferDatabase = Database::TransferDatabase::instance();

        /**
         * Period
         */
        int _period{};
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_TRANSFER_MONITORING_H
