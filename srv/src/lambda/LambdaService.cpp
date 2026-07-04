//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/lambda/LambdaService.h>

// These are only needed for the Docker-lifecycle helpers moved from LambdaCreator
#include <awsmock/core/ZipUtils.h>
#include <awsmock/core/exception/ContainerException.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Lambda"};
}

namespace Awsmock::Service {

    Dto::Lambda::CreateFunctionResponse LambdaService::CreateFunction(Dto::Lambda::CreateFunctionRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "create_function");
        log_debug << "Create function request, name: " << request.functionName;

        const auto accountId = Core::Configuration::instance().get<std::string>("awsmock.access.account-id");

        // Create an entity and set ARN
        Database::Entity::Lambda::Lambda lambda = {};
        lambda.arn = Core::AwsUtils::CreateLambdaArn(request.region, accountId, request.functionName);
        lambda.function = request.functionName;
        lambda.runtime = request.runtime;
        lambda.handler = request.handler;

        // Write the base64 file
        lambda.code.zipFile = request.functionName + "-" + request.version + ".b64";
        WriteBase64File(lambda.code.zipFile, request.code.zipFile);

        // Update database
        lambda.timeout = request.timeout;
        // lambda.state = Database::Entity::Lambda::LambdaState::Pending;
        // lambda.stateReason = "Initializing";
        // lambda.stateReasonCode = Database::Entity::Lambda::LambdaStateReasonCode::Creating;
        lambda = _lambdaDatabase->createOrUpdateLambda(lambda);

        // Create instance
        Database::Entity::Lambda::Instance instance;
        std::string instanceId = Core::AwsUtils::CreateLambdaInstanceId();

        // Create lambda
        lambda = CreateLambdaInstance(lambda, instanceId);
        log_info << "New lambda instance lastStart, name: " << lambda.function << ", instanceId: " << instanceId << ", status: " << Database::Entity::Lambda::RuntimeStatusToString(instance.status) << ", totalSize: " << lambda.instances.size();

