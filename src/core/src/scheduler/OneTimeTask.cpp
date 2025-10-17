
#include <awsmock/core/scheduler/OneTimeTask.h>

namespace AwsMock::Core {

    OneTimeTask::OneTimeTask(boost::asio::io_context &ioService, std::string const &name, long delay, handler_fn task)
        : _ios(ioService), _timer(ioService), _task(std::move(task)), _name(name), _delay(delay) {

        log_debug << "Create OneTimeTask '" << name << "'";

        // Schedule start to be run by the _io_service
        post(ioService, [this] { Start(); });
    }

    void OneTimeTask::Execute(boost::system::error_code const &e) {

        if (e != boost::asio::error::operation_aborted) {

            log_debug << "Execute OneTimeTask '" << _name << "'";

            _task();
        }
    }

    void OneTimeTask::Start() {
        log_debug << "Starting OneTimeTask '" << _name << "'";

        _timer.expires_after(std::chrono::seconds(_delay));
        StartWait();
    }

    void OneTimeTask::StartWait() {
        _timer.async_wait(boost::bind(&OneTimeTask::Execute, this, boost::asio::placeholders::error));
    }

}// namespace AwsMock::Core
