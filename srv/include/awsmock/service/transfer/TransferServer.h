//
// Created by vogje01 on 03/06/2023.
//

#pragma once

// AwsMock includes
#include <awsmock/core/EventBus.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/ftpserver/FtpServer.h>
#include <awsmock/repository/transfer/TransferMongoRepository.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/module/ModuleService.h>
#include <awsmock/service/s3/S3Service.h>
#include <awsmock/sftpserver/SftpServer.h>

namespace Awsmock::Service {

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
        explicit TransferServer();

        /**
         * @brief Shutdown server
         */
        void shutdown() override;

      private:

        mutable logger_t _logger{boost::log::keywords::channel = "Transfer"};

        /**
         * @brief Creates the transfer server bucket
         */
        void CreateTransferBucket() const;

        /**
         * @brief Creates the transfer server default directories
         *
         * @param userName name of the user
         */
        void CreateDirectories(const std::string &userName) const;

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
        void StartSftpServer(Database::Entity::Transfer::Transfer &server) const;

        /**
         * @brief Stops a single transfer manager
         *
         * @param server transfer manager entity
         */
        void StopTransferServer(Database::Entity::Transfer::Transfer &server);

        /**
         * @brief Start all transfer servers if they are not existing
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
         * @brief Back up the transfer server
         */
        static void BackupTransfer();

        /**
         * @brief Transfer database
         */
        std::shared_ptr<Database::ITransferRepository> _transferDatabase = Database::RepositoryFactory::instance().transferRepository();

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

    };

}// namespace Awsmock::Service
