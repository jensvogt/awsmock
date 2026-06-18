//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/core/DateTimeUtils.h>

namespace Awsmock::Core {

#ifdef _WIN32
    extern "C" char *strptime(const char *s, const char *f, struct tm *tm) {
        // std::get_time uses the same format parameters as strptime.
        std::istringstream input(s);
        input.imbue(std::locale(setlocale(LC_ALL, nullptr)));
        input >> std::get_time(tm, f);
        if (input.fail()) {
            return nullptr;
        }
        return (char *) (s + input.tellg());
    }
#endif

    std::string DateTimeUtils::ToISO8601(const system_clock::time_point &timePoint) {
        return std::format("{:%FT%TZ}", timePoint);
    }

    system_clock::time_point DateTimeUtils::FromISO8601(const std::string &dateString) {
        std::tm t = {};
        strptime(dateString.c_str(), "%FT%T", &t);
        t.tm_hour = t.tm_hour + 1;
        return system_clock::from_time_t(mktime(&t));
    }

    system_clock::time_point DateTimeUtils::FromUnixTimestamp(const long timestamp) {
        const system_clock::time_point tp{std::chrono::milliseconds{timestamp}};
#if __APPLE__
        return tp + std::chrono::hours(1);
#else
        return std::chrono::zoned_time{std::chrono::current_zone(), tp + std::chrono::seconds(UtcOffset())};
#endif
    }

#ifdef _WIN32
    system_clock::time_point DateTimeUtils::FromUnixTimestamp(const long long timestamp) {
        const system_clock::time_point tp{std::chrono::milliseconds{timestamp}};
        return std::chrono::zoned_time{std::chrono::current_zone(), tp + std::chrono::seconds(UtcOffset())};
    }
#endif

    std::string DateTimeUtils::HttpFormatNow() {
        return HttpFormat(system_clock::now());
    }

    std::string DateTimeUtils::HttpFormat(const system_clock::time_point &timePoint) {
        char buf[256];
        const time_t timeT = system_clock::to_time_t(timePoint);
        struct tm tm {};
        gmtime_r(&timeT, &tm);
        strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S GMT", &tm);
        return {buf};
    }

    long DateTimeUtils::UnixTimestamp(const system_clock::time_point &timePoint) {
        return std::chrono::duration_cast<std::chrono::seconds>(timePoint.time_since_epoch()).count();
    }

    long DateTimeUtils::UnixTimestampMs(const system_clock::time_point &timePoint) {
        return std::chrono::duration_cast<std::chrono::milliseconds>(timePoint.time_since_epoch()).count();
    }

    system_clock::time_point DateTimeUtils::LocalDateTimeNow() {
#if __APPLE__
        return std::chrono::system_clock::now();
#else
        return system_clock::time_point(std::chrono::zoned_time(std::chrono::current_zone(), system_clock::now()).get_local_time().time_since_epoch());
#endif
    }

    system_clock::time_point DateTimeUtils::UtcDateTimeNow() {
#if __APPLE__
        return system_clock::time_point{std::chrono::time_point_cast<std::chrono::milliseconds>(system_clock::now())};
#else
        return system_clock::now();
#endif
    }

    long DateTimeUtils::UtcOffset() {
#if __APPLE__
        time_t clock;
        const tm *localTime = localtime(&clock);
        return localTime->tm_gmtoff;
#else
        return std::chrono::current_zone()->get_info(system_clock::now()).offset.count();
#endif
    }

    system_clock::time_point DateTimeUtils::ConvertToUtc(const system_clock::time_point &value) {
        const long seconds = std::chrono::time_point_cast<std::chrono::seconds>(value).time_since_epoch().count();
        return system_clock::from_time_t(seconds - UtcOffset());
    }

    int DateTimeUtils::GetSecondsUntilMidnight() {
        using namespace std;
        using namespace std::chrono;

        const auto now = system_clock::now();
        const auto today = floor<days>(now);
        return 24 * 3600 - static_cast<int>(duration_cast<seconds>(now - today).count());
    }

}// namespace Awsmock::Core
