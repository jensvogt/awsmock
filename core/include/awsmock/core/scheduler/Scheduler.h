//
// Created by vogje01 on 10/7/24.
//

#pragma once

// C++ includes
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <string>

// Boost includes
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/system/error_code.hpp>

// AwsMock includes
#include <awsmock/core/CronUtils.h>
#include <awsmock/core/logging/LogStream.h>

namespace Awsmock::Core {

    /**
     * @brief Singleton scheduler that manages periodic, cron, and one-time tasks.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class Scheduler : boost::noncopyable {

      public:

        /**
         * @brief Task handler function type
         */
        using handler_fn = std::function<void()>;

        /**
         * @brief Initialize the singleton with an IO context (call once at startup).
         *
         * @param ioc boost IO context
         * @return reference to the singleton instance
         */
        static Scheduler &initialize(boost::asio::io_context &ioc);

        /**
         * @brief Access the singleton instance.
         *
         * @return reference to the singleton instance
         */
        static Scheduler &instance();

        /**
         * @brief Add a periodic or one-time task.
         *
         * @param name task name
         * @param task handler function
         * @param interval repeat interval in seconds (0 = run once)
         * @param delay initial delay in seconds before first execution
         */
        void AddTask(const std::string &name, handler_fn task, int interval = 0, int delay = 0);

        /**
         * @brief Add a fire-and-forget one-time task.
         *
         * The task runs once and the scheduler entry is automatically removed
         * afterwards, freeing the name slot for reuse.
         *
         * @param name task name
         * @param task handler function
         * @param delay delay in seconds before execution (0 = immediate)
         */
        void AddOneTimeTask(const std::string &name, handler_fn task, long delay = 0);

        /**
         * @brief Add a waitable one-time task.
         *
         * Like AddOneTimeTask but returns a shared_future<void> that becomes
         * ready when the task finishes (or carries the exception if it throws).
         * The scheduler entry is removed automatically; callers do not need to
         * call shutdown() for the task name.
         *
         * @param name task name
         * @param task handler function
         * @param delay delay in seconds before execution (0 = immediate)
         * @return shared_future<void> resolved when task completes
         */
        std::shared_future<void> AddWaitableOneTimeTask(const std::string &name, handler_fn task, long delay = 0);

        /**
         * @brief Add a cron-scheduled task.
         *
         * @param name task name
         * @param task handler function
         * @param cronExpression cron expression
         */
        void AddTask(const std::string &name, handler_fn task, const std::string &cronExpression);

        /**
         * @brief Shutdown all tasks.
         */
        void Shutdown() const;

        /**
         * @brief Shutdown a specific task by name.
         *
         * @param name task name
         */
        void Shutdown(const std::string &name);

      private:

        // ---- Periodic / one-time task entry ----
        struct PeriodicEntry : boost::noncopyable {
            PeriodicEntry(boost::asio::io_context &ioc, std::string name, long interval, handler_fn task, long delay);
            void Start();
            void Execute(const boost::system::error_code &e);
            void Stop();
            void StartWait();

            boost::asio::steady_timer timer;
            handler_fn task;
            std::string name;
            long interval{};
            long delay{};
        };

        // ---- Cron task entry ----
        struct CronEntry : boost::noncopyable {
            CronEntry(boost::asio::io_context &ioc, std::string name, std::string cronExpression, handler_fn task);
            void Start();
            void Execute(const boost::system::error_code &e);
            void Stop();
            void StartWait();

            boost::asio::steady_timer timer;
            handler_fn task;
            std::string name;
            std::string cronExpression;
            long next{};
        };

        explicit Scheduler(boost::asio::io_context &ioc);

        static std::unique_ptr<Scheduler> _instance;
        static std::mutex _mutex;

        boost::asio::io_context &_ioc;
        std::map<std::string, std::unique_ptr<PeriodicEntry>> _periodicTasks;
        std::map<std::string, std::unique_ptr<CronEntry>> _cronTasks;
    };

}// namespace Awsmock::Core
