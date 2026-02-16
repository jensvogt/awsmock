//
// Created by vogje01 on 10/7/24.
//

#ifndef AWSMOCK_CORE_CRON_TASK_H
#define AWSMOCK_CORE_CRON_TASK_H

// C++ includes
#include <functional>
#include <string>
#include <utility>

// Boost includes
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/chrono/duration.hpp>
#include <boost/noncopyable.hpp>

// AwsMock includes
#include <awsmock/core/CronUtils.h>
#include <awsmock/core/logging/LogStream.h>

namespace AwsMock::Core {

    /**
     * @brief Cron task
     *
     * @par
     * This class starts a task according to a cron schedule
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class CronTask : boost::noncopyable {

      public:

        /**
         * @Brief Handler function
         */
        typedef std::function<void()> handler_fn;

        /**
         * @brief Constructor
         *
         * @param ioService boost IO context
         * @param name task name
         * @param cronExpression cron expression
         * @param task task
         */
        CronTask(boost::asio::io_context &ioService, std::string const &name, std::string cronExpression, handler_fn task);

        /**
         * @brief Execute task
         * @param e error code
         */
        [[maybe_unused]] void Execute(boost::system::error_code const &e);

        /**
         * @brief Start the task
         */
        void Start();

        /**
         * @brief Stops the task
         */
        void Stop();

      private:

        /**
         * Wait for the cron delay
         */
        [[maybe_unused]] void StartWait();

        /**
         * IO context
         */
        boost::asio::io_context &_ios;

        /**
         * Timer
         */
        boost::asio::steady_timer _timer;

        /**
         * Task handler
         */
        handler_fn _task;

        /**
         * Task name
         */
        std::string _name;

        /**
         * Next execution wait time in seconds
         */
        long _next{};

        /**
         * Cron expression
         */
        std::string _cronExpression;
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_PERIODIC_TASK_H
