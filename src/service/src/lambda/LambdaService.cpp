//
// Created by vogje01 on 30/05/2023.
//
// C++ Test JSON image-transformation:
//{ "Records" : [ { "awsRegion" : "eu-central-1", "messageId" : "642154cc-7ac4-4d0f-8c6d-ad05f4b1e3fe", "receiptHandle" : "fJBf02J5dcn1cdYFJeks52bDpVIsUslIb1El6GPigH6sXD3HG6eBeVY4jTSR8HhuozLQEwKoJGdnd3jfi8R1zRk34oCwLxiQZRWcAYEO8QW6x56Ng2neeBzKEekx4Zzqhi2kXR991CUM4h31mvsrLLG7i3IPdZvJsdpyGusvrD51YPGfGZR9tzjhct2r5niEshOPjgR0t50Wb5uIFNgM7uFABHpT9Vyrd3fuRXBcDbp7yYi4pEdww761c1ylPRbSz2TihlcYvfLooJ5T20NFy6KcnwLruuv4LCKmo9N6YGCYHgAl9HhOCisCFS2UVn6l1sHTJAqKkCPKyp6oZsN0VY8ykl74iph4HIyAW74feuGnvecyzq8McDVVygGoMeERlutOifxMcJSOzjicXAiBmMEYu9JEHiqQxnP4CF1G4IVyVxJNXn3uR6NzDLCdRKOGr11KdKLlhLQFJ7vXKx4DMTR8fIXGbbmaeYLGn6ARUxjqvpR2TQN1jLgtMdtQ3DPs", "body" : "{\"lieferantenId\":\"ftpuser1\",\"dateinameOriginal\":\"97815662_16122024063817059.jpg\",\"eingangsverzeichnis\":\"ftpuser1/97815662_16122024063817059.jpg\",\"eingangsdatum\":\"2024-12-16T06:38:17.0\",\"dateinameS3\":\"ftpuser1/97815662_16122024063817059.jpg\",\"ablageortS3\":\"file-delivery\",\"fileType\":null}", "md5OfBody" : "76af3472e032f1106f1e8356daf17db3", "eventSource" : "aws:sqs", "eventSourceARN" : "arn:aws:sqs:eu-central-1:000000000000:ftp-file-distribution-image-queue", "attributes" : { "ApproximateFirstReceivedTimestamp" : "1734327509", "ApproximateReceivedCount" : "0", "SenderId" : "none", "SentTimestamp" : "1734327509", "VisibilityTimeout" : "90" } } ] }
#include <awsmock/service/lambda/LambdaService.h>
#include <thread>

namespace AwsMock::Service {

    boost::mutex LambdaService::_mutex;

    Dto::Lambda::CreateFunctionResponse LambdaService::CreateFunction(Dto::Lambda::CreateFunctionRequest &request) const {
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "method", "create_function");
        log_debug << "Create function request, name: " << request.functionName;

        const std::string accountId = Core::Configuration::instance().GetValueString("awsmock.access.account-id");

        // Save to file
        Database::Entity::Lambda::Lambda lambdaEntity;
        const std::string lambdaArn = Core::AwsUtils::CreateLambdaArn(request.region, accountId, request.functionName);

        std::string zippedCode;
        if (_lambdaDatabase.LambdaExists(request.region, request.functionName, request.runtime)) {

            // Get the existing entity
            lambdaEntity = _lambdaDatabase.GetLambdaByArn(lambdaArn);

        } else {

            Database::Entity::Lambda::Environment environment = {.variables = request.environment.variables};
            lambdaEntity = Dto::Lambda::Mapper::map(request);
            lambdaEntity.arn = lambdaArn;

            // Remove code
            if (!request.code.zipFile.empty()) {
                zippedCode = std::move(request.code.zipFile);
                lambdaEntity.code.zipFile.clear();
            }
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
        log_info << "Function created, name: " << request.functionName;

        return response;
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

    Dto::Lambda::GetFunctionResponse LambdaService::GetFunction(const std::string &region, const std::string &name) const {
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "method", "get_function");
        log_debug << "Get function request, region: " << region << " name: " << name;

        try {

            const Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByName(region, name);

            const Dto::Lambda::Configuration configuration = {.functionName = lambda.function, .handler = lambda.handler, .runtime = lambda.runtime, .lastUpdateStatus = "Successful", .state = LambdaStateToString(lambda.state), .stateReason = lambda.stateReason, .stateReasonCode = LambdaStateReasonCodeToString(lambda.stateReasonCode)};
            Dto::Lambda::GetFunctionResponse response = {.region = lambda.region, .configuration = configuration, .tags = lambda.tags};

            log_info << "Lambda function: " + response.ToJson();
            return response;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Lambda::GetFunctionCountersResponse LambdaService::GetFunctionCounters(const Dto::Lambda::GetFunctionCountersRequest &request) const {
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "method", "get_function");
        log_debug << "Get function request, region: " << request.region << " name: " << request.functionName;

        try {

            const Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByName(request.region, request.functionName);

            Dto::Lambda::GetFunctionCountersResponse response;
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
            response.environment = lambda.environment.variables;

            log_info << "Lambda function: " + response.ToJson();
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
            log_info << "Reset lambda function counters";

        } catch (bsoncxx::exception &ex) {
            log_error << "Reset function counters request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    std::string LambdaService::InvokeLambdaFunction(const std::string &functionName, const std::string &payload, const std::string &region, const std::string &user, const std::string &logType) const {
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
                log_info << "New lambda instance created, totalSize: " << lambda.instances.size();
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
        if (!logType.empty() && Core::StringUtils::EqualsIgnoreCase(logType, "Tail")) {

            // Synchronous execution
            output = InvokeLambdaSynchronously(hostName, port, payload);

        } else {
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
        const Database::Entity::Lambda::Lambda lambdaEntity = _lambdaDatabase.GetLambdaByName(request.region, request.functionName);

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
        for (Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByName(request.region, request.functionName); const auto &instance: lambda.instances) {

            if (dockerService.ContainerExists(instance.id)) {

                Dto::Docker::Container container = dockerService.GetContainerById(instance.id);
                dockerService.StopContainer(container.id);
                dockerService.DeleteContainer(container);
                log_debug << "Docker container deleted, function: " + request.functionName;
            }
        }

        // Delete the image, if existing
        if (dockerService.ImageExists(request.functionName, request.qualifier)) {
            Dto::Docker::Image image = dockerService.GetImageByName(request.functionName, request.qualifier);
            dockerService.DeleteImage(image.id);
            log_debug << "Docker image deleted, function: " + request.functionName;
        }

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
            count += std::erase_if(lambdaEntity.tags, [tag](const auto &item) {
                auto const &[k, v] = item;
                return k == tag;
            });
        }
        lambdaEntity = _lambdaDatabase.UpdateLambda(lambdaEntity);
        log_debug << "Delete tag request succeeded, arn: " + request.arn << " deleted: " << count;
    }

    std::string LambdaService::InvokeLambdaSynchronously(const std::string &host, int port, const std::string &payload) {
        Monitoring::MetricServiceTimer measure(LAMBDA_INVOCATION_TIMER);
        Monitoring::MetricService::instance().IncrementCounter(LAMBDA_INVOCATION_COUNT);
        log_debug << "Sending lambda invocation request, endpoint: " << host << ":" << port;

        const Core::HttpSocketResponse response = Core::HttpSocket::SendJson(http::verb::post, host, port, "/", payload, {});
        if (response.statusCode != http::status::ok) {
            log_debug << "HTTP error, httpStatus: " << response.statusCode << " body: " << response.body;
        }
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
}// namespace AwsMock::Service
