//
// Created by vogje01 on 20/06/2023.
//

#ifndef AWS_MOCK_CORE_LOG_STREAM_H
#define AWS_MOCK_CORE_LOG_STREAM_H

#define BOOST_LOG_DYN_LINK 1

// C++ standard includes
#include <filesystem>
#include <iostream>
#include <istream>
#include <string_view>

// Boost includes
#include <boost/log/attributes.hpp>
#include <boost/log/attributes/clock.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/channel_feature.hpp>
#include <boost/log/sources/channel_logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/move/utility_core.hpp>
#include <boost/phoenix.hpp>

// Awsmock includes
#include <awsmock/core/config/Configuration.h>

#define DEFAULT_LOG_SIZE (10 * 1024 * 1024)
#define DEFAULT_LOG_COUNT 5

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(my_logger, boost::log::sources::severity_logger<boost::log::trivial::severity_level>)

BOOST_LOG_ATTRIBUTE_KEYWORD(process_id, "ProcessID", boost::log::attributes::current_process_id::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(thread_id, "ThreadID", boost::log::attributes::current_thread_id::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(line, "Line", int)
BOOST_LOG_ATTRIBUTE_KEYWORD(file, "File", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(function, "Function", boost::log::attributes::function<std::string>)

namespace AwsMock::Core {

    /**
     * @brief Logging stream
     *
     * @par
     * Based on Boost::Log.
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

        /**
         * Log directory
         */
        static std::string logDir;

        /**
         * Log filename prefix
         */
        static std::string logPrefix;

        /**
         * Console appender
         */
        static boost::shared_ptr<boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend>> console_sink;

        /**
         * File appender
         */
        static boost::shared_ptr<boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend>> file_sink;
    };

}// namespace AwsMock::Core

#define log_fatal BOOST_LOG_SEV(my_logger::get(), boost::log::trivial::fatal) << boost::log::add_value("Line", __LINE__) << boost::log::add_value("File", __FILE__) << boost::log::add_value("Function", __PRETTY_FUNCTION__)
#define log_error BOOST_LOG_SEV(my_logger::get(), boost::log::trivial::error) << boost::log::add_value("Line", __LINE__) << boost::log::add_value("File", __FILE__) << boost::log::add_value("Function", __PRETTY_FUNCTION__)
#define log_warning BOOST_LOG_SEV(my_logger::get(), boost::log::trivial::warning) << boost::log::add_value("Line", __LINE__) << boost::log::add_value("File", __FILE__) << boost::log::add_value("Function", __PRETTY_FUNCTION__)
#define log_info BOOST_LOG_SEV(my_logger::get(), boost::log::trivial::info) << boost::log::add_value("Line", __LINE__) << boost::log::add_value("File", __FILE__) << boost::log::add_value("Function", __PRETTY_FUNCTION__)
#define log_debug BOOST_LOG_SEV(my_logger::get(), boost::log::trivial::debug) << boost::log::add_value("Line", __LINE__) << boost::log::add_value("File", __FILE__) << boost::log::add_value("Function", __PRETTY_FUNCTION__)
#define log_trace BOOST_LOG_SEV(my_logger::get(), boost::log::trivial::trace) << boost::log::add_value("Line", __LINE__) << boost::log::add_value("File", __FILE__) << boost::log::add_value("Function", __PRETTY_FUNCTION__)

#endif// AWS_MOCK_CORE_LOG_STREAM_H
