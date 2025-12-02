//
// Created by vogje01 on 06/10/2023.
//

#ifndef AWSMOCK_SERVICE_KMS_SERVER_H
#define AWSMOCK_SERVICE_KMS_SERVER_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/scheduler/PeriodicTask.h>
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/kms/KMSMonitoring.h>
#include <awsmock/service/kms/KMSWorker.h>
#include <awsmock/service/module/ModuleService.h>
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
        explicit KMSServer(Core::Scheduler &scheduler);

      private:

        /**
         * @brief Update counters
         */
        void UpdateCounter() const;

        /**
         * @brief Delete keys which are pending for deletion
         */
        void DeleteKeys() const;

        /**
         * @brief Backup the KMS objects
         */
        static void BackupKms();

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
         * @brief Metric service
         */
        Monitoring::MetricService &_metricService = Monitoring::MetricService::instance();

        /**
         * @brief Dynamo DB backup flag.
         *
         * @par
         * If true, backup tables and items based on cron expression
         */
        bool _backupActive;

        /**
         * @brief Dynamo DB backup cron schedule.
         *
         * @par
         * Cron schedule in form '* * * * * ?', with seconds, minutes, hours, dayOfMonth, month, dayOfWeek, year (optional)
         *
         * @see @link(https://github.com/mariusbancila/croncpp)croncpp
         */
        std::string _backupCron;

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
