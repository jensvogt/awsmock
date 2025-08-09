
#include <awsmock/core/scheduler/Scheduler.h>

namespace AwsMock::Core {

    Scheduler::Scheduler(boost::asio::io_context &ioc) : _ioc(ioc) {};

    void Scheduler::Run() {}

    void Scheduler::AddTask(std::string const &name, PeriodicTask::handler_fn const &task, int interval, int delay) {
        if (!_periodicTasks.contains(name)) {
            _periodicTasks[name] = std::make_unique<PeriodicTask>(std::ref(_ioc), name, interval, task, delay);
        }
    }

    void Scheduler::AddTask(std::string const &name, PeriodicTask::handler_fn const &task, const std::string &cronExpression) {
        if (!_periodicTasks.contains(name)) {
            _cronTasks[name] = std::make_unique<CronTask>(std::ref(_ioc), name, cronExpression, task);
        }
    }

    void Scheduler::Shutdown() const {
        _ioc.stop();
    }

}// namespace AwsMock::Core
