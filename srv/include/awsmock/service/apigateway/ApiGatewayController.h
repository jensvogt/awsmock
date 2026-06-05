#pragma once

// C++ includes
#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <sys/types.h>
#include <thread>
#include <utility>
#include <vector>

// Awsmock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/apigateway/model/ApiGatewayProcess.h>

namespace Awsmock::Service {

    bool waitForSocket(const std::string &path, int timeoutMs);

    bool spawnProcess(const std::shared_ptr<Dto::ModuleProcess> &svc);

    /**
     * @brief Manages and controls core service operations.
     *
     * ApiGatewayController is responsible for handling the startup, shutdown,
     * and monitoring of the various services within the system.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ApiGatewayController {
      public:

        /**
         * @brief Registers a module with the system.
         *
         * This method is used to add a module to the system, enabling it to
         * participate in the application's runtime operations.
         *
         * @param cfg configuration
         */
        void registerModule(const Dto::ProcessConfig &cfg);

        /**
         * @brief Starts a service by its name.
         *
         * This method attempts to start the specified service and ensures its activation
         * within the system, if possible.
         *
         * @param name The name of the service to be started.
         * @return True if the service successfully starts, otherwise false.
         */
        bool start(const std::string &name);

        /**
         * @brief Stops a running service by its name.
         *
         * This method attempts to stop a service identified by its name within
         * the specified timeout period. If the service cannot be stopped within
         * the given timeout, the operation may fail.
         *
         * @param name The name of the service to be stopped.
         * @param timeoutMs The maximum time, in milliseconds, to wait for the service to stop. Default is 5000 ms.
         * @return True if the service was stopped successfully, otherwise false.
         */
        bool stop(const std::string &name, int timeoutMs = 5000);

        /**
         * @brief Restarts the service by shutting it down and starting it again.
         *
         * The restart method is used to stop the currently running service and
         * immediately start it again to ensure the service resumes operation.
         *
         * @param name The name of the service to be stopped.
         * @return True if the restart operation succeeds, false otherwise.
         */
        bool restart(const std::string &name);

        /**
         * @brief Initiates the startup process for all registered services.
         *
         * The startAll method ensures that all services within the system are
         * started in the correct order and become operational. It handles
         * dependencies and configurations required to bring services online.
         */
        void startAll();

        /**
         * @brief Stops all currently running services.
         *
         * Iterates through the list of registered services and stops each one safely.
         * This method ensures thread safety via mutex locking to prevent race conditions
         * during the operation.
         */
        void stopAll();

        /**
         * @brief Retrieves the state of a specified service module.
         *
         * This method fetches the current state of a service module based on its name.
         * The state represents the operational status of the module in the system.
         *
         * @param name The name of the service module whose state is to be retrieved.
         * @return The state of the specified service module as a `Database::Entity::ModuleState`.
         */
        Database::Entity::ProcessState getState(const std::string &name);

        /**
         * @brief Returns a snapshot of all registered modules and their current states.
         *
         * Thread-safe; acquires the internal mutex before reading the service map.
         *
         * @return Vector of (name, state) pairs, one entry per registered module.
         */
        std::vector<std::pair<std::string, Database::Entity::ProcessState>> listModules();

        /**
         * @brief Returns the Unix-domain socket path for the named service.
         *
         * @param name Service name (e.g. "s3", "sqs").
         * @return Socket path, or empty string if the service is not registered.
         */
        std::string getSocketPath(const std::string &name);

        /**
         * @brief Waits for a process to terminate or for a timeout to elapse.
         *
         * This method monitors the specified process by its process ID, waiting for it
         * to exit or for the timeout to expire. It uses the provided timeout in milliseconds
         * to determine how long to wait.
         *
         * @param pid The process ID of the target process to monitor.
         * @param timeoutMs The maximum time in milliseconds to wait for the process to exit.
         * @return True if the process exited within the timeout duration, or false if the wait
         *         timed out before the process exited.
         */
        bool waitForExit(pid_t pid, int timeoutMs);

