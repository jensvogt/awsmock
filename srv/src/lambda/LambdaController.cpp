//
// Created by vogje01 on 28/05/2026.
//

#include <awsmock/service/lambda/LambdaController.h>

namespace Awsmock::Service {

    LambdaController::LambdaController(Core::Scheduler &scheduler)
        : AbstractServer("lambda-controller"),
          _lambdaDatabase(Database::LambdaDatabase::instance()),
          _containerService(ContainerService::instance()),
          _scheduler(scheduler) {

        const Core::Configuration &config = Core::Configuration::instance();
        _region = config.get<std::string>("awsmock.region");
        _healthCheckPeriod = config.getOr<int>("awsmock.modules.lambda.health-check-period", 30);
        log_debug << "Lambda controller health-check period: " << _healthCheckPeriod << "s";

        // Connect EventBus signals ------------------------------------------------

        Core::EventBus::instance().sigLambdaStart.connect(
                boost::signals2::signal<void(std::string, std::string)>::slot_type(
                        &LambdaController::OnStartLambda, this,
                        std::placeholders::_1, std::placeholders::_2));

        Core::EventBus::instance().sigLambdaStop.connect(
                boost::signals2::signal<void(std::string, std::string)>::slot_type(
                        &LambdaController::OnStopLambda, this,
                        std::placeholders::_1, std::placeholders::_2));

        Core::EventBus::instance().sigLambdaStartAll.connect(
                boost::signals2::signal<void(std::string)>::slot_type(
                        &LambdaController::OnStartAllLambdas, this, std::placeholders::_1));

        Core::EventBus::instance().sigLambdaStopAll.connect(
                boost::signals2::signal<void(std::string)>::slot_type(
                        &LambdaController::OnStopAllLambdas, this, std::placeholders::_1));

        Core::EventBus::instance().sigLambdaInvoke.connect(
                boost::signals2::signal<void(std::string, std::string, std::string, std::string,
                                             std::shared_ptr<std::promise<std::pair<int, std::string>>>)>::slot_type(&LambdaController::OnInvokeLambda, this,
                                                                                                                     std::placeholders::_1, std::placeholders::_2,
                                                                                                                     std::placeholders::_3, std::placeholders::_4,
                                                                                                                     std::placeholders::_5));

        Core::EventBus::instance().sigLambdaCheck.connect(
                boost::signals2::signal<void(std::string)>::slot_type(
                        &LambdaController::OnCheckLambda, this, std::placeholders::_1));

        // Periodic health check task ----------------------------------------------
        _scheduler.AddTask("lambda-controller-health", [this] { CheckContainerHealth(); }, _healthCheckPeriod, _healthCheckPeriod);

        log_debug << "Lambda controller initialized";
    }

    void LambdaController::Shutdown() {
        _scheduler.Shutdown("lambda-controller-health");
        log_info << "Lambda controller stopped";
    }

    // -------------------------------------------------------------------------
    // Signal handlers
    // -------------------------------------------------------------------------

    void LambdaController::OnStartLambda(const std::string &functionArn, const std::string &region) const {
        log_info << "Lambda start requested, arn: " << functionArn;
        try {
            Dto::Lambda::StartLambdaRequest request;
            request.region = region;
            request.functionArn = functionArn;
            _lambdaService.StartLambda(request);
            log_info << "Lambda started, arn: " << functionArn;
        } catch (std::exception &e) {
            log_error << "Lambda start failed, arn: " << functionArn << ", error: " << e.what();
        }
    }

    void LambdaController::OnStopLambda(const std::string &functionArn, const std::string &region) const {
        log_info << "Lambda stop requested, arn: " << functionArn;
        try {
            Dto::Lambda::StopLambdaRequest request;
            request.region = region;
            request.functionArn = functionArn;
            _lambdaService.StopLambda(request);
            log_info << "Lambda stopped, arn: " << functionArn;
        } catch (std::exception &e) {
            log_error << "Lambda stop failed, arn: " << functionArn << ", error: " << e.what();
        }
    }

