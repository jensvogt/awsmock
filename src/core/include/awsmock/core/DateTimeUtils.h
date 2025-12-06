//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWS_MOCK_CORE_DATETIME_UTILS_H
#define AWS_MOCK_CORE_DATETIME_UTILS_H

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

// Boost includes
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/locale/date_time.hpp>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>

namespace AwsMock::Core {

    using std::chrono::system_clock;

    template<class Clock, class Duration1, class Duration2>
    constexpr auto CeilTimePoint(std::chrono::time_point<Clock, Duration1> t, Duration2 m) noexcept {
        using R = std::chrono::time_point<Clock, Duration2>;
        auto r = std::chrono::time_point_cast<Duration2>(R{} + (t - R{}) / m * m);
        if (r < t)
            r += m;
        return r;
    }

    /**
     * @brief Date time utilities.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class DateTimeUtils {

      public:

        /**
         * @brief Returns the time_point in ISO8001 format
         *
         * @pre
         * Format is according to ToISO8601, for instance: '2024-04-28T15:07:37.035332Z'.
         *
         * @param timePoint point in time
         * @return time_point in ISO 8601 format
         */
        static std::string ToISO8601(const system_clock::time_point &timePoint);

        /**
         * @brief Returns the current date time in ISO8001 format
         *
         * @pre
         * Format is according to ToISO8601, for instance: '2024-04-28T15:07:37.035332Z'.
         *
         * @return time_point in ISO 8601 format
         */
        static std::string NowISO8601();

        /**
         * @brief Convert a ToISO8601 timestamp into a system time point.
         * @param dateString
         * @return time_point
         */
        static system_clock::time_point FromISO8601(const std::string &dateString);

        /**
         * @brief Convert a ToISO8601 UTC timestamp into a system time point which is local time in Europe/Berlin.
         * @param dateString
         * @return time_point
         */
        static system_clock::time_point FromISO8601UTC(const std::string &dateString);

        /**
         * @brief Returns the current time in HTTP format.
         *
         * @pre
         * Format is according to RFC822, for instance: 'Tue, 15 Nov 2010 08:12:31 +0200'.
         *
         * @return current time in HTTP format
         */
        static std::string HttpFormatNow();

        /**
         * @brief Returns the time_point in HTTP format
         *
         * @pre
         * Format is according to RFC822, for instance: 'Tue, 15 Nov 2010 08:12:31 +0200'.
         *
         * @param timePoint point in time
         * @return time_point in HTTP format
         */
        static std::string HttpFormat(const system_clock::time_point &timePoint);

        /**
         * @brief Returns the time_point in Unix epoch timestamp (UTC)
         *
         * @param timePoint point in time
         * @return time_point as Unix epoch timestamp
         */
        static long UnixTimestamp(const system_clock::time_point &timePoint);

        /**
         * @brief Returns the time_point in Java Unix epoch timestamp (UTC)
         *
         * @par
         * This returns the number of milliseconds since 01.01.1970.
         *
         * @param timePoint point in time
         * @return time_point as Unix epoch timestamp
         */
        static long UnixTimestampMs(const system_clock::time_point &timePoint);

        /**
         * @brief Returns the current Unix epoch timestamp (UTC)
         *
         * @return now as Unix epoch timestamp
         */
        static long UnixTimestampNow();

        /**
         * @brief Returns the time_point in Unix epoch timestamp (LocalTime)
         *
         * @param timePoint point in time
         * @return time_point as Unix epoch timestamp in local time
         */
        static long UnixTimestampLocal(const system_clock::time_point &timePoint);

        /**
         * @brief Get the localtime from unix timestamp
         *
         * @param timestamp UNIX timestamp
         * @return system_clock::time_point
         */
        static system_clock::time_point FromUnixTimestamp(long timestamp);

#ifdef _WIN32
        /**
         * @brief Get the localtime from unix timestamp
         *
         * @par
         * On Windows (using MSVC) the bson library converts a uint_64 in long long.
         * 
         * @param timestamp UNIX timestamp
         * @return system_clock::time_point
         */
        static system_clock::time_point FromUnixTimestamp(long long timestamp);
#endif

        /**
         * @brief Get the current local time
         *
         * @return local time.
         */
        static system_clock::time_point LocalDateTimeNow();

        /**
         * @brief Get the current UTC time
         *
         * @return UTC time.
         */
        static system_clock::time_point UtcDateTimeNow();

        /**
         * @brief Gets the difference in seconds between now and the given time in '00:00:00'
         *
         * @return number of seconds between now and the given time
         */
        static int GetSecondsUntilMidnight();

        /**
         * @brief Returns the offset to UTC in seconds.
         *
         * @return offset in seconds to UTC.
         */
        static long UtcOffset();

        /**
         * @brief Convert from ISO8601.
         *
         * @param now timestamp
         * @return ISO8601 string
         */
        static std::string FromISO8601(system_clock::time_point now);

        /**
         * @brief Convert a local time to a UTC time
         *
         * @param value local time
         * @return UTC time
         */
        static system_clock::time_point ConvertToUtc(const system_clock::time_point &value);
    };

}// namespace AwsMock::Core

#endif