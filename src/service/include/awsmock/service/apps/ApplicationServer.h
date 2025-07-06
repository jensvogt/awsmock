//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_APPLICATION_SERVER_H
#define AWSMOCK_SERVICE_APPLICATION_SERVER_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/repository/ApplicationDatabase.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/module/ModuleService.h>
#include <awsmock/service/monitoring/MetricService.h>

namespace AwsMock::Service {

    /**
     * @brief Application module server
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ApplicationServer final : public AbstractServer {

      public:

        /**
         * @brief Constructor
         */
        explicit ApplicationServer(Core::Scheduler &scheduler);

      private:

        /**
         * @brief Update counters
         */
        void UpdateCounter() const;

        /**
         * @brief Update application status
         */
        void UpdateApplications() const;

        /**
         * @brief Backup the application objects
         */
        static void BackupApplication();

        /**
         * @brief Metric service
         */
        Monitoring::MetricService &_metricService = Monitoring::MetricService::instance();

        /**
         * @brief Database connection
         */
        Database::ApplicationDatabase &_applicationDatabase = Database::ApplicationDatabase::instance();

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
         * Cognito module name
         */
        std::string _module;

        /**
         * Monitoring period
         */
        int _monitoringPeriod;

        /**
         * Asynchronous task scheduler
         */
        Core::Scheduler &_scheduler;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_COGNITO_SERVER_H