    void LambdaController::OnStartAllLambdas(const std::string &region) const {
        log_info << "Start-all-lambdas requested, region: " << region;
        try {
            _lambdaService.StartAllLambdas();
            log_info << "All lambdas started, region: " << region;
        } catch (std::exception &e) {
            log_error << "Start-all-lambdas failed, region: " << region << ", error: " << e.what();
        }
    }

    void LambdaController::OnStopAllLambdas(const std::string &region) const {
        log_info << "Stop-all-lambdas requested, region: " << region;
        try {
            _lambdaService.StopAllLambdas();
            log_info << "All lambdas stopped, region: " << region;
        } catch (std::exception &e) {
            log_error << "Stop-all-lambdas failed, region: " << region << ", error: " << e.what();
        }
    }

    void LambdaController::OnInvokeLambda(const std::string &region, const std::string &functionName, const std::string &payload, const std::string &invocationType,
                                          const std::shared_ptr<std::promise<std::pair<int, std::string>>> &promise) const {
        log_info << "Lambda invoke requested, function: " << functionName << ", type: " << invocationType;
        try {
            const auto accountId = Core::Configuration::instance().getAccountId();
            const std::string lambdaArn = Core::AwsUtils::CreateLambdaArn(region, accountId, functionName);

            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(lambdaArn);
            if (!lambda.enabled) {
                log_warning << "Lambda disabled, function: " << functionName;
                return;
            }

            // --- Step 1 & 2: find idle instance or start a new one, serialized per function ---
            Database::Entity::Lambda::Instance instance;
            {
                boost::mutex::scoped_lock lock(*GetOrCreateMutex(functionName));

                // Re-fetch after acquiring the lock to get the latest instance state
                lambda = _lambdaDatabase.GetLambdaByArn(lambdaArn);

                if (lambda.HasIdleInstance()) {
                    // Step 1 — use an existing idle container
                    instance = lambda.GetIdleInstance();
                    log_debug << "Idle instance found, function: " << functionName << ", instanceId: " << instance.instanceId;

                } else if (static_cast<int>(lambda.instances.size()) < lambda.concurrency) {
                    // Below maxConcurrency: start a new container
                    const std::string instanceId = Core::StringUtils::GenerateRandomHexString(8);
                    const LambdaCreator creator;
                    lambda = creator.AddInstance(lambda, instanceId);
                    instance = lambda.GetInstance(instanceId);
                    log_info << "New instance started, function: " << functionName << ", instanceId: " << instanceId << ", total: " << lambda.instances.size();

                } else {
                    // At maxConcurrency: wait for a slot to become idle
                    log_info << "Max concurrency reached, waiting for idle instance, function: " << functionName << ", concurrency: " << lambda.concurrency;
                    const auto deadline = system_clock::now() + std::chrono::seconds(lambda.timeout);
                    do {
                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                        lambda = _lambdaDatabase.GetLambdaByArn(lambdaArn);
                    } while (!lambda.HasIdleInstance() && system_clock::now() < deadline);

                    if (!lambda.HasIdleInstance()) {
                        log_error << "No idle instance available within timeout, function: " << functionName;
                        return;
                    }
                    instance = lambda.GetIdleInstance();
                    log_debug << "Idle instance available after wait, function: " << functionName << ", instanceId: " << instance.instanceId;
                }

                // Step 3 — claim the instance before releasing the lock
                _lambdaDatabase.SetInstanceValues(instance.containerId, Database::Entity::Lambda::InstanceRunning);
            }

            // Invoke outside the lock so other requests can claim their own instance in parallel
            const std::string &body = payload;
            const LambdaExecutor executor;
            const Database::Entity::Lambda::LambdaResult lambdaResult = executor.Invocation(lambda, instance, body);

            if (promise) {
                promise->set_value({static_cast<int>(lambdaResult.status), lambdaResult.responseBody});
            }

            Core::EventBus::instance().sigMetricRate(LAMBDA_INVOCATION_COUNT, "function_name", functionName);
            log_info << "Lambda invoked, function: " << functionName << ", status: " << lambdaResult.status;

        } catch (std::exception &e) {
            if (promise) {
                promise->set_value({500, e.what()});
            }
            log_error << "Lambda invoke failed, function: " << functionName << ", error: " << e.what();
        }
    }

