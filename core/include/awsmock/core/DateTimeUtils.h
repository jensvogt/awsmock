//
// Created by vogje01 on 30/05/2023.
//

#pragma once

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define BOOST_ASIO_NO_WIN32_LEAN_AND_MEAN
#include <boost/asio.hpp>
#include <windows.h>
#endif

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>

namespace Awsmock::Core {

    using std::chrono::system_clock;

    /**
     * @brief Date time utilities.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class DateTimeUtils {

      public:

        /**
         * @brief Returns the time_point in ISO 8601 format
         *
         * @pre
         * Format: '2024-04-28T15:07:37.035332Z'.
         *
         * @param timePoint point in time
         * @return time_point in ISO 8601 format
         */
        static std::string ToISO8601(const system_clock::time_point &timePoint);

        /**
         * @brief Convert an ISO 8601 timestamp string into a system time point.
         * @param dateString
         * @return time_point
         */
        static system_clock::time_point FromISO8601(const std::string &dateString);

        /**
         * @brief Returns the current time in HTTP date format (RFC 7231).
         *
         * @pre
         * Format: 'Tue, 15 Nov 2010 08:12:31 GMT'.
         *
         * @return current time in HTTP format
         */
        static std::string HttpFormatNow();

        /**
         * @brief Returns the time_point in HTTP date format (RFC 7231).
         *
         * @pre
         * Format: 'Tue, 15 Nov 2010 08:12:31 GMT'.
         *
         * @param timePoint point in time
         * @return time_point in HTTP format
         */
        static std::string HttpFormat(const system_clock::time_point &timePoint);

        /**
         * @brief Returns the time_point as Unix epoch seconds (UTC).
         *
         * @param timePoint point in time
         * @return seconds since Unix epoch
         */
        static long UnixTimestamp(const system_clock::time_point &timePoint);

        /**
         * @brief Returns the time_point as Unix epoch milliseconds (UTC).
         *
         * @param timePoint point in time
         * @return milliseconds since Unix epoch
         */
        static long UnixTimestampMs(const system_clock::time_point &timePoint);

        /**
         * @brief Convert a Unix epoch timestamp (milliseconds) to a time_point.
         *
         * @param timestamp milliseconds since Unix epoch
         * @return system_clock::time_point
         */
        static system_clock::time_point FromUnixTimestamp(long timestamp);

#ifdef _WIN32
        /**
         * @brief Windows overload for BSON's uint64→long long conversion.
         *
         * @param timestamp milliseconds since Unix epoch
         * @return system_clock::time_point
         */
        static system_clock::time_point FromUnixTimestamp(long long timestamp);
#endif

        /**
         * @brief Returns the current local time as a time_point.
         *
         * @return local time.
         */
        static system_clock::time_point LocalDateTimeNow();

        /**
         * @brief Returns the current UTC time as a time_point.
         *
         * @return UTC time.
         */
        static system_clock::time_point UtcDateTimeNow();

        /**
         * @brief Returns seconds remaining until midnight UTC.
         *
         * @return seconds until midnight
         */
        static int GetSecondsUntilMidnight();

        /**
         * @brief Returns the local timezone offset from UTC in seconds.
         *
         * @return offset in seconds
         */
        static long UtcOffset();

        /**
         * @brief Converts a local time_point to UTC.
         *
         * @param value local time
         * @return UTC time
         */
        static system_clock::time_point ConvertToUtc(const system_clock::time_point &value);
    };

}// namespace Awsmock::Core
