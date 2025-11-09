
#include <awsmock/ftpserver/FtpSession.h>

namespace AwsMock::FtpServer {
    FtpSession::FtpSession(boost::asio::io_context &awsIoc, const UserDatabase &user_database, std::string serverName, const std::function<void()> &completion_handler)
        : _completion_handler(completion_handler), _user_database(user_database), _io_service(awsIoc),
          command_socket_(awsIoc), command_write_strand_(awsIoc), data_type_binary_(true),
          data_acceptor_(awsIoc), data_buffer_strand_(awsIoc), file_rw_strand_(awsIoc),
          _ftpWorkingDirectory("/"), _serverName(std::move(serverName)), _awsIoc(awsIoc) {
        // Environment
        const Core::Configuration &configuration = Core::Configuration::instance();
        _region = configuration.GetValue<std::string>("awsmock.region");
        _bucket = configuration.GetValue<std::string>("awsmock.modules.transfer.bucket");
        _transferDir = configuration.GetValue<std::string>("awsmock.modules.transfer.data-dir");

        // S3 service
        _s3Service = std::make_shared<Service::S3Service>(_awsIoc);
    }

    FtpSession::~FtpSession() {
        log_debug << "Ftp Session shutting down";
        _completion_handler();
    }

    void FtpSession::start() {
        boost::beast::error_code ec;

        getSocket().set_option(boost::asio::ip::tcp::no_delay(true), ec);
        if (ec) {
            log_error << "Unable to set socket option tcp::no_delay: " << ec.message();
        }
        sendFtpMessage(FtpMessage(FtpReplyCode::SERVICE_READY_FOR_NEW_USER, "Welcome to AwsMock Transfer FTP Handler"));
        readFtpCommand();
    }

    boost::asio::ip::tcp::socket &FtpSession::getSocket() {
        return command_socket_;
    }

    void FtpSession::sendFtpMessage(const FtpMessage &message) {
        sendRawFtpMessage(message.str());
    }

    void FtpSession::sendFtpMessage(const FtpReplyCode code, const std::string &message) {
        sendFtpMessage(FtpMessage(code, message));
    }

    void FtpSession::sendRawFtpMessage(const std::string &raw_message) {
        command_write_strand_.post([me = shared_from_this(), raw_message]() {
                                       const bool write_in_progress = !me->command_output_queue_.empty();
                                       me->command_output_queue_.push_back(raw_message);
                                       if (!write_in_progress) {
                                           me->startSendingMessages();
                                       }
                                   },
                                   nullptr);
    }

    void FtpSession::startSendingMessages() {
        log_debug << "FTP >> " << Core::StringUtils::StripLineEndings(command_output_queue_.front());
        async_write(command_socket_,
                    boost::asio::buffer(command_output_queue_.front()),
                    command_write_strand_.wrap(
                            [me = shared_from_this()](const boost::beast::error_code &ec, std::size_t /*bytes_to_transfer*/) {
                                if (!ec) {
                                    me->command_output_queue_.pop_front();

                                    if (!me->command_output_queue_.empty()) {
                                        me->startSendingMessages();
                                    }
                                } else {
                                    log_error << "Command write error: " << ec.message();
                                }
                            }));
    }

    void FtpSession::readFtpCommand() {
        async_read_until(command_socket_,
                         command_input_stream_,
                         "\r\n",
                         [me = shared_from_this()](const boost::beast::error_code &ec, const std::size_t length) {
                             if (ec) {
                                 if (ec != boost::asio::error::eof) {
                                     log_error << "Read_until error: " << ec.message();
                                 } else {
                                     log_debug << "Control connection closed by client.";
                                 }

                                 // Close the data connection if it is open
                                 {
                                     boost::beast::error_code ec_;
                                     me->data_acceptor_.close(ec_);
                                 }
                                 {
                                     if (const auto data_socket = me->data_socket_weakptr_.lock()) {
                                         boost::beast::error_code ec_;
                                         data_socket->close(ec_);
                                     }
                                 }

                                 return;
                             }

                             std::istream stream(&(me->command_input_stream_));
                             std::string packet_string(length - 2, ' ');
                             // NOLINT(readability-container-data-pointer) Reason: I need a non-const pointer here, As I am directly reading into the buffer,
                             // but .data() returns a const pointer. I don't consider a const_cast to be better. Since C++11 this is safe, as strings are stored
                             // in contiguous memory.
                             stream.read(&packet_string[0], length - 2);

                             stream.ignore(2);// Remove the "\r\n"
                             log_debug << "FTP << " << packet_string;

                             me->handleFtpCommand(packet_string);
                         });
    }

    void FtpSession::handleFtpCommand(const std::string &command) {
        std::string parameters;

        const size_t space_index = command.find_first_of(' ');

        std::string ftp_command = command.substr(0, space_index);
        std::ranges::transform(ftp_command, ftp_command.begin(), [](const char c) {
            return static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        });

        if (space_index != std::string::npos) {
            parameters = command.substr(space_index + 1, std::string::npos);
        }

        const std::map<std::string, std::function<void(std::string)> > command_map{
                // Access control commands
                {"USER", std::bind(&FtpSession::handleFtpCommandUSER, this, std::placeholders::_1)},
                {"PASS", std::bind(&FtpSession::handleFtpCommandPASS, this, std::placeholders::_1)},
                {"ACCT", std::bind(&FtpSession::handleFtpCommandACCT, this, std::placeholders::_1)},
                {"CWD", std::bind(&FtpSession::handleFtpCommandCWD, this, std::placeholders::_1)},
                {"CDUP", std::bind(&FtpSession::handleFtpCommandCDUP, this, std::placeholders::_1)},
                {"REIN", std::bind(&FtpSession::handleFtpCommandREIN, this, std::placeholders::_1)},
                {"QUIT", std::bind(&FtpSession::handleFtpCommandQUIT, this, std::placeholders::_1)},

                // Transfer parameter commands
                {"PORT", std::bind(&FtpSession::handleFtpCommandPORT, this, std::placeholders::_1)},
                {"PASV", std::bind(&FtpSession::handleFtpCommandPASV, this, std::placeholders::_1)},
                {"EPRT", std::bind(&FtpSession::handleFtpCommandEPRT, this, std::placeholders::_1)},
                {"EPSV", std::bind(&FtpSession::handleFtpCommandEPSV, this, std::placeholders::_1)},
                {"TYPE", std::bind(&FtpSession::handleFtpCommandTYPE, this, std::placeholders::_1)},
                {"STRU", std::bind(&FtpSession::handleFtpCommandSTRU, this, std::placeholders::_1)},
                {"MODE", std::bind(&FtpSession::handleFtpCommandMODE, this, std::placeholders::_1)},
                {"LPRT", std::bind(&FtpSession::handleFtpCommandLPRT, this, std::placeholders::_1)},
                {"LPSV", std::bind(&FtpSession::handleFtpCommandLPSV, this, std::placeholders::_1)},

                // Ftp module commands
                {"RETR", std::bind(&FtpSession::handleFtpCommandRETR, this, std::placeholders::_1)},
                {"STOR", std::bind(&FtpSession::handleFtpCommandSTOR, this, std::placeholders::_1)},
                {"STOU", std::bind(&FtpSession::handleFtpCommandSTOU, this, std::placeholders::_1)},
                {"APPE", std::bind(&FtpSession::handleFtpCommandAPPE, this, std::placeholders::_1)},
                {"ALLO", std::bind(&FtpSession::handleFtpCommandALLO, this, std::placeholders::_1)},
                {"REST", std::bind(&FtpSession::handleFtpCommandREST, this, std::placeholders::_1)},
                {"RNFR", std::bind(&FtpSession::handleFtpCommandRNFR, this, std::placeholders::_1)},
                {"RNTO", std::bind(&FtpSession::handleFtpCommandRNTO, this, std::placeholders::_1)},
                {"ABOR", std::bind(&FtpSession::handleFtpCommandABOR, this, std::placeholders::_1)},
                {"DELE", std::bind(&FtpSession::handleFtpCommandDELE, this, std::placeholders::_1)},
                {"RMD", std::bind(&FtpSession::handleFtpCommandRMD, this, std::placeholders::_1)},
                {"MKD", std::bind(&FtpSession::handleFtpCommandMKD, this, std::placeholders::_1)},
                {"PWD", std::bind(&FtpSession::handleFtpCommandPWD, this, std::placeholders::_1)},
                {"LIST", std::bind(&FtpSession::handleFtpCommandLIST, this, std::placeholders::_1)},
                {"NLST", std::bind(&FtpSession::handleFtpCommandNLST, this, std::placeholders::_1)},
                {"SITE", std::bind(&FtpSession::handleFtpCommandSITE, this, std::placeholders::_1)},
                {"SYST", std::bind(&FtpSession::handleFtpCommandSYST, this, std::placeholders::_1)},
                {"STAT", std::bind(&FtpSession::handleFtpCommandSTAT, this, std::placeholders::_1)},
                {"HELP", std::bind(&FtpSession::handleFtpCommandHELP, this, std::placeholders::_1)},
                {"NOOP", std::bind(&FtpSession::handleFtpCommandNOOP, this, std::placeholders::_1)},

                // Modern FTP Commands
                {"FEAT", std::bind(&FtpSession::handleFtpCommandFEAT, this, std::placeholders::_1)},
                {"OPTS", std::bind(&FtpSession::handleFtpCommandOPTS, this, std::placeholders::_1)},
                {"SIZE", std::bind(&FtpSession::handleFtpCommandSIZE, this, std::placeholders::_1)},
        };

        if (const auto command_it = command_map.find(ftp_command); command_it != command_map.end()) {
            command_it->second(parameters);
            _lastCommand = ftp_command;
        } else {
            sendFtpMessage(FtpReplyCode::SYNTAX_ERROR_UNRECOGNIZED_COMMAND, "Unrecognized command");
        }

        if (_lastCommand == "QUIT") {
            // Close command socket
            command_write_strand_.wrap([me = shared_from_this()]() {
                me->command_socket_.close();
            });
        } else {
            // Wait for next command
            readFtpCommand();
        }
    }

