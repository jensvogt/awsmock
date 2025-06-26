
#include <awsmock/core/scheduler/CronTask.h>

namespace AwsMock::Core {

    CronTask::CronTask(boost::asio::io_context &ioService, std::string const &name, std::string cronExpression, handler_fn task)
        : _ios(ioService), _timer(ioService), _task(std::move(task)), _name(name), _cronExpression(std::move(cronExpression)) {

        log_debug << "Create CronTask '" << name << "'";

        // Schedule start to be run by the _io_service
        post(ioService, [this] { Start(); });
    }

    void CronTask::Execute(boost::system::error_code const &e) {

        if (e != boost::asio::error::operation_aborted) {

            log_debug << "Execute CronTask '" << _name << "'";

            _task();

            _next = CronUtils::GetNextExecutionTimeSeconds(_cronExpression);
            _timer.expires_at(_timer.expires_at() + boost::posix_time::seconds(_next));
            StartWait();
        }
    }

    void CronTask::Start() {
        log_debug << "Start CronTask '" << _name << "'";

        _next = CronUtils::GetNextExecutionTimeSeconds(_cronExpression);
        _timer.expires_from_now(boost::posix_time::seconds(_next));
        StartWait();
    }

    void CronTask::StartWait() {
        _timer.async_wait(boost::bind(&CronTask::Execute, this, boost::asio::placeholders::error));
    }

}// namespace AwsMock::Core