        return Dto::Lambda::Mapper::map(request, lambda);
    }

    void LambdaService::UploadFunctionCode(const Dto::Lambda::UploadFunctionCodeRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "upload_function_code");
        log_debug << "Upload function code request, arn: " << request.functionArn;

        if (!_lambdaDatabase->lambdaExistsByArn(request.functionArn)) {
            log_warning << "Lambda function does not exist, arn: " << request.functionArn;
            throw Core::ServiceException("Lambda function does not exist, arn: " + request.functionArn);
        }

        if (request.functionCode.empty()) {
            log_warning << "Lambda function code is empty, arn: " << request.functionArn;
            throw Core::ServiceException("Lambda function code is empty, arn: " + request.functionArn);
        }

        // Get lambda function
        Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByArn(request.functionArn);

        // Stop and delete all containers/images
        CleanupDocker(lambda);

        // Write the base64 file
        lambda.code.zipFile = lambda.function + "-" + request.version + ".b64";
        WriteBase64File(lambda.code.zipFile, request.functionCode);

        // Update lambda attributes
        // lambda.status = Database::Entity::Lambda::LambdaState::Pending;
        lambda.invocations = 0;
        lambda.avgDuration = 0;
        lambda.dockerTag = request.version;
        lambda.tags["version"] = request.version;
        lambda.tags["dockerTag"] = request.version;
        // lambda.stateReason = "Initializing";
        // lambda.stateReasonCode = Database::Entity::Lambda::LambdaStateReasonCode::Creating;
        lambda = _lambdaDatabase->updateLambda(lambda);

        // Clear lambda results
        const long count = _lambdaDatabase->deleteResultsCounters(lambda.arn);
        log_debug << "Lambda results cleared, arn: " << lambda.arn << " count: " << count;

        // Create instance
        const std::string instanceId = Core::AwsUtils::CreateLambdaInstanceId();

        // Update the lambda function
        CreateLambdaInstance(lambda, instanceId);

        // Update init file
        sigLambdaCodeUpdated(lambda.function);
        log_debug << "Lambda function code updated, function: " << lambda.function;
    }

    Dto::Lambda::ListFunctionResponse LambdaService::ListFunctions(const std::string &region) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "list_functions");
        log_debug << "List functions request, region: " << region;

        try {
            const std::vector<Database::Entity::Lambda::Lambda> lambdas = _lambdaDatabase->listLambdas(region);

            Dto::Lambda::ListFunctionResponse response;
            for (const auto &lambda: lambdas) {
                response.functions.emplace_back(Dto::Lambda::Mapper::mapFunction(lambda));
            }
            log_debug << "Lambda list outcome: " << response.ToJson();
            return response;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Lambda::ListFunctionCountersResponse LambdaService::ListFunctionCounters(const Dto::Lambda::ListFunctionCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "list_function_counters");
        log_debug << "List function counters request, region: " << request.region;

        try {
            const std::vector<Database::Entity::Lambda::Lambda> lambdas = _lambdaDatabase->listLambdaCounters(request.region, request.prefix, request.pageSize, request.pageIndex, Dto::Common::SortColumnMapper::map(request.sortColumns));
            const long count = _lambdaDatabase->lambdaCount(request.region);

            Dto::Lambda::ListFunctionCountersResponse response = Dto::Lambda::Mapper::map(lambdas);
            response.total = count;

            log_trace << "Lambda list function counters, result: " << response.ToString();
            return response;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Lambda::ListLambdaEnvironmentCountersResponse LambdaService::ListLambdaEnvironmentCounters(const Dto::Lambda::ListLambdaEnvironmentCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "list_environment_counters");
        log_debug << "List lambda environment counters request, lambdaArn: " << request.lambdaArn;

        try {
            const Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByArn(request.lambdaArn);

            Dto::Lambda::ListLambdaEnvironmentCountersResponse response;
            response.total = static_cast<long>(lambda.environment.variables.size());

            std::vector<std::pair<std::string, std::string>> environments;
            for (const auto &[fst, snd]: lambda.environment.variables) {
                environments.emplace_back(fst, snd);
            }

            // Sorting
            if (request.sortColumns.at(0).column == "key") {
                std::ranges::sort(environments, [request](const std::pair<std::string, std::string> &a, const std::pair<std::string, std::string> &b) {
                    if (request.sortColumns.at(0).sortDirection == -1) {
                        return a.first <= b.first;
                    }
                    return a.first > b.first;
                });
            } else if (request.sortColumns.at(0).column == "value") {
                std::ranges::sort(environments, [request](const std::pair<std::string, std::string> &a, const std::pair<std::string, std::string> &b) {
                    if (request.sortColumns.at(0).sortDirection == -1) {
                        return a.second <= b.second;
                    }
                    return a.second > b.second;
                });
            }

            // Paging
            if (request.pageSize > 0) {
                auto endArray = environments.begin() + request.pageSize * (request.pageIndex + 1);
                if (request.pageSize * (request.pageIndex + 1) > environments.size()) {
                    endArray = environments.end();
                }
                response.environmentCounters = std::vector(environments.begin() + request.pageIndex * request.pageSize, endArray);
            } else {
                response.environmentCounters = environments;
            }
            log_trace << "Lambda list environments counters, result: " << response.ToString();
            return response;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Lambda::ListLambdaEventSourceCountersResponse LambdaService::ListLambdaEventSourceCounters(const Dto::Lambda::ListLambdaEventSourceCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "list_event_source_counters");
        log_debug << "List lambda event source counters request, lambdaArn: " << request.lambdaArn;

        try {
            const Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByArn(request.lambdaArn);

            Dto::Lambda::ListLambdaEventSourceCountersResponse response;
            response.total = static_cast<long>(lambda.eventSources.size());

            // Map to DTO
            std::vector<Dto::Lambda::EventSourceMapping> eventSources = Dto::Lambda::Mapper::mapCounters(lambda.arn, lambda.eventSources);

            // Sorting
            if (request.sortColumns.at(0).column == "eventSourceArn") {
                std::ranges::sort(eventSources, [request](const Dto::Lambda::EventSourceMapping &a, const Dto::Lambda::EventSourceMapping &b) {
                    if (request.sortColumns.at(0).sortDirection == -1) {
                        return a.eventSourceArn <= b.eventSourceArn;
                    }
                    return a.eventSourceArn > b.eventSourceArn;
                });
            } else if (request.sortColumns.at(0).column == "uuid") {
                std::ranges::sort(eventSources, [request](const Dto::Lambda::EventSourceMapping &a, const Dto::Lambda::EventSourceMapping &b) {
                    if (request.sortColumns.at(0).sortDirection == -1) {
                        return a.uuid <= b.uuid;
                    }
                    return a.uuid > b.uuid;
                });
            }

            // Paging
            auto endArray = eventSources.begin() + request.pageSize * (request.pageIndex + 1);
            if (request.pageSize * (request.pageIndex + 1) > eventSources.size()) {
                endArray = eventSources.end();
            }
            response.eventSourceCounters = std::vector(eventSources.begin() + request.pageIndex * request.pageSize, endArray);

            log_trace << "Lambda list event source counters, result: " << response.ToString();
            return response;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void LambdaService::AddLambdaEnvironment(const Dto::Lambda::AddEnvironmentRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "add_lambda_environment");
        log_debug << "List lambda environment counters request, lambdaArn: " << request.functionArn;

        if (!_lambdaDatabase->lambdaExistsByArn(request.functionArn)) {
            log_warning << "Lambda function does not exist, arn: " << request.functionArn;
            throw Core::ServiceException("Lambda function does not exist, arn: " + request.functionArn);
        }
        try {
            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByArn(request.functionArn);
            lambda.environment.variables[request.environmentKey] = request.environmentValue;
            lambda = _lambdaDatabase->updateLambda(lambda);
            log_trace << "Lambda environments added, lambdaArn: " << lambda.arn;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void LambdaService::UpdateLambdaEnvironment(const Dto::Lambda::UpdateFunctionEnvironmentRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "update_lambda_environment");
        log_debug << "Update lambda environment request, lambdaArn: " << request.functionArn << ", key: " << request.environmentKey << ", value: " << request.environmentValue;

        if (!_lambdaDatabase->lambdaExistsByArn(request.functionArn)) {
            log_warning << "Lambda function does not exist, arn: " << request.functionArn;
            throw Core::ServiceException("Lambda function does not exist, arn: " + request.functionArn);
        }
        try {
            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByArn(request.functionArn);
            lambda.environment.variables[request.environmentKey] = request.environmentValue;
            lambda = _lambdaDatabase->updateLambda(lambda);
            log_trace << "Lambda environments updated, lambdaArn: " << lambda.arn;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void LambdaService::DeleteLambdaEnvironment(const Dto::Lambda::DeleteEnvironmentRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "delete_lambda_environment");
        log_debug << "Delete lambda environment request, lambdaArn: " << request.functionArn << ", key: " << request.environmentKey;

        if (!_lambdaDatabase->lambdaExistsByArn(request.functionArn)) {
            log_warning << "Lambda function does not exist, arn: " << request.functionArn;
            throw Core::ServiceException("Lambda function does not exist, arn: " + request.functionArn);
        }

        try {
            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByArn(request.functionArn);
            std::string environmentKey = request.environmentKey;
            const auto count = std::erase_if(lambda.environment.variables, [environmentKey](const auto &item) {
                auto const &[key, value] = item;
                return key == environmentKey;
            });
            lambda = _lambdaDatabase->updateLambda(lambda);
            log_trace << "Lambda environments deleted, lambdaArn: " << lambda.arn << " count: " << count;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void LambdaService::UpdateLambda(const Dto::Lambda::UpdateLambdaRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "update_lambda");
        log_debug << "Add lambda event source counters request, lambdaArn: " << request.lambdaArn;

        if (!_lambdaDatabase->lambdaExistsByArn(request.lambdaArn)) {
            log_warning << "Lambda function does not exist, arn: " << request.lambdaArn;
            throw Core::ServiceException("Lambda function does not exist, arn: " + request.lambdaArn);
        }

        try {
            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByArn(request.lambdaArn);
            lambda.enabled = request.enabled;
            lambda = _lambdaDatabase->updateLambda(lambda);
            log_trace << "Lambda updated, lambdaArn: " << lambda.arn;

            if (!request.enabled) {
                Dto::Lambda::StopLambdaRequest stopRequest;
                stopRequest.lambdaArn = request.lambdaArn;
                stopRequest.region = request.region;
                StopLambda(stopRequest);
            } else if (lambda.instances.empty()) {
                Dto::Lambda::StartLambdaRequest startRequest;
                startRequest.lambdaArn = request.lambdaArn;
                startRequest.region = request.region;
                StartLambda(startRequest);
            }
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Lambda::UpdateFunctionCodeResponse LambdaService::UpdateFunctionCode(const Dto::Lambda::UpdateFunctionCodeRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "update_lambda");
        log_debug << "Update lambda function code request, functionName: " << request.functionName;

        std::string lambdaArn = Core::AwsUtils::ConvertLambdaNameToArn(request.region, Core::AwsUtils::GetDefaultAccountId(), request.functionName);

        std::vector<Database::Entity::Lambda::Lambda> lambdas = _lambdaDatabase->listLambdas({});

        if (!_lambdaDatabase->lambdaExistsByArn(lambdaArn)) {
            log_warning << "Lambda function does not exist, arn: " << lambdaArn;
            throw Core::ServiceException("Lambda function does not exist, arn: " + lambdaArn);
        }

        try {
            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByArn(lambdaArn);
            lambda.enabled = true;
            lambda.modified = system_clock::now();
            lambda = _lambdaDatabase->updateLambda(lambda);
            log_trace << "Lambda updated, lambdaArn: " << lambda.arn;

            // Write the base64 file
            lambda.code.zipFile = request.functionName + "-" + request.revisionId + ".b64";
            WriteBase64File(lambda.code.zipFile, request.zipFile);

            // Find idle instance
            Database::Entity::Lambda::Instance instance;
            std::string instanceId = Core::AwsUtils::CreateLambdaInstanceId();
            log_debug << "Created lambda instance, instanceId: " << instanceId;

            // Create lambda
            lambda = CreateLambdaInstance(lambda, instanceId);

            // Start lambda
            Dto::Lambda::StartLambdaRequest startRequest;
            startRequest.lambdaArn = lambdaArn;
            startRequest.region = request.region;
            StartLambda(startRequest);
            log_debug << "Lambda started, containerName: " << request.functionName + "-" + instanceId;

            // Create response object
            Dto::Lambda::UpdateFunctionCodeResponse response{};
            response.region = lambda.region;
            response.functionName = lambda.function;
            response.functionArn = lambda.arn;
            response.handler = lambda.handler;
            return response;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void LambdaService::AddEventSource(const Dto::Lambda::AddEventSourceRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "add_event_source");
        log_debug << "Add lambda event source counters request, lambdaArn: " << request.functionArn;

        if (!_lambdaDatabase->lambdaExistsByArn(request.functionArn)) {
            log_warning << "Lambda function does not exist, arn: " << request.functionArn;
            throw Core::ServiceException("Lambda function does not exist, arn: " + request.functionArn);
        }

        try {
            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByArn(request.functionArn);
            Database::Entity::Lambda::EventSourceMapping eventSourceMapping;
            eventSourceMapping.type = request.type;
            eventSourceMapping.eventSourceArn = request.eventSourceArn;
            eventSourceMapping.batchSize = request.batchSize;
            eventSourceMapping.maximumBatchingWindowInSeconds = request.maximumBatchingWindowInSeconds;
            eventSourceMapping.enabled = request.enabled;
            eventSourceMapping.uuid = request.uuid.empty() ? Core::StringUtils::CreateRandomUuid() : request.uuid;

            // If type is SQS, create SQS notification configuration
            CreateResourceNotification(request);

            lambda.eventSources.emplace_back(eventSourceMapping);
            lambda = _lambdaDatabase->updateLambda(lambda);
            log_trace << "Lambda event sources added, lambdaArn: " << lambda.arn;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void LambdaService::DeleteEventSource(const Dto::Lambda::DeleteEventSourceRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "delete_event_source");
        log_debug << "Delete lambda event source request, lambdaArn: " << request.functionArn << ", eventSourceArn: " << request.eventSourceArn;

        if (!_lambdaDatabase->lambdaExistsByArn(request.functionArn)) {
            log_warning << "Lambda function does not exist, arn: " << request.functionArn;
            throw Core::ServiceException("Lambda function does not exist, arn: " + request.functionArn);
        }

        try {
            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByArn(request.functionArn);
            std::string eventSourceArn = request.eventSourceArn;
            const auto count = std::erase_if(lambda.eventSources, [eventSourceArn](const auto &item) {
                return item.eventSourceArn == eventSourceArn;
            });
            lambda = _lambdaDatabase->updateLambda(lambda);
            log_trace << "Lambda event sources deleted, lambdaArn: " << lambda.arn << " count: " << count;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Lambda::ListLambdaTagCountersResponse LambdaService::ListLambdaTagCounters(const Dto::Lambda::ListLambdaTagCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "list_tag_counters");
        log_debug << "List lambda tag counters request, lambdaArn: " << request.lambdaArn;

        try {
            const Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByArn(request.lambdaArn);

            Dto::Lambda::ListLambdaTagCountersResponse response;
            response.total = static_cast<long>(lambda.tags.size());

            std::vector<std::pair<std::string, std::string>> tags;
            for (const auto &[fst, snd]: lambda.tags) {
                tags.emplace_back(fst, snd);
            }

            // Sorting
            if (request.sortColumns.at(0).column == "key") {
                std::ranges::sort(tags, [request](const std::pair<std::string, std::string> &a, const std::pair<std::string, std::string> &b) {
                    if (request.sortColumns.at(0).sortDirection == -1) {
                        return a.first <= b.first;
                    }
                    return a.first > b.first;
                });
            } else if (request.sortColumns.at(0).column == "value") {
                std::ranges::sort(tags, [request](const std::pair<std::string, std::string> &a, const std::pair<std::string, std::string> &b) {
                    if (request.sortColumns.at(0).sortDirection == -1) {
                        return a.second <= b.second;
                    }
                    return a.second > b.second;
                });
            }

            // Paging
            auto endArray = tags.begin() + request.pageSize * (request.pageIndex + 1);
            if (request.pageSize * (request.pageIndex + 1) > tags.size()) {
                endArray = tags.end();
            }
            response.tagCounters = std::vector(tags.begin() + request.pageIndex * request.pageSize, endArray);

            log_trace << "Lambda list tags counters, result: " << response.ToString();
            return response;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Lambda::ListLambdaInstanceCountersResponse LambdaService::ListLambdaInstanceCounters(const Dto::Lambda::ListLambdaInstanceCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "list_instance_counters");
        log_debug << "List lambda instance counters request, lambdaArn: " << request.lambdaArn;

        try {
            const Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByArn(request.lambdaArn);

            Dto::Lambda::ListLambdaInstanceCountersResponse response;
            response.total = static_cast<long>(lambda.instances.size());

            for (const auto &instance: lambda.instances) {
                Dto::Lambda::InstanceCounter instanceCounter;
                instanceCounter.instanceId = instance.instanceId;
                instanceCounter.containerId = instance.containerId;
                instanceCounter.status = Database::Entity::Lambda::RuntimeStatusToString(instance.status);
                instanceCounter.hostname = instance.hostName;
                instanceCounter.publicPort = instance.publicPort;
                instanceCounter.privatePort = instance.privatePort;
                instanceCounter.lastInvocation = instance.lastInvocation;
                response.instanceCounters.emplace_back(instanceCounter);
            }

            // Sorting
            if (!request.sortColumns.empty()) {
                if (request.sortColumns.at(0).column == "instanceId") {
                    std::ranges::sort(response.instanceCounters, [request](const Dto::Lambda::InstanceCounter &a, const Dto::Lambda::InstanceCounter &b) {
                        if (request.sortColumns.at(0).sortDirection == -1) {
                            return a.instanceId <= b.instanceId;
                        }
                        return a.instanceId > b.instanceId;
                    });
                } else if (request.sortColumns.at(0).column == "containerId") {
                    std::ranges::sort(response.instanceCounters, [request](const Dto::Lambda::InstanceCounter &a, const Dto::Lambda::InstanceCounter &b) {
                        if (request.sortColumns.at(0).sortDirection == -1) {
                            return a.containerId <= b.containerId;
                        }
                        return a.containerId > b.containerId;
                    });
                } else if (request.sortColumns.at(0).column == "status") {
                    std::ranges::sort(response.instanceCounters, [request](const Dto::Lambda::InstanceCounter &a, const Dto::Lambda::InstanceCounter &b) {
                        if (request.sortColumns.at(0).sortDirection == -1) {
                            return a.status <= b.status;
                        }
                        return a.status > b.status;
                    });
                }
            }

            // Paging
            if (request.pageSize > 0) {
                auto endArray = response.instanceCounters.begin() + request.pageSize * (request.pageIndex + 1);
                if (request.pageSize * (request.pageIndex + 1) > response.instanceCounters.size()) {
                    endArray = response.instanceCounters.end();
                }
                response.instanceCounters = std::vector(response.instanceCounters.begin() + request.pageIndex * request.pageSize, endArray);
            }

            log_trace << "Lambda list instances counters, result: " << response.ToString();
            return response;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Lambda::GetLambdaInstanceCounterResponse LambdaService::GetLambdaInstanceCounter(const Dto::Lambda::GetLambdaInstanceCounterRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "get_instance_counter");
        log_debug << "Get lambda instance counter request, lambdaArn: " << request.lambdaArn << ", instanceId: " << request.instanceId;

        try {
            const Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByArn(request.lambdaArn);

            const auto it = std::ranges::find_if(lambda.instances, [&request](const Database::Entity::Lambda::Instance &i) {
                return i.instanceId == request.instanceId;
            });

            if (it == lambda.instances.end()) {
                log_warning << "Lambda instance not found, lambdaArn: " << request.lambdaArn << ", instanceId: " << request.instanceId;
                throw Core::NotFoundException("Lambda instance not found, instanceId: " + request.instanceId);
            }

            Dto::Lambda::GetLambdaInstanceCounterResponse response;
            response.instanceCounter.lambdaName = lambda.function;
            response.instanceCounter.instanceId = it->instanceId;
            response.instanceCounter.containerId = it->containerId;
            response.instanceCounter.status = Database::Entity::Lambda::RuntimeStatusToString(it->status);
            response.instanceCounter.hostname = it->hostName;
            response.instanceCounter.publicPort = it->publicPort;
            response.instanceCounter.privatePort = it->privatePort;
            response.instanceCounter.lastStart = it->lastStart;
            response.instanceCounter.lastInvocation = it->lastInvocation;
            response.instanceCounter.lastStop = it->lastStop;

            log_trace << "Lambda get instance counter, result: " << response.ToString();
            return response;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void LambdaService::AddLambdaTag(const Dto::Lambda::AddTagRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "add_lambda_tag");
        log_debug << "List lambda tag counters request, lambdaArn: " << request.functionArn;

        try {
            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByArn(request.functionArn);
            lambda.tags[request.tagKey] = request.tagValue;
            lambda = _lambdaDatabase->updateLambda(lambda);
            log_trace << "Lambda tags added, lambdaArn: " << lambda.arn;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void LambdaService::UpdateLambdaTag(const Dto::Lambda::UpdateFunctionTagRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "update_lambda_tag");
        log_debug << "Update lambda tag request, lambdaArn: " << request.functionArn << ", key: " << request.tagKey << ", value: " << request.tagValue;

        if (!_lambdaDatabase->lambdaExistsByArn(request.functionArn)) {
            log_warning << "Lambda function does not exist, arn: " << request.functionArn;
            throw Core::ServiceException("Lambda function does not exist, arn: " + request.functionArn);
        }

        try {
            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByArn(request.functionArn);
            lambda.tags[request.tagKey] = request.tagValue;
            lambda = _lambdaDatabase->updateLambda(lambda);
            log_trace << "Lambda tags updated, lambdaArn: " << lambda.arn;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void LambdaService::DeleteLambdaTag(const Dto::Lambda::DeleteTagRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "delete_lambda_tag");
        log_debug << "Delete lambda tag request, lambdaArn: " << request.functionArn << ", key: " << request.tagKey;

        try {
            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByArn(request.functionArn);
            std::string tagKey = request.tagKey;
            const auto count = std::erase_if(lambda.tags, [tagKey](const auto &item) {
                auto const &[key, value] = item;
                return key == tagKey;
            });
            lambda = _lambdaDatabase->updateLambda(lambda);
            log_trace << "Lambda tags deleted, lambdaArn: " << lambda.arn << " count: " << count;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Lambda::GetFunctionResponse LambdaService::GetFunction(const std::string &region, const std::string &name) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "get_function");
        log_debug << "Get function request, region: " << region << " name: " << name;

        try {
            const Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByName(region, name);

            Dto::Lambda::Function function;
            function.functionName = lambda.function,
            function.handler = lambda.handler,
            function.runtime = lambda.runtime,
            function.lastUpdateStatus = "Successful";
            // function.state = LambdaStateToString(lambda.state),
            // function.stateReason = lambda.stateReason,
            // function.stateReasonCode = LambdaStateReasonCodeToString(lambda.stateReasonCode);
            // function.stateReasonCode = LambdaStateReasonCodeToString(lambda.stateReasonCode);

            Dto::Lambda::GetFunctionResponse response;
            response.region = lambda.region;
            response.configuration = function;
            response.tags = lambda.tags;

            log_info << "Lambda function: " + response.ToJson();
            return response;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Lambda::GetFunctionCountersResponse LambdaService::GetFunctionCounters(const Dto::Lambda::GetFunctionCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "get_function_counters");
        log_debug << "Get function request, lambdaArn: " << request.lambdaArn;

        try {
            const Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByArn(request.lambdaArn);

            Dto::Lambda::GetFunctionCountersResponse response;
            response.id = lambda.oid;
            response.functionArn = lambda.arn;
            response.region = lambda.region;
            response.functionName = lambda.function;
            response.handler = lambda.handler;
            response.runtime = lambda.runtime;
            response.user = lambda.user;
            response.role = lambda.role;
            response.size = lambda.codeSize;
            response.zipFile = lambda.code.zipFile;
            response.s3Bucket = lambda.code.s3Bucket;
            response.s3Key = lambda.code.s3Key;
            response.s3ObjectVersion = lambda.code.s3ObjectVersion;
            response.concurrency = lambda.concurrency;
            response.instances = static_cast<long>(lambda.instances.size());
            response.invocations = lambda.invocations;
            response.avgDuration = lambda.avgDuration;
            response.lifetime = lambda.timeout;
            response.enabled = lambda.enabled;
            response.state = lambda.instances.empty() ? "Inactive" : "Active";
            response.lastStarted = lambda.lastStarted;
            response.lastInvocation = lambda.lastInvocation;
            response.created = lambda.created;
            response.modified = lambda.modified;
            response.tags = lambda.tags;
            response.version = lambda.dockerTag;
            response.environment = lambda.environment.variables;
            return response;
        } catch (bsoncxx::exception &ex) {
            log_error << "Lambda list request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    void LambdaService::ResetFunctionCounters(const Dto::Lambda::ResetFunctionCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "reset_function_counters");
        log_debug << "Reset function counters request, region: " << request.region << " name: " << request.functionName;

        try {
            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByName(request.region, request.functionName);
            lambda.avgDuration = 0;
            lambda.invocations = 0;
            lambda = _lambdaDatabase->updateLambda(lambda);
            log_info << "Reset lambda function counters, function: " << lambda.function;
        } catch (bsoncxx::exception &ex) {
            log_error << "Reset function counters request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::Lambda::LambdaResult LambdaService::InvokeLambdaFunction(const std::string &region, const std::string &functionName, const std::string &payload, const Dto::Lambda::LambdaInvocationType &invocationType) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "invoke_lambda_function");
        log_debug << "Invocation lambda function, functionName: " << functionName;

        // REQUEST_RESPONSE
        if (invocationType == Dto::Lambda::LambdaInvocationType::REQUEST_RESPONSE) {
            const auto promise = std::make_shared<std::promise<std::pair<int, std::string>>>();
            auto future = promise->get_future();
            Core::EventBus::instance().sigLambdaInvoke(region, functionName, payload, Dto::Lambda::LambdaInvocationTypeToString(invocationType), promise);
            const auto [status, body] = future.get();
            Dto::Lambda::LambdaResult result;
            result.status = status;
            result.responseBody = body;
            return result;
        }

        // EVENT: fire-and-forget
        Core::EventBus::instance().sigLambdaInvoke(region, functionName, payload, Dto::Lambda::LambdaInvocationTypeToString(invocationType), nullptr);
        return {};
    }

    void LambdaService::CreateTag(const Dto::Lambda::CreateTagRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "create_tag");
        log_debug << "Create tag request, arn: " << request.arn;

        if (!_lambdaDatabase->lambdaExistsByArn(request.arn)) {
            log_warning << "Lambda function does not exist, arn: " << request.arn;
            throw Core::ServiceException("Lambda function does not exist");
        }

        // Get the existing entity
        Database::Entity::Lambda::Lambda lambdaEntity = _lambdaDatabase->getLambdaByArn(request.arn);
        for (const auto &[fst, snd]: request.tags) {
            lambdaEntity.tags.emplace(fst, snd);
        }
        lambdaEntity = _lambdaDatabase->updateLambda(lambdaEntity);
        log_debug << "Create tag request succeeded, arn: " + request.arn << " size: " << lambdaEntity.tags.size();
    }

    Dto::Lambda::ListTagsResponse LambdaService::ListTags(const std::string &arn) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "list_tags");
        log_debug << "List tags request, arn: " << arn;

        if (!_lambdaDatabase->lambdaExistsByArn(arn)) {
            log_warning << "Lambda function does not exist, arn: " << arn;
            throw Core::ServiceException("Lambda function does not exist");
        }

        // Get the existing entity
        Dto::Lambda::ListTagsResponse response;
        Database::Entity::Lambda::Lambda lambdaEntity = _lambdaDatabase->getLambdaByArn(arn);
        for (const auto &[fst, snd]: lambdaEntity.tags) {
            response.tags.emplace(fst, snd);
        }
        log_debug << "List tag request succeeded, arn: " + arn << " size: " << lambdaEntity.tags.size();

        return response;
    }

    Dto::Lambda::AccountSettingsResponse LambdaService::GetAccountSettings() const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "get_account_settings");
        log_debug << "Get account settings";

        Dto::Lambda::AccountSettingsResponse response;

        // 50 MB
        response.accountLimit.codeSizeUnzipped = 50 * 1024 * 1024L;
        response.accountLimit.codeSizeZipped = 50 * 1024 * 1024L;

        // 1000 concurrent executions (that are irrelevant in AwsMock environment)
        response.accountLimit.concurrentExecutions = 1000;

        // 75 GB
        response.accountLimit.totalCodeSize = 75 * 1024 * 1024 * 1024L;
        log_debug << "List account limits: " << response.ToJson();

        // 75 GB
        response.accountUsage.totalCodeSize = 10 * 1024 * 1024L;
        response.accountUsage.functionCount = _lambdaDatabase->lambdaCount({});

        return response;
    }

    Dto::Lambda::CreateEventSourceMappingsResponse LambdaService::CreateEventSourceMappings(const Dto::Lambda::CreateEventSourceMappingsRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "create_event_source_mapping");
        log_debug << "Create event source mapping, arn: " << request.functionName << " sourceArn: " << request.eventSourceArn;

        if (!_lambdaDatabase->lambdaExists(request.functionName)) {
            log_warning << "Lambda function does not exist, function: " << request.functionName;
            throw Core::NotFoundException("Lambda function does not exist");
        }

        // Get the existing entity
        Database::Entity::Lambda::Lambda lambdaEntity = _lambdaDatabase->getLambdaByName(request.region, request.functionName);

        // Map request to entity
        Database::Entity::Lambda::EventSourceMapping eventSourceMapping = Dto::Lambda::Mapper::map(request);
        eventSourceMapping.uuid = Core::StringUtils::CreateRandomUuid();

        // Check existence
        if (lambdaEntity.HasEventSource(request.eventSourceArn)) {
            log_warning << "Event source exists already, function: " << request.functionName << " sourceArn: " << request.eventSourceArn;
            throw Core::BadRequestException("Event source exists already");
        }

        // Update database
        lambdaEntity.eventSources.emplace_back(eventSourceMapping);
        lambdaEntity = _lambdaDatabase->updateLambda(lambdaEntity);
        log_debug << "Lambda function updated, function: " << lambdaEntity.function;

        // Create a response (which is actually the request)
        Dto::Lambda::CreateEventSourceMappingsResponse response;
        response.functionName = request.functionName;
        response.eventSourceArn = request.eventSourceArn;
        response.batchSize = request.batchSize;
        response.maximumBatchingWindowInSeconds = request.maximumBatchingWindowInSeconds;
        response.enabled = request.enabled;
        response.uuid = eventSourceMapping.uuid;

        log_trace << "Response: " << response.ToJson();
        log_debug << "Event source mapping lastStart, function: " << response.functionName << " sourceArn: " << response.eventSourceArn;
        return response;
    }

    Dto::Lambda::ListEventSourceMappingsResponse LambdaService::ListEventSourceMappings(const Dto::Lambda::ListEventSourceMappingsRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "list_event_source_mapping");
        log_debug << "List event source mappings, functionName: " << request.functionName << " sourceArn: " << request.eventSourceArn;

        if (!_lambdaDatabase->lambdaExists(request.functionName)) {
            log_warning << "Lambda function does not exist, function: " << request.functionName;
            throw Core::NotFoundException("Lambda function does not exist");
        }

        // Get the existing entity
        const Database::Entity::Lambda::Lambda lambdaEntity = _lambdaDatabase->getLambdaByName(request.region, request.functionName);

        return Dto::Lambda::Mapper::map(lambdaEntity.eventSources);
    }

    Dto::Lambda::ListLambdaArnsResponse LambdaService::ListLambdaArns() const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "list_lambda_arns");
        log_trace << "List all queues ARNs request";

        try {
            const std::vector<Database::Entity::Lambda::Lambda> lambdaList = _lambdaDatabase->listLambdas({});
            Dto::Lambda::ListLambdaArnsResponse listLambdaArnsResponse;
            for (const auto &lambda: lambdaList) {
                listLambdaArnsResponse.lambdaArns.emplace_back(lambda.arn);
            }
            log_trace << "Lambda create function ARN list response: " << listLambdaArnsResponse.ToJson();
            return listLambdaArnsResponse;
        } catch (Core::DatabaseException &exc) {
            log_error << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::Lambda::GetLambdaResultCounterResponse LambdaService::GetLambdaResultCounter(const Dto::Lambda::GetLambdaResultCounterRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "get_lambda_result");
        log_debug << "Get lambda result counter request, region: " << request.region << ", oid: " << request.oid;

        if (!_lambdaDatabase->lambdaResultExists(request.oid)) {
            log_warning << "Lambda function result does not exist, oid: " << request.oid;
            throw Core::NotFoundException("Lambda function result does not exist, oid: " + request.oid);
        }

        Database::Entity::Lambda::LambdaResult lambdaResult = _lambdaDatabase->getLambdaResultCounter(request.oid);
        log_trace << "Lambda result found, lambdaResult: " << lambdaResult;

        return Dto::Lambda::Mapper::map(lambdaResult);
    }

    Dto::Lambda::ListLambdaResultCountersResponse LambdaService::ListLambdaResultCounters(const Dto::Lambda::ListLambdaResultCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "list_lambda_results");
        log_debug << "List function counters request, region: " << request.region;

        try {
            const std::vector<Database::Entity::Lambda::LambdaResult> lambdaResults = _lambdaDatabase->listLambdaResultCounters(request.lambdaArn, request.prefix, request.pageSize, request.pageIndex, Dto::Common::SortColumnMapper::map(request.sortColumns));
            const long count = _lambdaDatabase->lambdaResultsCount(request.lambdaArn);

            Dto::Lambda::ListLambdaResultCountersResponse response = Dto::Lambda::Mapper::map(lambdaResults);
            response.total = count;

            log_trace << "Lambda list function counters, result: " << response.ToString();
            return response;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    long LambdaService::DeleteLambdaResultCounter(const Dto::Lambda::DeleteLambdaResultCounterRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "delete_lambda_result");
        log_debug << "Delete lambda result counters request, region: " << request.region;

        try {
            const long count = _lambdaDatabase->deleteResultsCounter(request.oid);
            log_trace << "Lambda result counter deleted, count: " << count;
            return count;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }


    long LambdaService::DeleteLambdaResultCounters(const Dto::Lambda::DeleteLambdaResultCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "delete_lambda_result");
        log_debug << "Delete lambda result counters request, region: " << request.region;

        try {
            const long count = _lambdaDatabase->deleteResultsCounters(request.lambdaArn);
            log_trace << "Lambda result counter deleted, arn: " << request.lambdaArn;
            return count;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void LambdaService::EnableLambda(const Dto::Lambda::EnableLambdaRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "enable_lambda");
        log_debug << "Enable lambda request, name: " << request.function.functionName;

        if (!_lambdaDatabase->lambdaExists(request.function.functionName)) {
            log_warning << "Lambda does not exist, name: " << request.function.functionName;
            return;
        }

        // Get lambda
        Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByName(request.region, request.function.functionName);
        lambda.enabled = true;
        lambda = _lambdaDatabase->updateLambda(lambda);
        log_debug << "Lambda enabled, name: " << request.function.functionName;
    }

    void LambdaService::EnableAllLambdas(const Dto::Lambda::EnableAllLambdasRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "enable_all_lambdas");
        log_debug << "Enable all lambdas request, region: " << request.region;

        // Get lambda
        for (std::vector<Database::Entity::Lambda::Lambda> lambdas = _lambdaDatabase->listLambdas(request.region); auto &lambda: lambdas) {
            lambda.enabled = true;
            lambda = _lambdaDatabase->updateLambda(lambda);
        }
        log_debug << "All lambdas enabled, region: " << request.region;
    }

    void LambdaService::DisableLambda(const Dto::Lambda::DisableLambdaRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "disable_lambda");
        log_debug << "Disable lambda request, name: " << request.function.functionName;

        if (!_lambdaDatabase->lambdaExists(request.function.functionName)) {
            log_warning << "Lambda does not exist, name: " << request.function.functionName;
            return;
        }

        // Get lambda
        Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByName(request.region, request.function.functionName);
        lambda.enabled = false;
        lambda = _lambdaDatabase->updateLambda(lambda);
        log_debug << "Lambda disabled, name: " << request.function.functionName;
    }

    void LambdaService::DisableAllLambdas(const Dto::Lambda::DisableAllLambdasRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "disable_all_lambdas");
        log_debug << "Disable all lambdas request, region: " << request.region;

        // Get lambda
        for (std::vector<Database::Entity::Lambda::Lambda> lambdas = _lambdaDatabase->listLambdas(request.region); auto &lambda: lambdas) {
            lambda.enabled = false;
            lambda = _lambdaDatabase->updateLambda(lambda);
        }
        log_debug << "All lambdas disabled, region: " << request.region;
    }

    void LambdaService::UpdateLambdaRuntimeStatus(const std::string &region, const Dto::Lambda::LambdaStatus &status) const {
        log_debug << "Lambda runtime status update, function: " << status.functionName << ", status: " << Dto::Lambda::runtimeStatusToString(status.runtimeStatus) << ", instanceId: " << status.instanceId;

        if (!_lambdaDatabase->lambdaExists(status.functionName)) {
            log_warning << "GRT status update: lambda not found, function: " << status.functionName;
            return;
        }

        // Match by instanceId (LRT reads AWSMOCK_INSTANCE_ID env var), then fall back to
        // adopting the first "unknown"-status instance (GRT not yet updated to use env var).
        Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByName(region, status.functionName);
        bool found = false;
        for (auto &instance: lambda.instances) {
            if (instance.instanceId == status.instanceId) {
                instance.status = Dto::Lambda::Mapper::mapRuntimeStatus(status.runtimeStatus);
                instance.invocations = status.invocations;
                instance.avgDuration = status.avgDuration;
                instance.lastStart = status.lastStart;
                instance.lastStop = status.lastStop;
                instance.lastInvocation = status.lastInvocation;
                found = true;
                break;
            }
        }

        if (!found) {
            // LRT reported an ID that doesn't match — adopt the first server-created instance
            // that hasn't registered yet (status==unknown means no LRT contact so far).
            for (auto &instance: lambda.instances) {
                if (instance.status == Database::Entity::Lambda::unknown) {
                    instance.instanceId = status.instanceId;
                    instance.privatePort = status.port;
                    instance.invocations = status.invocations;
                    instance.avgDuration = status.avgDuration;
                    instance.lastStart = status.lastStart;
                    instance.lastStop = status.lastStop;
                    instance.lastInvocation = status.lastInvocation;
                    instance.status = Dto::Lambda::Mapper::mapRuntimeStatus(status.runtimeStatus);
                    found = true;
                    log_info << "Lambda LRT adopted server instance, function: " << status.functionName << ", instanceId: " << status.instanceId;
                    break;
                }
            }
        }

        // Propagate aggregate stats to the lambda entity
        lambda.invocations = lambda.getTotalInvocations();
        lambda.avgDuration = lambda.getAvgDuration();
        lambda = _lambdaDatabase->updateLambda(lambda);
        log_debug << "Lambda runtime status applied, function: " << lambda.function << ", invocations: " << status.invocations;
    }

    void LambdaService::StartLambda(const Dto::Lambda::StartLambdaRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "start_function");
        log_debug << "Start function, lambdaArn: " + request.lambdaArn;

        if (!_lambdaDatabase->lambdaExistsByArn(request.lambdaArn)) {
            log_error << "Lambda function does not exist, lambdaArn: " << request.lambdaArn;
            throw Core::ServiceException("Lambda function does not exist, lambdaArn: " + request.lambdaArn);
        }

        // Get lambda function
        Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByArn(request.lambdaArn);

        // Check state
        if (lambda.instances.size() > lambda.concurrency) {
            log_warning << "Maximal number of instances exceeded, lambdaArn: " << request.lambdaArn << ", concurrency: " << lambda.concurrency;
            return;
        }

        // Update state
        //lambda = _lambdaDatabase->updateLambda(lambda);

        // Create the lambda function asynchronously
        const std::string instanceId = Core::AwsUtils::CreateLambdaInstanceId();
        CreateLambdaInstance(lambda, instanceId);

        log_info << "Lambda function started, lambdaArn: " + lambda.arn;
    }

    void LambdaService::StartAllLambdas() const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "start_all_lambdas");
        log_debug << "Start all lambdas";

        for (const auto &lambda: _lambdaDatabase->listLambdas({})) {
            if (lambda.enabled) {
                Dto::Lambda::StartLambdaRequest request;
                request.lambdaArn = lambda.arn;
                request.region = lambda.region;
                StartLambda(request);
            } else {
                log_debug << "Lambda function disabled, name: " << lambda.function;
            }
        }
        log_info << "All lambda function started";
    }

    void LambdaService::StopLambda(const Dto::Lambda::StopLambdaRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "stop_function");
        log_debug << "Stop function, lambdaArn: " + request.lambdaArn;

        if (!_lambdaDatabase->lambdaExistsByArn(request.lambdaArn)) {
            log_error << "Lambda function does not exist, lambdaArn: " << request.lambdaArn;
            throw Core::ServiceException("Lambda function does not exist, lambdaArn: " + request.lambdaArn);
        }

        // Get lambda function
        Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByArn(request.lambdaArn);

        // Delete the containers, if existing
        const ContainerService &dockerService = ContainerService::instance();
        for (const auto &instance: lambda.instances) {
            if (dockerService.ContainerExists(instance.containerId)) {
                Dto::Docker::Container container = dockerService.GetContainerById(instance.containerId);
                dockerService.KillContainer(container.id);
                lambda.RemoveInstanceByContainerId(container.id);
                log_debug << "Docker container stopped, containerId: " + container.id;
            }
        }

        // Update state
        lambda.instances.clear();
        lambda = _lambdaDatabase->updateLambda(lambda);

        // Prune containers
        //dockerService.PruneContainers();
        log_info << "Lambda function stopped, lambdaArn: " + lambda.arn;
    }

    void LambdaService::StopAllLambdas() const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "stop_all_lambdas");
        log_debug << "Stop all lambdas";

        for (const auto &lambda: _lambdaDatabase->listLambdas({})) {
            Dto::Lambda::StopLambdaRequest request;
            request.lambdaArn = lambda.arn;
            request.region = lambda.region;
            StopLambda(request);
        }
        log_info << "All lambda function stopped";
    }

    void LambdaService::StopLambdaInstance(const Dto::Lambda::StopLambdaInstanceRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "stop_instance");
        log_debug << "Stop instance, lambdaArn: " + request.functionArn << ", instanceId: " + request.instanceId;

        if (!_lambdaDatabase->lambdaExistsByArn(request.functionArn)) {
            log_error << "Lambda function does not exist, lambdaArn: " << request.functionArn;
            throw Core::ServiceException("Lambda function does not exist, lambdaArn: " + request.functionArn);
        }

        // Get lambda function
        Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByArn(request.functionArn);

        // Delete the containers, if existing
        const ContainerService &dockerService = ContainerService::instance();
        for (const auto &instance: lambda.instances) {
            if (instance.instanceId == request.instanceId && dockerService.ContainerExists(instance.containerId)) {
                const Dto::Docker::Container container = dockerService.GetContainerById(instance.containerId);
                dockerService.KillContainer(container.id);
                log_debug << "Docker container stopped, containerId: " + container.id;
                break;
            }
        }

        // Update state
        lambda.RemoveInstance(request.instanceId);
        lambda = _lambdaDatabase->updateLambda(lambda);

        // Prune containers
        //dockerService.PruneContainers();
        log_info << "Lambda instance stopped, lambdaArn: " + lambda.arn << ", instanceId: " + request.instanceId;
    }

    void LambdaService::DeleteImage(const Dto::Lambda::DeleteImageRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "delete_image");
        log_debug << "Delete image, lambdaArn: " + request.functionArn;

        if (!_lambdaDatabase->lambdaExistsByArn(request.functionArn)) {
            log_error << "Lambda function does not exist, lambdaArn: " << request.functionArn;
            throw Core::ServiceException("Lambda function does not exist, lambdaArn: " + request.functionArn);
        }

        // Get lambda function
        Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByArn(request.functionArn);

        // Cleanup docker
        CleanupDocker(lambda);

        // Update state
        lambda = _lambdaDatabase->updateLambda(lambda);

        // Prune containers
        //ContainerService::instance().PruneContainers();
        log_info << "Lambda function stopped, lambdaArn: " + lambda.arn;
    }

    void LambdaService::RebuildLambda(const Dto::Lambda::RebuildLambdaRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "rebuild_function");
        log_debug << "Rebuild lambda function: " + request.name << ":" << request.version;

        const ContainerService &dockerService = ContainerService::instance();

        if (!_lambdaDatabase->lambdaExists(request.name)) {
            log_error << "Lambda function does not exist, function: " + request.name;
            throw Core::ServiceException("Lambda function does not exist");
        }

        // Delete the containers, if existing
        Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByName(request.region, request.name);
        for (const auto &instance: lambda.instances) {
            if (dockerService.ContainerExists(instance.containerId)) {
                if (Dto::Docker::Container container = dockerService.GetContainerById(instance.containerId); !container.id.empty()) {
                    dockerService.KillContainer(container.id);
                    dockerService.DeleteContainer(container);
                    log_debug << "Docker container deleted, function: " + request.name << ":" << request.version;
                }
            }
        }

        // Update instances
        lambda.instances.clear();
        lambda = _lambdaDatabase->updateLambda(lambda);

        // Delete the image, if existing
        if (dockerService.ImageExists(request.name, request.version)) {
            const Dto::Docker::Image image = dockerService.GetImageByName(request.name, lambda.dockerTag);
            dockerService.DeleteImage(image.id);
            log_debug << "Docker image deleted, function: " + request.name << ":" << request.version;
        }

        // Create instance
        const std::string instanceId = Core::AwsUtils::CreateLambdaInstanceId();

        // Update the lambda function
        CreateLambdaInstance(lambda, instanceId);

        log_info << "Lambda function rebuild, name: " + request.name << ":" << request.version << ", newId: " << instanceId;
    }

    void LambdaService::DeleteFunction(const Dto::Lambda::DeleteFunctionRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "delete_function");
        log_debug << "Delete function: " + request.ToString();

        const ContainerService &dockerService = ContainerService::instance();

        if (!_lambdaDatabase->lambdaExists(request.functionName)) {
            log_error << "Lambda function does not exist, function: " + request.functionName;
            //throw Core::ServiceException("Lambda function does not exist");
            return;
        }

        // Delete the containers, if existing
        const Database::Entity::Lambda::Lambda lambda = _lambdaDatabase->getLambdaByName(request.region, request.functionName);
        for (const auto &instance: lambda.instances) {
            if (dockerService.ContainerExists(instance.containerId)) {
                Dto::Docker::Container container = dockerService.GetContainerById(instance.containerId);
                dockerService.KillContainer(container.id);
                dockerService.DeleteContainer(container);
                log_debug << "Docker container deleted, function: " + request.functionName;
            }
        }

        // Delete the image, if existing
        if (dockerService.ImageExists(request.functionName, lambda.dockerTag)) {
            const Dto::Docker::Image image = dockerService.GetImageByName(request.functionName, lambda.dockerTag);
            dockerService.DeleteImage(image.id);
            log_debug << "Docker image deleted, function: " + request.functionName;
        }

        // Prune containers
        //dockerService.PruneContainers();
        log_debug << "Docker image deleted, function: " + request.functionName;

        _lambdaDatabase->deleteLambda(request.functionName);
        log_info << "Lambda function deleted, function: " + request.functionName;
    }

    void LambdaService::DeleteTags(const Dto::Lambda::DeleteTagsRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "delete_tags");
        log_trace << "Delete tags: " + request.ToString();

        if (!_lambdaDatabase->lambdaExistsByArn(request.arn)) {
            log_error << "Lambda function does not exist, arn: " + request.arn;
            throw Core::ServiceException("Lambda function does not exist");
        }

        // Get the existing entity
        Dto::Lambda::ListTagsResponse response;
        Database::Entity::Lambda::Lambda lambdaEntity = _lambdaDatabase->getLambdaByArn(request.arn);
        long count = 0;
        for (const auto &tag: request.tags) {
            count += static_cast<long>(std::erase_if(lambdaEntity.tags, [tag](const auto &item) {
                auto const &[k, v] = item;
                return k == tag;
            }));
        }
        lambdaEntity = _lambdaDatabase->updateLambda(lambdaEntity);
        log_debug << "Delete tag request succeeded, arn: " + lambdaEntity.arn << " deleted: " << count;
    }

    std::string LambdaService::GetLambdaCodePath(const Database::Entity::Lambda::Lambda &lambda) {
        const auto lambdaDir = Core::Configuration::instance().get<std::string>("awsmock.modules.lambda.data-dir");
        return Core::FileUtils::appendPath(lambdaDir, lambda.function + "-" + lambda.dockerTag + ".b64");
    }

    std::string LambdaService::GetLambdaCodeFromS3(const Database::Entity::Lambda::Lambda &lambda) const {
        const auto s3DataDir = Core::Configuration::instance().get<std::string>("awsmock.modules.s3.data-dir");

        const Database::Entity::S3::Object object = _s3Database->getObject(lambda.region, lambda.code.s3Bucket, lambda.code.s3Key);
        return Core::FileUtils::appendPath(s3DataDir, object.internalName);
    }

    void LambdaService::CleanupDocker(Database::Entity::Lambda::Lambda &lambda) {
        // Delete docker containers
        for (const auto &container: ContainerService::instance().ListContainerByImageName(lambda.function)) {
            ContainerService::instance().KillContainer(container.id);
            ContainerService::instance().DeleteContainer(container.id);
        }
        log_debug << "Docker containers deleted, function: " << lambda.function << ", count: " << lambda.instances.size();
        lambda.instances.clear();

        // Delete image
        if (ContainerService::instance().ImageExists(lambda.function, lambda.dockerTag)) {
            ContainerService::instance().DeleteImage(lambda.function + ":" + lambda.dockerTag);
            log_debug << "Docker images deleted, function: " << lambda.function;
        }
        log_info << "Done cleanup docker, function: " << lambda.function;
    }

    void LambdaService::CreateResourceNotification(const Dto::Lambda::AddEventSourceRequest &request) const {

        if (request.type == "S3") {

            if (!_s3Database->bucketExists(request.eventSourceArn)) {
                log_error << "S3 bucket does not exist: " << request.eventSourceArn;
                throw Core::ServiceException("S3 bucket does not exist: " + request.eventSourceArn);
            }
            Database::Entity::S3::Bucket bucket = _s3Database->getBucketByArn(request.eventSourceArn);

            if (bucket.HasLambdaNotification(request.functionArn)) {
                log_error << "S3 bucket has already notification to function: " << request.functionArn;
                throw Core::ServiceException("S3 bucket has already notification to function: " + request.functionArn);
            }

            // Convert filter rules
            Database::Entity::S3::FilterRule filterRule;
            filterRule.name = request.filterRuleType;
            filterRule.value = request.filterRuleValue;
            std::vector<Database::Entity::S3::FilterRule> filterRules;
            filterRules.emplace_back(filterRule);

            // Check for update
            Database::Entity::S3::LambdaNotification lambdaNotification;
            lambdaNotification.lambdaArn = request.functionArn;
            lambdaNotification.events = request.events;
            lambdaNotification.filterRules = filterRules;

            // Send S3 put notification request
            bucket.lambdaNotifications.emplace_back(lambdaNotification);
            bucket = _s3Database->createOrUpdateBucket(bucket);
            log_debug << "Bucket updated, name: " << bucket.name;

        } else if (request.type == "SQS") {

            if (!_sqsDatabase->queueArnExists(request.eventSourceArn)) {
                log_error << "SQS queue does not exist: " << request.eventSourceArn;
                throw Core::ServiceException("Bucket does not exist: " + request.eventSourceArn);
            }

            Database::Entity::SQS::Queue queue = _sqsDatabase->getQueueByArn(request.eventSourceArn);

            // Convert filter rules
            Database::Entity::S3::FilterRule filterRule;
            filterRule.name = request.filterRuleType;
            filterRule.value = request.filterRuleValue;
            std::vector<Database::Entity::S3::FilterRule> filterRules;
            filterRules.emplace_back(filterRule);

            // Create lambda notification
            Database::Entity::S3::LambdaNotification lambdaNotification;
            lambdaNotification.lambdaArn = request.eventSourceArn;
            lambdaNotification.events = request.events;
            lambdaNotification.filterRules = filterRules;

            // Send S3 put notification request
            queue = _sqsDatabase->createOrUpdateQueue(queue);
            log_debug << "Queue updated, name: " << queue.name;

        } else if (request.type == "SNS") {

            if (!_snsDatabase->topicExists(request.eventSourceArn)) {
                log_error << "SNS topic does not exist: " << request.eventSourceArn;
                throw Core::ServiceException("Bucket does not exist: " + request.eventSourceArn);
            }

            Database::Entity::SNS::Topic topic = _snsDatabase->getTopicByArn(request.eventSourceArn);

            // Convert filter rules
            Database::Entity::S3::FilterRule filterRule;
            filterRule.name = request.filterRuleType;
            filterRule.value = request.filterRuleValue;
            std::vector<Database::Entity::S3::FilterRule> filterRules;
            filterRules.emplace_back(filterRule);

            // Create lambda notification
            Database::Entity::S3::LambdaNotification lambdaNotification;
            lambdaNotification.lambdaArn = request.eventSourceArn;
            lambdaNotification.events = request.events;
            lambdaNotification.filterRules = filterRules;

            // Send S3 put notification request
            topic = _snsDatabase->createOrUpdateTopic(topic);
            log_debug << "Topic updated, name: " << topic.topicName;
        }
    }

    void LambdaService::WriteBase64File(const std::string &base64File, const std::string &content) {
        auto lambdaDir = Core::Configuration::instance().get<std::string>("awsmock.modules.lambda.data-dir");
        std::string base64FullFile = Core::FileUtils::appendPath(lambdaDir, base64File);
        log_debug << "Using Base64File: " << base64FullFile;

        std::ofstream ofs(base64FullFile);
        ofs << content;
        ofs.close();
        log_debug << "New Base64 file written: " << content;
    }

    // -------------------------------------------------------------------------
    // Docker lifecycle helpers (moved from LambdaCreator)
    // -------------------------------------------------------------------------

    Database::Entity::Lambda::Lambda LambdaService::CreateLambdaInstance(Database::Entity::Lambda::Lambda &lambda, const std::string &instanceId) const {
        log_debug << "Start creating lambda function, instanceId: " << instanceId;
        try {
            CreateInstance(instanceId, lambda, lambda.code.zipFile);
            lambda.avgDuration = 0;
            lambda.invocations = 0;
            lambda.lastStarted = system_clock::now();
            lambda.codeSize = static_cast<long>(lambda.code.zipFile.size());
            lambda = _lambdaDatabase->updateLambda(lambda);
            log_info << "Lambda function instance started: " << lambda.function << " instanceId: " << instanceId << ", instances: " << lambda.instances.size();
            return lambda;
        } catch (const Core::ContainerException &e) {
            log_error << "Error creating lambda function: " << e.what();
        }
        return {};
    }

    Database::Entity::Lambda::Lambda LambdaService::AddInstance(Database::Entity::Lambda::Lambda &lambda, const std::string &instanceId) const {
        log_debug << "Adding lambda instance, function: " << lambda.function << ", instanceId: " << instanceId;
        try {
            CreateInstance(instanceId, lambda, lambda.code.zipFile);
            lambda.lastStarted = system_clock::now();
            lambda = _lambdaDatabase->updateLambda(lambda);
            log_info << "Lambda instance added, function: " << lambda.function << ", instanceId: " << instanceId << ", total instances: " << lambda.instances.size();
            return lambda;
        } catch (const Core::ContainerException &e) {
            log_error << "Error adding lambda instance, function: " << lambda.function << ", error: " << e.what();
        }
        return lambda;
    }

    std::string LambdaService::CreateInstance(const std::string &instanceId, Database::Entity::Lambda::Lambda &lambda, const std::string &functionCode) const {

        const auto privatePort = Core::Configuration::instance().get<std::string>("awsmock.modules.lambda.private-port");

        if (lambda.dockerTag.empty()) {
            lambda.dockerTag = GetDockerTag(lambda);
            lambda.tags["dockerTag"] = lambda.dockerTag;
            lambda.tags["version"] = lambda.dockerTag;
            log_debug << "Using docker tag: " << lambda.dockerTag;
        }

        if (!ContainerService::instance().ImageExists(lambda.function, lambda.dockerTag)) {
            CreateDockerImage(functionCode, lambda, lambda.dockerTag);
        }

        const int hostPort = Core::SystemUtils::GetNextFreePort();
        const std::string containerName = lambda.function + "-" + instanceId;
        if (!ContainerService::instance().ContainerExists(containerName)) {
            CreateDockerContainer(lambda, instanceId, hostPort, lambda.dockerTag);
        }

        Dto::Docker::InspectContainerResponse inspectContainerResponse = ContainerService::instance().InspectContainer(containerName);

        if (!inspectContainerResponse.state.running && !inspectContainerResponse.id.empty()) {
            ContainerService::instance().StartContainer(inspectContainerResponse.id, inspectContainerResponse.GetContainerName());
            ContainerService::instance().WaitForContainer(inspectContainerResponse.id);
            log_info << "Lambda docker container started, function: " << lambda.function << ", containerId: " << inspectContainerResponse.id;
        }

        inspectContainerResponse = ContainerService::instance().InspectContainer(containerName);
        Database::Entity::Lambda::Instance instance;
        instance.instanceId = instanceId;
        instance.containerName = containerName;
        instance.lastStart = system_clock::now();
        if (!inspectContainerResponse.id.empty()) {
            instance.hostName = _dockerized ? containerName : std::string("localhost");
            instance.publicPort = _dockerized ? stoi(privatePort) : hostPort;
            instance.privatePort = stoi(privatePort);
            instance.containerId = inspectContainerResponse.id;
            lambda.containerSize = inspectContainerResponse.sizeRootFs;
        }
        lambda.instances.emplace_back(instance);

        return inspectContainerResponse.id;
    }

    void LambdaService::CreateDockerImage(const std::string &zipFile, Database::Entity::Lambda::Lambda &lambdaEntity, const std::string &dockerTag) {
        log_info << "Start creating docker image, name: " << lambdaEntity.function << ":" << dockerTag;

        std::string codeDir = Core::DirUtils::CreateTempDir();

        const auto lambdaDir = Core::Configuration::instance().get<std::string>("awsmock.modules.lambda.data-dir");
        const std::string base64FullFile = Core::FileUtils::appendPath(lambdaDir, zipFile);
        if (!Core::FileUtils::FileExists(base64FullFile)) {
            log_error << "Base64 file does not exist, path: " << base64FullFile;
            throw Core::ContainerException("Base64 file does not exist, path: " + base64FullFile);
        }

        const std::string functionCode = Core::FileUtils::ReadFile(base64FullFile);

        codeDir = UnpackZipFile(codeDir, functionCode, lambdaEntity.runtime);

        const std::string imageFile = ContainerService::instance().BuildLambdaImage(codeDir, lambdaEntity);
        if (imageFile.empty()) {
            Core::DirUtils::DeleteDirectory(codeDir);
            throw Core::ContainerException("Failed to build Lambda image, function: " + lambdaEntity.function + ", runtime: " + lambdaEntity.runtime);
        }

        const Dto::Docker::Image image = ContainerService::instance().GetImageByName(lambdaEntity.function, dockerTag);
        lambdaEntity.imageId = image.id;
        lambdaEntity.imageSize = image.size;
        lambdaEntity.codeSha256 = Core::Crypto::GetSha256FromFile(imageFile);

        Core::DirUtils::DeleteDirectory(codeDir);
        log_info << "Finished creating docker image, name: " << lambdaEntity.function << " size: " << std::to_string(lambdaEntity.codeSize);
    }

    void LambdaService::CreateDockerContainer(const Database::Entity::Lambda::Lambda &lambda, const std::string &instanceId, const int hostPort, const std::string &dockerTag) {
        log_info << "Creating docker container, function: " << lambda.function << " hostPort: " << hostPort << " dockerTag: " << dockerTag;
        try {
            const std::string containerName = lambda.function + "-" + instanceId;
            std::vector<std::string> environment = GetEnvironment(lambda);
            environment.emplace_back("AWSMOCK_INSTANCE_ID=" + instanceId);
            environment.emplace_back("AWSMOCK_PUBLIC_PORT=" + std::to_string(hostPort));
            const std::vector<std::string> cmd = GetCmd(lambda);
            Dto::Docker::CreateContainerResponse response = ContainerService::instance().CreateContainer(lambda.function, containerName, dockerTag, environment, hostPort, cmd);
            log_info << "Docker container created, function: " << lambda.function << " containerId: " << response.id << ", hostPost: " << response.hostPort;
        } catch (std::exception &exc) {
            log_error << exc.what();
        }
    }

    std::vector<std::string> LambdaService::GetEnvironment(const Database::Entity::Lambda::Lambda &lambda) {
        std::vector<std::string> environment;
        environment.reserve(lambda.environment.variables.size() + 1);
        for (const auto &[fst, snd]: lambda.environment.variables) {
            environment.emplace_back(fst + "=" + snd);
        }
        environment.emplace_back("AWS_LAMBDA_FUNCTION_TIMEOUT=" + std::to_string(lambda.timeout));
        return environment;
    }

    std::vector<std::string> LambdaService::GetCmd(const Database::Entity::Lambda::Lambda &lambda) {
        if (lambda.handler.empty()) {
            return {};
        }
        std::string providedRuntime = Core::StringUtils::ToLower(lambda.runtime);
        Core::StringUtils::Replace(providedRuntime, ".", "-");
        const auto supportedRuntime = Core::Configuration::instance().get<std::string>("awsmock.modules.lambda.runtime." + providedRuntime);
        if (Core::StringUtils::ContainsIgnoreCase(supportedRuntime, "awsmock-lrt")) {
            return {};
        }
        return {lambda.handler};
    }

    std::string LambdaService::GetDockerTag(const Database::Entity::Lambda::Lambda &lambda) {
        if (lambda.HasTag("version")) {
            return lambda.GetTagValue("version");
        }
        if (lambda.HasTag("dockerTag")) {
            return lambda.GetTagValue("dockerTag");
        }
        if (lambda.HasTag("tag")) {
            return lambda.GetTagValue("tag");
        }
        return "latest";
    }

    std::string LambdaService::UnpackZipFile(const std::string &codeDir, const std::string &functionCode, const std::string &runtime) {
        const auto tempDir = Core::Configuration::instance().get<std::string>("awsmock.temp-dir");
        const std::string zipFile = Core::FileUtils::appendPath(tempDir, Core::AwsUtils::CreateLambdaInstanceId() + ".zip");
        Core::Crypto::Base64Decode(functionCode, zipFile);

        try {
            if (Core::StringUtils::ContainsIgnoreCase(runtime, "java")) {
                const std::string classesDir = Core::FileUtils::appendPath(codeDir, "classes");
                Core::DirUtils::EnsureDirectoryExists(classesDir);
                Core::ZipUtils::Unzip(zipFile, classesDir);
            } else {
                Core::ZipUtils::Unzip(zipFile, codeDir);
            }
            Core::FileUtils::RemoveFile(zipFile);
            return codeDir;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace Awsmock::Service