    ////////////////////////////////////////////////////////
    // FTP Commands
    ////////////////////////////////////////////////////////

    // Access control commands

    void FtpSession::handleFtpCommandUSER(const std::string &param) {
        _logged_in_user = nullptr;
        _usernameForLogin = param;
        _ftpWorkingDirectory = "/";

        if (param.empty()) {
            sendFtpMessage(FtpReplyCode::SYNTAX_ERROR_PARAMETERS, "Please provide username");
        } else {
            sendFtpMessage(FtpReplyCode::USER_NAME_OK, "Please enter password");
        }
    }

    void FtpSession::handleFtpCommandPASS(const std::string &param) {
        if (_lastCommand != "USER") {
            sendFtpMessage(FtpReplyCode::COMMANDS_BAD_SEQUENCE, "Please specify username first");
        } else {
            if (const auto user = _user_database.getUser(_usernameForLogin, param); user != nullptr) {
                _logged_in_user = user;
                sendFtpMessage(FtpReplyCode::USER_LOGGED_IN, "Login successful");
            } else {
                sendFtpMessage(FtpReplyCode::NOT_LOGGED_IN, "Failed to log in");
            }
        }
    }

    void FtpSession::handleFtpCommandACCT(const std::string & /*param*/) {
        sendFtpMessage(FtpReplyCode::SYNTAX_ERROR_UNRECOGNIZED_COMMAND, "Unsupported command");
    }

    void FtpSession::handleFtpCommandCWD(const std::string &param) {
        sendFtpMessage(executeCWD(param));
    }

    void FtpSession::handleFtpCommandCDUP(const std::string & /*param*/) {
        if (!_logged_in_user) {
            sendFtpMessage(FtpReplyCode::NOT_LOGGED_IN, "Not logged in");
            return;
        }
        if (static_cast<int>(_logged_in_user->permissions_ & Permission::DirList) == 0) {
            sendFtpMessage(FtpReplyCode::ACTION_NOT_TAKEN, "Permission denied");
            return;
        }

        if (_ftpWorkingDirectory != "/") {
            // Only CDUP when we are not already at the root directory
            if (const auto cwd_reply = executeCWD(".."); cwd_reply.replyCode() == FtpReplyCode::FILE_ACTION_COMPLETED) {
                // The CWD returns FILE_ACTION_COMPLETED on success, while CDUP returns COMMAND_OK on success.
                sendFtpMessage(FtpReplyCode::COMMAND_OK, cwd_reply.message());
                return;
            } else {
                sendFtpMessage(cwd_reply);
                return;
            }
        } else {
            sendFtpMessage(FtpReplyCode::ACTION_NOT_TAKEN, "Already at root directory");
            return;
        }
    }

    void FtpSession::handleFtpCommandREIN(const std::string & /*param*/) {
        sendFtpMessage(FtpReplyCode::COMMAND_NOT_IMPLEMENTED, "Unsupported command");
    }

    void FtpSession::handleFtpCommandQUIT(const std::string & /*param*/) {
        _logged_in_user = nullptr;
        sendFtpMessage(FtpReplyCode::SERVICE_CLOSING_CONTROL_CONNECTION, "Connection shutting down");
    }

    // Transfer parameter commands

    void FtpSession::handleFtpCommandPORT(const std::string & /*param*/) {
        sendFtpMessage(FtpReplyCode::SYNTAX_ERROR_UNRECOGNIZED_COMMAND, "FTP active mode is not supported by this manager");
    }

