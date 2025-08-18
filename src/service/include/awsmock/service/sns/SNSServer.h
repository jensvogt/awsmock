//
// Created by vogje01 on 06/10/2023.
//

#ifndef AWSMOCK_SERVICE_SNS_SERVER_H
#define AWSMOCK_SERVICE_SNS_SERVER_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/scheduler/PeriodicTask.h>
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/repository/SNSDatabase.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/module/ModuleService.h>
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
        explicit SNSServer(Core::Scheduler &scheduler);

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
         * @brief Backup the SNS topics and messages
         */
        static void BackupSns();

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
         * @brief SNS backup flag.
         *
         * @par
         * If true, backup tables and items based on cron expression
         */
        bool _backupActive;

        /**
         * @brief SNS backup cron schedule.
         *
         * @par
         * Cron schedule in form '* * * * * ?', with seconds, minutes, hours, dayOfMonth, month, dayOfWeek, year (optional)
         *
         * @see @link(https://github.com/mariusbancila/croncpp)croncpp
         */
        std::string _backupCron;

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
