//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/transfer/TransferServer.h>

namespace AwsMock::Service {
    TransferServer::TransferServer(Core::PeriodicScheduler &scheduler) : AbstractServer("transfer"), _transferDatabase(Database::TransferDatabase::instance()) {
        // REST manager configuration
        _monitoringPeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.transfer.monitoring.period");

        // Check module active
        if (!IsActive("transfer")) {
            log_info << "Transfer module inactive";
            return;
        }
        log_info << "Transfer server starting";

        // Start SNS monitoring update counters
        scheduler.AddTask("monitoring-transfer-counters", [this] { UpdateCounter(); }, _monitoringPeriod);

        // Create transfer bucket
        CreateTransferBucket();

        // Start all transfer servers
        StartTransferServers();

        // Set running
        SetRunning();

        log_info << "Transfer server initialized";
    }

    void TransferServer::CreateTransferBucket() {
        Dto::S3::CreateBucketRequest request;
        request.owner = Core::Configuration::instance().GetValue<std::string>("awsmock.user");
        request.region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");
        request.name = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.transfer.bucket");
        if (const S3Service s3Service; !s3Service.BucketExists(request.region, request.name)) {
            log_debug << "Creating bucket " << request.name << ", region: " << request.region << ", owner: " << request.owner;
            Dto::S3::CreateBucketResponse response = s3Service.CreateBucket(request);
        }
    }

    void TransferServer::CreateDirectories(const std::string &userName) {
        const std::string basePath = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.transfer.data-dir");
        for (const auto &directory: Core::Configuration::instance().GetValueArray<std::string>("awsmock.modules.transfer.directories")) {
            if (std::string dirPath = basePath + Core::FileUtils::separator() + userName + Core::FileUtils::separator() + directory; !Core::DirUtils::DirectoryExists(dirPath)) {
                Core::DirUtils::MakeDirectory(dirPath, true);
                log_debug << "Created directory, path: " << dirPath;
            }
        }
    }

    void TransferServer::StartTransferServer(Database::Entity::Transfer::Transfer &server) {

        for (const auto protocol: server.protocols) {
            if (protocol == Database::Entity::Transfer::Protocol::FTP) {
                StartFtpServer(server);
            } else if (protocol == Database::Entity::Transfer::Protocol::SFTP) {
                StartSftpServer(server);
            }
        }

        // Update database
        server.lastStarted = system_clock::now();
        server.state = Database::Entity::Transfer::ServerState::ONLINE;
        server = _transferDatabase.UpdateTransfer(server);
        log_info << "Transfer server started, serverId: " << server.serverId << " address: " << server.listenAddress << ", protocols: " << server.protocols.size();
    }

    void TransferServer::StartFtpServer(Database::Entity::Transfer::Transfer &server) {

        // Get base dir
        const std::string baseDir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.transfer.data-dir");
        const int port = Core::Configuration::instance().GetValue<int>("awsmock.modules.transfer.ftp.port");
        const std::string address = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.transfer.ftp.address");

        // Create a transfer manager thread
        _ftpServer = std::make_shared<FtpServer::FtpServer>(server.serverId, port, address);
        _transferServerList[server.serverId] = _ftpServer;

        // Add users
        for (const auto &user: server.users) {
            std::string homeDir = baseDir + Core::FileUtils::separator() + user.homeDirectory;

            // Ensure the home directory exists
            Core::DirUtils::EnsureDirectory(homeDir);
            log_debug << "User created, userId: " << user.userName << " homeDir: " << homeDir;

            // Create default directories
            CreateDirectories(user.userName);

            // Add to FTP manager
            if (_ftpServer->addUser(user.userName, user.password, homeDir, FtpServer::Permission::All)) {
                log_debug << "User created successfully";
            }
        }
        if (_ftpServer->start(server.concurrency)) {
            log_info << "FTP server started, id: " << server.serverId << ", endpoint: " << address << ":" << port;
        }
    }

    void TransferServer::StartSftpServer(Database::Entity::Transfer::Transfer &server) {

        // Get base dir
        const std::string baseDir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.transfer.data-dir");
        const int port = Core::Configuration::instance().GetValue<int>("awsmock.modules.transfer.sftp.port");
        const std::string address = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.transfer.sftp.address");
        const std::string hostKey = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.transfer.sftp.host-key");

        SftpServer _sftpServer;

        // Add users
        for (const auto &user: server.users) {

            // Create a home directory
            std::string homeDir = baseDir + Core::FileUtils::separator() + user.homeDirectory;
            Core::DirUtils::EnsureDirectory(homeDir);

            // Add user
            SftpServer::AddUser(user.userName, user.password, homeDir);

            // Ensure the home directory exists
            log_debug << "User created, userId: " << user.userName << " homeDir: " << homeDir;
        }

        // Start a detached thread
        boost::thread t(boost::ref(_sftpServer), std::to_string(port), hostKey, address, server.serverId);
        t.detach();
        log_info << "SFTP server started, id: " << server.serverId << ", endpoint: " << address << ":" << port;
    }

    void TransferServer::StopTransferServer(Database::Entity::Transfer::Transfer &server) {

        // Create a transfer manager thread
        const std::shared_ptr<FtpServer::FtpServer> ftpServer = _transferServerList[server.serverId];
        ftpServer->stop();

        // Update database
        server.state = Database::Entity::Transfer::ServerState::OFFLINE;
        log_info << "Transfer server " << server.serverId << " stopped, address = " << server.listenAddress;
    }

    void TransferServer::StartTransferServers() {
        log_info << "Starting transfer servers, count: " << _transferDatabase.CountServers(_region);

        for (std::vector<Database::Entity::Transfer::Transfer> transfers = _transferDatabase.ListServers(_region); auto &transfer: transfers) {
            if (transfer.state == Database::Entity::Transfer::ServerState::ONLINE) {
                StartTransferServer(transfer);
            }
        }
    }

    void TransferServer::CheckTransferServers() {
        log_trace << "Checking transfer servers";

        for (std::vector<Database::Entity::Transfer::Transfer> transfers = _transferDatabase.ListServers(_region); auto &transfer: transfers) {
            if (transfer.state == Database::Entity::Transfer::ServerState::ONLINE) {
                if (auto it = _transferServerList.find(transfer.serverId); it == _transferServerList.end()) {
                    StartTransferServer(transfer);
                    log_info << "Transfer server started, serverId: " << transfer.serverId;
                }
            } else if (transfer.state == Database::Entity::Transfer::ServerState::OFFLINE) {
                if (auto it = _transferServerList.find(transfer.serverId); it != _transferServerList.end()) {
                    StopTransferServer(transfer);
                    log_info << "Transfer server stopped, serverId: " << transfer.serverId;
                }
            }
        }

        for (const auto &key: _transferServerList | std::views::keys) {
            if (!_transferDatabase.TransferExists(key)) {
                Database::Entity::Transfer::Transfer server = _transferDatabase.GetTransferByServerId(_region, key);
                StopTransferServer(server);
                log_info << "Transfer server stopped, serverId: " << key;
            }
        }
    }

    void TransferServer::UpdateCounter() const {
        log_trace << "Transfer monitoring starting";

        const long servers = _transferDatabase.CountServers();
        _metricService.SetGauge(TRANSFER_SERVER_COUNT, static_cast<double>(servers));

        log_trace << "Transfer monitoring finished";
    }
}// namespace AwsMock::Service