    void FtpSession::handleFtpCommandPASV(const std::string & /*param*/) {
        if (!_logged_in_user) {
            log_error << "Not logged in";
            sendFtpMessage(FtpReplyCode::NOT_LOGGED_IN, "Not logged in");
            return;
        }

        if (data_acceptor_.is_open()) {
            boost::beast::error_code ec;
            ec = data_acceptor_.close(ec);
            if (ec) {
                log_error << "Error closing data acceptor: " << ec.message();
            }
        }

        // In the case of a dockerized FTP server, we need to use some special ports
        boost::asio::ip::tcp::endpoint endpoint;
        if (Core::Configuration::instance().GetValue<bool>("awsmock.dockerized")) {
            int minPort = Core::Configuration::instance().GetValue<int>("awsmock.modules.transfer.ftp.pasv-min");
            int maxPort = Core::Configuration::instance().GetValue<int>("awsmock.modules.transfer.ftp.pasv-max");
            int port = Core::RandomUtils::NextInt(minPort, maxPort);
            endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port);
        } else {
            endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 0);
        }
        log_trace << "Passive mode endpoint: " << endpoint.address().to_string() << ":" << endpoint.port();
        {
            boost::beast::error_code ec;
            ec = data_acceptor_.open(endpoint.protocol(), ec);
            if (ec) {
                log_error << "Error opening data acceptor: " << ec.message();
                sendFtpMessage(FtpReplyCode::SERVICE_NOT_AVAILABLE, "Failed to enter passive mode.");
                return;
            }
        }
        {
            boost::beast::error_code ec;
            ec = data_acceptor_.bind(endpoint, ec);
            if (ec) {
                log_error << "Error binding data acceptor: " << ec.message() << " endpoint: " << endpoint.address().to_string() << ":" << endpoint.port();
                sendFtpMessage(FtpReplyCode::SERVICE_NOT_AVAILABLE, "Failed to enter passive mode.");
                return;
            }
        }
        {
            boost::beast::error_code ec;
            ec = data_acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
            if (ec) {
                log_error << "Error listening on data acceptor: " << ec.message();
                sendFtpMessage(FtpReplyCode::SERVICE_NOT_AVAILABLE, "Failed to enter passive mode.");
                return;
            }
        }

        // Split address and port into bytes and get the port the OS chose for us
        boost::asio::ip::address_v4::bytes_type ip_bytes;
        if (Core::Configuration::instance().GetValue<bool>("awsmock.dockerized")) {
            ip_bytes = boost::asio::ip::make_address_v4("127.0.0.1").to_bytes();
        } else {
            ip_bytes = boost::asio::ip::make_address_v4("127.0.0.1").to_bytes();
        }
        auto port = data_acceptor_.local_endpoint().port();
        log_debug << "Server suggested port: " << port;

        // Form reply string
        std::stringstream stream;
        stream << "(";
        for (size_t i = 0; i < 4; i++) {
            stream << static_cast<int>(ip_bytes[i]) << ",";
        }
        stream << ((port >> 8) & 0xff) << "," << (port & 0xff) << ")";

        sendFtpMessage(FtpReplyCode::ENTERING_PASSIVE_MODE, "Entering passive mode " + stream.str());
    }

    void FtpSession::handleFtpCommandEPRT(const std::string & /*param*/) {
        if (!_logged_in_user) {
            sendFtpMessage(FtpReplyCode::NOT_LOGGED_IN, "Not logged in");
            return;
        }

        if (data_acceptor_.is_open()) {
            boost::beast::error_code ec;
            data_acceptor_.close(ec);
            if (ec) {
                log_error << "Error closing data acceptor: " << ec.message();
            }
        }

        const boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v6(), 0);
        {
            boost::beast::error_code ec;
            data_acceptor_.open(endpoint.protocol(), ec);
            if (ec) {
                log_error << "Error opening data acceptor: " << ec.message();
                sendFtpMessage(FtpReplyCode::SERVICE_NOT_AVAILABLE, "Failed to enter passive mode.");
                return;
            }
        }
        {
            boost::beast::error_code ec;
            data_acceptor_.bind(endpoint, ec);
            if (ec) {
                log_error << "Error binding data acceptor: " << ec.message();
                sendFtpMessage(FtpReplyCode::SERVICE_NOT_AVAILABLE, "Failed to enter passive mode.");
                return;
            }
        }
        {
            boost::beast::error_code ec;
            data_acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
            if (ec) {
                log_error << "Error listening on data acceptor: " << ec.message();
                sendFtpMessage(FtpReplyCode::SERVICE_NOT_AVAILABLE, "Failed to enter passive mode.");
                return;
            }
        }

        // Split address and port into bytes and get the port the OS chose for us
        const auto ip_bytes = command_socket_.local_endpoint().address().to_v4().to_bytes();
        const auto port = data_acceptor_.local_endpoint().port();

        // Form reply string
        std::stringstream stream;
        stream << "(";
        for (size_t i = 0; i < 8; i++) {
            stream << static_cast<int>(ip_bytes[i]) << ",";
        }
        stream << ((port >> 8) & 0xff) << "," << (port & 0xff) << ")";

        sendFtpMessage(FtpReplyCode::ENTERING_PASSIVE_MODE, "Entering passive mode " + stream.str());
    }

    void FtpSession::handleFtpCommandLPRT(const std::string & /*param*/) {
        if (!_logged_in_user) {
            sendFtpMessage(FtpReplyCode::NOT_LOGGED_IN, "Not logged in");
            return;
        }

        if (data_acceptor_.is_open()) {
            boost::beast::error_code ec;
            data_acceptor_.close(ec);
            if (ec) {
                log_error << "Error closing data acceptor: " << ec.message();
            }
        }

        const boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v6(), 0);
        {
            boost::beast::error_code ec;
            data_acceptor_.open(endpoint.protocol(), ec);
            if (ec) {
                log_error << "Error opening data acceptor: " << ec.message();
                sendFtpMessage(FtpReplyCode::SERVICE_NOT_AVAILABLE, "Failed to enter passive mode.");
                return;
            }
        }
        {
            boost::beast::error_code ec;
            data_acceptor_.bind(endpoint, ec);
            if (ec) {
                log_error << "Error binding data acceptor: " << ec.message();
                sendFtpMessage(FtpReplyCode::SERVICE_NOT_AVAILABLE, "Failed to enter passive mode.");
                return;
            }
        }
        {
            boost::beast::error_code ec;
            data_acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
            if (ec) {
                log_error << "Error listening on data acceptor: " << ec.message();
                sendFtpMessage(FtpReplyCode::SERVICE_NOT_AVAILABLE, "Failed to enter passive mode.");
                return;
            }
        }

        // Split address and port into bytes and get the port the OS chose for us
        const auto ip_bytes = command_socket_.local_endpoint().address().to_v4().to_bytes();
        const auto port = data_acceptor_.local_endpoint().port();

        // Form reply string
        std::stringstream stream;
        stream << "(";
        for (size_t i = 0; i < 8; i++) {
            stream << static_cast<int>(ip_bytes[i]) << ",";
        }
        stream << ((port >> 8) & 0xff) << "," << (port & 0xff) << ")";

        sendFtpMessage(FtpReplyCode::ENTERING_LONG_PASSIVE_MODE, "Entering passive mode " + stream.str());
    }

    void FtpSession::handleFtpCommandEPSV(const std::string & /*param*/) {
        if (!_logged_in_user) {
            sendFtpMessage(FtpReplyCode::NOT_LOGGED_IN, "Not logged in");
            return;
        }

        if (data_acceptor_.is_open()) {
            boost::beast::error_code ec;
            data_acceptor_.close(ec);
            if (ec) {
                log_error << "Error closing data acceptor: " << ec.message();
            }
        }

        boost::asio::ip::tcp::endpoint endpoint;
        if (Core::Configuration::instance().GetValue<bool>("awsmock.dockerized")) {
            const int minPort = Core::Configuration::instance().GetValue<int>("awsmock.modules.transfer.ftp.pasv-min");
            const int maxPort = Core::Configuration::instance().GetValue<int>("awsmock.modules.transfer.ftp.pasv-max");
            const int port = Core::RandomUtils::NextInt(minPort, maxPort);
            endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v6(), port);
        } else {
            endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v6(), 0);
        }
        log_trace << "Passive mode endpoint: " << endpoint.address().to_string() << ":" << endpoint.port();
        {
            boost::beast::error_code ec;
            data_acceptor_.open(endpoint.protocol(), ec);
            if (ec) {
                log_error << "Error opening data acceptor: " << ec.message();
                sendFtpMessage(FtpReplyCode::SERVICE_NOT_AVAILABLE, "Failed to enter passive mode.");
                return;
            }
        }
        {
            boost::beast::error_code ec;
            data_acceptor_.bind(endpoint, ec);
            if (ec) {
                log_error << "Error binding data acceptor: " << ec.message();
                sendFtpMessage(FtpReplyCode::SERVICE_NOT_AVAILABLE, "Failed to enter passive mode.");
                return;
            }
        }
        {
            boost::beast::error_code ec;
            data_acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
            if (ec) {
                log_error << "Error listening on data acceptor: " << ec.message();
                sendFtpMessage(FtpReplyCode::SERVICE_NOT_AVAILABLE, "Failed to enter passive mode.");
                return;
            }
        }
        // Retrieve the address object
        boost::asio::ip::address addr = command_socket_.local_endpoint().address();

        // Check if the address is IPv6
        std::array<unsigned char, 16> ip_bytes{};
        if (addr.is_v6()) {

            ip_bytes = command_socket_.local_endpoint().address().to_v6().to_bytes();

        } else if (addr.is_v4()) {

            // Handle the IPv4 case (if necessary, you might log an error or convert it)
            const boost::asio::ip::address_v4::bytes_type ip_bytes_v4 = addr.to_v4().to_bytes();

        } else {
            log_error << "Error: Local endpoint address is neither IPv4 nor IPv6.";
            return;
        }

        // Split address and port into bytes and get the port the OS chose for us