        /**
         * @brief Initializes and starts the watchdog mechanism.
         *
         * This method is responsible for activating the watchdog to monitor
         * system operations and ensure their reliability by detecting and
         * handling potential failures.
         */
        void startWatchdog();

        /**
         * @brief Handles the termination of a child process.
         *
         * Invoked when a child process exits, this method manages cleanup and
         * performs necessary actions based on the process's exit status.
         */
        void onChildExit();

        /**
         * @brief Restarts all active services in the system.
         *
         * This method stops and then reinitializes all currently running services,
         * ensuring they are restarted in a clean state.
         */
        void restartAll();

      private:

        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "ApiGateway"};

        /**
         * @brief Maintains a collection of service modules managed by the system.
         *
         * The `_services` map associates module names (as strings) with their respective
         * `ModuleProcess` instances. Each `ModuleProcess` object contains configuration
         * and runtime state information for a specific module, allowing the system to
         * track and manage module processes, such as their IDs, states, and restart count.
         */
        std::map<std::string, std::shared_ptr<Dto::ModuleProcess>> _services;

        /**
         * @brief Ensures thread-safe access to shared resources.
         *
         * _mutex is a recursive mutex used to prevent race conditions
         * when accessing or modifying shared data in a multithreaded environment.
         * It allows the same thread to acquire the lock multiple times without
         * causing a deadlock.
         */
        std::recursive_mutex _mutex;

        /**
         * @brief Thread responsible for monitoring and managing system processes.
         *
         * _watchdog is used to run a separate thread that ensures the healthy state of
         * processes and services managed by the ApiGatewayController. It may include tasks
         * such as detecting unexpected process exits, triggering restarts, or logging warnings.
         */
        std::thread _watchdog;

        /**
         * @brief Indicates whether the service is currently running.
         *
         * This atomic boolean flag is used to control and monitor the running state
         * of the ApiGatewayController, enabling thread-safe operations to start or stop
         * the service processes.
         */
        std::atomic<bool> _running{true};

        /**
         * @brief Retrieves a service module by name.
         *
         * This method retrieves the service module instance associated
         * with the given name, allowing access to its configuration,
         * process state, and runtime details.
         *
         * @param name The name of the service module to retrieve.
         * @return A shared pointer to the ModuleProcess instance corresponding
         *         to the specified service module name.
         */
        std::shared_ptr<Dto::ModuleProcess> getService(const std::string &name);

        /**
         * @brief Finds a module process by its process ID (PID).
         *
         * This method retrieves the `ModuleProcess` instance associated with the
         * specified process ID. It allows access to the module's runtime details
         * and state information based on the provided PID.
         *
         * @param pid The process ID of the module to locate.
         * @return A shared pointer to the corresponding `ModuleProcess` instance,
         *         or `nullptr` if no module is found with the specified PID.
         */
        std::shared_ptr<Dto::ModuleProcess> findByPid(pid_t pid);

        /**
         * @brief Calculates the stable runtime of a module process.
         *
         * This method determines the duration for which the given module process has
         * been in a stable, operational state.
         *
         * @param svc A shared pointer to the module process for which the stable runtime is to be calculated.
         * @return The stable runtime of the module process in milliseconds.
         */
        static long stableRuntime(const std::shared_ptr<Dto::ModuleProcess> &svc);

        /**
         * @brief Schedules a restart for the specified module process.
         *
         * This function is responsible for planning the restart of a given module
         * process by managing necessary delays, calculating stability runtime,
         * and ensuring appropriate restart timing to maintain operational stability.
         *
         * @param svc A shared pointer to the module process that needs to be restarted.
         */
        static void scheduleRestart(const std::shared_ptr<Dto::ModuleProcess> &svc);
    };

}// namespace Awsmock::Service