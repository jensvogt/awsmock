//
// Created by vogje01 on 28/05/2026.
//

#include <awsmock/service/lambda/LambdaController.h>

namespace Awsmock::Service {

    LambdaController::LambdaController() : AbstractServer("lambda-controller"), _containerService(ContainerService::instance()) {

        const Core::Configuration &config = Core::Configuration::instance();
        _region = config.get<std::string>("awsmock.region");
        _healthCheckPeriod = config.getOr<int>("awsmock.modules.lambda.health-check-period", 30);
        _startupTimeout = config.getOr<int>("awsmock.modules.lambda.startup-timeout", 120);
        _invocationTimeout = config.getOr<int>("awsmock.modules.lambda.invocation-timeout", 60);
        log_debug << "Lambda controller health-check period: " << _healthCheckPeriod << "s, startup-timeout: " << _startupTimeout << "s, invocation-timeout: " << _invocationTimeout << "s";

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
        Core::Scheduler::instance().AddTask("lambda-controller-health", [this] { CheckContainerHealth(); }, _healthCheckPeriod, _healthCheckPeriod);

        log_debug << "Lambda controller initialized";
    }

    void LambdaController::shutdown() {
        Core::Scheduler::instance().Shutdown("lambda-controller-health");
        log_info << "Lambda controller stopped";
    }

    // -------------------------------------------------------------------------
    // Signal handlers
    // -------------------------------------------------------------------------

    void LambdaController::OnStartLambda(const std::string &lambdaArn, const std::string &region) const {
        log_info << "Lambda start requested, arn: " << lambdaArn;
        try {
            Dto::Lambda::StartLambdaRequest request;
            request.region = region;
            request.lambdaArn = lambdaArn;
            _lambdaService.StartLambda(request);
            log_info << "Lambda started, arn: " << lambdaArn;
        } catch (std::exception &e) {
            log_error << "Lambda start failed, arn: " << lambdaArn << ", error: " << e.what();
        }
    }

