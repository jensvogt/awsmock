#pragma once

#include <algorithm>
#include <deque>
#include <fstream>
#include <functional>
#include <iomanip>
#include <map>
#include <sstream>
#include <utility>
#ifdef WIN32
#include <direct.h>
#endif

// Asio includes
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

// AwsMock includes
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/ftpserver/Filesystem.h>
#include <awsmock/ftpserver/FtpMessage.h>
#include <awsmock/ftpserver/FtpUser.h>
#include <awsmock/ftpserver/UserDatabase.h>
#include <awsmock/service/s3/S3Service.h>

namespace AwsMock::FtpServer {
    typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;

    class FtpSession : public std::enable_shared_from_this<FtpSession> {

      private:

        struct IoFile {
            IoFile(const std::string &filename, std::string user, const std::ios::openmode mode) : file_stream_(filename, mode), stream_buffer_(1024 * 1024), _fileName(filename), _user(std::move(user)) {
                file_stream_.rdbuf()->pubsetbuf(stream_buffer_.data(), static_cast<std::streamsize>(stream_buffer_.size()));
            }

            // Copy
            IoFile(const IoFile &) = delete;

            IoFile &operator=(const IoFile &) = delete;

            // Move disabled (as we are storing the shared_from_this() pointer in lambda captures)
            IoFile &operator=(IoFile &&) = delete;

            IoFile(IoFile &&) = delete;

            ~IoFile() {
                file_stream_.flush();
                file_stream_.close();
            }

            std::fstream file_stream_;
            std::vector<char> stream_buffer_;
            std::string _fileName;
            std::string _user;
        };

        ////////////////////////////////////////////////////////
        // Public API
        ////////////////////////////////////////////////////////
      public:

        FtpSession(boost::asio::io_context &awsIoc, const UserDatabase &user_database, std::string serverName, const std::function<void()> &completion_handler);

        // Copy (disabled, as we are inheriting from shared_from_this)
        FtpSession(const FtpSession &) = delete;

        FtpSession &operator=(const FtpSession &) = delete;

        // Move (disabled, as we are inheriting from shared_from_this)
        FtpSession &operator=(FtpSession &&) = delete;

        FtpSession(FtpSession &&) = delete;

        ~FtpSession();

        void start();

        boost::asio::ip::tcp::socket &getSocket();

        ////////////////////////////////////////////////////////
        // FTP command-socket
        ////////////////////////////////////////////////////////
      private:

        void sendFtpMessage(const FtpMessage &message);

        void sendFtpMessage(FtpReplyCode code, const std::string &message);

        void sendFtpMessageSftp(FtpReplyCode code, const std::string &message);

        void sendRawFtpMessage(const std::string &raw_message);

        void sendRawFtpMessageSftp(const std::string &raw_message);

        void startSendingMessages();

        void readFtpCommand();

        void readFtpCommandSftp();

        void handleFtpCommand(const std::string &command);

        ////////////////////////////////////////////////////////
        // FTP Commands
        ////////////////////////////////////////////////////////
      private:

        // Access control commands
        void handleFtpCommandUSER(const std::string &param);

        void handleFtpCommandPASS(const std::string &param);

        void handleFtpCommandACCT(const std::string &param);

        void handleFtpCommandCWD(const std::string &param);

        void handleFtpCommandCDUP(const std::string &param);

        void handleFtpCommandREIN(const std::string &param);

        void handleFtpCommandQUIT(const std::string &param);

        // Transfer parameter commands
        void handleFtpCommandPORT(const std::string &param);

        void handleFtpCommandEPRT(const std::string &param);

        void handleFtpCommandPASV(const std::string &param);

        void handleFtpCommandEPSV(const std::string &param);

        void handleFtpCommandTYPE(const std::string &param);

        void handleFtpCommandSTRU(const std::string &param);

        void handleFtpCommandMODE(const std::string &param);

        void handleFtpCommandLPRT(const std::string &param);

        void handleFtpCommandLPSV(const std::string &param);

        // Ftp module commands
        void handleFtpCommandRETR(const std::string &param);

        void handleFtpCommandSIZE(const std::string &param);

        void handleFtpCommandSTOR(const std::string &param);

        void handleFtpCommandSTOU(const std::string &param);

        void handleFtpCommandAPPE(const std::string &param);

        void handleFtpCommandALLO(const std::string &param);

        void handleFtpCommandREST(const std::string &param);

        void handleFtpCommandRNFR(const std::string &param);

        void handleFtpCommandRNTO(const std::string &param);

        void handleFtpCommandABOR(const std::string &param);

        void handleFtpCommandDELE(const std::string &param);

        void handleFtpCommandRMD(const std::string &param);

        void handleFtpCommandMKD(const std::string &param);

        void handleFtpCommandPWD(const std::string &param);

        void handleFtpCommandLIST(const std::string &param);

        void handleFtpCommandNLST(const std::string &param);

        void handleFtpCommandSITE(const std::string &param);

        void handleFtpCommandSYST(const std::string &param);

        void handleFtpCommandSTAT(const std::string &param);

        void handleFtpCommandHELP(const std::string &param);

        void handleFtpCommandNOOP(const std::string &param);

        // Modern FTP Commands
        void handleFtpCommandFEAT(const std::string &param);

