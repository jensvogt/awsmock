//
// Created by vogje01 on 30/05/2023.
//
#include <awsmock/service/lambda/LambdaService.h>

namespace AwsMock::Service {
    boost::mutex LambdaService::_mutex;

    Dto::Lambda::CreateFunctionResponse LambdaService::CreateFunction(Dto::Lambda::CreateFunctionRequest &request) const {
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "method", "create_function");
        log_debug << "Create function request, name: " << request.functionName;

        const std::string accountId = Core::Configuration::instance().GetValueString("awsmock.access.account-id");

        // Create entity and set ARN
        Database::Entity::Lambda::Lambda lambdaEntity;
        const std::string lambdaArn = Core::AwsUtils::CreateLambdaArn(request.region, accountId, request.functionName);

        std::string zippedCode;
        if (_lambdaDatabase.LambdaExists(request.region, request.functionName, request.runtime)) {

            // Get the existing entity
            lambdaEntity = _lambdaDatabase.GetLambdaByArn(lambdaArn);
            const std::string fileName = GetLambdaCodePath(lambdaEntity);
            if (!Core::FileUtils::FileExists(fileName)) {
                throw Core::ServiceException("Lambda base64 encoded code does not exists, fileName: " + fileName);
            }
            zippedCode = Core::FileUtils::ReadFile(fileName);

        } else {

            Database::Entity::Lambda::Environment environment = {.variables = request.environment.variables};
            lambdaEntity = Dto::Lambda::Mapper::map(request);
            lambdaEntity.arn = lambdaArn;

            // Remove code
            if (!request.code.zipFile.empty()) {
                zippedCode = std::move(request.code.zipFile);
                lambdaEntity.code.zipFile.clear();
            }
            lambdaEntity.code.zipFile = GetLambdaCodePath(lambdaEntity);
        }

        // Update database
        lambdaEntity.timeout = request.timeout;
        lambdaEntity.state = Database::Entity::Lambda::LambdaState::Pending;
        lambdaEntity.stateReason = "Initializing";
        lambdaEntity.stateReasonCode = Database::Entity::Lambda::LambdaStateReasonCode::Creating;
        lambdaEntity = _lambdaDatabase.CreateOrUpdateLambda(lambdaEntity);

        // Create lambda function asynchronously
        const std::string instanceId = Core::StringUtils::GenerateRandomHexString(8);
        LambdaCreator lambdaCreator;
        boost::thread t(boost::ref(lambdaCreator), zippedCode, lambdaEntity.oid, instanceId);
        t.detach();
        log_debug << "Lambda create started, function: " << lambdaEntity.function;

        // Create response
        Dto::Lambda::CreateFunctionResponse response = Dto::Lambda::Mapper::map(request, lambdaEntity);
        log_info << "Function created, name: " << request.functionName << " status: " << lambdaEntity.state;