    void LambdaController::OnStopLambda(const std::string &lambdaArn, const std::string &region) const {
        log_info << "Lambda stop requested, arn: " << lambdaArn;
        try {
            Dto::Lambda::StopLambdaRequest request;
            request.region = region;
            request.lambdaArn = lambdaArn;
            _lambdaService.StopLambda(request);
            log_info << "Lambda stopped, arn: " << lambdaArn;
        } catch (std::exception &e) {
            log_error << "Lambda stop failed, arn: " << lambdaArn << ", error: " << e.what();
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

            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByArn(lambdaArn);
            if (!lambda.enabled) {
                log_warning << "Lambda disabled, function: " << functionName;
                if (promise) {
                    promise->set_value({403, "Lambda function is disabled"});
                }
                return;
            }

            // --- Step 1 & 2: find idle instance or start a new one, serialized per function ---
            Database::Entity::Lambda::Instance instance;
            {
                boost::mutex::scoped_lock lock(*GetOrCreateMutex(functionName));

                // Re-fetch after acquiring the lock to get the latest instance state
                lambda = _lambdaDatabase->getLambdaByArn(lambdaArn);

                if (lambda.HasIdleInstance()) {
                    // Step 1 — use an existing idle container
                    instance = lambda.GetIdleInstance();
                    log_debug << "Idle instance found, function: " << functionName << ", instanceId: " << instance.instanceId;

                } else if (static_cast<int>(lambda.instances.size()) < lambda.concurrency) {
                    // Below maxConcurrency: start a new container
                    const std::string instanceId = Core::AwsUtils::CreateLambdaInstanceId();
                    lambda = _lambdaService.AddInstance(lambda, instanceId);
                    log_info << "New instance started, function: " << functionName << ", instanceId: " << instanceId << ", total: " << lambda.instances.size();

                    // Wait until the Lambda Runtime inside the new container reports idle
                    const auto startDeadline = system_clock::now() + std::chrono::seconds(_startupTimeout);
                    do {
                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                        lambda = _lambdaDatabase->getLambdaByArn(lambdaArn);
                    } while (lambda.GetInstance(instanceId).status != Database::Entity::Lambda::RuntimeStatus::idle && system_clock::now() < startDeadline);

                    if (lambda.GetInstance(instanceId).status != Database::Entity::Lambda::RuntimeStatus::idle) {
                        log_error << "New instance did not become idle within timeout, function: " << functionName << ", instanceId: " << instanceId;
                        if (promise) {
                            promise->set_value({500, "Lambda instance did not start within timeout"});
                        }
                        return;
                    }
                    instance = lambda.GetInstance(instanceId);

                } else {
                    // At maxConcurrency: wait for a slot to become idle
                    log_info << "Max concurrency reached, waiting for idle instance, function: " << functionName << ", concurrency: " << lambda.concurrency;
                    const auto deadline = system_clock::now() + std::chrono::seconds(_invocationTimeout);
                    do {
                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                        lambda = _lambdaDatabase->getLambdaByArn(lambdaArn);
                    } while (!lambda.HasIdleInstance() && system_clock::now() < deadline);

                    if (!lambda.HasIdleInstance()) {
                        log_error << "No idle instance available within timeout, function: " << functionName;
                        if (promise) {
                            promise->set_value({503, "No idle Lambda instance available within timeout"});
                        }
                        return;
                    }
                    instance = lambda.GetIdleInstance();
                    log_debug << "Idle instance available after wait, function: " << functionName << ", instanceId: " << instance.instanceId;
                }

                // Set runtime status immediately, so no other invocation can use this instance
                instance.status = Database::Entity::Lambda::RuntimeStatus::running;
                if (const auto it = std::ranges::find_if(lambda.instances, [&](const Database::Entity::Lambda::Instance &i) { return i.instanceId == instance.instanceId; });
                    it != lambda.instances.end()) {
                    it->status = Database::Entity::Lambda::RuntimeStatus::running;
                }
                lambda = _lambdaDatabase->updateLambda(lambda);
            }

            // Invoke outside the lock so other requests can claim their own instance in parallel
            InvokeInstance(lambda, instance, payload, promise);

            // Reset instance to idle so it can accept further invocations
            {
                boost::mutex::scoped_lock lock(*GetOrCreateMutex(functionName));
                lambda = _lambdaDatabase->getLambdaByArn(lambdaArn);
                if (const auto it = std::ranges::find_if(lambda.instances, [&](const Database::Entity::Lambda::Instance &i) { return i.instanceId == instance.instanceId; });
                    it != lambda.instances.end()) {
                    it->status = Database::Entity::Lambda::RuntimeStatus::idle;
                }
                lambda = _lambdaDatabase->updateLambda(lambda);
                log_debug << "Instance reset to idle, function: " << lambda.function << ", instanceId: " << instance.instanceId;
            }

            Core::EventBus::instance().sigMetricRate(LAMBDA_INVOCATION_COUNT, "function_name", functionName);
            log_info << "Lambda invoked, function: " << functionName;

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
            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByArn(functionArn);

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
                lambda = _lambdaDatabase->updateLambda(lambda);
            }
            Core::EventBus::instance().sigMetricGauge(LAMBDA_INSTANCES_COUNT, "function_name", lambda.function, static_cast<double>(lambda.instances.size()));
        } catch (std::exception &e) {
            log_error << "Lambda check failed, arn: " << functionArn << ", error: " << e.what();
        }
    }

    // -------------------------------------------------------------------------
    // Instance invocation (moved from LambdaExecutor)
    // -------------------------------------------------------------------------

    void LambdaController::InvokeInstance(Database::Entity::Lambda::Lambda &lambda, Database::Entity::Lambda::Instance &instance, const std::string &payload,
                                          const std::shared_ptr<std::promise<std::pair<int, std::string>>> &promise) const {
        Monitoring::MonitoringTimer measure(LAMBDA_INVOCATION_TIMER, LAMBDA_INVOCATION_COUNT, "function_name", lambda.function);
        log_debug << "Sending lambda invocation request, function: " << lambda.function << " endpoint: " << instance.hostName << ":" << instance.publicPort;

        const system_clock::time_point start = system_clock::now();
        const Core::HttpSocketResponse response = Core::HttpSocket::SendJson(http::verb::post, instance.hostName, instance.publicPort, "/invoke", payload, {}, _invocationTimeout > 0 ? _invocationTimeout : 60);
        const long duration = std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now() - start).count();

        log_info << "Getting lambda logs, containerId: " << instance.containerId;
        std::string logs = _containerService.GetContainerLogs(instance.containerId, start);

        logs = Core::StringUtils::RemoveColorCoding(logs);
        logs = Core::StringUtils::SanitizeUtf8(logs);
        logs = Core::StringUtils::RemoveUnprintableAscii(logs);

        Database::Entity::Lambda::LambdaResult lambdaResult;
        lambdaResult.region = lambda.region;
        lambdaResult.instanceId = instance.instanceId;
        lambdaResult.containerId = instance.containerId;
        lambdaResult.status = response.statusCode;
        lambdaResult.httpStatusCode = Core::HttpUtils::StatusCodeToString(response.statusCode);
        lambdaResult.requestBody = payload;
        lambdaResult.responseBody = response.body;
        lambdaResult.logMessages = logs;
        lambdaResult.lambdaName = lambda.function;
        lambdaResult.lambdaArn = lambda.arn;
        lambdaResult.duration = duration;
        lambdaResult = _lambdaDatabase->createLambdaResult(lambdaResult);

        Core::EventBus::instance().sigMetricGauge(LAMBDA_RUNTIME_TIMER, "function_name", lambda.function, duration);

        if (promise) {
            promise->set_value({static_cast<int>(lambdaResult.status), lambdaResult.responseBody});
        }

        log_info << "Lambda invocation finished, lambda: " << lambda.function;
    }

    // -------------------------------------------------------------------------
    // Scheduled health-check
    // -------------------------------------------------------------------------

    void LambdaController::CheckContainerHealth() const {
        log_trace << "Lambda controller health-check starting";
        try {
            for (Database::Entity::Lambda::LambdaList lambdas = _lambdaDatabase->listLambdas(_region); auto &lambda: lambdas) {
                if (lambda.instances.empty()) {
                    continue;
                }

                bool updated = false;
                for (auto it = lambda.instances.begin(); it != lambda.instances.end();) {
                    try {
                        if (const Dto::Docker::InspectContainerResponse inspection = _containerService.InspectContainer(it->containerId);
                            inspection.status == http::status::ok && !inspection.state.running) {
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
                    // if (lambda.instances.empty()) {
                    //     lambda.state = Database::Entity::Lambda::Inactive;
                    // }
                    lambda = _lambdaDatabase->updateLambda(lambda);
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
