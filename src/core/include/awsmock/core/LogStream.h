//
// Created by vogje01 on 20/06/2023.
//

#ifndef AWS_MOCK_CORE_LOG_STREAM_H
#define AWS_MOCK_CORE_LOG_STREAM_H

// C++ standard includes
#include <filesystem>
#include <iostream>
#include <istream>
#include <string_view>

// Plog includes
#define PLOG_OMIT_LOG_DEFINES
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Init.h>
#include <plog/Initializers/ConsoleInitializer.h>
#include <plog/Log.h>

// AwsMock includes
#include <awsmock/core/LogFormatter.h>

#define DEFAULT_LOG_SIZE (10 * 1024 * 1024)
#define DEFAULT_LOG_COUNT 5

namespace AwsMock::Core {

    /**
     * @brief Logging stream
     *
     * Based on <a https://github.com/SergiusTheBest/plog>Plog</a>.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class LogStream {

      public:

        /**
         * @brief Constructor
         */
        LogStream() = default;

        /**
         * @brief Initialization
         */
        static void Initialize();

        /**
         * @brief Set the maximum severity
         *
         * @param severity PLog severity string
         */
        static void SetSeverity(const std::string &severity);

        /**
         * @brief Set the log filename
         *
         * @param filename name of the log file
         */
        static void SetFilename(const std::string &filename);

      private:

        /**
         * Log size
         */
        static long logSize;

        /**
         * Log count
         */
        static int logCount;
    };

}// namespace AwsMock::Core

#define log_fatal PLOG_FATAL
#define log_error PLOG_ERROR
#define log_warning PLOG_WARNING
#define log_info PLOG_INFO
#define log_debug PLOG_DEBUG
#define log_trace PLOG_VERBOSE

#endif// AWS_MOCK_CORE_LOG_STREAM_H
