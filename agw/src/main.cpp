// ===================================================================================
// awsmock-agw — standalone HTTP reverse-agw process for the AwsMock API Gateway.
//
// Started by ApiGatewayController via fork/exec. The controller waits for the
// process to create a listening UNIX-domain socket at --socket-path before
// considering it ready (see ApiGatewayController::spawnProcess / waitForSocket).
// ===================================================================================

#include <cstring>
#include <iostream>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#include <afunix.h>
#else
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#endif

// Boost includes
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/program_options.hpp>

// Awsmock includes
#include <awsmock/agw/ProxyListener.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/apigateway/model/ProxyConfig.h>

namespace po = boost::program_options;
namespace net = boost::asio;

namespace {
    logger_t _logger{boost::log::keywords::channel = "ApiGatewayProxy"};

    /**
     * @brief RAII owner of a UNIX-domain socket used only as a readiness signal.
     *
     * ApiGatewayController::waitForSocket() does a non-blocking connect() loop to
     * this path. As soon as the socket is in the LISTEN state to connect() succeeds,
     * and the controller marks the process as RUNNING.  We never accept() — the
     * kernel queues the connection attempt in the backlog, which is sufficient.
     */
    class ReadinessSocket {
      public:

        explicit ReadinessSocket(const std::string &path) : _path(path) {
#ifdef _WIN32
            WSADATA wsaData;
            WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
            unlink(path.c_str());

            _fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
            if (_fd < 0) {
                log_error << "ReadinessSocket: socket() failed: " << strerror(errno);
                return;
            }

            sockaddr_un addr{};
            addr.sun_family = AF_UNIX;
            std::strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path) - 1);

            if (::bind(_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
                log_error << "ReadinessSocket: bind() failed: " << strerror(errno);
#ifdef _WIN32
                ::closesocket(_fd);
#else
                close(_fd);
#endif
                _fd = -1;
                return;
            }
            if (listen(_fd, 8) < 0) {
                log_error << "ReadinessSocket: listen() failed: " << strerror(errno);
#ifdef _WIN32
                ::closesocket(_fd);
#else
                close(_fd);
#endif
                _fd = -1;
            }
        }

        ~ReadinessSocket() {
            if (_fd >= 0) {
#ifdef _WIN32
                ::closesocket(_fd);
                WSACleanup();
#else
                close(_fd);
#endif
                unlink(_path.c_str());
            }
        }

        ReadinessSocket(const ReadinessSocket &) = delete;
        ReadinessSocket &operator=(const ReadinessSocket &) = delete;

        [[nodiscard]] bool ok() const { return _fd >= 0; }

      private:

        int _fd = -1;
        std::string _path;
    };
}// namespace

int main(int argc, char *argv[]) {
    Awsmock::Core::LogStream::Initialize();

    po::options_description desc("awsmock-agw — API Gateway HTTP agw");
    // clang-format off
    desc.add_options()
        ("help,h",      "show this help and exit")
        ("name,n",      po::value<std::string>()->default_value("agw"),        "API name (used in logs)")
        ("port,p",      po::value<unsigned short>()->default_value(8080),      "TCP port to listen on")
        ("target-host", po::value<std::string>()->required(),                  "backend hostname or IP")
        ("target-port", po::value<unsigned short>()->required(),               "backend TCP port")
        ("socket-path", po::value<std::string>()->required(),                  "UNIX socket path for readiness signal")
        ("threads,t",   po::value<int>()->default_value(4),                    "I/O thread-pool size")
        ("log-level",   po::value<std::string>()->default_value("info"),       "log level (trace/debug/info/warning/error)")
        ("timeout",     po::value<int>()->default_value(30),                   "per-request timeout in seconds")
        ("body-limit",  po::value<long>()->default_value(64L * 1024 * 1024),   "maximum body size in bytes");
    // clang-format on

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        if (vm.contains("help")) {
            std::cout << "\n"
                      << desc << "\n"
                      << "Started automatically by ApiGatewayController.\n\n";
            return 0;
        }
        po::notify(vm);
    } catch (const po::error &e) {
        std::cerr << "Error: " << e.what() << "\n\n"
                  << desc << "\n";
        return 1;
    }

    Awsmock::Core::LogStream::SetSeverity(vm["log-level"].as<std::string>());

    Awsmock::Dto::ApiGateway::ProxyConfig cfg;
    cfg.name = vm["name"].as<std::string>();
    cfg.listenPort = vm["port"].as<unsigned short>();
    cfg.targetHost = vm["target-host"].as<std::string>();
    cfg.targetPort = vm["target-port"].as<unsigned short>();
    cfg.socketPath = vm["socket-path"].as<std::string>();
    cfg.threads = vm["threads"].as<int>();
    cfg.timeoutSeconds = vm["timeout"].as<int>();
    cfg.bodyLimitBytes = vm["body-limit"].as<long>();

    log_info << "awsmock-agw '" << cfg.name << "' starting"
             << " | listen=:" << cfg.listenPort
             << " | backend=" << cfg.targetHost << ":" << cfg.targetPort
             << " | threads=" << cfg.threads;

    net::io_context ioc{cfg.threads};

    // Start accepting client connections before we signal readiness.
    std::make_shared<Awsmock::Agw::ProxyListener>(ioc, cfg)->Run();

    // Signal readiness to ApiGatewayController.  The controller's waitForSocket()
    // loop polls this path — it succeeds as soon as our UNIX socket is in LISTEN.
    ReadinessSocket readiness{cfg.socketPath};
    if (!readiness.ok()) {
        log_error << "Could not create readiness socket at '" << cfg.socketPath << "'";
        return 1;
    }
    log_info << "awsmock-agw '" << cfg.name << "' ready (socket=" << cfg.socketPath << ")";

    // Graceful shutdown on SIGINT / SIGTERM
    net::signal_set signals{ioc, SIGINT, SIGTERM};
    signals.async_wait([&](const boost::system::error_code &, int sig) {
        log_info << "awsmock-agw '" << cfg.name << "': signal " << sig << " — stopping";
        ioc.stop();
    });

    // Run the io_context across a thread pool
    std::vector<std::thread> threads;
    threads.reserve(static_cast<std::size_t>(cfg.threads - 1));
    for (int i = 0; i < cfg.threads - 1; ++i)
        threads.emplace_back([&ioc] { ioc.run(); });
    ioc.run();
    for (auto &t: threads) t.join();

    log_info << "awsmock-agw '" << cfg.name << "' stopped";
    return 0;
}
