//
// Created by vogje01 on 06/10/2023.
//

#ifndef AWSMOCK_SERVICE_SSM_SERVER_H
#define AWSMOCK_SERVICE_SSM_SERVER_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/scheduler/PeriodicTask.h>
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/repository/SSMDatabase.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/module/ModuleService.h>

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
        explicit SSMServer(Core::Scheduler &scheduler);

      private:

        /**
         * @brief Update counters
         */
        void UpdateCounter() const;

        /**
         * @brief Backup the SSM objects
         */
        static void BackupSsm();

        /**
         * @brief Shutdown server
         */
        void Shutdown() override;

        /**
         * @brief Database connection
         */
        Database::SSMDatabase &_ssmDatabase = Database::SSMDatabase::instance();

        /**
         * @brief SSM server backup flag.
         *
         * @par
         * If true, backup tables and items based on cron expression
         */
        bool _backupActive;

        /**
         * @brief SSM server backup cron schedule.
         *
         * @par
         * Cron schedule in form '* * * * * ?', with seconds, minutes, hours, dayOfMonth, month, dayOfWeek, year (optional)
         *
         * @see @link(https://github.com/mariusbancila/croncpp)croncpp
         */
        std::string _backupCron;

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
        /**
         * Asynchronous task scheduler
         */
        Core::Scheduler &_scheduler;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_KMS_SERVER_H