//        const auto ip_bytes = command_socket_.local_endpoint().address().to_v6().to_bytes();
        const auto port = data_acceptor_.local_endpoint().port();

        // Form reply string
        std::stringstream stream;
        stream << "(";
        for (size_t i = 0; i < 8; i++) {
            stream << static_cast<int>(ip_bytes[i]) << ",";
        }
        stream << ((port >> 8) & 0xff) << "," << (port & 0xff) << ")";

        sendFtpMessage(FtpReplyCode::ENTERING_PASSIVE_MODE, "Entering passive mode " + stream.str());
    }

    void FtpSession::handleFtpCommandLPSV(const std::string & /*param*/) {
        if (!_logged_in_user) {
            sendFtpMessage(FtpReplyCode::NOT_LOGGED_IN, "Not logged in");
            return;
        }

        if (data_acceptor_.is_open()) {
            boost::beast::error_code ec;
            data_acceptor_.close(ec);
            if (ec) {
                log_error << "Error closing data acceptor: " << ec.message();
            }
        }

        boost::asio::ip::tcp::endpoint endpoint;
        if (Core::Configuration::instance().GetValue<bool>("awsmock.dockerized")) {
            const int minPort = Core::Configuration::instance().GetValue<int>("awsmock.modules.transfer.ftp.pasv-min");
            const int maxPort = Core::Configuration::instance().GetValue<int>("awsmock.modules.transfer.ftp.pasv-max");
            const int port = Core::RandomUtils::NextInt(minPort, maxPort);
            endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v6(), port);
        } else {
            endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v6(), 0);
        }
        log_trace << "Passive mode endpoint: " << endpoint.address().to_string() << ":" << endpoint.port();
        {
            boost::beast::error_code ec;
            data_acceptor_.open(endpoint.protocol(), ec);
            if (ec) {
                log_error << "Error opening data acceptor: " << ec.message();
                sendFtpMessage(FtpReplyCode::SERVICE_NOT_AVAILABLE, "Failed to enter passive mode.");
                return;
            }
        }
        {
            boost::beast::error_code ec;
            data_acceptor_.bind(endpoint, ec);
            if (ec) {
                log_error << "Error binding data acceptor: " << ec.message();
                sendFtpMessage(FtpReplyCode::SERVICE_NOT_AVAILABLE, "Failed to enter passive mode.");
                return;
            }
        }
        {
            boost::beast::error_code ec;
            data_acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
            if (ec) {
                log_error << "Error listening on data acceptor: " << ec.message();
                sendFtpMessage(FtpReplyCode::SERVICE_NOT_AVAILABLE, "Failed to enter passive mode.");
                return;
            }
        }

        // Split address and port into bytes and get the port the OS chose for us
        const auto ip_bytes = command_socket_.local_endpoint().address().to_v6().to_bytes();
        const auto port = data_acceptor_.local_endpoint().port();

        // Form reply string
        std::stringstream stream;
        stream << "(";
        for (size_t i = 0; i < 8; i++) {
            stream << static_cast<int>(ip_bytes[i]) << ",";
        }
        stream << ((port >> 8) & 0xff) << "," << (port & 0xff) << ")";

        sendFtpMessage(FtpReplyCode::ENTERING_LONG_PASSIVE_MODE, "Entering passive mode " + stream.str());
    }

    void FtpSession::handleFtpCommandTYPE(const std::string &param) {
        if (!_logged_in_user) {
            sendFtpMessage(FtpReplyCode::NOT_LOGGED_IN, "Not logged in");
            return;
        }

        if (param == "A") {
            data_type_binary_ = false;
            // TODO: The ASCII mode currently does not work as RFC 959 demands it. It
            // should perform line ending conversion, which it doesn't. But as we are
            // living in the 21st centry, nobody should use ASCII mode anyways.
            sendFtpMessage(FtpReplyCode::COMMAND_OK, "Switching to ASCII mode");
            return;
        }
        if (param == "I") {
            data_type_binary_ = true;
            sendFtpMessage(FtpReplyCode::COMMAND_OK, "Switching to binary mode");
            return;
        }
        sendFtpMessage(FtpReplyCode::COMMAND_NOT_IMPLEMENTED_FOR_PARAMETER, "Unknown or unsupported type");
    }

    void FtpSession::handleFtpCommandSTRU(const std::string & /*param*/) {
        sendFtpMessage(FtpReplyCode::SYNTAX_ERROR_UNRECOGNIZED_COMMAND, "Unsupported command");
    }

    void FtpSession::handleFtpCommandMODE(const std::string & /*param*/) {
        sendFtpMessage(FtpReplyCode::SYNTAX_ERROR_UNRECOGNIZED_COMMAND, "Unsupported command");
    }

    // Ftp module commands
    void FtpSession::handleFtpCommandRETR(const std::string &param) {
        if (!_logged_in_user) {
            sendFtpMessage(FtpReplyCode::NOT_LOGGED_IN, "Not logged in");
            return;
        }
        if (static_cast<int>(_logged_in_user->permissions_ & Permission::FileRead) == 0) {
            sendFtpMessage(FtpReplyCode::ACTION_NOT_TAKEN, "Permission denied");
            return;
        }
        if (!data_acceptor_.is_open()) {
            sendFtpMessage(FtpReplyCode::ERROR_OPENING_DATA_CONNECTION, "Error opening data connection");
            return;
        }

        const std::string local_path = toLocalPath(param);

        const std::ios::openmode open_mode = (data_type_binary_ ? (std::ios::in | std::ios::binary) : (std::ios::in));
        const auto file = std::make_shared<IoFile>(local_path, _logged_in_user->password_, open_mode);

        if (!file->file_stream_.good()) {
            sendFtpMessage(FtpReplyCode::ACTION_ABORTED_LOCAL_ERROR, "Error opening file for transfer");
            return;
        }

        sendFtpMessage(FtpReplyCode::FILE_STATUS_OK_OPENING_DATA_CONNECTION, "Sending file");
        sendFile(file);
        _metricService.IncrementCounter(TRANSFER_SERVER_DOWNLOAD_COUNT);
    }

    void FtpSession::handleFtpCommandSIZE(const std::string &param) {
        if (!_logged_in_user) {
            sendFtpMessage(FtpReplyCode::NOT_LOGGED_IN, "Not logged in");
            return;
        }
        if (static_cast<int>(_logged_in_user->permissions_ & Permission::FileRead) == 0) {
            sendFtpMessage(FtpReplyCode::ACTION_NOT_TAKEN, "Permission denied");
            return;
        }

        const std::string local_path = toLocalPath(param);

        const std::ios::openmode open_mode =
                std::ios::ate | (data_type_binary_ ? (std::ios::in | std::ios::binary) : (std::ios::in));
#if defined(WIN32) && !defined(__GNUG__)
        // TODO fix windows porting issues
        std::ifstream file(local_path, open_mode);
        //        std::wstring wLocalPath = Core::Utf8ToWide(local_path.c_str());
        //        std::ifstream file(wLocalPath, open_mode);
#else
        std::ifstream file(local_path, open_mode);
#endif

        if (!file.good()) {
            sendFtpMessage(FtpReplyCode::ACTION_ABORTED_LOCAL_ERROR, "Error opening file for size retrieval");
            return;
        }

        // RFC 3659 actually states that the returned size should depend on the STRU, MODE, and TYPE and that
        // the returned size should be exact. We don't comply with this here. The size returned is the
        // size for TYPE=I.
        auto file_size = file.tellg();
        if (std::fstream::pos_type(-1) == file_size) {
            sendFtpMessage(FtpReplyCode::ACTION_ABORTED_LOCAL_ERROR, "Error getting file size");
            return;
        }

        // Form reply string
        std::stringstream rep;
        rep << file_size;

        sendFtpMessage(FtpReplyCode::FILE_STATUS, rep.str());
    }

    void FtpSession::handleFtpCommandSTOR(const std::string &param) {
        if (!_logged_in_user) {
            sendFtpMessage(FtpReplyCode::NOT_LOGGED_IN, "Not logged in");
            return;
        }

        // TODO: the ACTION_NOT_TAKEN reply is not RCF 959 conform. Apparently in
        // 1985 nobody anticipated that you might not want anybody uploading files
        // to your manager. We use the return code anyways, as the popular FileZilla
        // Handler also returns that code as "Permission denied"
        if (static_cast<int>(_logged_in_user->permissions_ & Permission::FileWrite) == 0) {
            sendFtpMessage(FtpReplyCode::ACTION_NOT_TAKEN, "Permission denied");
            return;
        }
        if (!data_acceptor_.is_open()) {
            sendFtpMessage(FtpReplyCode::ERROR_OPENING_DATA_CONNECTION, "Error opening data connection");
            return;
        }

        const std::string local_path = toLocalPath(param);

        if (const auto file_status = FileStatus(local_path); file_status.isOk()) {
            if (file_status.type() == FileType::RegularFile && static_cast<int>(_logged_in_user->permissions_ & Permission::FileDelete) == 0) {
                sendFtpMessage(FtpReplyCode::ACTION_NOT_TAKEN_FILENAME_NOT_ALLOWED, "File already exists. Permission denied to overwrite file.");
            } else if (file_status.type() == FileType::Dir) {
                sendFtpMessage(FtpReplyCode::ACTION_NOT_TAKEN_FILENAME_NOT_ALLOWED, "Cannot create file. A directory with that name already exists.");
            }
        }

        const std::ios::openmode open_mode = (data_type_binary_ ? (std::ios::out | std::ios::binary) : (std::ios::out));
        const auto file = std::make_shared<IoFile>(local_path, _logged_in_user->_username, open_mode);

        if (!file->file_stream_.good()) {
            sendFtpMessage(FtpReplyCode::ACTION_ABORTED_LOCAL_ERROR, "Error opening file for transfer");
            return;
        }

        sendFtpMessage(FtpReplyCode::FILE_STATUS_OK_OPENING_DATA_CONNECTION, "Receiving file");
        receiveFile(file);
        log_debug << "STOR ended";
    }

    void FtpSession::handleFtpCommandSTOU(const std::string & /*param*/) {
        sendFtpMessage(FtpReplyCode::SYNTAX_ERROR_UNRECOGNIZED_COMMAND, "Command not implemented");
    }

    void FtpSession::handleFtpCommandAPPE(const std::string &param) {
        if (!_logged_in_user) {
            sendFtpMessage(FtpReplyCode::NOT_LOGGED_IN, "Not logged in");
            return;
        }
        if (static_cast<int>(_logged_in_user->permissions_ & Permission::FileAppend) == 0) {
            sendFtpMessage(FtpReplyCode::ACTION_NOT_TAKEN, "Permission denied");
            return;
        }
        if (!data_acceptor_.is_open()) {
            sendFtpMessage(FtpReplyCode::ERROR_OPENING_DATA_CONNECTION, "Error opening data connection");
            return;
        }

        const std::string local_path = toLocalPath(param);

        if (const auto existing_file_filestatus = FileStatus(local_path); !existing_file_filestatus.isOk() || (existing_file_filestatus.type() != FileType::RegularFile)) {
            sendFtpMessage(FtpReplyCode::ACTION_NOT_TAKEN, "File does not exist.");
            return;
        }

        const std::ios::openmode open_mode = data_type_binary_ ? std::ios::out | std::ios::app | std::ios::binary : std::ios::out | std::ios::app;
        const auto file = std::make_shared<IoFile>(local_path, _logged_in_user->_username, open_mode);

        if (!file->file_stream_.good()) {
            sendFtpMessage(FtpReplyCode::ACTION_ABORTED_LOCAL_ERROR, "Error opening file for transfer");
            return;
        }

        sendFtpMessage(FtpReplyCode::FILE_STATUS_OK_OPENING_DATA_CONNECTION, "Receiving file");
        receiveFile(file);
    }

    void FtpSession::handleFtpCommandALLO(const std::string & /*param*/) {
        sendFtpMessage(FtpReplyCode::SYNTAX_ERROR_UNRECOGNIZED_COMMAND, "Command not implemented");
    }

    void FtpSession::handleFtpCommandREST(const std::string & /*param*/) {
        sendFtpMessage(FtpReplyCode::COMMAND_NOT_IMPLEMENTED, "Command not implemented");
    }

    void FtpSession::handleFtpCommandRNFR(const std::string &param) {
        _renameFromPath.clear();

        if (const auto is_renamable_error = checkIfPathIsRenamable(param); is_renamable_error.replyCode() == FtpReplyCode::COMMAND_OK) {
            _renameFromPath = param;
            sendFtpMessage(FtpReplyCode::FILE_ACTION_NEEDS_FURTHER_INFO, "Enter target name");
            return;
        } else {
            sendFtpMessage(is_renamable_error);
            return;
        }
    }

    void FtpSession::handleFtpCommandRNTO(const std::string &param) {
        if (!_logged_in_user) {
            sendFtpMessage(FtpReplyCode::NOT_LOGGED_IN, "Not logged in");
            return;
        }

        if (_lastCommand != "RNFR" || _renameFromPath.empty()) {
            sendFtpMessage(FtpReplyCode::COMMANDS_BAD_SEQUENCE, "Please specify target file first");
            return;
        }

        if (param.empty()) {
            sendFtpMessage(FtpReplyCode::SYNTAX_ERROR_PARAMETERS, "No target name given");
            return;
        }

        // TODO: returning neiher FILE_ACTION_NOT_TAKEN nor ACTION_NOT_TAKEN are
        // RFC 959 conform. Apparently back in 1985 it was assumed that the RNTO
        // command will always succeed, as long as you enter a valid target file
        // name. Thus we use the two return codes anyways, the popular FileZilla
        // FTP Handler uses those as well.

        if (const auto is_renamable_error = checkIfPathIsRenamable(_renameFromPath); is_renamable_error.replyCode() == FtpReplyCode::COMMAND_OK) {
            const std::string local_from_path = toLocalPath(_renameFromPath);
            const std::string local_to_path = toLocalPath(param);

            // Check if the source file exists already. We simple disallow overwriting a
            // file be renaming (the behavior of the native rename command on Windows
            // and Linux differs; Windows will not overwrite files, Linux will).
            if (FileStatus(local_to_path).isOk()) {
                sendFtpMessage(FtpReplyCode::FILE_ACTION_NOT_TAKEN, "Target path exists already.");
                return;
            }
            if (rename(local_from_path.c_str(), local_to_path.c_str()) == 0) {
                sendFtpMessage(FtpReplyCode::FILE_ACTION_COMPLETED, "OK");
                return;
            }
            sendFtpMessage(FtpReplyCode::FILE_ACTION_NOT_TAKEN, "Error renaming file");
        } else {
            sendFtpMessage(is_renamable_error);
        }
    }

    void FtpSession::handleFtpCommandABOR(const std::string & /*param*/) {
        sendFtpMessage(FtpReplyCode::COMMAND_NOT_IMPLEMENTED, "Command not implemented");
    }

    void FtpSession::handleFtpCommandDELE(const std::string &param) {
        if (!_logged_in_user) {
            sendFtpMessage(FtpReplyCode::NOT_LOGGED_IN, "Not logged in");
            return;
        }
        const std::string local_path = toLocalPath(param);

        auto file_status = FileStatus(local_path);

        if (!file_status.isOk()) {
            sendFtpMessage(FtpReplyCode::ACTION_NOT_TAKEN, "Resource does not exist");
            return;
        }
        if (file_status.type() != FileType::RegularFile) {
            sendFtpMessage(FtpReplyCode::ACTION_NOT_TAKEN, "Resource is not a file");
            return;
        }
        if (static_cast<int>(_logged_in_user->permissions_ & Permission::FileDelete) == 0) {
            sendFtpMessage(FtpReplyCode::ACTION_NOT_TAKEN, "Permission denied");
            return;
        }
        if (unlink(local_path.c_str()) == 0) {
            SendDeleteObjectRequest(_logged_in_user->_username, local_path);
            sendFtpMessage(FtpReplyCode::FILE_ACTION_COMPLETED, "Successfully deleted file");
            return;
        }
        sendFtpMessage(FtpReplyCode::FILE_ACTION_NOT_TAKEN, "Unable to delete file");
    }

    void FtpSession::handleFtpCommandRMD(const std::string &param) {
        if (!_logged_in_user) {
            sendFtpMessage(FtpReplyCode::NOT_LOGGED_IN, "Not logged in");
            return;
        }
        if (static_cast<int>(_logged_in_user->permissions_ & Permission::DirDelete) == 0) {
            sendFtpMessage(FtpReplyCode::ACTION_NOT_TAKEN, "Permission denied");
            return;
        }

        const std::string local_path = toLocalPath(param);
#ifdef _WIN32
        _wrmdir(reinterpret_cast<const wchar_t *>(local_path.c_str()));
#else
        if (rmdir(local_path.c_str()) == 0) {
            sendFtpMessage(FtpReplyCode::FILE_ACTION_COMPLETED, "Successfully removed directory");
            return;
        }
        // If would be a good idea to return a 4xx error code here (-> temp error)
        // (e.g. FILE_ACTION_NOT_TAKEN), but RFC 959 assumes that all directory
        // errors are permanent.
        sendFtpMessage(FtpReplyCode::ACTION_NOT_TAKEN, "Unable to remove directory");
#endif
    }

    void FtpSession::handleFtpCommandMKD(const std::string &param) {
        if (!_logged_in_user) {
            sendFtpMessage(FtpReplyCode::NOT_LOGGED_IN, "Not logged in");
            return;
        }
        if (static_cast<int>(_logged_in_user->permissions_ & Permission::DirCreate) == 0) {
            sendFtpMessage(FtpReplyCode::ACTION_NOT_TAKEN, "Permission denied");
            return;
        }

        auto local_path = toLocalPath(param);
#ifdef _WIN32
        if (_wmkdir(reinterpret_cast<const wchar_t *>(reinterpret_cast<const char *>(local_path.c_str() == 0)))) {
            sendFtpMessage(FtpReplyCode::PATHNAME_CREATED, createQuotedFtpPath(toAbsoluteFtpPath(param)) + " Successfully created");
            return;
        }
#else
        if (constexpr mode_t mode = 0755; mkdir(local_path.c_str(), mode) == 0) {
            sendFtpMessage(FtpReplyCode::PATHNAME_CREATED,
                           createQuotedFtpPath(toAbsoluteFtpPath(param)) + " Successfully created");
            return;
        }
#endif
        // If would be a good idea to return a 4xx error code here (-> temp error)
        // (e.g. FILE_ACTION_NOT_TAKEN), but RFC 959 assumes that all directory
        // errors are permanent.
        sendFtpMessage(FtpReplyCode::ACTION_NOT_TAKEN, "Unable to create directory");
    }

    void FtpSession::handleFtpCommandPWD(const std::string & /*param*/) {
        // RFC 959 does not allow returning NOT_LOGGED_IN here, so we abuse ACTION_NOT_TAKEN for that.
        if (!_logged_in_user) {
            sendFtpMessage(FtpReplyCode::ACTION_NOT_TAKEN, "Not logged in");
            return;
        }

        sendFtpMessage(FtpReplyCode::PATHNAME_CREATED, createQuotedFtpPath(_ftpWorkingDirectory));
    }

    void FtpSession::handleFtpCommandLIST(const std::string &param) {
        if (!_logged_in_user) {
            log_error << "Not logged in";
            sendFtpMessage(FtpReplyCode::NOT_LOGGED_IN, "Not logged in");
            return;
        }

        // RFC 959 does not allow ACTION_NOT_TAKEN (-> permanent error), so we return a temporary error (FILE_ACTION_NOT_TAKEN).
        if (static_cast<int>(_logged_in_user->permissions_ & Permission::DirList) == 0) {
            log_error << "Permission denied";
            sendFtpMessage(FtpReplyCode::FILE_ACTION_NOT_TAKEN, "Permission denied");
            return;
        }

        // Deal with some unusual commands like "LIST -a", "LIST -a dirname".
        // Some FTP clients send those commands, as if they would call ls on unix.
        //
        // We try to support those parameters (or rather ignore them), even though
        // this technically breaks listing directories that actually use "-a" etc.
        // as directory name. As most clients however first CWD into a directory and
        // call LIST without parameter afterward and starting a directory name with
        // "-a " / "-l " / "-al " / "-la " is not that common, the compatibility
        // benefit should outperform te potential problems by a lot.
        //
        // The RFC-violating LIST command now shall be:
        //
        //   LIST [<SP> <arg>] [<SP> <pathname>] <CRLF>
        //
        //   with:
        //
        //     <arg> ::= -a
        //             | -l
        //             | -al
        //             | -la
        //

        std::string path2dst;
        if ((param == "-a") || (param == "-l") || (param == "-al") || (param == "-la")) {
            path2dst = "";
        } else if ((param.substr(0, 3) == "-a " || param.substr(0, 3) == "-l ") && (param.size() > 3)) {
            path2dst = param.substr(3);
        } else if ((param.substr(0, 4) == "-al " || param.substr(0, 4) == "-la ") && (param.size() > 4)) {
            path2dst = param.substr(4);
        } else {
            path2dst = param;
        }

        try {
            const std::string local_path = toLocalPath(path2dst);
            log_debug << "Local path: " << local_path;

            if (const auto dir_status = FileStatus(local_path); dir_status.isOk()) {
                if (dir_status.type() == FileType::Dir) {
                    if (dir_status.canOpenDir()) {
                        sendFtpMessage(FtpReplyCode::FILE_STATUS_OK_OPENING_DATA_CONNECTION, "Sending directory listing");
                        sendDirectoryListing(dirContent(local_path));
                        log_debug << "Directory listing sent";
                    } else {
                        log_error << "Permission denied";
                        sendFtpMessage(FtpReplyCode::FILE_ACTION_NOT_TAKEN, "Permission denied");
                    }
                } else {
                    // TODO: RFC959: If the pathname specifies a file then the manager should send current information on the file.
                    log_error << "Not a directory, directory: " << local_path;
                    sendFtpMessage(FtpReplyCode::FILE_ACTION_NOT_TAKEN, "Path is not a directory");
                }
            } else {
                log_error << "Path does not exist, directory: " << local_path;
                sendFtpMessage(FtpReplyCode::FILE_ACTION_NOT_TAKEN, "Path does not exist");
            }
        } catch (std::exception &exc) {
            log_error << "Unknown exception: " << exc.what();
        }
    }

    void FtpSession::handleFtpCommandNLST(const std::string &param) {
        if (!_logged_in_user) {
            sendFtpMessage(FtpReplyCode::NOT_LOGGED_IN, "Not logged in");
            return;
        }

        // RFC 959 does not allow ACTION_NOT_TAKEN (-> permanent error), so we return a temporary error (FILE_ACTION_NOT_TAKEN).
        if (static_cast<int>(_logged_in_user->permissions_ & Permission::DirList) == 0) {
            sendFtpMessage(FtpReplyCode::FILE_ACTION_NOT_TAKEN, "Permission denied");
            return;
        }

        const std::string local_path = toLocalPath(param);

        if (const auto dir_status = FileStatus(local_path); dir_status.isOk()) {
            if (dir_status.type() == FileType::Dir) {
                if (dir_status.canOpenDir()) {
                    sendFtpMessage(FtpReplyCode::FILE_STATUS_OK_OPENING_DATA_CONNECTION, "Sending name list");
                    sendNameList(dirContent(local_path));
                    return;
                }
                sendFtpMessage(FtpReplyCode::FILE_ACTION_NOT_TAKEN, "Permission denied");
                return;
            }
            // TODO: RFC959: If the pathname specifies a file then the manager should send current information on the file.
            sendFtpMessage(FtpReplyCode::FILE_ACTION_NOT_TAKEN, "Path is not a directory");
            return;
        }
        sendFtpMessage(FtpReplyCode::FILE_ACTION_NOT_TAKEN, "Path does not exist");
    }

    void FtpSession::handleFtpCommandSITE(const std::string & /*param*/) {
        sendFtpMessage(FtpReplyCode::SYNTAX_ERROR_UNRECOGNIZED_COMMAND, "Command not implemented");
    }

    void FtpSession::handleFtpCommandSYST(const std::string & /*param*/) {
        // Always returning "UNIX" when being asked for the operating system.
        // Some clients (Mozilla Firefox for example) may disconnect, when we
        // return an unknown operating system here. As depending on the Handler's
        // operating system is a horrible feature anyways, we simply fake it.
        //
        // Unix should be the best compatible value here, as we emulate Unix-like
        // outputs for other commands (-> LIST) on all operating systems.
        sendFtpMessage(FtpReplyCode::NAME_SYSTEM_TYPE, "UNIX");
    }

    void FtpSession::handleFtpCommandSTAT(const std::string & /*param*/) {
        sendFtpMessage(FtpReplyCode::COMMAND_NOT_IMPLEMENTED, "Command not implemented");
    }

    void FtpSession::handleFtpCommandHELP(const std::string & /*param*/) {
        sendFtpMessage(FtpReplyCode::COMMAND_NOT_IMPLEMENTED, "Command not implemented");
    }

    void FtpSession::handleFtpCommandNOOP(const std::string & /*param*/) {
        sendFtpMessage(FtpReplyCode::COMMAND_OK, "OK");
    }

    // Modern FTP Commands
    void FtpSession::handleFtpCommandFEAT(const std::string & /*param*/) {
        std::stringstream ss;
        ss << "211- Feature List:\r\n";
        ss << " UTF8\r\n";
        ss << " SIZE\r\n";
        ss << " LANG EN\r\n";
        ss << "211 END\r\n";
        sendRawFtpMessage(ss.str());
    }

    void FtpSession::handleFtpCommandOPTS(const std::string &param) {
        std::string param_upper = param;
        std::ranges::transform(param_upper, param_upper.begin(), [](const char c) {
            return static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        });

        if (param_upper == "UTF8 ON") {
            sendFtpMessage(FtpReplyCode::COMMAND_OK, "OK");
            return;
        }

        sendFtpMessage(FtpReplyCode::COMMAND_NOT_IMPLEMENTED_FOR_PARAMETER, "Unrecognized parameter");
    }

    /**
     * Sends a directory listing to the connected FTP client.
     *
     * This function asynchronously accepts a data connection request,
     * generates a Unix-like directory listing (formatted as per FTP standards),
     * and sends the listing to the client. If a connection error occurs,
     * the transfer is aborted, and an appropriate FTP message is sent.
     *
     * @param directory_content A map containing the directory contents to be listed.
     * Each key is a string representing the filename, and the value is a FileStatus
     * object that provides metadata such as type, size, permissions, owner, and group.
     *
     * Error Handling:
     * - If an error occurs during the acceptance of the data connection, an appropriate
     *   FTP message is sent to indicate a transfer abortion.
     *
     * Implementation Notes:
     * - The directory content is converted into a Unix-style file list string using
     *   `std::stringstream`. Each entry includes file attributes and details in a
     *   formatted output.
     * - The directory listing is copied into a raw character vector (`std::vector<char>`)
     *   for efficient transfer over the network.
     * - Two calls to addDataToBufferAndSend are made:
     *   1. To send the actual directory data.
     *   2. To send an empty buffer, denoting the end of the transfer.
     *
     * Design Considerations:
     * - The function uses `shared_from_this` to ensure the session object remains
     *   valid during asynchronous operations.
     * - A weak pointer (`data_socket_weakptr_`) is maintained for the data socket
     *   to prevent dangling references.
     *
     * Asynchronous Behavior:
     * - The method relies on Boost.Asio asynchronous operations for handling the
     *   client connection and data transfer.
     *
     * TODOs:
     * - Close the acceptor after the socket connects.
     * - Extend the implementation to fetch file data from S3 buckets/keys if required.
     */

    void FtpSession::sendDirectoryListing(const std::map<std::string, FileStatus> &directory_content) {
        auto data_socket = std::make_shared<boost::asio::ip::tcp::socket>(_io_service);
        data_socket_weakptr_ = data_socket;

        data_acceptor_.async_accept(*data_socket, [data_socket, directory_content, me = shared_from_this()](auto ec) {
            if (ec) {
                log_error << "Error sending directory listing, error: " << ec.message();
                me->sendFtpMessage(FtpReplyCode::TRANSFER_ABORTED, "Data transfer aborted");
                return;
            }
            // TODO: close acceptor after connect?
            // TODO: get the data from S3 buckets/keys
            // Create a Unix-like file list
            std::stringstream stream;
            // NOLINT(misc-const-correctness) Reason: False detection, this cannot be made const
            for (const auto &[fst, snd]: directory_content) {
                const std::string &filename(fst);
                const FileStatus &file_status(snd);

                stream << (file_status.type() == FileType::Dir ? 'd' : '-') << file_status.permissionString() << "   1 ";
                stream << std::setw(10) << file_status.ownerString() << " " << std::setw(10)
                        << file_status.groupString() << " ";
                stream << std::setw(10) << file_status.fileSize() << " ";
                stream << file_status.timeString() << " ";
                stream << filename;
                stream << "\r\n";
            }

            // Copy the file list into a raw char vector
            const std::string dir_listing_string = stream.str();
            const auto dir_listing_rawdata = std::make_shared<std::vector<char> >();
            dir_listing_rawdata->reserve(dir_listing_string.size());
            std::ranges::copy(dir_listing_string, std::back_inserter(*dir_listing_rawdata));

            // Send the string out
            me->addDataToBufferAndSend(dir_listing_rawdata, data_socket);
            me->addDataToBufferAndSend(std::shared_ptr<std::vector<char> >(), data_socket);
        });
    }

    void FtpSession::sendNameList(const std::map<std::string, FileStatus> &directory_content) {
        auto data_socket = std::make_shared<boost::asio::ip::tcp::socket>(_io_service);
        data_socket_weakptr_ = data_socket;

        data_acceptor_.async_accept(*data_socket, [data_socket, directory_content, me = shared_from_this()](auto ec) {
            if (ec) {
                me->sendFtpMessage(FtpReplyCode::TRANSFER_ABORTED, "Data transfer aborted");
                return;
            }

            // Create a file list
            std::stringstream stream;
            // NOLINT(misc-const-correctness) Reason: False detection, this cannot be made const
            for (const auto &key: directory_content | std::views::keys) {
                stream << key;
                stream << "\r\n";
            }

            // Copy the file list into a raw char vector
            const std::string dir_listing_string = stream.str();
            const auto dir_listing_rawdata = std::make_shared<std::vector<char> >();
            dir_listing_rawdata->reserve(dir_listing_string.size());
            std::ranges::copy(dir_listing_string, std::back_inserter(*dir_listing_rawdata));

            // Send the string out
            me->addDataToBufferAndSend(dir_listing_rawdata, data_socket);
            me->addDataToBufferAndSend(std::shared_ptr<std::vector<char> >(), data_socket);
            // Nullpointer indicates the end of transmission
        });
    }

    void FtpSession::sendFile(const std::shared_ptr<IoFile> &file) {
        auto data_socket = std::make_shared<boost::asio::ip::tcp::socket>(_io_service);
        data_socket_weakptr_ = data_socket;

        data_acceptor_.async_accept(*data_socket, [data_socket, file, me = shared_from_this()](auto ec) {
            if (ec) {
                me->sendFtpMessage(FtpReplyCode::TRANSFER_ABORTED, "Data transfer aborted");
                return;
            }

            // Start sending multiple buffers at once
            me->readDataFromFileAndSend(file, data_socket);
            me->readDataFromFileAndSend(file, data_socket);
            me->readDataFromFileAndSend(file, data_socket);
        });
        _metricService.SetGauge(TRANSFER_SERVER_FILESIZE_DOWNLOAD, {}, {}, static_cast<double>(Core::FileUtils::FileSize(file->_fileName)));
    }

    void FtpSession::readDataFromFileAndSend(const std::shared_ptr<IoFile> &file,
                                             const std::shared_ptr<boost::asio::ip::tcp::socket> &data_socket) {
        file_rw_strand_.post([me = shared_from_this(), file, data_socket]() {
                                 if (file->file_stream_.eof()) return;

                                 const auto buffer = std::make_shared<std::vector<char> >(1024 * 1024 * 1);
                                 file->file_stream_.read(buffer->data(), static_cast<std::streamsize>(buffer->size()));
                                 const auto bytes_read = file->file_stream_.gcount();
                                 buffer->resize(static_cast<size_t>(bytes_read));

                                 if (!file->file_stream_.eof()) {
                                     me->addDataToBufferAndSend(buffer,
                                                                data_socket,
                                                                [me, file, data_socket]() {
                                                                    me->readDataFromFileAndSend(file, data_socket);
                                                                });
                                 } else {
                                     me->addDataToBufferAndSend(buffer, data_socket);
                                     me->addDataToBufferAndSend(std::shared_ptr<std::vector<char> >(nullptr), data_socket);
                                 }
                             },
                             nullptr);
    }

    void FtpSession::addDataToBufferAndSend(const std::shared_ptr<std::vector<char> > &data,
                                            const std::shared_ptr<boost::asio::ip::tcp::socket> &data_socket,
                                            const std::function<void(void)> &fetch_more) {
        data_buffer_strand_.post([me = shared_from_this(), data, data_socket, fetch_more]() {
                                     const bool write_in_progress = (!me->data_buffer_.empty());

                                     me->data_buffer_.push_back(data);

                                     if (!write_in_progress) {
                                         me->writeDataToSocket(data_socket, fetch_more);
                                     }
                                 },
                                 nullptr);
    }

    void FtpSession::writeDataToSocket(const std::shared_ptr<boost::asio::ip::tcp::socket> &data_socket,
                                       const std::function<void(void)> &fetch_more) {
        data_buffer_strand_.post(
                [me = shared_from_this(), data_socket, fetch_more]() {
                    if (auto data = me->data_buffer_.front()) {
                        // Send out the buffer
                        async_write(*data_socket,
                                    boost::asio::buffer(*data),
                                    me->data_buffer_strand_.wrap(
                                            [me, data_socket, data, fetch_more](
                                            const boost::beast::error_code ec,
                                            std::size_t /*bytes_to_transfer*/) {
                                                me->data_buffer_.pop_front();

                                                if (ec) {
                                                    log_error << "Data write error: " << ec.message();
                                                    return;
                                                }

                                                fetch_more();

                                                if (!me->data_buffer_.empty()) {
                                                    me->writeDataToSocket(data_socket, fetch_more);
                                                }
                                            }));
                    } else {
                        // we got to the end of transmission
                        me->data_buffer_.pop_front();
                        me->sendFtpMessage(FtpReplyCode::CLOSING_DATA_CONNECTION, "Done");
                    }
                },
                nullptr);
    }

    ////////////////////////////////////////////////////////
    // FTP data-socket receive
    ////////////////////////////////////////////////////////

    void FtpSession::receiveFile(const std::shared_ptr<IoFile> &file) {
        auto data_socket = std::make_shared<boost::asio::ip::tcp::socket>(_io_service);
        data_socket_weakptr_ = data_socket;

        data_acceptor_.async_accept(*data_socket,
                                    [data_socket, file, me = shared_from_this()](auto ec) {
                                        if (ec) {
                                            me->sendFtpMessage(FtpReplyCode::TRANSFER_ABORTED, "Data transfer aborted");
                                            return;
                                        }

                                        me->receiveDataFromSocketAndWriteToFile(file, data_socket);
                                    });
    }

    void FtpSession::receiveDataFromSocketAndWriteToFile(const std::shared_ptr<IoFile> &file, const std::shared_ptr<boost::asio::ip::tcp::socket> &data_socket) {
        const auto buffer = std::make_shared<std::vector<char> >(1024 * 1024 * 1);

        async_read(*data_socket,
                   boost::asio::buffer(*buffer),
                   boost::asio::transfer_at_least(buffer->size()),
                   [me = shared_from_this(), file, data_socket, buffer](const boost::beast::error_code &ec, const std::size_t length) {
                       buffer->resize(length);
                       if (ec) {
                           if (length > 0) {
                               me->writeDataToFile(buffer, file);
                           }
                           me->endDataReceiving(file);
                       } else if (length > 0) {
                           me->writeDataToFile(buffer, file, [me, file, data_socket]() {
                               me->receiveDataFromSocketAndWriteToFile(file, data_socket);
                           });
                       }
                   });
    }

    void FtpSession::writeDataToFile(const std::shared_ptr<std::vector<char> > &data, const std::shared_ptr<IoFile> &file, const std::function<void()> &fetch_more) {
        file_rw_strand_.post([me = shared_from_this(), data, file, fetch_more] {
                                 fetch_more();
                                 file->file_stream_.write(data->data(), static_cast<std::streamsize>(data->size()));
                             },
                             nullptr);
    }

    void FtpSession::endDataReceiving(const std::shared_ptr<IoFile> &file) {
        file_rw_strand_.post([me = shared_from_this(), file] {
                                 file->file_stream_.flush();
                                 file->file_stream_.close();
                                 me->sendFtpMessage(FtpReplyCode::CLOSING_DATA_CONNECTION, "Done");

                                 // Send to AWS S3
                                 me->SendCreateObjectRequest(file->_user, file->_fileName);
                             },
                             nullptr);
    }

    ////////////////////////////////////////////////////////
    // Helpers
    ////////////////////////////////////////////////////////
    std::string FtpSession::toAbsoluteFtpPath(const std::string &rel_or_abs_ftp_path) const {
        std::string absolute_ftp_path;

        if (!rel_or_abs_ftp_path.empty() && (rel_or_abs_ftp_path[0] == '/')) {
            absolute_ftp_path = rel_or_abs_ftp_path;
        } else {
            absolute_ftp_path = cleanPath(_ftpWorkingDirectory + "/" + rel_or_abs_ftp_path, false, '/');
        }

        return absolute_ftp_path;
    }

    std::string FtpSession::toLocalPath(const std::string &ftp_path) const {
        assert(_logged_in_user);

        // First, make the ftp path absolute if it isn't already
        const std::string absolute_ftp_path = toAbsoluteFtpPath(ftp_path);

        // Now map it to the local filesystem
        return cleanPathNative(_logged_in_user->local_root_path_ + "/" + absolute_ftp_path);
    }

    std::string FtpSession::createQuotedFtpPath(const std::string &unquoted_ftp_path) {
        std::string output;
        output.reserve(unquoted_ftp_path.size() * 2 + 2);
        output.push_back('\"');

        for (const char c: unquoted_ftp_path) {
            output.push_back(c);
            if (c == '\"')// Escape quote by double-quote
                output.push_back(c);
        }

        output.push_back('\"');

        return output;
    }

    FtpMessage FtpSession::checkIfPathIsRenamable(const std::string &ftp_path) const {
        if (!_logged_in_user) return {FtpReplyCode::NOT_LOGGED_IN, "Not logged in"};

        if (!ftp_path.empty()) {
            // Is the given path a file or a directory?

            if (const auto file_status = FileStatus(toLocalPath(ftp_path)); file_status.isOk()) {
                // Calculate required permissions to rename the given resource
                auto required_permissions = Permission::None;
                if (file_status.type() == FileType::Dir) {
                    required_permissions = Permission::DirRename;
                } else {
                    required_permissions = Permission::FileRename;
                }

                // Send error if the permissions are insufficient
                if ((_logged_in_user->permissions_ & required_permissions) != required_permissions) {
                    return {FtpReplyCode::ACTION_NOT_TAKEN, "Permission denied"};
                }

                return {FtpReplyCode::COMMAND_OK, ""};
            } else {
                return {FtpReplyCode::ACTION_NOT_TAKEN, "File does not exist"};
            }
        } else {
            return {FtpReplyCode::SYNTAX_ERROR_PARAMETERS, "Empty path"};
        }
    }

    FtpMessage FtpSession::executeCWD(const std::string &param) {
        if (!_logged_in_user) {
            return {FtpReplyCode::NOT_LOGGED_IN, "Not logged in"};
        }
        if (static_cast<int>(_logged_in_user->permissions_ & Permission::DirList) == 0) {
            return {FtpReplyCode::ACTION_NOT_TAKEN, "Permission denied"};
        }

        if (param.empty()) {
            return {FtpReplyCode::SYNTAX_ERROR_PARAMETERS, "No path given"};
        }

        std::string absolute_new_working_dir;

        if (param[0] == '/') {
            // Absolute path given
            absolute_new_working_dir = cleanPath(param, false, '/');
        } else {
            // Make the path absolute
            absolute_new_working_dir = cleanPath(_ftpWorkingDirectory + "/" + param, false, '/');
        }

        const auto local_path = toLocalPath(absolute_new_working_dir);
        log_debug << "Checking file status on file, path: " << local_path;
        const FileStatus file_status(local_path);

        if (!file_status.isOk()) {
            return {FtpReplyCode::ACTION_NOT_TAKEN, "Failed ot change directory: The given resource does not exist or permission denied."};
        }
        if (file_status.type() != FileType::Dir) {
            return {FtpReplyCode::ACTION_NOT_TAKEN, "Failed ot change directory: The given resource is not a directory."};
        }
        if (!file_status.canOpenDir()) {
            return {FtpReplyCode::ACTION_NOT_TAKEN, "Failed ot change directory: Permission denied."};
        }
        _ftpWorkingDirectory = absolute_new_working_dir;
        return {FtpReplyCode::FILE_ACTION_COMPLETED, "Working directory changed to " + _ftpWorkingDirectory};
    }

    void FtpSession::SendCreateObjectRequest(const std::string &user, const std::string &fileName) const {
        const std::string key = GetKey(fileName);
        std::map<std::string, std::string> metadata;
        metadata["user-agent"] = _serverName;
        metadata["user-agent-id"] = _logged_in_user->_username + "@" + _serverName;

        // Get content type
        const std::string contentType = Core::FileUtils::GetContentType(fileName, key);
        const long contentLength = Core::FileUtils::FileSize(fileName);

        Dto::S3::PutObjectRequest request;
        request.region = _region;
        request.bucket = _bucket;
        request.key = key;
        request.owner = user;
        request.contentType = contentType;
        request.contentLength = contentLength;
        request.metadata = metadata;

        std::ifstream ifs(fileName, std::ios::binary);
        _s3Service->PutObject(request, ifs);
        ifs.close();

        _metricService.IncrementCounter(TRANSFER_SERVER_UPLOAD_COUNT);
        _metricService.SetGauge(TRANSFER_SERVER_FILESIZE_UPLAOD, {}, {}, static_cast<double>(Core::FileUtils::FileSize(fileName)));
        log_debug << "File uploaded, fileName: " << fileName;
    }

    void FtpSession::SendDeleteObjectRequest(const std::string &user, const std::string &fileName) const {
        Dto::S3::DeleteObjectRequest request;
        request.region = _region;
        request.user = user;
        request.bucket = _bucket;
        request.key = GetKey(fileName);
        _s3Service->DeleteObject(request);

        log_debug << "Delete object request send, fileName: " << fileName;
    }

    std::string FtpSession::GetKey(const std::string &path) const {
        //std::string key = Core::StringUtils::StripBeginning(path, _transferDir);
        std::string key = path;
#ifdef _WIN32
        Core::StringUtils::Replace(key, "\\", "/");
#endif
        if (!key.empty() && key[0] == '/') {
            return key.substr(1);
        }
        return key;
    }
}// namespace AwsMock::FtpServer
