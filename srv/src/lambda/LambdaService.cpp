
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/lambda/LambdaService.h>

namespace AwsMock::Service {

    std::map<std::string, std::shared_ptr<boost::mutex>> LambdaService::_instanceMutex;

    Dto::Lambda::CreateFunctionResponse LambdaService::CreateFunction(Dto::Lambda::CreateFunctionRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "create_function");
        log_debug << "Create function request, name: " << request.functionName;

        const auto accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");

        // Create an entity and set ARN
        Database::Entity::Lambda::Lambda lambda = {};
        lambda.arn = Core::AwsUtils::CreateLambdaArn(request.region, accountId, request.functionName);
        lambda.function = request.functionName;
        lambda.runtime = request.runtime;
        lambda.handler = request.handler;

        // Create mutex
        _instanceMutex[request.functionName] = std::make_shared<boost::mutex>();

        // Write the base64 file
        lambda.code.zipFile = request.functionName + "-" + request.version + ".b64";
        WriteBase64File(lambda.code.zipFile, request.code.zipFile);

        // Update database
        lambda.timeout = request.timeout;
        lambda.state = Database::Entity::Lambda::LambdaState::Pending;
        lambda.stateReason = "Initializing";
        lambda.stateReasonCode = Database::Entity::Lambda::LambdaStateReasonCode::Creating;
        lambda = _lambdaDatabase.CreateOrUpdateLambda(lambda);

        // Find idle instance
        Database::Entity::Lambda::Instance instance;

        // Create instance
        std::string instanceId = Core::StringUtils::GenerateRandomHexString(8);

        // Create lambda
        LambdaCreator lambdaCreator;
        lambda = lambdaCreator.CreateLambda(lambda, instanceId);
        log_info << "New lambda instance created, name: " << lambda.function << ", instanceId: " << instanceId << ", status: " << Database::Entity::Lambda::LambdaInstanceStatusToString(instance.status) << ", totalSize: " << lambda.instances.size();

