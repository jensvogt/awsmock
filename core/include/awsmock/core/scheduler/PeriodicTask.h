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
#include <awsmock/core/logging/LogStream.h>

namespace AwsMock::Core {

    /**
     * @brief Periodic task
     *
     * @par
     * This class starts a task which will be repeated after each interval. The periodic interval must be given in seconds, together with an initial delay in seconds.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class PeriodicTask : boost::noncopyable {

      public:

        /**
         * Task handler
         */
        typedef std::function<void()> handler_fn;

        /**
         * @brief Constructor
         *
         * @param ioService boost IO context
         * @param name task name
         * @param interval periodic interval in seconds
         * @param task task
         * @param delay initial delay in seconds
         */
        PeriodicTask(boost::asio::io_context &ioService, std::string const &name, int interval, handler_fn task, int delay);

        /**
         * @brief Execute the task by calling the handler functor.
         *
         * @param e error code
         */
        [[maybe_unused]] void Execute(boost::system::error_code const &e);

        /**
         * @brief Start the task scheduler
         */
        void Start();

        /**
         * @brief Stops the periodic task
         */
        void Stop();

      private:

        /**
         * Wait for the delay
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
         * Interval in seconds
         */
        long _interval{};

        /**
         * Initial delay
         */
        long _delay{};
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_PERIODIC_TASK_H
