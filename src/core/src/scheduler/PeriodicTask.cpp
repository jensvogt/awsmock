
#include "awsmock/core/CronUtils.h"


#include <awsmock/core/scheduler/PeriodicTask.h>

#include <utility>

namespace AwsMock::Core {

    PeriodicTask::PeriodicTask(boost::asio::io_context &ioService, std::string const &name, const int interval, handler_fn task, const int delay)
        : ioService(ioService), timer(ioService), task(std::move(task)), name(name), interval(interval), _delay(delay) {

        log_debug << "Create PeriodicTask '" << name << "'";

        // Schedule start to be run by the _io_service
        post(ioService, [this] { start(); });
    }

    PeriodicTask::PeriodicTask(boost::asio::io_context &ioService, std::string const &name, std::string cronExpression, handler_fn task)
        : ioService(ioService), timer(ioService), task(std::move(task)), name(name), _cronExpression(std::move(cronExpression)) {

        log_debug << "Create CronTask '" << name << "'";

        // Schedule start to be run by the _io_service
        post(ioService, [this] { startCron(); });
    }

    [[maybe_unused]] void PeriodicTask::execute(boost::system::error_code const &e) {

        if (e != boost::asio::error::operation_aborted) {

            log_debug << "Execute PeriodicTask '" << name << "'";

            task();

            timer.expires_at(timer.expires_at() + boost::posix_time::seconds(interval));
            start_wait();
        }
    }

    [[maybe_unused]] void PeriodicTask::executeCron(boost::system::error_code const &e) {

        if (e != boost::asio::error::operation_aborted) {

            log_debug << "Execute PeriodicTask '" << name << "'";

            task();

            _delay = CronUtils::GetNextExecutionTimeSeconds(_cronExpression);
            timer.expires_at(timer.expires_at() + boost::posix_time::seconds(_delay));
            start_wait();
        }
    }

    void PeriodicTask::start() {
        log_debug << "Start PeriodicTask '" << name << "'";

        // Wait for first execution time
        if (_delay > 0) {
            std::this_thread::sleep_for(std::chrono::seconds(_delay));
        }

        // Uncomment if you want to call the handler on startup (i.e. at time 0)
        task();

        timer.expires_from_now(boost::posix_time::seconds(interval));
        start_wait();
    }

    void PeriodicTask::startCron() {
        log_debug << "Start CronTask '" << name << "'";

        _delay = CronUtils::GetNextExecutionTimeSeconds(_cronExpression);
        timer.expires_from_now(boost::posix_time::seconds(_delay));
        start_wait_cron();
    }

    [[maybe_unused]] void PeriodicTask::start_wait() {
        timer.async_wait(boost::bind(&PeriodicTask::execute, this, boost::asio::placeholders::error));
    }

    [[maybe_unused]] void PeriodicTask::start_wait_cron() {
        timer.async_wait(boost::bind(&PeriodicTask::executeCron, this, boost::asio::placeholders::error));
    }

}// namespace AwsMock::Core
