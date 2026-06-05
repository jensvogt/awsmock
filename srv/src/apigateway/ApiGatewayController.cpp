
// Awsmock includes
#include <awsmock/service/apigateway/ApiGatewayController.h>
#include <awsmock/entity/module/ModuleState.h>
#include <awsmock/repository/RepositoryFactory.h>
#include <awsmock/service/apigateway/ApiGatewayControllerPlatform.h>

namespace Awsmock::Service {

    static logger_t _logger{boost::log::keywords::channel = "ApiGateway"};

    static Database::Entity::Module::ModuleState toModuleState(const Database::Entity::ProcessState state) {
        switch (state) {
            case Database::Entity::ProcessState::RUNNING: return Database::Entity::Module::ModuleState::RUNNING;
            case Database::Entity::ProcessState::STARTING:
            case Database::Entity::ProcessState::RESTARTING: return Database::Entity::Module::ModuleState::STARTING;
            default: return Database::Entity::Module::ModuleState::STOPPED;
        }
    }

    // Reads lines from fd until EOF and re-emits each line through Boost.Log.
    // Runs on a detached background thread; closes fd when done.
    static void drainPipe(const int fd, const bool /*isError*/) {
        std::string line;
        char ch;
        auto emit = [&](const std::string &raw) {
            log_raw(raw);
        };
#ifdef _WIN32
        while (_read(fd, &ch, 1) == 1) {
#else
            while (read(fd, &ch, 1) == 1) {
#endif
            if (ch == '\n') {
                emit(line);
                line.clear();
            } else
                line += ch;
        }
        if (!line.empty()) emit(line);
#ifdef _WIN32
        _close(fd);
#else
        close(fd);
#endif
    }

    bool waitForSocket(const std::string &path, const int timeoutMs) {
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);

        while (std::chrono::steady_clock::now() < deadline) {
#ifdef _WIN32
            const SOCKET fd = socket(AF_UNIX, SOCK_STREAM, 0);
            if (fd == INVALID_SOCKET) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            sockaddr_un addr{};
            addr.sun_family = AF_UNIX;
            strncpy_s(addr.sun_path, sizeof(addr.sun_path), path.c_str(), _TRUNCATE);
            if (connect(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == 0) {
                closesocket(fd);
                return true;
            }
            closesocket(fd);
#else
            const int fd = socket(AF_UNIX, SOCK_STREAM, 0);
            sockaddr_un addr{};
            addr.sun_family = AF_UNIX;
            std::strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path) - 1);
            if (connect(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == 0) {
                close(fd);
                return true;
            }
            close(fd);
#endif
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        return false;
    }

#ifdef _WIN32
    bool spawnProcess(const std::shared_ptr<Dto::ModuleProcess> &svc) {
        HANDLE hOutRead = nullptr, hOutWrite = nullptr;
        HANDLE hErrRead = nullptr, hErrWrite = nullptr;
        SECURITY_ATTRIBUTES sa{sizeof(sa), nullptr, TRUE};

        if (!CreatePipe(&hOutRead, &hOutWrite, &sa, 0) ||
            !CreatePipe(&hErrRead, &hErrWrite, &sa, 0)) {
            svc->state = Database::Entity::ProcessState::CRASHED;
            return false;
        }
        SetHandleInformation(hOutRead, HANDLE_FLAG_INHERIT, 0);
        SetHandleInformation(hErrRead, HANDLE_FLAG_INHERIT, 0);

        STARTUPINFOA si{};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdOutput = hOutWrite;
        si.hStdError = hErrWrite;
        si.hStdInput = INVALID_HANDLE_VALUE;

        std::string cmdLine = "\"" + svc->config.executable + "\"";
        for (auto &a: svc->config.args) cmdLine += " " + a;

        PROCESS_INFORMATION pi{};
        if (!CreateProcessA(nullptr, cmdLine.data(), nullptr, nullptr, TRUE,
                            CREATE_NEW_PROCESS_GROUP, nullptr, nullptr, &si, &pi)) {
            CloseHandle(hOutRead);
            CloseHandle(hOutWrite);
            CloseHandle(hErrRead);
            CloseHandle(hErrWrite);
            svc->state = Database::Entity::ProcessState::CRASHED;
            return false;
        }

        CloseHandle(hOutWrite);
        CloseHandle(hErrWrite);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);

        svc->pid = pi.dwProcessId;
        svc->state = Database::Entity::ProcessState::STARTING;
        svc->startTime = std::chrono::steady_clock::now();

        const int outFd = _open_osfhandle(reinterpret_cast<intptr_t>(hOutRead), 0);
        const int errFd = _open_osfhandle(reinterpret_cast<intptr_t>(hErrRead), 0);
        std::thread(drainPipe, outFd, false).detach();
        std::thread(drainPipe, errFd, true).detach();

        if (waitForSocket(svc->config.socketPath, 5000)) {
            svc->state = Database::Entity::ProcessState::RUNNING;
            log_info << "Service ready, name: " << svc->config.name << ", pid: " << svc->pid;
            Database::RepositoryFactory::instance().moduleRepository()->setState(svc->config.name, toModuleState(svc->state));
            return true;
        }

        log_error << "Service " << svc->config.name << " did not become ready, killing pid " << svc->pid;
        if (HANDLE hProc = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, FALSE, svc->pid)) {
            TerminateProcess(hProc, 1);
            WaitForSingleObject(hProc, 2000);
            CloseHandle(hProc);
        }
        svc->pid = 0;
        svc->state = Database::Entity::ProcessState::PENDING_RESTART;
        svc->lastCrashTime = std::chrono::steady_clock::now();
        Database::RepositoryFactory::instance().moduleRepository()->setState(svc->config.name, toModuleState(svc->state));
        return false;
    }
