
#include <cassert>
#include <ranges>

#include <awsmock/core/scheduler/Scheduler.h>

namespace AwsMock::Core {

    logger_t _logger{boost::log::keywords::channel = "Core"};

    // Static member definitions
    std::unique_ptr<Scheduler> Scheduler::_instance;
    std::mutex Scheduler::_mutex;

    // ---- Singleton management ----

    Scheduler &Scheduler::initialize(boost::asio::io_context &ioc) {
        std::lock_guard lock(_mutex);
        if (!_instance) {
            _instance.reset(new Scheduler(ioc));
            log_debug << "Scheduler initialized";
        }
        return *_instance;
    }

    Scheduler &Scheduler::instance() {
        assert(_instance && "Scheduler::initialize() must be called before instance()");
        return *_instance;
    }

    Scheduler::Scheduler(boost::asio::io_context &ioc) : _ioc(ioc) {
    }

    // ---- Public API ----

    void Scheduler::AddTask(const std::string &name, handler_fn task, int interval, int delay) {
        if (!_periodicTasks.contains(name)) {
            _periodicTasks[name] = std::make_unique<PeriodicEntry>(std::ref(_ioc), name, interval, std::move(task), delay);
        }
    }

    void Scheduler::AddOneTimeTask(const std::string &name, handler_fn task, long delay) {
        if (!_periodicTasks.contains(name)) {
            auto wrapped = [this, name, t = std::move(task)]() mutable {
                try {
                    t();
                } catch (const std::exception &exc) {
                    log_error << "One-time task '" << name << "' threw an unhandled exception: " << exc.what();
                } catch (...) {
                    log_error << "One-time task '" << name << "' threw an unknown exception";
                }
                boost::asio::post(_ioc, [this, name] { Shutdown(name); });
            };
            _periodicTasks[name] = std::make_unique<PeriodicEntry>(std::ref(_ioc), name, 0, std::move(wrapped), delay);
        }
    }

    std::shared_future<void> Scheduler::AddWaitableOneTimeTask(const std::string &name, handler_fn task, long delay) {
        auto promise = std::make_shared<std::promise<void>>();
        std::shared_future<void> future = promise->get_future().share();
        if (!_periodicTasks.contains(name)) {
            auto wrapped = [this, name, t = std::move(task), p = std::move(promise)]() mutable {
                try {
                    t();
                    p->set_value();
                } catch (...) {
                    p->set_exception(std::current_exception());
                }
                boost::asio::post(_ioc, [this, name] { Shutdown(name); });
            };
            _periodicTasks[name] = std::make_unique<PeriodicEntry>(std::ref(_ioc), name, 0, std::move(wrapped), delay);
        }
        return future;
    }

    void Scheduler::AddTask(const std::string &name, handler_fn task, const std::string &cronExpression) {
        if (!_cronTasks.contains(name)) {
            _cronTasks[name] = std::make_unique<CronEntry>(std::ref(_ioc), name, cronExpression, std::move(task));
        }
    }

    void Scheduler::Shutdown() const {
        for (const auto &val: _periodicTasks | std::views::values) {
            val->Stop();
        }
        for (const auto &val: _cronTasks | std::views::values) {
            val->Stop();
        }
    }

    void Scheduler::Shutdown(const std::string &name) {
        if (_periodicTasks.contains(name)) {
            _periodicTasks[name]->Stop();
            _periodicTasks.erase(name);
        }
        if (_cronTasks.contains(name)) {
            _cronTasks[name]->Stop();
            _cronTasks.erase(name);
        }
    }

    // ---- PeriodicEntry ----

    Scheduler::PeriodicEntry::PeriodicEntry(boost::asio::io_context &ioc, std::string name, long interval, handler_fn task, long delay)
        : timer(ioc), task(std::move(task)), name(std::move(name)), interval(interval), delay(delay) {
        log_debug << "Create periodic task '" << this->name << "'";
        post(ioc, [this] { Start(); });
    }

    void Scheduler::PeriodicEntry::Start() {
        log_debug << "Starting periodic task '" << name << "'";
        if (delay > 0) {
            timer.expires_after(std::chrono::seconds(delay));
            StartWait();
        } else if (interval > 0) {
            task();
            timer.expires_after(std::chrono::seconds(interval));
            StartWait();
        } else {
            task();
        }
    }

    void Scheduler::PeriodicEntry::Execute(const boost::system::error_code &e) {
        if (e != boost::asio::error::operation_aborted) {
            log_debug << "Execute periodic task '" << name << "'";
            task();
            if (interval > 0) {
                timer.expires_after(std::chrono::seconds(interval));
                StartWait();
            }
        }
    }

    void Scheduler::PeriodicEntry::Stop() {
        log_debug << "Stopping periodic task '" << name << "'";
        timer.cancel();
        log_debug << "Periodic task '" << name << "' stopped";
    }

    void Scheduler::PeriodicEntry::StartWait() {
        timer.async_wait([this](const boost::system::error_code &e) { Execute(e); });
    }

    // ---- CronEntry ----

    Scheduler::CronEntry::CronEntry(boost::asio::io_context &ioc, std::string name, std::string cronExpression, handler_fn task)
        : timer(ioc), task(std::move(task)), name(std::move(name)), cronExpression(std::move(cronExpression)) {
        log_debug << "Create cron task '" << this->name << "'";
        post(ioc, [this] { Start(); });
    }

    void Scheduler::CronEntry::Start() {
        log_debug << "Starting cron task '" << name << "'";
        next = CronUtils::GetNextExecutionTimeSeconds(cronExpression);
        timer.expires_after(std::chrono::seconds(next));
        StartWait();
    }

    void Scheduler::CronEntry::Execute(const boost::system::error_code &e) {
        if (e != boost::asio::error::operation_aborted) {
            log_debug << "Execute cron task '" << name << "'";
            task();
            next = CronUtils::GetNextExecutionTimeSeconds(cronExpression);
            timer.expires_after(std::chrono::seconds(next));
            StartWait();
        }
    }

    void Scheduler::CronEntry::Stop() {
        log_debug << "Stopping cron task '" << name << "'";
        timer.cancel();
        log_debug << "Cron task '" << name << "' stopped";
    }

    void Scheduler::CronEntry::StartWait() {
        timer.async_wait([this](const boost::system::error_code &e) { Execute(e); });
    }

}// namespace AwsMock::Core
