//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_SERVICE_TRANSFER_SERVER_H
#define AWSMOCK_SERVICE_TRANSFER_SERVER_H

// AwsMock includes
#include <awsmock/core/EventBus.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/scheduler/PeriodicTask.h>
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/ftpserver/FtpServer.h>
#include <awsmock/repository/TransferDatabase.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/module/ModuleService.h>
#include <awsmock/service/s3/S3Service.h>
#include <awsmock/sftpserver/SftpServer.h>

namespace AwsMock::Service {

    /**
     * @brief Transfer module server
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class TransferServer final : public AbstractServer {

      public:

        /**
         * @brief Constructor
         */
        explicit TransferServer(Core::Scheduler &scheduler, boost::asio::io_context &ioc);

      private:

        /**
         * @brief Creates the transfer server bucket
         */
        void CreateTransferBucket() const;

        /**
         * @brief Creates the transfer server default directories
         *
         * @param userName name of the user
         */
        static void CreateDirectories(const std::string &userName);

        /**
         * @brief Starts a single transfer manager
         *
         * @param server transfer manager entity
         */
        void StartTransferServer(Database::Entity::Transfer::Transfer &server);

        /**
         * @brief Starts an FTP server
         *
         * @param server transfer manager entity
         */
        void StartFtpServer(Database::Entity::Transfer::Transfer &server);

        /**
         * @brief Starts an SFTP server
         *
         * @param server transfer manager entity
         */
        static void StartSftpServer(Database::Entity::Transfer::Transfer &server);

        /**
         * @brief Stops a single transfer manager
         *
         * @param server transfer manager entity
         */
        void StopTransferServer(Database::Entity::Transfer::Transfer &server);

        /**
         * @brief Start all transfer servers, if they are not existing
         */
        void StartTransferServers();

        /**
         * @brief Check transfer servers
         */
        void CheckTransferServers();

        /**
         * @brief Update counters
         */
        void UpdateCounter() const;

        /**
         * @brief Backup the transfer server
         */
        static void BackupTransfer();

        /**
         * @brief Shutdown server
         */
        void Shutdown() override;

        /**
         * Transfer database
         */
        Database::TransferDatabase &_transferDatabase;

        /**
         * @brief Metric service
         */
        Monitoring::MetricService &_metricService = Monitoring::MetricService::instance();

        /**
         * AWS region
         */
        std::string _region;

        /**
         * AWS client ID
         */
        std::string _clientId;

        /**
         * AWS user
         */
        std::string _user;

        /**
         * AWS S3 bucket
         */
        std::string _bucket;

        /**
         * Base dir for all FTP users
         */
        std::string _baseDir;

        /**
         * Base URL for all S3 request
         */
        std::string _baseUrl;

        /**
         * Server userPoolId
         */
        std::string _serverId;

        /**
         * Monitoring period
         */
        int _monitoringPeriod;

        /**
         * @brief Transfer server backup flag.
         *
         * @par
         * If true, backup tables and items based on cron expression
         */
        bool _backupActive;

        /**
         * @brief Transfer server backup cron schedule.
         *
         * @par
         * Cron schedule in form '* * * * * ?', with seconds, minutes, hours, dayOfMonth, month, dayOfWeek, year (optional)
         *
         * @see @link(https://github.com/mariusbancila/croncpp)croncpp
         */
        std::string _backupCron;

        /**
         * List of transfer servers
         */
        std::map<std::string, std::shared_ptr<FtpServer::FtpServer>> _transferServerList;

        /**
         * Actual FTP manager
         */
        std::shared_ptr<FtpServer::FtpServer> _ftpServer;

        /**
         * Actual SFTP manager
         */
        std::shared_ptr<SftpServer> _sftpServer;

        /**
         * Asynchronous task scheduler
         */
        boost::asio::io_context &_ioc;

        /**
         * @brif Asynchronous tasks scheduler
         */
        Core::Scheduler &_scheduler;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_TRANSFER_SERVER_H
