//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_S3_SERVER_H
#define AWSMOCK_SERVICE_S3_SERVER_H

// C++ includes
#include <sys/types.h>
#include <sys/stat.h>

// Boost includes
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/asio/thread_pool.hpp>
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringCollector.h>
#include <awsmock/core/scheduler/PeriodicTask.h>
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/module/ModuleService.h>
#include <awsmock/service/s3/S3Service.h>

namespace AwsMock::Service {

    using namespace boost::filesystem;

    /**
     * @brief S3 module server
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class S3Server final : public AbstractServer {

      public:

        /**
         * @brief Constructor
         */
        explicit S3Server(Core::Scheduler &scheduler);

      private:

        /**
         * @brief Synchronize S3 object between filesystem and database.
         */
        [[maybe_unused]] void SyncObjects() const;

        /**
         * Update counters
         */
        void UpdateCounter() const;

        /**
         * @brief Backup the S3 buckets and object
         */
        static void BackupS3();

        /**
         * Metric service
         */
        Monitoring::MetricService &_metricService = Monitoring::MetricService::instance();

        /**
         * Database connection
         */
        Database::S3Database &_s3Database = Database::S3Database::instance();

        /**
         * Monitoring period
         */
        int _monitoringPeriod{};

        /**
         * Worker directory object synchronization period
         */
        int _syncPeriod;

        /**
         * S3 bucket counter period
         */
        int _counterPeriod;

        /**
         * @brief Backup directory
         *
         * @par
         * If true, backup tables and items based on cron expression
         */
        std::string _backupDir;

        /**
         * @brief S3 backup flag.
         *
         * @par
         * If true, backup buckets and object based on cron expression
         */
        bool _backupActive;

        /**
         * @brief Backup retention in days
         *
         * @par
         * If positive, delete old backups with modification date < now - backupRetention days
         */
        int _backupRetention;

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
         * Monitoring collector
         */
        Core::MonitoringCollector &_monitoringCollector;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_S3_SERVER_H
