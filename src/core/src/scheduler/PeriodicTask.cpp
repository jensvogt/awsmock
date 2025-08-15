
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

            _timer.expires_at(_timer.expires_at() + boost::posix_time::seconds(_interval));
            StartWait();
        }
    }

    void PeriodicTask::Start() {
        log_debug << "Start PeriodicTask '" << _name << "'";

        // Wait for the first execution time
        if (_delay > 0) {
            _timer.expires_from_now(boost::posix_time::seconds(_delay));
            StartWait();
        } else {
            _task();
            _timer.expires_from_now(boost::posix_time::seconds(_interval));
            StartWait();
        }
    }

    void PeriodicTask::StartWait() {
        auto callback = [this](const boost::system::error_code &error) {
            Execute(error);
        };
        _timer.async_wait(callback);
    }

}// namespace AwsMock::Core
