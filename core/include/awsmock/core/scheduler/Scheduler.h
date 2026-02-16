//
// Created by vogje01 on 10/7/24.
//

#ifndef AWSMOCK_CORE_PERIODIC_SCHEDULER_H
#define AWSMOCK_CORE_PERIODIC_SCHEDULER_H

// C++ includes
#include <chrono>
#include <map>
#include <string>

// Boost includes
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/system/error_code.hpp>

// AwsMock includes
#include <awsmock/core/CronUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/scheduler/CronTask.h>
#include <awsmock/core/scheduler/PeriodicTask.h>

namespace AwsMock::Core {

    using std::chrono::system_clock;

    class Scheduler : boost::noncopyable {

      public:

        explicit Scheduler(boost::asio::io_context &ioc);

        /**
         * @brief Main routine
         */
        static void Run();

        /**
         * @brief Add a task to the scheduler.
         *
         * @param name name of the task
         * @param task task function
         * @param interval interval in seconds
         * @param delay start delay in seconds
         */
        void AddTask(std::string const &name, PeriodicTask::handler_fn const &task, int interval, int delay = 0);

        /**
         * @brief Add a cron task to the scheduler
         *
         * @param name name of the task
         * @param task task function
         * @param cronExpression cron expression
         */
        void AddTask(std::string const &name, PeriodicTask::handler_fn const &task, const std::string &cronExpression);

        /**
         * @brief Shutdown the scheduler
         */
        void Shutdown() const;

        /**
         * @brief Shutdown a specific task
         *
         * @param name name of the task
         */
        void Shutdown(const std::string &name);

      private:

        /**
         * Boost asio IO service
         */
        boost::asio::io_context &_ioc;

        /**
         * Periodic task list
         */
        std::map<std::string, std::unique_ptr<PeriodicTask>> _periodicTasks;

        /**
         * Cron task list
         */
        std::map<std::string, std::unique_ptr<CronTask>> _cronTasks;
    };

}// namespace AwsMock::Core

#endif//AWSMOCK_CORE_PERIODIC_SCHEDULER_H
