//
// Created by vogje01 on 2/16/26.
//

#pragma once

// C++ standard includes
#include <future>
#include <memory>
#include <utility>

// Boost includes
#include <boost/signals2/connection.hpp>
#include <boost/signals2/signal.hpp>

namespace Awsmock::Core {

    /**
     * @brief General event bus using boost signals2
     *
     * @author jens.vogt\@opitz-consulting.com
     * @since 1.14.7
     */
    class EventBus {
      public:

        /**
         * @brief Singleton instance
         */
        static EventBus &instance() {
            static EventBus eventBus;
            return eventBus;
        }

        /**
         * @brief Signal for an FTP server file upload.
         *
         * @par
         * The template parameter are containing the filename, the username who uploaded the file and the file metadata. Metadata
         * is a string map, which contains usually the user-agent and user-agent-id.
         *
         * @tparam username
         * @tparam filename
         * @tparam metadata
         */
        boost::signals2::signal<void(std::string, std::string, std::map<std::string, std::string>)> sigFtpUpload;

        /**
         * @brief Signal for system shutdown
         */
        boost::signals2::signal<void()> sigShutdown;

        /**
         * @brief Signal monitoring system
         */
        boost::signals2::signal<void(std::string, std::string, std::string, double)> sigMetricGauge;

        /**
         * @brief Signal monitoring system
         */
        boost::signals2::signal<void(std::string, std::string, std::string)> sigMetricRate;

        /**
         * @brief Signal monitoring system
         */
        boost::signals2::signal<void(std::map<std::string, double>)> sigCollector;

        /**
         * @brief Signal to start a single lambda function by ARN.
         *
         * @tparam functionArn lambda function ARN
         * @tparam region AWS region
         */
        boost::signals2::signal<void(std::string, std::string)> sigLambdaStart;

        /**
         * @brief Signal to stop a single lambda function by ARN.
         *
         * @tparam functionArn lambda function ARN
         * @tparam region AWS region
         */
        boost::signals2::signal<void(std::string, std::string)> sigLambdaStop;

        /**
         * @brief Signal to start all lambda functions in a region.
         *
         * @tparam region AWS region
         */
        boost::signals2::signal<void(std::string)> sigLambdaStartAll;

        /**
         * @brief Signal to stop all lambda functions in a region.
         *
         * @tparam region AWS region
         */
        boost::signals2::signal<void(std::string)> sigLambdaStopAll;

        /**
         * @brief Signal to invoke a lambda function.
         *
         * @tparam region AWS region
         * @tparam functionName lambda function name
         * @tparam payload JSON payload
         * @tparam invocationType "RequestResponse" or "Event"
         * @tparam promise result channel; nullptr for fire-and-forget (EVENT invocations)
         */
        boost::signals2::signal<void(std::string, std::string, std::string, std::string,
                                     std::shared_ptr<std::promise<std::pair<int, std::string>>>)>
                sigLambdaInvoke;

        /**
         * @brief Signal to check the Docker daemon status of a lambda function.
         *
         * @tparam functionArn lambda function ARN
         */
        boost::signals2::signal<void(std::string)> sigLambdaCheck;
    };

};// namespace Awsmock::Core
