//
// Created by vogje01 on 06/10/2023.
//

#ifndef AWSMOCK_SERVICE_SNS_SERVER_H
#define AWSMOCK_SERVICE_SNS_SERVER_H

// AwsMock includes
#include <awsmock/core/EventBus.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/scheduler/PeriodicTask.h>
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/repository/SNSDatabase.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/module/ModuleService.h>

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
         * @brief Shutdown server
         */
        void Shutdown() override;

        /**
         * @brief Database connection
         */
        Database::SNSDatabase &_snsDatabase = Database::SNSDatabase::instance();

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
         * Monitoring collector
         */
        Core::MonitoringCollector &_monitoringCollector;

        /**
         * @brif Asynchronous tasks scheduler
         */
        Core::Scheduler &_scheduler;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_SNSSERVER_H
