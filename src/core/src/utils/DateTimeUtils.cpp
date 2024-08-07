//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/core/DateTimeUtils.h>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace AwsMock::Core {

    std::string DateTimeUtils::AwsDatetime(const Poco::DateTime &dateTime) {
        return Poco::DateTimeFormatter::format(dateTime, Poco::DateTimeFormat::ISO8601_FRAC_FORMAT);
    }

    std::string DateTimeUtils::AwsDatetimeNow() {
        Poco::DateTime now;
        return Poco::DateTimeFormatter::format(now, Poco::DateTimeFormat::ISO8601_FRAC_FORMAT);
    }

    std::string DateTimeUtils::ISO8601(const system_clock::time_point &timePoint) {
        return std::format("{:%FT%TZ}", timePoint);
    }

    std::string DateTimeUtils::ISO8601Now() {
        return std::format("{:%FT%TZ}", system_clock::now());
    }

    std::string DateTimeUtils::HttpFormat(const system_clock::time_point &timePoint) {
        char buf[256];
        time_t timeT = system_clock::to_time_t(timePoint);
        struct tm tm = *gmtime(&timeT);
        strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
        return {buf};
    }


};// namespace AwsMock::Core
