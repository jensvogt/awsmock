#pragma once

// C++ includes
#include <string>

namespace Awsmock::Dto::ApiGateway {

    /**
     * @brief Runtime configuration for an in-process HTTP reverse proxy listener.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ProxyConfig {

        /**
         * @brief Human-readable API name, used in log messages.
         */
        std::string name;

        /**
         * @brief Address to bind the proxy's TCP listener (default: all interfaces).
         */
        std::string listenAddress = "0.0.0.0";

        /**
         * @brief TCP port the proxy listens on for incoming HTTP requests.
         */
        unsigned short listenPort = 8080;

        /**
         * @brief Hostname or IP of the backend service to forward requests to.
         */
        std::string targetHost;

        /**
         * @brief TCP port of the backend service.
         */
        unsigned short targetPort = 80;

        /**
         * @brief Number of io_context worker threads (default: 4).
         */
        int threads = 4;

        /**
         * @brief Per-request timeout in seconds (default: 30).
         */
        int timeoutSeconds = 30;

        /**
         * @brief Maximum HTTP body size in bytes (default: 64 MiB).
         */
        long bodyLimitBytes = 64L * 1024 * 1024;
    };

}// namespace Awsmock::Dto::ApiGateway
