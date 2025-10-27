#pragma once

// C++ includes
#include <atomic>
#include <string>
#include <thread>
#include <vector>

// Asio includes
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

// AwsMock includes
#include <awsmock/core/ServerCertificate.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/ftpserver/FtpSession.h>
#include <awsmock/ftpserver/FtpUser.h>
#include <awsmock/ftpserver/UserDatabase.h>

namespace AwsMock::FtpServer {

    class FtpServerImpl {

      public:

        /**
         * Constructor
         *
         * @param serverName name of the manager
         * @param address listen address
         * @param port listen port
         * @param awsIoc AwsMock IO context
         */
        FtpServerImpl(std::string serverName, std::string address, uint16_t port, boost::asio::io_context &awsIoc);

        /**
         * Copy (constructor disabled)
         *
         * @param ftpServerImpl server implementation
         */
        FtpServerImpl(const FtpServerImpl &ftpServerImpl) = delete;

        /**
         * Copy (constructor disabled)
         *
         * @param ftpServerImpl server implementation
         * @return FTP server implementation
         */
        FtpServerImpl &operator=(const FtpServerImpl &ftpServerImpl) = delete;

        /**
         * Assigment constructor (disabled, as we are storing the this pointer in lambda captures)
         *
         * @param ftpServerImpl server implementation
         * @return FTP manager implementation
         */
        FtpServerImpl &operator=(FtpServerImpl &&ftpServerImpl) = delete;

        /**
         * Assigment constructor (disabled, as we are storing the this pointer in lambda captures)
         *
         * @param ftpServerImpl server implementation
         */
        FtpServerImpl(FtpServerImpl &&ftpServerImpl) = delete;

        /**
         * Destructor
         */
        ~FtpServerImpl();

        /**
         * Add normal user.
         *
         * @param username name of the user
         * @param password user password
         * @param local_root_path home directory
         * @param permissions permissions
         * @return true if successful
         */
        bool addUser(const std::string &username, const std::string &password, const std::string &local_root_path, Permission permissions);

        /**
         * Add normal user.
         *
         * @param local_root_path home directory
         * @param permissions permissions
         * @return true if successful
         */
        bool addUserAnonymous(const std::string &local_root_path, Permission permissions);

        /**
         * Start the manager
         *
         * @param thread_count thread count
         * @return true if successful
         */
        bool start(size_t thread_count = 1);

        /**
         * Stop the manager
         */
        void stop()const;

        /**
         * Return the number of open connections
         *
         * @return number of open connections
         */
        int getOpenConnectionCount();

        /**
         * Return the listen port
         *
         * @return listening port
         */
        uint16_t getPort() const;

        /**
         * Return the listen address
         *
         * @return listening address
         */
        std::string getAddress() const;

      private:

        /**
         * Accept FTP session
         *
         * @param ftp_session FTP session
         * @param error error handler
         */
        void acceptFtpSession(const std::shared_ptr<FtpSession> &ftp_session, boost::beast::error_code const &error);

        /**
         * User database
         */
        UserDatabase _ftpUsers;

        /**
         * FTP port
         */
        const uint16_t _port;

        /**
         * Socket listen address
         */
        const std::string _address;

        /**
         * FTP session thread pool
         */
        std::vector<std::thread> _threadPool;

        /**
         * Asio IO module
         */
        //boost::asio::io_context _ioService;

        /**
         * Asio session acceptor
         */
        boost::asio::ip::tcp::acceptor _acceptor;

        /**
         * Number of open connections
         */
        std::atomic<int> _openConnectionCount;

        /**
         * Name of the manager
         */
        std::string _serverName;

        /**
         * AwsMock IOC
         */
        boost::asio::io_context &_awsIoc;
    };
}// namespace AwsMock::FtpServer