        return Dto::Lambda::Mapper::map(request, lambda);
    }

    void LambdaService::UploadFunctionCode(const Dto::Lambda::UploadFunctionCodeRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "upload_function_code");
        log_debug << "Upload function code request, arn: " << request.functionArn;

        if (!_lambdaDatabase.LambdaExistsByArn(request.functionArn)) {
            log_warning << "Lambda function does not exist, arn: " << request.functionArn;
            throw Core::ServiceException("Lambda function does not exist, arn: " + request.functionArn);
        }

        if (request.functionCode.empty()) {
            log_warning << "Lambda function code is empty, arn: " << request.functionArn;
            throw Core::ServiceException("Lambda function code is empty, arn: " + request.functionArn);
        }

        // Get lambda function
        Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(request.functionArn);

        // Stop and delete all containers/images
        CleanupDocker(lambda);

        // Write the base64 file
        lambda.code.zipFile = lambda.function + "-" + request.version + ".b64";
        WriteBase64File(lambda.code.zipFile, request.functionCode);

        // Update lambda attributes
        lambda.state = Database::Entity::Lambda::LambdaState::Pending;
        lambda.invocations = 0;
        lambda.averageRuntime = 0;
        lambda.dockerTag = request.version;
        lambda.tags["version"] = request.version;
        lambda.tags["dockerTag"] = request.version;
        lambda.stateReason = "Initializing";
        lambda.stateReasonCode = Database::Entity::Lambda::LambdaStateReasonCode::Creating;
        lambda = _lambdaDatabase.UpdateLambda(lambda);

        // Clear lambda results
        const long count = _lambdaDatabase.DeleteResultsCounters(lambda.arn);
        log_debug << "Lambda results cleared, arn: " << lambda.arn << " count: " << count;

        // Create instance
        const std::string instanceId = Core::StringUtils::GenerateRandomHexString(8);

        // Update the lambda function
        const LambdaCreator lambdaCreator;
        lambdaCreator.CreateLambda(lambda, instanceId);
        log_debug << "Lambda function code updated, function: " << lambda.function;
    }

    Dto::Lambda::ListFunctionResponse LambdaService::ListFunctions(const std::string &region) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "list_functions");
        log_debug << "List functions request, region: " << region;

        try {
            const std::vector<Database::Entity::Lambda::Lambda> lambdas = _lambdaDatabase.ListLambdas(region);

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

            const std::vector<Database::Entity::Lambda::Lambda> lambdas = _lambdaDatabase.ListLambdaCounters(request.region, request.prefix, request.pageSize, request.pageIndex, Dto::Common::Mapper::map(request.sortColumns));
            const long count = _lambdaDatabase.LambdaCount(request.region);

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

            const Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(request.lambdaArn);

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

            const Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(request.lambdaArn);

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
        log_debug << "List lambda environment counters request, functionArn: " << request.functionArn;

        if (!_lambdaDatabase.LambdaExistsByArn(request.functionArn)) {
            log_warning << "Lambda function does not exist, arn: " << request.functionArn;
            throw Core::ServiceException("Lambda function does not exist, arn: " + request.functionArn);
        }
        try {

            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(request.functionArn);
            lambda.environment.variables[request.environmentKey] = request.environmentValue;
            lambda = _lambdaDatabase.UpdateLambda(lambda);
            log_trace << "Lambda environments added, lambdaArn: " << lambda.arn;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void LambdaService::UpdateLambdaEnvironment(const Dto::Lambda::UpdateFunctionEnvironmentRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "update_lambda_environment");
        log_debug << "Update lambda environment request, functionArn: " << request.functionArn << ", key: " << request.environmentKey << ", value: " << request.environmentValue;

        if (!_lambdaDatabase.LambdaExistsByArn(request.functionArn)) {
            log_warning << "Lambda function does not exist, arn: " << request.functionArn;
            throw Core::ServiceException("Lambda function does not exist, arn: " + request.functionArn);
        }
        try {

            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(request.functionArn);
            lambda.environment.variables[request.environmentKey] = request.environmentValue;
            lambda = _lambdaDatabase.UpdateLambda(lambda);
            log_trace << "Lambda environments updated, lambdaArn: " << lambda.arn;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void LambdaService::DeleteLambdaEnvironment(const Dto::Lambda::DeleteEnvironmentRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "delete_lambda_environment");
        log_debug << "Delete lambda environment request, functionArn: " << request.functionArn << ", key: " << request.environmentKey;

        if (!_lambdaDatabase.LambdaExistsByArn(request.functionArn)) {
            log_warning << "Lambda function does not exist, arn: " << request.functionArn;
            throw Core::ServiceException("Lambda function does not exist, arn: " + request.functionArn);
        }

        try {

            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(request.functionArn);
            std::string environmentKey = request.environmentKey;
            const auto count = std::erase_if(lambda.environment.variables, [environmentKey](const auto &item) {
                auto const &[key, value] = item;
                return key == environmentKey;
            });
            lambda = _lambdaDatabase.UpdateLambda(lambda);
            log_trace << "Lambda environments deleted, lambdaArn: " << lambda.arn << " count: " << count;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void LambdaService::UpdateLambda(const Dto::Lambda::UpdateLambdaRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "update_lambda");
        log_debug << "Add lambda event source counters request, functionArn: " << request.functionArn;

        if (!_lambdaDatabase.LambdaExistsByArn(request.functionArn)) {
            log_warning << "Lambda function does not exist, arn: " << request.functionArn;
            throw Core::ServiceException("Lambda function does not exist, arn: " + request.functionArn);
        }

        try {

            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(request.functionArn);
            lambda.enabled = request.enabled;
            lambda = _lambdaDatabase.UpdateLambda(lambda);
            log_trace << "Lambda updated, lambdaArn: " << lambda.arn;

            if (!request.enabled) {
                Dto::Lambda::StopLambdaRequest stopRequest;
                stopRequest.functionArn = request.functionArn;
                stopRequest.region = request.region;
                StopLambda(stopRequest);
            } else {
                Dto::Lambda::StartLambdaRequest startRequest;
                startRequest.functionArn = request.functionArn;
                startRequest.region = request.region;
                StartLambda(startRequest);
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void LambdaService::AddEventSource(const Dto::Lambda::AddEventSourceRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "add_event_source");
        log_debug << "Add lambda event source counters request, functionArn: " << request.functionArn;

        if (!_lambdaDatabase.LambdaExistsByArn(request.functionArn)) {
            log_warning << "Lambda function does not exist, arn: " << request.functionArn;
            throw Core::ServiceException("Lambda function does not exist, arn: " + request.functionArn);
        }

        try {

            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(request.functionArn);
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
            lambda = _lambdaDatabase.UpdateLambda(lambda);
            log_trace << "Lambda event sources added, lambdaArn: " << lambda.arn;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void LambdaService::DeleteEventSource(const Dto::Lambda::DeleteEventSourceRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "delete_event_source");
        log_debug << "Delete lambda event source request, functionArn: " << request.functionArn << ", eventSourceArn: " << request.eventSourceArn;

        if (!_lambdaDatabase.LambdaExistsByArn(request.functionArn)) {
            log_warning << "Lambda function does not exist, arn: " << request.functionArn;
            throw Core::ServiceException("Lambda function does not exist, arn: " + request.functionArn);
        }

        try {

            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(request.functionArn);
            std::string eventSourceArn = request.eventSourceArn;
            const auto count = std::erase_if(lambda.eventSources, [eventSourceArn](const auto &item) {
                return item.eventSourceArn == eventSourceArn;
            });
            lambda = _lambdaDatabase.UpdateLambda(lambda);
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

            const Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(request.lambdaArn);

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

            const Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(request.lambdaArn);

            Dto::Lambda::ListLambdaInstanceCountersResponse response;
            response.total = static_cast<long>(lambda.instances.size());

            for (const auto &instance: lambda.instances) {
                Dto::Lambda::InstanceCounter instanceCounter;
                instanceCounter.instanceId = instance.instanceId;
                instanceCounter.containerId = instance.containerId;
                instanceCounter.status = Database::Entity::Lambda::LambdaInstanceStatusToString(instance.status);
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

    void LambdaService::AddLambdaTag(const Dto::Lambda::AddTagRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "add_lambda_tag");
        log_debug << "List lambda tag counters request, functionArn: " << request.functionArn;

        try {

            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(request.functionArn);
            lambda.tags[request.tagKey] = request.tagValue;
            lambda = _lambdaDatabase.UpdateLambda(lambda);
            log_trace << "Lambda tags added, lambdaArn: " << lambda.arn;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void LambdaService::UpdateLambdaTag(const Dto::Lambda::UpdateFunctionTagRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "update_lambda_tag");
        log_debug << "Update lambda tag request, functionArn: " << request.functionArn << ", key: " << request.tagKey << ", value: " << request.tagValue;

        if (!_lambdaDatabase.LambdaExistsByArn(request.functionArn)) {
            log_warning << "Lambda function does not exist, arn: " << request.functionArn;
            throw Core::ServiceException("Lambda function does not exist, arn: " + request.functionArn);
        }

        try {

            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(request.functionArn);
            lambda.tags[request.tagKey] = request.tagValue;
            lambda = _lambdaDatabase.UpdateLambda(lambda);
            log_trace << "Lambda tags updated, lambdaArn: " << lambda.arn;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void LambdaService::DeleteLambdaTag(const Dto::Lambda::DeleteTagRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "delete_lambda_tag");
        log_debug << "Delete lambda tag request, functionArn: " << request.functionArn << ", key: " << request.tagKey;

        try {

            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(request.functionArn);
            std::string tagKey = request.tagKey;
            const auto count = std::erase_if(lambda.tags, [tagKey](const auto &item) {
                auto const &[key, value] = item;
                return key == tagKey;
            });
            lambda = _lambdaDatabase.UpdateLambda(lambda);
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
            const Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByName(region, name);

            Dto::Lambda::Function function;
            function.functionName = lambda.function,
            function.handler = lambda.handler,
            function.runtime = lambda.runtime,
            function.lastUpdateStatus = "Successful",
            function.state = LambdaStateToString(lambda.state),
            function.stateReason = lambda.stateReason,
            function.stateReasonCode = LambdaStateReasonCodeToString(lambda.stateReasonCode);
            function.stateReasonCode = LambdaStateReasonCodeToString(lambda.stateReasonCode);

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
        log_debug << "Get function request, functionArn: " << request.functionArn;

        try {
            const Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(request.functionArn);

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
            response.averageRuntime = lambda.averageRuntime;
            response.enabled = lambda.enabled;
            response.state = Database::Entity::Lambda::LambdaStateToString(lambda.state);
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
            Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByName(request.region, request.functionName);
            lambda.averageRuntime = 0;
            lambda.invocations = 0;
            lambda = _lambdaDatabase.UpdateLambda(lambda);
            log_info << "Reset lambda function counters, function: " << lambda.function;
        } catch (bsoncxx::exception &ex) {
            log_error << "Reset function counters request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::Lambda::LambdaResult LambdaService::InvokeLambdaFunction(const std::string &region, const std::string &functionName, std::string &payload, const Dto::Lambda::LambdaInvocationType &invocationType) const {

        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "invoke_lambda_function");
        log_debug << "Invocation lambda function, functionName: " << functionName;

        const auto accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
        const auto lambdaArn = Core::AwsUtils::CreateLambdaArn(region, accountId, functionName);

        // Get the lambda entity
        Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(lambdaArn);
        log_debug << "Got lambda entity, name: " << lambda.function;

        // Check status
        if (!lambda.enabled) {
            log_warning << "Lambda function is disabled, arn: " << lambdaArn;
            throw Core::ServiceException("Lambda function is disabled, arn: " + lambdaArn);
        }

        // Find an idle instance
        Database::Entity::Lambda::Instance instance;
        FindIdleInstance(lambda, instance);
        log_info << "Found idle instance, name: " << lambda.function << ", instanceId: " << instance.instanceId << ", containerId: " << instance.containerId << ", hostName: " << instance.hostName << ", port: " << instance.publicPort;

        // Execution depending on the invocation type
        Dto::Lambda::LambdaResult result{};
        if (invocationType == Dto::Lambda::REQUEST_RESPONSE) {
            Database::Entity::Lambda::LambdaResult lambdaResult = lambdaExecutor.Invocation(lambda, instance, payload);
            result = Dto::Lambda::Mapper::mapResult(lambdaResult);
        } else if (invocationType == Dto::Lambda::EVENT) {
            Database::Entity::Lambda::LambdaResult lambdaResult = lambdaExecutor.Invocation(lambda, instance, payload);
            result = Dto::Lambda::Mapper::mapResult(lambdaResult);
            log_debug << "Lambda result, lambda: " << result.functionArn << ", result: " << result.status;
        }
        return result;
    }

    void LambdaService::CreateTag(const Dto::Lambda::CreateTagRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "create_tag");
        log_debug << "Create tag request, arn: " << request.arn;

        if (!_lambdaDatabase.LambdaExistsByArn(request.arn)) {
            log_warning << "Lambda function does not exist, arn: " << request.arn;
            throw Core::ServiceException("Lambda function does not exist");
        }

        // Get the existing entity
        Database::Entity::Lambda::Lambda lambdaEntity = _lambdaDatabase.GetLambdaByArn(request.arn);
        for (const auto &[fst, snd]: request.tags) {
            lambdaEntity.tags.emplace(fst, snd);
        }
        lambdaEntity = _lambdaDatabase.UpdateLambda(lambdaEntity);
        log_debug << "Create tag request succeeded, arn: " + request.arn << " size: " << lambdaEntity.tags.size();
    }

    Dto::Lambda::ListTagsResponse LambdaService::ListTags(const std::string &arn) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "list_tags");
        log_debug << "List tags request, arn: " << arn;

        if (!_lambdaDatabase.LambdaExistsByArn(arn)) {
            log_warning << "Lambda function does not exist, arn: " << arn;
            throw Core::ServiceException("Lambda function does not exist");
        }

        // Get the existing entity
        Dto::Lambda::ListTagsResponse response;
        Database::Entity::Lambda::Lambda lambdaEntity = _lambdaDatabase.GetLambdaByArn(arn);
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
        response.accountUsage.functionCount = _lambdaDatabase.LambdaCount();

        return response;
    }

    Dto::Lambda::CreateEventSourceMappingsResponse LambdaService::CreateEventSourceMappings(const Dto::Lambda::CreateEventSourceMappingsRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "create_event_source_mapping");
        log_debug << "Create event source mapping, arn: " << request.functionName << " sourceArn: " << request.eventSourceArn;

        if (!_lambdaDatabase.LambdaExists(request.functionName)) {
            log_warning << "Lambda function does not exist, function: " << request.functionName;
            throw Core::NotFoundException("Lambda function does not exist");
        }

        // Get the existing entity
        Database::Entity::Lambda::Lambda lambdaEntity = _lambdaDatabase.GetLambdaByName(request.region, request.functionName);

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
        lambdaEntity = _lambdaDatabase.UpdateLambda(lambdaEntity);
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
        log_debug << "Event source mapping created, function: " << response.functionName << " sourceArn: " << response.eventSourceArn;
        return response;
    }

    Dto::Lambda::ListEventSourceMappingsResponse LambdaService::ListEventSourceMappings(const Dto::Lambda::ListEventSourceMappingsRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "list_event_source_mapping");
        log_debug << "List event source mappings, functionName: " << request.functionName << " sourceArn: " << request.eventSourceArn;

        if (!_lambdaDatabase.LambdaExists(request.functionName)) {
            log_warning << "Lambda function does not exist, function: " << request.functionName;
            throw Core::NotFoundException("Lambda function does not exist");
        }

        // Get the existing entity
        const Database::Entity::Lambda::Lambda lambdaEntity = _lambdaDatabase.GetLambdaByName(request.region, request.functionName);

        return Dto::Lambda::Mapper::map(lambdaEntity.eventSources);
    }

    Dto::Lambda::ListLambdaArnsResponse LambdaService::ListLambdaArns() const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "list_lambda_arns");
        log_trace << "List all queues ARNs request";

        try {
            const std::vector<Database::Entity::Lambda::Lambda> lambdaList = _lambdaDatabase.ListLambdas();
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

        if (!_lambdaDatabase.LambdaResultExists(request.oid)) {
            log_warning << "Lambda function result does not exist, oid: " << request.oid;
            throw Core::NotFoundException("Lambda function result does not exist, oid: " + request.oid);
        }

        Database::Entity::Lambda::LambdaResult lambdaResult = _lambdaDatabase.GetLambdaResultCounter(request.oid);
        log_trace << "Lambda result found, lambdaResult: " << lambdaResult;

        return Dto::Lambda::Mapper::map(lambdaResult);
    }

    Dto::Lambda::ListLambdaResultCountersResponse LambdaService::ListLambdaResultCounters(const Dto::Lambda::ListLambdaResultCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "list_lambda_results");
        log_debug << "List function counters request, region: " << request.region;

        try {

            const std::vector<Database::Entity::Lambda::LambdaResult> lambdaResults = _lambdaDatabase.ListLambdaResultCounters(request.lambdaArn, request.prefix, request.pageSize, request.pageIndex, Dto::Common::Mapper::map(request.sortColumns));
            const long count = _lambdaDatabase.LambdaResultsCount(request.lambdaArn);

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

            const long count = _lambdaDatabase.DeleteResultsCounter(request.oid);
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

            const long count = _lambdaDatabase.DeleteResultsCounters(request.lambdaArn);
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

        if (!_lambdaDatabase.LambdaExists(request.function.functionName)) {
            log_warning << "Lambda does not exist, name: " << request.function.functionName;
            return;
        }

        // Get lambda
        Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByName(request.region, request.function.functionName);
        lambda.enabled = true;
        lambda = _lambdaDatabase.UpdateLambda(lambda);
        log_debug << "Lambda enabled, name: " << request.function.functionName;
    }

    void LambdaService::EnableAllLambdas(const Dto::Lambda::EnableAllLambdasRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "enable_all_lambdas");
        log_debug << "Enable all lambdas request, region: " << request.region;

        // Get lambda
        for (std::vector<Database::Entity::Lambda::Lambda> lambdas = _lambdaDatabase.ListLambdas(request.region); auto &lambda: lambdas) {
            lambda.enabled = true;
            lambda = _lambdaDatabase.UpdateLambda(lambda);
        }
        log_debug << "All lambdas enabled, region: " << request.region;
    }

    void LambdaService::DisableLambda(const Dto::Lambda::DisableLambdaRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "disable_lambda");
        log_debug << "Disable lambda request, name: " << request.function.functionName;

        if (!_lambdaDatabase.LambdaExists(request.function.functionName)) {
            log_warning << "Lambda does not exist, name: " << request.function.functionName;
            return;
        }

        // Get lambda
        Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByName(request.region, request.function.functionName);
        lambda.enabled = false;
        lambda = _lambdaDatabase.UpdateLambda(lambda);
        log_debug << "Lambda disabled, name: " << request.function.functionName;
    }

    void LambdaService::DisableAllLambdas(const Dto::Lambda::DisableAllLambdasRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "disable_all_lambdas");
        log_debug << "Disable all lambdas request, region: " << request.region;

        // Get lambda
        for (std::vector<Database::Entity::Lambda::Lambda> lambdas = _lambdaDatabase.ListLambdas(request.region); auto &lambda: lambdas) {
            lambda.enabled = false;
            lambda = _lambdaDatabase.UpdateLambda(lambda);
        }
        log_debug << "All lambdas disabled, region: " << request.region;
    }

    void LambdaService::StartLambda(const Dto::Lambda::StartLambdaRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "start_function");
        log_debug << "Start function, functionArn: " + request.functionArn;

        if (!_lambdaDatabase.LambdaExistsByArn(request.functionArn)) {
            log_error << "Lambda function does not exist, functionArn: " << request.functionArn;
            throw Core::ServiceException("Lambda function does not exist, functionArn: " + request.functionArn);
        }

        // Get lambda function
        Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(request.functionArn);

        // Check state
        if (lambda.state == Database::Entity::Lambda::Active) {
            log_info << "Lambda function already running, functionArn: " << request.functionArn;
            return;
        }

        // Update state
        lambda.state = Database::Entity::Lambda::Pending;
        lambda = _lambdaDatabase.UpdateLambda(lambda);

        // Create mutex
        if (!_instanceMutex.contains(lambda.function)) {
            _instanceMutex[lambda.function] = std::make_shared<boost::mutex>();
        }

        // Create the lambda function asynchronously
        const std::string instanceId = Core::StringUtils::GenerateRandomHexString(8);
        const LambdaCreator lambdaCreator;
        lambdaCreator.CreateLambda(lambda, instanceId);

        log_info << "Lambda function started, functionArn: " + lambda.arn;
    }

    void LambdaService::StartAllLambdas() const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "start_all_lambdas");
        log_debug << "Start all lambdas";

        for (const auto &lambda: _lambdaDatabase.ListLambdas()) {
            Dto::Lambda::StartLambdaRequest request;
            request.functionArn = lambda.arn;
            request.region = lambda.region;
            StartLambda(request);
        }
        log_info << "All lambda function started";
    }

    void LambdaService::StopLambda(const Dto::Lambda::StopLambdaRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "stop_function");
        log_debug << "Stop function, functionArn: " + request.functionArn;

        if (!_lambdaDatabase.LambdaExistsByArn(request.functionArn)) {
            log_error << "Lambda function does not exist, functionArn: " << request.functionArn;
            throw Core::ServiceException("Lambda function does not exist, functionArn: " + request.functionArn);
        }

        // Get lambda function
        Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(request.functionArn);

        // Check state
        if (lambda.state == Database::Entity::Lambda::Inactive) {
            log_info << "Lambda function already inactive, functionArn: " << request.functionArn;
            return;
        }

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
        lambda.state = Database::Entity::Lambda::Inactive;
        lambda = _lambdaDatabase.UpdateLambda(lambda);

        // Prune containers
        dockerService.PruneContainers();
        log_info << "Lambda function stopped, functionArn: " + lambda.arn;
    }

    void LambdaService::StopAllLambdas() const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "stop_all_lambdas");
        log_debug << "Stop all lambdas";

        for (const auto &lambda: _lambdaDatabase.ListLambdas()) {
            Dto::Lambda::StopLambdaRequest request;
            request.functionArn = lambda.arn;
            request.region = lambda.region;
            StopLambda(request);
        }
        log_info << "All lambda function stoped";
    }

    void LambdaService::StopLambdaInstance(const Dto::Lambda::StopLambdaInstanceRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "stop_instance");
        log_debug << "Stop instance, functionArn: " + request.functionArn << ", instanceId: " + request.instanceId;

        if (!_lambdaDatabase.LambdaExistsByArn(request.functionArn)) {
            log_error << "Lambda function does not exist, functionArn: " << request.functionArn;
            throw Core::ServiceException("Lambda function does not exist, functionArn: " + request.functionArn);
        }

        // Get lambda function
        Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(request.functionArn);

        // Check state
        if (lambda.state == Database::Entity::Lambda::Inactive) {
            log_info << "Lambda function not running, functionArn: " << request.functionArn;
            return;
        }

        // Delete the containers, if existing
        const ContainerService &dockerService = ContainerService::instance();
        for (const auto &instance: lambda.instances) {
            if (instance.instanceId == request.instanceId && dockerService.ContainerExists(instance.containerId)) {
                Dto::Docker::Container container = dockerService.GetContainerById(instance.containerId);
                dockerService.StopContainer(container.id);
                dockerService.DeleteContainer(container);
                log_debug << "Docker container stopped and deleted, containerId: " + container.id;
            }
        }

        // Update state
        lambda.RemoveInstance(request.instanceId);
        lambda = _lambdaDatabase.UpdateLambda(lambda);

        // Prune containers
        dockerService.PruneContainers();
        log_info << "Lambda instance stopped, functionArn: " + lambda.arn << ", instanceId: " + request.instanceId;
    }

    void LambdaService::DeleteImage(const Dto::Lambda::DeleteImageRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "delete_image");
        log_debug << "Delete image, functionArn: " + request.functionArn;

        if (!_lambdaDatabase.LambdaExistsByArn(request.functionArn)) {
            log_error << "Lambda function does not exist, functionArn: " << request.functionArn;
            throw Core::ServiceException("Lambda function does not exist, functionArn: " + request.functionArn);
        }

        // Get lambda function
        Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(request.functionArn);

        // Cleanup docker
        CleanupDocker(lambda);

        // Update state
        lambda.state = Database::Entity::Lambda::Inactive;
        lambda = _lambdaDatabase.UpdateLambda(lambda);

        // Prune containers
        ContainerService::instance().PruneContainers();
        log_info << "Lambda function stopped, functionArn: " + lambda.arn;
    }

    void LambdaService::DeleteFunction(const Dto::Lambda::DeleteFunctionRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "delete_function");
        log_debug << "Delete function: " + request.ToString();

        ContainerService &dockerService = ContainerService::instance();

        if (!_lambdaDatabase.LambdaExists(request.functionName)) {
            log_error << "Lambda function does not exist, function: " + request.functionName;
            throw Core::ServiceException("Lambda function does not exist");
        }

        // Delete the containers, if existing
        const Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByName(request.region, request.functionName);
        for (const auto &instance: lambda.instances) {
            if (dockerService.ContainerExists(instance.containerId)) {
                Dto::Docker::Container container = dockerService.GetContainerById(instance.containerId);
                dockerService.StopContainer(container.id);
                dockerService.DeleteContainer(container);
                log_debug << "Docker container deleted, function: " + request.functionName;
            }
        }

        // Delete the image, if existing
        if (dockerService.ImageExists(request.functionName, lambda.dockerTag)) {
            Dto::Docker::Image image = dockerService.GetImageByName(request.functionName, lambda.dockerTag);
            dockerService.DeleteImage(image.id);
            log_debug << "Docker image deleted, function: " + request.functionName;
        }

        // Prune containers
        dockerService.PruneContainers();
        log_debug << "Docker image deleted, function: " + request.functionName;

        _lambdaDatabase.DeleteLambda(request.functionName);
        log_info << "Lambda function deleted, function: " + request.functionName;
    }

    void LambdaService::DeleteTags(const Dto::Lambda::DeleteTagsRequest &request) const {
        Monitoring::MonitoringTimer measure(LAMBDA_SERVICE_TIMER, LAMBDA_SERVICE_COUNTER, "action", "delete_tags");
        log_trace << "Delete tags: " + request.ToString();

        if (!_lambdaDatabase.LambdaExistsByArn(request.arn)) {
            log_error << "Lambda function does not exist, arn: " + request.arn;
            throw Core::ServiceException("Lambda function does not exist");
        }

        // Get the existing entity
        Dto::Lambda::ListTagsResponse response;
        Database::Entity::Lambda::Lambda lambdaEntity = _lambdaDatabase.GetLambdaByArn(request.arn);
        long count = 0;
        for (const auto &tag: request.tags) {
            count += static_cast<long>(std::erase_if(lambdaEntity.tags, [tag](const auto &item) {
                auto const &[k, v] = item;
                return k == tag;
            }));
        }
        lambdaEntity = _lambdaDatabase.UpdateLambda(lambdaEntity);
        log_debug << "Delete tag request succeeded, arn: " + lambdaEntity.arn << " deleted: " << count;
    }

    void LambdaService::FindIdleInstance(Database::Entity::Lambda::Lambda &lambda, Database::Entity::Lambda::Instance &instance) const {
        boost::mutex::scoped_lock lock(*_instanceMutex[lambda.function]);

        // Synchronize the docker daemon with the DB
        lambda = _lambdaDatabase.GetLambdaByArn(lambda.arn);

        // Check existing instances
        if (lambda.HasIdleInstance()) {

            instance = lambda.GetIdleInstance();

        } else if (lambda.instances.size() < lambda.concurrency) {

            // Create instance
            std::string instanceId = Core::StringUtils::GenerateRandomHexString(8);

            // Create lambda
            LambdaCreator lambdaCreator;
            lambda = lambdaCreator.CreateLambda(lambda, instanceId);
            log_info << "New lambda instance created, name: " << lambda.function << ", instanceId: " << instanceId << ", totalSize: " << lambda.instances.size();
            instance = lambda.GetInstance(instanceId);

        } else {

            // Wait for an idle instance
            instance = WaitForIdleInstance(lambda);
        }
        _lambdaDatabase.SetInstanceValues(instance.containerId, Database::Entity::Lambda::InstanceRunning);
        log_info << "Lambda instances: name: " << lambda.function << ", count(running/idle/total): " << lambda.CountRunningInstances() << "/" << lambda.CountIdleInstances() << "/" << lambda.instances.size();
    }

    Database::Entity::Lambda::Instance LambdaService::WaitForIdleInstance(Database::Entity::Lambda::Lambda &lambda) const {
        const system_clock::time_point deadline = system_clock::now() + std::chrono::seconds(lambda.timeout);

        lambda = _lambdaDatabase.GetLambdaByArn(lambda.arn);
        while (!lambda.HasIdleInstance() && system_clock::now() < deadline) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            lambda = _lambdaDatabase.GetLambdaByArn(lambda.arn);
        }
        return lambda.GetIdleInstance();
    }

    std::string LambdaService::GetLambdaCodePath(const Database::Entity::Lambda::Lambda &lambda) {
        const auto lambdaDir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.lambda.data-dir");
        return lambdaDir + Core::FileUtils::separator() + lambda.function + "-" + lambda.dockerTag + ".b64";
    }

    std::string LambdaService::GetLambdaCodeFromS3(const Database::Entity::Lambda::Lambda &lambda) const {
        const auto s3DataDir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.s3.data-dir");

        const Database::Entity::S3::Object object = _s3Database.GetObject(lambda.region, lambda.code.s3Bucket, lambda.code.s3Key);
        return s3DataDir + Core::FileUtils::separator() + object.internalName;
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

            if (!_s3Database.BucketExists(request.eventSourceArn)) {
                log_error << "S3 bucket does not exist: " << request.eventSourceArn;
                throw Core::ServiceException("S3 bucket does not exist: " + request.eventSourceArn);
            }
            Database::Entity::S3::Bucket bucket = _s3Database.GetBucketByArn(request.eventSourceArn);

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
            _s3Database.CreateOrUpdateBucket(bucket);

        } else if (request.type == "SQS") {

            if (!_sqsDatabase.QueueArnExists(request.eventSourceArn)) {
                log_error << "SQS queue does not exist: " << request.eventSourceArn;
                throw Core::ServiceException("Bucket does not exist: " + request.eventSourceArn);
            }

            Database::Entity::SQS::Queue queue = _sqsDatabase.GetQueueByArn(request.eventSourceArn);

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
            _sqsDatabase.CreateOrUpdateQueue(queue);

        } else if (request.type == "SNS") {

            if (!_snsDatabase.TopicExists(request.eventSourceArn)) {
                log_error << "SNS topic does not exist: " << request.eventSourceArn;
                throw Core::ServiceException("Bucket does not exist: " + request.eventSourceArn);
            }

            Database::Entity::SNS::Topic topic = _snsDatabase.GetTopicByArn(request.eventSourceArn);

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
            _snsDatabase.CreateOrUpdateTopic(topic);
        }
    }

    void LambdaService::WriteBase64File(const std::string &base64File, const std::string &content) {

        auto lambdaDir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.lambda.data-dir");
        std::string base64FullFile = lambdaDir + Core::FileUtils::separator() + base64File;
        log_debug << "Using Base64File: " << base64FullFile;

        std::ofstream ofs(base64FullFile);
        ofs << content;
        ofs.close();
        log_debug << "New Base64 file written: " << content;
    }
}// namespace AwsMock::Service
