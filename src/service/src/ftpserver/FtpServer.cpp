

// AwsMock includes
#include <awsmock/ftpserver/FtpServer.h>
#include <awsmock/ftpserver/FtpServerImpl.h>

#include <utility>

namespace AwsMock::FtpServer {

    FtpServer::FtpServer(std::string serverName, const int port, std::string address, boost::asio::io_context &awsIoc) : _serverName(std::move(serverName)), _address(std::move(address)), _port(port), _maxThreads(8), _awsIoc(awsIoc) {

        _ftp_server = std::make_unique<FtpServerImpl>(_serverName, _address, _port, _awsIoc);
        log_debug << "FTP manager configured, name: " << _serverName << " endpoint: " << _address << ":" << port;
    }

    FtpServer::~FtpServer() = default;

    bool FtpServer::addUser(const std::string &username, const std::string &password, const std::string &local_root_path, const Permission permissions) const {
        return _ftp_server->addUser(username, password, local_root_path, permissions);
    }

    bool FtpServer::addUserAnonymous(const std::string &local_root_path, const Permission permissions) const {
        return _ftp_server->addUserAnonymous(local_root_path, permissions);
    }

    bool FtpServer::start(const size_t thread_count) const {
        assert(thread_count > 0);
        return _ftp_server->start(thread_count);
    }

    void FtpServer::stop() const {
        _ftp_server->stop();
    }

    int FtpServer::getOpenConnectionCount() const {
        return _ftp_server->getOpenConnectionCount();
    }

    int FtpServer::getPort() const {
        return _port;
    }

    std::string FtpServer::getAddress() const {
        return _ftp_server->getAddress();
    }
}// namespace AwsMock::FtpServer
