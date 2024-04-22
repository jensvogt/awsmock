//
// Created by vogje01 on 4/21/24.
//

#ifndef AWSMOCK_SERVICE_LAMBDA_MONITORING_H
#define AWSMOCK_SERVICE_LAMBDA_MONITORING_H

// AwsMock includes
#include <awsmock/core/MetricDefinition.h>
#include <awsmock/core/MetricService.h>
#include <awsmock/core/Timer.h>
#include <awsmock/repository/LambdaDatabase.h>

namespace AwsMock::Service {

    class LambdaMonitoring : public Core::Timer {

      public:

        /**
         * Constructor
         */
        explicit LambdaMonitoring(int timeout) : Core::Timer("lambda-monitoring", timeout) {}

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
        Database::LambdaDatabase &_lambdaDatabase = Database::LambdaDatabase::instance();

        /**
         * Period
         */
        int _period{};

    };
}
#endif // AWSMOCK_SERVICE_LAMBDA_MONITORING_H