//
// Created by vogje01 on 6/10/25.
//

#include <awsmock/core/CronUtils.h>

namespace AwsMock::Core {

    system_clock::time_point CronUtils::GetNextExecutionTime(const std::string &cron) {
        try {
            const auto cronExpression = make_cron(cron);

            const std::time_t now = std::time(nullptr);
            const tm *temp = localtime(&now);
            std::tm nextStart = cron_next(cronExpression, *temp);
#ifdef WIN32
            return system_clock::from_time_t(mktime(&nextStart));
#else
            return system_clock::from_time_t(timelocal(&nextStart));
#endif

        } catch (bad_cronexpr const &ex) {
            log_error << "Invalid cron expression, cron: " << cron << ", error: " << ex.what();
        }

        return system_clock::now();
    }

    long CronUtils::GetNextExecutionTimeSeconds(const std::string &cron) {
        const system_clock::time_point nextExecutionTime = GetNextExecutionTime(cron);
        if (const long next = duration_cast<std::chrono::seconds>(nextExecutionTime - system_clock::now()).count(); next < 1) {
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        return duration_cast<std::chrono::seconds>(nextExecutionTime - system_clock::now()).count();
    }

}// namespace AwsMock::Core