#else
    bool spawnProcess(const std::shared_ptr<Dto::ModuleProcess> &svc) {
        int outPipe[2], errPipe[2];
        pipe(outPipe);
        pipe(errPipe);

        const pid_t pid = fork();

        if (pid < 0) {
            svc->state = Database::Entity::ProcessState::CRASHED;
            return false;
        }

        if (pid == 0) {
            dup2(outPipe[1], STDOUT_FILENO);
            dup2(errPipe[1], STDERR_FILENO);

            close(outPipe[0]);
            close(outPipe[1]);
            close(errPipe[0]);
            close(errPipe[1]);

            setsid();

            std::vector<const char *> argv;
            argv.push_back(svc->config.executable.c_str());
            for (auto &a: svc->config.args) argv.push_back(a.c_str());
            argv.push_back(nullptr);

            execvp(svc->config.executable.c_str(), const_cast<char **>(argv.data()));
            _exit(127);
        }

        close(outPipe[1]);
        close(errPipe[1]);

        svc->pid = pid;
        svc->state = Database::Entity::ProcessState::STARTING;
        svc->startTime = std::chrono::steady_clock::now();
        svc->stdoutFd = outPipe[0];
        svc->stderrFd = errPipe[0];

        std::thread(drainPipe, outPipe[0], false).detach();
        std::thread(drainPipe, errPipe[0], true).detach();

        if (waitForSocket(svc->config.socketPath, 5000)) {
            svc->state = Database::Entity::ProcessState::RUNNING;
            log_info << "Service ready, name: " << svc->config.name << ", pid: " << svc->pid;
            Database::RepositoryFactory::instance().moduleRepository()->setState(svc->config.name, toModuleState(svc->state));
            return true;
        }

        log_error << "Service " << svc->config.name << " did not become ready, killing pid " << svc->pid;
        kill(svc->pid, SIGKILL);
        waitpid(svc->pid, nullptr, 0);
        svc->pid = -1;
        svc->state = Database::Entity::ProcessState::PENDING_RESTART;
        svc->lastCrashTime = std::chrono::steady_clock::now();
        Database::RepositoryFactory::instance().moduleRepository()->setState(svc->config.name, toModuleState(svc->state));
        return false;
    }
#endif

    void ApiGatewayController::registerModule(const Dto::ProcessConfig &cfg) {
        std::lock_guard lock(_mutex);
        _services[cfg.name] = std::make_shared<Dto::ModuleProcess>();
        _services[cfg.name]->config = cfg;
    }

    bool ApiGatewayController::start(const std::string &name) {
        const auto svc = getService(name);
        if (!svc) return false;
        const bool ok = spawnProcess(svc);
        Database::RepositoryFactory::instance().moduleRepository()->setState(svc->config.name, toModuleState(svc->state));
        return ok;
    }

    bool ApiGatewayController::stop(const std::string &name, const int timeoutMs) {
        std::lock_guard lock(_mutex);
        const auto svc = getService(name);
#ifdef _WIN32
        if (!svc || svc->pid == 0 || svc->pid == static_cast<pid_t>(-1)) return false;
#else
        if (!svc || svc->pid <= 0) return false;
#endif

        svc->state = Database::Entity::ProcessState::STOPPING;
        log_info << "Stopping service '" << name << "' (pid " << svc->pid << ")";

#ifdef _WIN32
        if (const HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, svc->pid)) {
            TerminateProcess(hProc, 0);
            CloseHandle(hProc);
        }