    std::shared_ptr<boost::mutex> LambdaController::GetOrCreateMutex(const std::string &functionName) const {
        std::lock_guard guard(_mutexMapLock);
        auto &ptr = _instanceMutex[functionName];
        if (!ptr) {
            ptr = std::make_shared<boost::mutex>();
        }
        return ptr;
    }

    void LambdaController::OnCheckLambda(const std::string &functionArn) const {
        log_debug << "Lambda check requested, arn: " << functionArn;
        try {
            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(functionArn);

            bool updated = false;
            for (auto it = lambda.instances.begin(); it != lambda.instances.end();) {
                const Dto::Docker::InspectContainerResponse inspection = _containerService.InspectContainer(it->containerId);
                const bool running = inspection.state.running;
                log_debug << "Lambda instance container status, containerId: " << it->containerId << ", status: " << inspection.state.status << ", running: " << running;

                if (!running) {
                    log_info << "Lambda instance container not running, removing, containerId: " << it->containerId << ", function: " << lambda.function;
                    it = lambda.instances.erase(it);
                    updated = true;
                } else {
                    ++it;
                }
            }

            if (updated) {
                if (lambda.instances.empty()) {
                    lambda.state = Database::Entity::Lambda::Inactive;
                }
                _lambdaDatabase.UpdateLambda(lambda);
            }
            Core::EventBus::instance().sigMetricGauge(LAMBDA_INSTANCES_COUNT, "function_name", lambda.function, static_cast<double>(lambda.instances.size()));
        } catch (std::exception &e) {
            log_error << "Lambda check failed, arn: " << functionArn << ", error: " << e.what();
        }
    }

    // -------------------------------------------------------------------------
    // Scheduled health-check
    // -------------------------------------------------------------------------

    void LambdaController::CheckContainerHealth() const {
        log_trace << "Lambda controller health-check starting";
        try {
            for (Database::Entity::Lambda::LambdaList lambdas = _lambdaDatabase.ListLambdas(_region); auto &lambda: lambdas) {
                if (lambda.instances.empty()) {
                    continue;
                }

                bool updated = false;
                for (auto it = lambda.instances.begin(); it != lambda.instances.end();) {
                    try {
                        if (const Dto::Docker::InspectContainerResponse inspection = _containerService.InspectContainer(it->containerId); !inspection.state.running) {
                            log_info << "Health-check: container not running, removing, containerId: " << it->containerId << ", function: " << lambda.function;
                            it = lambda.instances.erase(it);
                            updated = true;
                        } else {
                            ++it;
                        }
                    } catch (std::exception &e) {
                        log_warning << "Health-check: could not inspect container: " << it->containerId << ", removing, error: " << e.what();
                        it = lambda.instances.erase(it);
                        updated = true;
                    }
                }

                if (updated) {
                    if (lambda.instances.empty()) {
                        lambda.state = Database::Entity::Lambda::Inactive;
                    }
                    _lambdaDatabase.UpdateLambda(lambda);
                    log_debug << "Health-check: lambda updated, function: " << lambda.function << ", remaining instances: " << lambda.instances.size();
                }
                Core::EventBus::instance().sigMetricGauge(LAMBDA_INSTANCES_COUNT, "function_name", lambda.function, static_cast<double>(lambda.instances.size()));
            }
        } catch (std::exception &e) {
            log_error << "Lambda controller health-check failed, error: " << e.what();
        }
        log_debug << "Lambda controller health-check finished";
    }

}// namespace Awsmock::Service
