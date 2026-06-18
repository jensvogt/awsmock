//
// Created by vogje01 on 06/10/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/EventBus.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/kms/KMSMonitoring.h>
#include <awsmock/service/kms/KMSWorker.h>
#include <awsmock/service/module/ModuleService.h>

namespace Awsmock::Service {

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
        explicit KMSServer();

        /**
         * @brief Shutdown the server
         */
        void shutdown() override;

      private:

        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "KMS"};

        /**
         * @brief Update counters
         */
        void updateCounter() const;

        /**
         * @brief Delete keys which are pending for deletion
         */
        void deleteKeys() const;

        /**
         * @brief Back up the KMS objects
         */
        static void backupKms();

        /**
         * KMS database
         */
        std::shared_ptr<Database::IKMSRepository> _kmsDatabase = Database::RepositoryFactory::instance().kmsRepository();

        /**
         * SNS monitoring
         */
        KMSMonitoring _kmsMonitoring;

        /**
         * SNS worker
         */
        KMSWorker _kmsWorker;

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

}// namespace Awsmock::Service