#else
        kill(svc->pid, SIGTERM);
#endif
        if (waitForExit(svc->pid, timeoutMs)) {
            log_info << "Service '" << name << "' stopped cleanly";
#ifdef _WIN32
            svc->pid = 0;
#else
            svc->pid = -1;
#endif
            svc->state = Database::Entity::ProcessState::STOPPED;
            Database::RepositoryFactory::instance().moduleRepository()->setState(svc->config.name, toModuleState(svc->state));
            return true;
        }

        log_warning << "Service '" << name << "' did not stop in " << timeoutMs << "ms, sending SIGKILL";
#ifdef _WIN32
        if (HANDLE hProc = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, FALSE, svc->pid)) {
            TerminateProcess(hProc, 1);
            WaitForSingleObject(hProc, 2000);
            CloseHandle(hProc);
        }
        if (waitForExit(svc->pid, 2000)) {
#else
            kill(svc->pid, SIGKILL);
            if (waitForExit(svc->pid, 2000)) {
#endif
            log_info << "Service '" << name << "' killed";
#ifdef _WIN32
            svc->pid = 0;
#else
            svc->pid = -1;
#endif
            svc->state = Database::Entity::ProcessState::STOPPED;
            Database::RepositoryFactory::instance().moduleRepository()->setState(svc->config.name, toModuleState(svc->state));
            return true;
        }

        Database::RepositoryFactory::instance().moduleRepository()->setState(svc->config.name, toModuleState(svc->state));
        log_error << "ERROR: Service '" << name << "' could not be killed";
        return false;
    }

    bool ApiGatewayController::restart(const std::string &name) {
        stop(name);
        std::this_thread::sleep_for(std::chrono::milliseconds(getService(name)->config.restartDelayMs));
        return start(name);
    }

    void ApiGatewayController::startAll() {
        std::lock_guard lock(_mutex);
        for (auto &svc: _services | std::views::values) {
            spawnProcess(svc);
        }
    }

    void ApiGatewayController::stopAll() {
        std::lock_guard lock(_mutex);
        for (const auto &name: _services | std::views::keys) stop(name);
    }

    Database::Entity::ProcessState ApiGatewayController::getState(const std::string &name) {
        const auto svc = getService(name);
        return svc ? svc->state : Database::Entity::ProcessState::STOPPED;
    }

    std::string ApiGatewayController::getSocketPath(const std::string &name) {
        const auto svc = getService(name);
        if (!svc || svc->state != Database::Entity::ProcessState::RUNNING) return {};
        return svc->config.socketPath;
    }

    std::vector<std::pair<std::string, Database::Entity::ProcessState> > ApiGatewayController::listModules() {
        std::lock_guard lock(_mutex);
        std::vector<std::pair<std::string, Database::Entity::ProcessState> > result;
        result.reserve(_services.size());
        for (const auto &[name, svc]: _services) result.emplace_back(name, svc->state);
        return result;
    }

    bool ApiGatewayController::waitForExit(const pid_t pid, const int timeoutMs) {
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);

        while (std::chrono::steady_clock::now() < deadline) {
#ifdef _WIN32
            const HANDLE hProc = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION, FALSE, pid);
            if (!hProc) return true;
            if (const DWORD wr = WaitForSingleObject(hProc, 0); wr == WAIT_OBJECT_0) {
                DWORD code = 0;
                GetExitCodeProcess(hProc, &code);
                CloseHandle(hProc);
                log_info << "Process " << pid << " exited with code " << code;
                return true;
            }
            CloseHandle(hProc);
