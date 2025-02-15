
#include <awsmock/core/scheduler/PeriodicScheduler.h>

namespace AwsMock::Core {

    PeriodicScheduler::PeriodicScheduler(boost::asio::io_context &io_service) : _io_service(io_service){};

    void PeriodicScheduler::Run() {
    }

    void PeriodicScheduler::AddTask(std::string const &name, PeriodicTask::handler_fn const &task, int interval, int delay) {
        if (!_tasks.contains(name)) {
            _tasks[name] = std::make_unique<PeriodicTask>(std::ref(_io_service), name, interval, task, delay);
        }
    }

    void PeriodicScheduler::Shutdown() const {
        _io_service.stop();
    }

}// namespace AwsMock::Core
