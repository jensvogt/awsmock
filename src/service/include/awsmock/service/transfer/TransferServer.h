//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_SERVICE_TRANSFER_SERVER_H
#define AWSMOCK_SERVICE_TRANSFER_SERVER_H

// C++ standard includes
#include <map>
#include <memory>
#include <string>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/scheduler/PeriodicScheduler.h>
#include <awsmock/core/scheduler/PeriodicTask.h>
#include <awsmock/dto/s3/CreateBucketConstraint.h>
#include <awsmock/ftpserver/FtpServer.h>
#include <awsmock/repository/TransferDatabase.h>
#include <awsmock/service/common/AbstractServer.h>
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
        explicit TransferServer(Core::PeriodicScheduler &scheduler);

      private:

        /**
         * @brief Creates the transfer server bucket
         */
        static void CreateTransferBucket();

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
}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_TRANSFER_SERVER_H
