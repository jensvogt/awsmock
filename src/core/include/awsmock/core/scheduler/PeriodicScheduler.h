//
// Created by vogje01 on 10/7/24.
//

#ifndef AWSMOCK_CORE_PERIODIC_SCHEDULER_H
#define AWSMOCK_CORE_PERIODIC_SCHEDULER_H

// C++ includes
#include <chrono>
#include <functional>
#include <string>
#include <utility>

// Boost includes
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/system/error_code.hpp>

// AwsMOck includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/scheduler/PeriodicTask.h>

namespace AwsMock::Core {

    using std::chrono::system_clock;

    class PeriodicScheduler : boost::noncopyable {

      public:

        explicit PeriodicScheduler(boost::asio::io_service &io_service);

        /**
         * Main routine
         */
        void Run();

        /**
         * @brief Add a task to the scheduler.
         *
         * @param name name of the task
         * @param task task function
         * @param interval interval in seconds
         * @param startTime time for the first execution (should be something like (00:00, oder 04:00)
         */
        void AddTask(std::string const &name, PeriodicTask::handler_fn const &task, int interval, int delay = 0);

        /**
         * @brief Shutdown the scheduler
         */
        void Shutdown();

      private:

        /**
         * Boost asio IO service
         */
        boost::asio::io_service &_io_service;

        /**
         * Task list
         */
        std::vector<std::unique_ptr<PeriodicTask>> _tasks;
    };

}// namespace AwsMock::Core

#endif//AWSMOCK_CORE_PERIODIC_SCHEDULER_H
