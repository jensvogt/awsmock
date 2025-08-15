//
// Created by vogje01 on 6/10/25.
//

#ifndef AWS_MOCK_CORE_CRON_UTILS_H
#define AWS_MOCK_CORE_CRON_UTILS_H

// C++ includes
#include <chrono>
#include <string>
#include <thread>

// Awsmock includes
#include <awsmock/core/Cron.h>
#include <awsmock/core/config/Configuration.h>

namespace AwsMock::Core {

    using std::chrono::system_clock;

    /**
     * @brief Utilities to handle CRON jobs
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class CronUtils {

      public:

        /**
         * Constructor
         */
        CronUtils() = default;

        /**
         * Destruktor
         */
        ~CronUtils() = default;

        /**
         * @brief Get the next cron execution time as timestamp
         *
         * @param cron cron expression
         * @return timepoint of next execution
         */
        static system_clock::time_point GetNextExecutionTime(const std::string &cron);

        /**
         * @brief Get the next cron execution time as seconds from now
         *
         * @param cron cron expression
         * @return number of seconds from now
         */
        static long GetNextExecutionTimeSeconds(const std::string &cron);
    };

}// namespace AwsMock::Core

#endif// AWS_MOCK_CORE_CRON_UTILS_H
