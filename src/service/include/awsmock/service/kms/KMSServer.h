//
// Created by vogje01 on 06/10/2023.
//

#ifndef AWSMOCK_SERVICE_KMS_SERVER_H
#define AWSMOCK_SERVICE_KMS_SERVER_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/scheduler/PeriodicScheduler.h>
#include <awsmock/core/scheduler/PeriodicTask.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/kms/KMSMonitoring.h>
#include <awsmock/service/kms/KMSWorker.h>
#include <awsmock/service/monitoring/MetricService.h>

namespace AwsMock::Service {

    /**
     * @brief KMS HTTP server
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class KMSServer final : public AbstractServer {

      public:

        /**
         * @brief Constructor
         */
        explicit KMSServer(Core::PeriodicScheduler &scheduler);

      private:

        /**
         * KMS database
         */
        Database::KMSDatabase &_kmsDatabase;

        /**
         * SNS monitoring
         */
        KMSMonitoring _kmsMonitoring;

        /**
         * SNS worker
         */
        KMSWorker _kmsWorker;

        /**
         * @brief KMS server period
         *
         * <p>
         * Used for the background threads (cleanup, reset, retention, etc.)
         * </p>
         */
        int _removePeriod;

        /**
         * KMS monitoring period
         */
        int _monitoringPeriod;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_KMS_SERVER_H