        void handleFtpCommandOPTS(const std::string &param);

        ////////////////////////////////////////////////////////
        // FTP data-socket send
        ////////////////////////////////////////////////////////
      private:

        void sendDirectoryListing(const std::map<std::string, FileStatus> &directory_content);

        void sendNameList(const std::map<std::string, FileStatus> &directory_content);

        void sendFile(const std::shared_ptr<IoFile> &file);

        void readDataFromFileAndSend(const std::shared_ptr<IoFile> &file, const std::shared_ptr<boost::asio::ip::tcp::socket> &data_socket);

        void addDataToBufferAndSend(
                const std::shared_ptr<std::vector<char>> &data,
                const std::shared_ptr<boost::asio::ip::tcp::socket> &data_socket,
                const std::function<void(void)> &fetch_more = []() { return; });

        void writeDataToSocket(const std::shared_ptr<boost::asio::ip::tcp::socket> &data_socket, const std::function<void(void)> &fetch_more);

        ////////////////////////////////////////////////////////
        // FTP data-socket receive
        ////////////////////////////////////////////////////////
      private:

        void receiveFile(const std::shared_ptr<IoFile> &file);

        void receiveDataFromSocketAndWriteToFile(const std::shared_ptr<IoFile> &file, const std::shared_ptr<boost::asio::ip::tcp::socket> &data_socket);

        void writeDataToFile(const std::shared_ptr<std::vector<char>> &data, const std::shared_ptr<IoFile> &file, const std::function<void()> &fetch_more = []() { return; });

        void endDataReceiving(const std::shared_ptr<IoFile> &file);

        ////////////////////////////////////////////////////////
        // Helpers
        ////////////////////////////////////////////////////////
      private:

        std::string toAbsoluteFtpPath(const std::string &rel_or_abs_ftp_path) const;

        std::string toLocalPath(const std::string &ftp_path) const;

        static std::string createQuotedFtpPath(const std::string &unquoted_ftp_path);

        /**
         * @brief Checks if a path is renamable
         *
         * Checks if the current user can rename the given path. A path is renameable
         * if it exists, a user is logged in and the user has sufficient permissions
         * (file / dir / both) to rename it.
         *
         * @param ftp_path: The source path
         *
         * @return (COMMAND_OK, "") if the path can be renamed or any other meaningfull error message if not.
         */
        FtpMessage checkIfPathIsRenamable(const std::string &ftp_path) const;

        FtpMessage executeCWD(const std::string &param);

        /**
         * @brief Send file to AWS S3
         *
         * @par
         * The S3 request will have 2 metadata fields: user-agent=serverName and user-agent-id=FTP-user\@serverName.
         *
         * @param user user name
         * @param fileName filename
         */
        void SendCreateObjectRequest(const std::string &user, const std::string &fileName) const;

        /**
         * @brief Delete file in AWS S3
         *
         * @param user user name
         * @param fileName filename
         */
        void SendDeleteObjectRequest(const std::string &user, const std::string &fileName) const;

        /**
         * @brief Extract the S3 key from the file path.
         *
         * @param path file system path
         * @return S3 key
         */
        std::string GetKey(const std::string &path) const;

        ////////////////////////////////////////////////////////
        // Member variables
        ////////////////////////////////////////////////////////
      private:

        /**
         * SFTP handshake
         */
        void DoHandshake();

        /**
         * Metric service
         */
        Monitoring::MetricService &_metricService = Monitoring::MetricService::instance();

        /**
         * Completion handler
         */
        const std::function<void()> _completion_handler;

        /**
         * User management
         */
        const UserDatabase &_user_database;

        /**
         * Current user
         */
        std::shared_ptr<FtpUser> _logged_in_user;

        /**
         * Global IO module
         */
        boost::asio::io_context &_io_service;

        /**
         * Command Socket
         */
        boost::asio::ip::tcp::socket command_socket_;
        boost::asio::io_context::strand command_write_strand_;
        boost::asio::streambuf command_input_stream_;
        std::deque<std::string> command_output_queue_;

        std::string _lastCommand;
        std::string _renameFromPath;
        std::string _usernameForLogin;

        /**
         * @brief Data Socket (=> passive mode)
         */
        bool data_type_binary_;
        boost::asio::ip::tcp::acceptor data_acceptor_;
        std::weak_ptr<boost::asio::ip::tcp::socket> data_socket_weakptr_;
        std::deque<std::shared_ptr<std::vector<char>>> data_buffer_;
        boost::asio::io_context::strand data_buffer_strand_;
        boost::asio::io_context::strand file_rw_strand_;

        /**
         * Current state
         */
        std::string _ftpWorkingDirectory;

        /**
         * Server name
         */
        std::string _serverName;

        /**
         * S3 service
         */
        std::shared_ptr<Service::S3Service> _s3Service;

        /**
         * Transfer manager base dir
         */
        std::string _baseDir;

        /**
         * S3 module base URL
         */
        std::string _baseUrl;

        /**
         * Transfer server base dir
         */
        std::string _transferDir;

        /**
         * S3 bucket name
         */
        std::string _bucket;

        /**
         * AWS region
         */
        std::string _region;

        /**
         * AwsMock IOC
         */
        boost::asio::io_context &_awsIoc;
    };
}// namespace AwsMock::FtpServer
