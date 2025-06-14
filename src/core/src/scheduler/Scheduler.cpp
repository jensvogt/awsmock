
#include <awsmock/core/scheduler/Scheduler.h>

namespace AwsMock::Core {

    Scheduler::Scheduler(boost::asio::io_context &io_service) : _io_service(io_service) {};

    void Scheduler::Run() {}

    void Scheduler::AddTask(std::string const &name, PeriodicTask::handler_fn const &task, int interval, int delay) {
        if (!_periodicTasks.contains(name)) {
            _periodicTasks[name] = std::make_unique<PeriodicTask>(std::ref(_io_service), name, interval, task, delay);
        }
    }

    void Scheduler::AddTask(std::string const &name, PeriodicTask::handler_fn const &task, const std::string &cronExpression) {
        if (!_periodicTasks.contains(name)) {
            _cronTasks[name] = std::make_unique<CronTask>(std::ref(_io_service), name, cronExpression, task);
        }
    }

    void Scheduler::Shutdown() const {
        _io_service.stop();
    }

}// namespace AwsMock::Core
