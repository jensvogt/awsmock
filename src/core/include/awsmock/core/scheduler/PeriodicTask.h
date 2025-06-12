//
// Created by vogje01 on 10/7/24.
//

#ifndef AWSMOCK_CORE_PERIODIC_TASK_H
#define AWSMOCK_CORE_PERIODIC_TASK_H

// C++ includes
#include <functional>
#include <string>

// Boost includes
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/chrono/duration.hpp>
#include <boost/noncopyable.hpp>

// AwsMock includes
#include <awsmock/core/LogStream.h>

namespace AwsMock::Core {

    class PeriodicTask : boost::noncopyable {

      public:

        typedef std::function<void()> handler_fn;

        PeriodicTask(boost::asio::io_context &ioService, std::string const &name, int interval, handler_fn task, int delay);

        PeriodicTask(boost::asio::io_context &ioService, std::string const &name, std::string cronExpression, handler_fn task);

        [[maybe_unused]] void execute(boost::system::error_code const &e);

        [[maybe_unused]] void executeCron(boost::system::error_code const &e);

        void start();

        void startCron();

      private:

        /**
         * Wait for the delay
         */
        [[maybe_unused]] void start_wait();

        /**
         * Wait for the cron delay
         */
        [[maybe_unused]] void start_wait_cron();

        /**
         * IO context
         */
        boost::asio::io_context &ioService;

        /**
         * Timer
         */
        boost::asio::deadline_timer timer;

        /**
         * Task handler
         */
        handler_fn task;

        /**
         * Task name
         */
        std::string name;

        /**
         * Interval in seconds
         */
        long interval{};

        /**
         * Initial delay
         */
        long _delay{};

        /**
         * Cron expression
         */
        std::string _cronExpression;
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_PERIODIC_TASK_H
