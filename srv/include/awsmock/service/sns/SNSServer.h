//
// Created by vogje01 on 06/10/2023.
//

#pragma once

// AwsMock includes
#include <awsmock/core/EventBus.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/repository/RepositoryFactory.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/module/ModuleService.h>

namespace Awsmock::Service {

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

        /**
         * @brief Shutdown server
         */
        void Shutdown() override;

      private:

        /**
         * Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "SNS"};

        /**
         * @brief Delete resources, which are over the retention period.
         */
        void DeleteOldMessages() const;

        /**
         * @brief Update counters
         */
        void UpdateCounter() const;

        /**
         * @brief Back up the SNS topics and messages
         */
        static void BackupSns();

        /**
         * @brief Database connection
         */
        std::shared_ptr<Database::ISNSRepository> _snsDatabase = Database::RepositoryFactory::instance().snsRepository();

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
         * @brif Asynchronous tasks scheduler
         */
        Core::Scheduler &_scheduler;
    };

}// namespace Awsmock::Service
