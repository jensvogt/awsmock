
#include "awsmock/core/CronUtils.h"


#include <awsmock/core/scheduler/PeriodicTask.h>

#include <utility>

namespace AwsMock::Core {

    PeriodicTask::PeriodicTask(boost::asio::io_context &ioService, std::string const &name, const int interval, handler_fn task, const int delay)
        : _ios(ioService), _timer(ioService), _task(std::move(task)), _name(name), _interval(interval), _delay(delay) {

        log_debug << "Create PeriodicTask '" << name << "'";

        // Schedule start to be run by the _io_service
        post(ioService, [this] { Start(); });
    }

    void PeriodicTask::Execute(boost::system::error_code const &e) {

        if (e != boost::asio::error::operation_aborted) {

            log_debug << "Execute PeriodicTask '" << _name << "'";

            _task();

            _timer.expires_after(std::chrono::seconds(_interval));
            StartWait();
        }
    }

    void PeriodicTask::Start() {
        log_debug << "Starting PeriodicTask '" << _name << "'";

        if (_delay > 0) {
            // Wait for the first execution time
            _timer.expires_after(std::chrono::seconds(_delay));
            StartWait();
        } else if (_interval > 0) {
            // Periodic task
            _task();
            _timer.expires_after(std::chrono::seconds(_interval));
            StartWait();
        } else {
            // One time task
            _task();
        }
    }

    void PeriodicTask::Stop() {
        log_debug << "Stopping PeriodicTask '" << _name << "'";
        _timer.cancel();
        log_info << "PeriodicTask '" << _name << "' stopped";
    }

    void PeriodicTask::StartWait() {
        auto callback = [this](const boost::system::error_code &error) {
            Execute(error);
        };
        _timer.async_wait(callback);
    }

}// namespace AwsMock::Core