#else
            int status;
            const pid_t result = waitpid(pid, &status, WNOHANG);

            if (result == pid) {
                if (WIFEXITED(status)) {
                    log_info << "Process " << pid << " exited with code " << WEXITSTATUS(status);
                } else if (WIFSIGNALED(status)) {
                    log_info << "Process " << pid << " killed by signal " << WTERMSIG(status);
                }
                return true;
            }

            if (result == -1) {
                if (errno == ECHILD) return true;
                log_error << "Waitpid error: " << strerror(errno);
                return false;
            }
#endif
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        return false;
    }

    void ApiGatewayController::startWatchdog() {
        _watchdog = std::thread([this] {
            while (_running) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::lock_guard<std::recursive_mutex> lock(_mutex);

                for (auto &[name, svc]: _services) {
                    if (svc->state != Database::Entity::ProcessState::PENDING_RESTART) continue;

                    if (svc->config.maxRestarts != -1 &&
                        svc->restartCount >= svc->config.maxRestarts) {
                        log_error << "Service " << name << " exceeded max restarts (" << svc->config.maxRestarts << "), giving up";
                        svc->state = Database::Entity::ProcessState::STOPPED;
                        continue;
                    }

                    auto now = std::chrono::steady_clock::now();
                    if (const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - svc->lastCrashTime).count(); elapsed < svc->currentDelay) {
                        log_debug << "Service " << name << " waiting for backoff (" << elapsed << "ms / " << svc->currentDelay << "ms)";
                        continue;
                    }

                    log_info << "Restarting " << name << " (attempt " << svc->restartCount + 1 << ", delay was " << svc->currentDelay << "ms)";
                    svc->state = Database::Entity::ProcessState::RESTARTING;
                    svc->restartCount++;
                    svc->currentDelay = std::min(svc->currentDelay * 2, svc->config.maxRestartDelayMs);

                    if (const bool ok = spawnProcess(svc); !ok) {
                        log_error << "Service " << name << " failed to start";
                        svc->state = Database::Entity::ProcessState::PENDING_RESTART;
                        svc->lastCrashTime = std::chrono::steady_clock::now();
                    } else {
                        if (stableRuntime(svc) > 60) {
                            log_info << "Service " << svc->config.name << " was stable, resetting backoff";
                            svc->restartCount = 0;
                            svc->currentDelay = svc->config.restartDelayMs;
                        }
                    }
                }
            }
        });
    }

    void ApiGatewayController::onChildExit() {
#ifndef _WIN32
        int status;
        pid_t pid;
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
            auto svc = findByPid(pid);
            if (!svc) continue;

            if (svc->state == Database::Entity::ProcessState::STOPPING || svc->state == Database::Entity::ProcessState::STOPPED) {
                svc->pid = -1;
                svc->state = Database::Entity::ProcessState::STOPPED;
                Database::RepositoryFactory::instance().moduleRepository()->setState(svc->config.name, toModuleState(svc->state));
                continue;
            }

            svc->pid = -1;
            svc->state = Database::Entity::ProcessState::CRASHED;
            svc->lastCrashTime = std::chrono::steady_clock::now();
            Database::RepositoryFactory::instance().moduleRepository()->setState(svc->config.name, toModuleState(svc->state));
            log_warning << "Service " << svc->config.name << " crashed (pid=" << pid << ")";

            if (svc->config.autoRestart) scheduleRestart(svc);
        }
#endif
    }

    void ApiGatewayController::restartAll() {
        std::lock_guard lock(_mutex);
        for (const auto &name: _services | std::views::keys) restart(name);
    }

    std::shared_ptr<Dto::ModuleProcess> ApiGatewayController::getService(const std::string &name) {
        std::lock_guard lock(_mutex);
        const auto it = _services.find(name);
        if (it == _services.end()) return nullptr;
        return it->second;
    }

    std::shared_ptr<Dto::ModuleProcess> ApiGatewayController::findByPid(const pid_t pid) {
        std::lock_guard lock(_mutex);
        for (auto &svc: _services | std::views::values)
            if (svc->pid == pid) return svc;
        return nullptr;
    }

    long ApiGatewayController::stableRuntime(const std::shared_ptr<Dto::ModuleProcess> &svc) {
        const auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(now - svc->startTime).count();
    }

    void ApiGatewayController::scheduleRestart(const std::shared_ptr<Dto::ModuleProcess> &svc) {
        svc->lastCrashTime = std::chrono::steady_clock::now();
        svc->state = Database::Entity::ProcessState::CRASHED;
    }

} // namespace Awsmock::Service
