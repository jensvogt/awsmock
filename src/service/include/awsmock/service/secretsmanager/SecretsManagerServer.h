//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_SERVICE_SECRETSMANAGER_SERVER_H
#define AWSMOCK_SERVICE_SECRETSMANAGER_SERVER_H

// C++ standard includes
#include <chrono>
#include <string>

// Boost includes
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/scheduler/PeriodicTask.h>
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/repository/SecretsManagerDatabase.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/module/ModuleService.h>
#include <awsmock/service/monitoring/MetricService.h>

#define SECRETSMANAGER_DEFAULT_MONITORING_PERIOD 300

namespace AwsMock::Service {

    /**
     * @brief Secret manager module server.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SecretsManagerServer final : public AbstractServer {

      public:

        /**
         * @brief Constructor
         */
        explicit SecretsManagerServer(Core::Scheduler &scheduler);

      private:

        /**
         * @brief Update counters
         */
        void UpdateCounter() const;

        /**
         * @brief Backup the secrets manager objects
         */
        static void BackupSecretsManger();

        /**
         * @brief Database connection
         */
        Database::SecretsManagerDatabase &_secretsManagerDatabase = Database::SecretsManagerDatabase::instance();

        /**
         * @brief Metric service
         */
        Monitoring::MetricService &_metricService = Monitoring::MetricService::instance();

        /**
         * @brief Secrets manager backup flag.
         *
         * @par
         * If true, backup tables and items based on cron expression
         */
        bool _backupActive;

        /**
         * @brief Secrets manager backup cron schedule.
         *
         * @par
         * Cron schedule in form '* * * * * ?', with seconds, minutes, hours, dayOfMonth, month, dayOfWeek, year (optional)
         *
         * @see @link(https://github.com/mariusbancila/croncpp)croncpp
         */
        std::string _backupCron;

        /**
         * Monitoring period
         */
        int _monitoringPeriod;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_SECRETSMANAGER_SERVER_H