        return response;
    }

    void LambdaService::UploadFunctionCode(const Dto::Lambda::UploadFunctionCodeRequest &request) const {
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "method", "upload_function_code");
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

        lambda.state = Database::Entity::Lambda::LambdaState::Pending;
        lambda.dockerTag = request.version;
        lambda.stateReason = "Initializing";
        lambda.stateReasonCode = Database::Entity::Lambda::LambdaStateReasonCode::Creating;
        lambda = _lambdaDatabase.UpdateLambda(lambda);

        // Create lambda function asynchronously
        const std::string instanceId = Core::StringUtils::GenerateRandomHexString(8);
        LambdaCreator lambdaCreator;
        boost::thread t(boost::ref(lambdaCreator), request.functionCode, lambda.oid, instanceId);
        t.detach();

        log_debug << "Lambda function code updated, function: " << lambda.function;
    }

    Dto::Lambda::ListFunctionResponse LambdaService::ListFunctions(const std::string &region) const {
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "method", "list_functions");
        log_debug << "List functions request, region: " << region;

        try {
            const std::vector<Database::Entity::Lambda::Lambda> lambdas = _lambdaDatabase.ListLambdas(region);

            auto response = Dto::Lambda::ListFunctionResponse(lambdas);
            log_debug << "Lambda list outcome: " << response.ToJson();
            return response;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Lambda::ListFunctionCountersResponse LambdaService::ListFunctionCounters(const Dto::Lambda::ListFunctionCountersRequest &request) const {
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "method", "list_function_counters");
        log_debug << "List function counters request, region: " << request.region;

        try {

            const std::vector<Database::Entity::Lambda::Lambda> lambdas = _lambdaDatabase.ListLambdaCounters(request.region, request.prefix, request.pageSize, request.pageIndex, request.sortColumns);
            const long count = _lambdaDatabase.LambdaCount(request.region);

            Dto::Lambda::ListFunctionCountersResponse response = Dto::Lambda::Mapper::map(request, lambdas);
            response.total = count;

            log_trace << "Lambda list function counters, result: " << response.ToString();
            return response;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Lambda::ListLambdaTagCountersResponse LambdaService::ListLambdaTagCounters(const Dto::Lambda::ListLambdaTagCountersRequest &request) const {
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "method", "list_tag_counters");
        log_debug << "List lambda tag counters request, lambdaArn: " << request.lambdaArn;

        try {

            const Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(request.lambdaArn);

            Dto::Lambda::ListLambdaTagCountersResponse response;
            response.total = lambda.tags.size();

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

    void LambdaService::AddLambdaTag(const Dto::Lambda::AddFunctionTagRequest &request) const {
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "method", "add_lambda_tag");
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
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "method", "update_lambda_tag");
        log_debug << "Update lambda tag request, functionArn: " << request.functionArn << ", key: " << request.tagKey << ", value: " << request.tagValue;

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

    void LambdaService::DeleteLambdaTag(const Dto::Lambda::DeleteFunctionTagRequest &request) const {
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "method", "delete_lambda_tag");
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
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "method", "get_function");
        log_debug << "Get function request, region: " << region << " name: " << name;

        try {
            const Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByName(region, name);

            const Dto::Lambda::Configuration configuration = {
                    .functionName = lambda.function,
                    .handler = lambda.handler,
                    .runtime = lambda.runtime,
                    .lastUpdateStatus = "Successful",
                    .state = LambdaStateToString(lambda.state),
                    .stateReason = lambda.stateReason,
                    .stateReasonCode = LambdaStateReasonCodeToString(lambda.stateReasonCode)};

            Dto::Lambda::GetFunctionResponse response = {
                    .region = lambda.region,
                    .configuration = configuration,
                    .tags = lambda.tags};

            log_info << "Lambda function: " + response.ToJson();
            return response;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Lambda::GetFunctionCountersResponse LambdaService::GetFunctionCounters(const Dto::Lambda::GetFunctionCountersRequest &request) const {
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "method", "get_function_counters");
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
            response.concurrency = lambda.concurrency;
            response.invocations = lambda.invocations;
            response.averageRuntime = lambda.averageRuntime;
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
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "method", "reset_function_counters");
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

    std::string LambdaService::InvokeLambdaFunction(const std::string &functionName, const std::string &payload, const std::string &region, bool synchron) const {
        boost::mutex::scoped_lock lock(_mutex);
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "method", "invoke_lambda_function");
        log_debug << "Invocation lambda function, functionName: " << functionName;

        std::string accountId = Core::Configuration::instance().GetValueString("awsmock.access.account-id");
        std::string lambdaArn = Core::AwsUtils::CreateLambdaArn(region, accountId, functionName);

        // Get the lambda entity
        Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(lambdaArn);
        log_debug << "Got lambda entity, name: " << lambda.function;

        // Find an idle instance
        std::string instanceId = FindIdleInstance(lambda);
        if (instanceId.empty()) {

            // Check max concurrency
            if (lambda.instances.size() < lambda.concurrency) {
                // Create instance
                instanceId = Core::StringUtils::GenerateRandomHexString(8);
                LambdaCreator lambdaCreator;
                boost::thread t(boost::ref(lambdaCreator), lambda.code.zipFile, lambda.oid, instanceId);
                t.join();

                // Replace lambda
                lambda = _lambdaDatabase.GetLambdaByArn(lambdaArn);
                log_info << "New lambda instance created, name: " << functionName << ", totalSize: " << lambda.instances.size();
            } else {
                WaitForIdleInstance(lambda);
            }
        }

        Database::Entity::Lambda::Instance instance = lambda.GetInstance(instanceId);

        // Get the hostname, the hostname is different from a manager running as Linux host and a manager running as docker container.
        std::string hostName = GetHostname(instance);
        int port = GetContainerPort(instance);

        // Send invocation request
        std::string output;
        if (synchron) {

            // Synchronous execution
            output = InvokeLambdaSynchronously(hostName, port, payload, lambda.oid, instance.id);

        } else {

            // Asynchronous execution
            LambdaExecutor lambdaExecutor;
            boost::thread t(boost::ref(lambdaExecutor), lambda.oid, instance.containerId, hostName, port, payload, lambda.function);
            t.detach();
        }
        log_debug << "Lambda invocation notification send, name: " << lambda.function << " endpoint: " << instance.containerName << ":" << instance.hostPort;

        // Update database
        lambda = _lambdaDatabase.UpdateLambda(lambda);
        log_debug << "Lambda entity invoked, name: " << lambda.function;
        return output;
    }

    void LambdaService::CreateTag(const Dto::Lambda::CreateTagRequest &request) const {
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "method", "create_tag");
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
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "method", "list_tags");
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
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "method", "get_account_settings");
        log_debug << "Get account settings";

        Dto::Lambda::AccountSettingsResponse response;

        // 50 MB
        response.accountLimit.codeSizeUnzipped = 50 * 1024 * 1024L;
        response.accountLimit.codeSizeZipped = 50 * 1024 * 1024L;

        // 1000 concurrent executions (which is irrelevant in AwsMock environment)
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
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "method", "create_event_source_mapping");
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

        // Create response (which is actually the request)
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
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "method", "list_event_source_mapping");
        log_debug << "List event source mappings, functionName: " << request.functionName << " sourceArn: " << request.eventSourceArn;

        if (!_lambdaDatabase.LambdaExists(request.functionName)) {
            log_warning << "Lambda function does not exist, function: " << request.functionName;
            throw Core::NotFoundException("Lambda function does not exist");
        }

        // Get the existing entity
        const Database::Entity::Lambda::Lambda lambdaEntity = _lambdaDatabase.GetLambdaByName(
                request.region,
                request.functionName);

        return Dto::Lambda::Mapper::map(lambdaEntity.arn, lambdaEntity.eventSources);
    }

    void LambdaService::DeleteFunction(const Dto::Lambda::DeleteFunctionRequest &request) const {
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "method", "delete_function");
        log_debug << "Delete function: " + request.ToString();

        ContainerService &dockerService = ContainerService::instance();

        if (!_lambdaDatabase.LambdaExists(request.functionName)) {
            log_error << "Lambda function does not exist, function: " + request.functionName;
            throw Core::ServiceException("Lambda function does not exist");
        }

        // Delete the containers, if existing
        Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByName(request.region, request.functionName);
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

    void LambdaService::DeleteTags(Dto::Lambda::DeleteTagsRequest &request) const {
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "method", "delete_tags");
        log_trace << "Delete tags: " + request.ToString();

        if (!_lambdaDatabase.LambdaExistsByArn(request.arn)) {
            log_error << "Lambda function does not exist, arn: " + request.arn;
            throw Core::ServiceException("Lambda function does not exist");
        }

        // Get the existing entity
        Dto::Lambda::ListTagsResponse response;
        Database::Entity::Lambda::Lambda lambdaEntity = _lambdaDatabase.GetLambdaByArn(request.arn);
        int count = 0;
        for (const auto &tag: request.tags) {
            count += std::erase_if(lambdaEntity.tags,
                                   [tag](const auto &item) {
                                       auto const &[k, v] = item;
                                       return k == tag;
                                   });
        }
        lambdaEntity = _lambdaDatabase.UpdateLambda(lambdaEntity);
        log_debug << "Delete tag request succeeded, arn: " + lambdaEntity.arn << " deleted: " << count;
    }

    std::string LambdaService::InvokeLambdaSynchronously(const std::string &host, const int port, const std::string &payload, const std::string &oid, const std::string &containerId) {
        Monitoring::MetricServiceTimer measure(LAMBDA_INVOCATION_TIMER);
        Monitoring::MetricService::instance().IncrementCounter(LAMBDA_INVOCATION_COUNT);
        log_debug << "Sending lambda invocation request, endpoint: " << host << ":" << port;

        // Set status
        Database::LambdaDatabase::instance().SetInstanceStatus(containerId, Database::Entity::Lambda::InstanceRunning);
        Database::LambdaDatabase::instance().SetLastInvocation(oid, system_clock::now());
        const system_clock::time_point start = system_clock::now();

        const Core::HttpSocketResponse response = Core::HttpSocket::SendJson(http::verb::post, host, port, "/", payload, {});
        if (response.statusCode != http::status::ok) {
            log_error << "HTTP error, httpStatus: " << response.statusCode << " body: " << response.body << " payload: " << payload;
            Database::LambdaDatabase::instance().SetInstanceStatus(containerId, Database::Entity::Lambda::InstanceFailed);
        }

        // Set status
        Database::LambdaDatabase::instance().SetInstanceStatus(containerId, Database::Entity::Lambda::InstanceIdle);
        Database::LambdaDatabase::instance().SetAverageRuntime(oid, std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now() - start).count());

        log_debug << "Lambda invocation finished send, status: " << response.statusCode;
        log_info << "Lambda output: " << response.body;
        return response.body.substr(0, MAX_OUTPUT_LENGTH);
    }

    std::string LambdaService::FindIdleInstance(Database::Entity::Lambda::Lambda &lambda) {
        if (lambda.instances.empty()) {
            log_debug << "No idle instances found";
            return {};
        }
        for (const auto &instance: lambda.instances) {
            if (instance.status == Database::Entity::Lambda::InstanceIdle) {
                log_debug << "Found idle instance, id: " << instance.id;
                return instance.id;
            }
        }
        return {};
    }

    std::string LambdaService::GetHostname(Database::Entity::Lambda::Instance &instance) {
        return Core::Configuration::instance().GetValueBool("awsmock.dockerized") ? instance.containerName : "localhost";
    }

    int LambdaService::GetContainerPort(const Database::Entity::Lambda::Instance &instance) {
        return Core::Configuration::instance().GetValueBool("awsmock.dockerized") ? 8080 : instance.hostPort;
    }

    void LambdaService::WaitForIdleInstance(Database::Entity::Lambda::Lambda &lambda) {
        const system_clock::time_point deadline = system_clock::now() + std::chrono::seconds(lambda.timeout);
        while (!lambda.HasIdleInstance() && system_clock::now() < deadline) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    std::string LambdaService::GetLambdaCodePath(const Database::Entity::Lambda::Lambda &lambda) {
        const std::string lambdaDir = Core::Configuration::instance().GetValueString("awsmock.modules.lambda.data-dir");
        return lambdaDir + "/" + lambda.function + "-" + lambda.dockerTag + ".b64";
    }

    void LambdaService::CleanupDocker(Database::Entity::Lambda::Lambda &lambda) {
        for (const auto &instance: lambda.instances) {
            ContainerService::instance().StopContainer(instance.containerId);
            ContainerService::instance().DeleteContainers(lambda.function, lambda.dockerTag);
        }
        ContainerService::instance().DeleteImage(lambda.imageId);
        log_debug << "Done cleanup instances, function: " << lambda.function;

        lambda.instances.clear();
        log_debug << "Done cleanup instances, function: " << lambda.function;

        ContainerService::instance().DeleteImage(lambda.imageId);
        log_debug << "Done cleanup image, function: " << lambda.function;

        log_info << "Done cleanup docker, function: " << lambda.function;
    }
}// namespace AwsMock::Service
