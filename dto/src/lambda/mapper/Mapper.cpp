//
// Created by vogje01 on 5/10/24.
//

#include <awsmock/dto/lambda/mapper/Mapper.h>

namespace Awsmock::Dto::Lambda {

    Function Mapper::mapFunction(const Database::Entity::Lambda::Lambda &lambdaEntity) {

        Function function;
        function.functionName = lambdaEntity.function;
        function.functionArn = lambdaEntity.arn;
        function.codeSha256 = lambdaEntity.codeSha256;
        function.runtime = lambdaEntity.runtime;
        function.handler = lambdaEntity.handler;
        function.timeout = lambdaEntity.timeout;
        // function.state = Database::Entity::Lambda::LambdaStateToString(lambdaEntity.state);
        // function.stateReason = lambdaEntity.stateReason;
        // function.stateReasonCode = Database::Entity::Lambda::LambdaStateReasonCodeToString(lambdaEntity.stateReasonCode);

        // Environment
        function.environment.variables = lambdaEntity.environment.variables;

        return function;
    }

    CreateFunctionResponse Mapper::map(const CreateFunctionRequest &request, const Database::Entity::Lambda::Lambda &lambdaEntity) {

        CreateFunctionResponse response;
        response.region = request.region;
        response.user = request.user;
        response.functionArn = lambdaEntity.arn;
        response.functionName = lambdaEntity.function;
        response.runtime = lambdaEntity.runtime;
        response.role = lambdaEntity.role;
        response.handler = lambdaEntity.handler;
        response.memorySize = request.memorySize;
        response.codeSize = lambdaEntity.codeSize;
        response.timeout = lambdaEntity.timeout;
        response.codeSha256 = lambdaEntity.codeSha256;

        // Environment
        response.environment.variables = lambdaEntity.environment.variables;

        // Ephemeral storage
        response.ephemeralStorage.size = lambdaEntity.ephemeralStorage.size;

        return response;
    }

    CreateFunctionRequest Mapper::map(const Database::Entity::Lambda::Lambda &lambdaEntity) {

        CreateFunctionRequest request;
        request.region = lambdaEntity.region;
        request.user = lambdaEntity.user;
        request.functionName = lambdaEntity.function;
        request.runtime = lambdaEntity.runtime;
        request.role = lambdaEntity.role;
        request.handler = lambdaEntity.handler;
        request.memorySize = lambdaEntity.memorySize;
        request.timeout = lambdaEntity.timeout;

        // Environment
        request.environment.variables = lambdaEntity.environment.variables;

        // Ephemeral storage
        request.ephemeralStorage.size = lambdaEntity.ephemeralStorage.size;

        // Core
        request.code.zipFile = lambdaEntity.code.zipFile;
        request.code.s3Bucket = lambdaEntity.code.s3Bucket;
        request.code.s3Key = lambdaEntity.code.s3Key;
        request.code.s3ObjectVersion = lambdaEntity.code.s3ObjectVersion;

        return request;
    }

    Database::Entity::Lambda::Lambda Mapper::map(const CreateFunctionRequest &request) {
        Database::Entity::Lambda::Lambda lambda;
        lambda.region = request.region;
        lambda.user = request.user;
        lambda.function = request.functionName;
        lambda.handler = request.handler;
        lambda.role = request.role;
        lambda.environment.variables = request.environment.variables;
        lambda.runtime = request.runtime;
        lambda.code.zipFile = request.code.zipFile;
        lambda.code.s3Bucket = request.code.s3Bucket;
        lambda.code.s3Key = request.code.s3Key;
        lambda.code.s3ObjectVersion = request.code.s3ObjectVersion;
        lambda.tags = request.tags;
        return lambda;
    }

    Database::Entity::Lambda::EventSourceMapping Mapper::map(const CreateEventSourceMappingsRequest &request) {
        Database::Entity::Lambda::EventSourceMapping eventSourceMapping;
        eventSourceMapping.eventSourceArn = request.eventSourceArn;
        eventSourceMapping.batchSize = request.batchSize;
        eventSourceMapping.maximumBatchingWindowInSeconds = request.maximumBatchingWindowInSeconds;
        eventSourceMapping.enabled = request.enabled;
        return eventSourceMapping;
    }

    ListEventSourceMappingsResponse Mapper::map(const std::vector<Database::Entity::Lambda::EventSourceMapping> &eventSourceMappings) {
        ListEventSourceMappingsResponse response;
        for (auto &eventSourceMapping: eventSourceMappings) {
            EventSourceMapping eventSourceMappingDto;
            eventSourceMappingDto.type = eventSourceMapping.type;
            eventSourceMappingDto.eventSourceArn = eventSourceMapping.eventSourceArn;
            eventSourceMappingDto.batchSize = eventSourceMapping.batchSize;
            eventSourceMappingDto.maximumBatchingWindowInSeconds = eventSourceMapping.maximumBatchingWindowInSeconds;
            eventSourceMappingDto.uuid = eventSourceMapping.uuid;
            response.eventSourceMappings.emplace_back(eventSourceMappingDto);
        }
        return response;
    }

    EventSourceMapping Mapper::map(const std::string &functionArn, const Database::Entity::Lambda::EventSourceMapping &eventSourceMappings) {
        EventSourceMapping eventSourceMappingDto;
        eventSourceMappingDto.functionArn = functionArn;
        eventSourceMappingDto.type = eventSourceMappings.type;
        eventSourceMappingDto.eventSourceArn = eventSourceMappings.eventSourceArn;
        eventSourceMappingDto.batchSize = eventSourceMappings.batchSize;
        eventSourceMappingDto.maximumBatchingWindowInSeconds = eventSourceMappings.maximumBatchingWindowInSeconds;
        eventSourceMappingDto.uuid = eventSourceMappings.uuid;
        return eventSourceMappingDto;
    }

    std::vector<EventSourceMapping> Mapper::mapCounters(const std::string &functionArn, const std::vector<Database::Entity::Lambda::EventSourceMapping> &eventSourceMappingEntities) {
        std::vector<EventSourceMapping> eventSourceMappings;
        for (const auto &eventSourceMapping: eventSourceMappingEntities) {
            eventSourceMappings.emplace_back(map(functionArn, eventSourceMapping));
        }
        return eventSourceMappings;
    }

    ListFunctionCountersResponse Mapper::map(const std::vector<Database::Entity::Lambda::Lambda> &lambdaEntities) {
        ListFunctionCountersResponse response;
        for (auto &lambdaEntity: lambdaEntities) {
            FunctionCounter counter;
            counter.functionName = lambdaEntity.function;
            counter.functionArn = lambdaEntity.arn;
            counter.invocations = lambdaEntity.invocations;
            counter.runtime = lambdaEntity.runtime;
            counter.handler = lambdaEntity.handler;
            counter.zipFile = lambdaEntity.code.zipFile;
            counter.enabled = lambdaEntity.enabled;
            counter.active = !lambdaEntity.instances.empty();
            counter.version = lambdaEntity.dockerTag;
            counter.averageRuntime = lambdaEntity.avgDuration;
            counter.instances = static_cast<long>(lambdaEntity.instances.size());
            counter.created = lambdaEntity.created;
            counter.modified = lambdaEntity.modified;
            response.functionCounters.emplace_back(counter);
        }
        return response;
    }

    LambdaResultCounter Mapper::mapCounter(const Database::Entity::Lambda::LambdaResult &resultEntity) {
        LambdaResultCounter counter;
        counter.oid = resultEntity.oid;
        counter.lambdaArn = resultEntity.lambdaArn;
        counter.lambdaName = resultEntity.lambdaName;
        counter.runtime = resultEntity.runtime;
        counter.duration = resultEntity.duration;
        counter.instanceId = resultEntity.instanceId;
        counter.containerId = resultEntity.containerId;
        counter.requestBody = resultEntity.requestBody;
        counter.responseBody = resultEntity.responseBody;
        counter.logMessages = resultEntity.logMessages;
        // counter.lambdaStatus = resultEntity.lambdaStatus;
        counter.httpStatusCode = resultEntity.httpStatusCode;
        counter.timestamp = resultEntity.timestamp;
        return counter;
    }

    ListLambdaResultCountersResponse Mapper::map(const std::vector<Database::Entity::Lambda::LambdaResult> &lambdaResultEntities) {
        ListLambdaResultCountersResponse response;
        for (auto &lambdaResultEntity: lambdaResultEntities) {
            response.lambdaResultCounters.emplace_back(mapCounter(lambdaResultEntity));
        }
        return response;
    }

    GetLambdaResultCounterResponse Mapper::map(const Database::Entity::Lambda::LambdaResult &resultEntity) {
        GetLambdaResultCounterResponse response;
        response.lambdaResultCounter = mapCounter(resultEntity);
        return response;
    }

    LambdaResult Mapper::mapResult(const Database::Entity::Lambda::LambdaResult &resultEntity) {
        LambdaResult result;
        result.functionArn = resultEntity.lambdaArn;
        result.responseBody = resultEntity.responseBody;
        result.status = static_cast<int>(resultEntity.status);
        return result;
    }

    Database::Entity::Lambda::RuntimeStatus Mapper::mapRuntimeStatus(const LambdaRuntimeStatus status) {
        switch (status) {
            case LambdaRuntimeStatus::starting:
                return Database::Entity::Lambda::RuntimeStatus::starting;
            case LambdaRuntimeStatus::idle:
                return Database::Entity::Lambda::RuntimeStatus::idle;
            case LambdaRuntimeStatus::running:
                return Database::Entity::Lambda::RuntimeStatus::running;
            case LambdaRuntimeStatus::failed:
                return Database::Entity::Lambda::RuntimeStatus::failed;
            case LambdaRuntimeStatus::stopped:
                return Database::Entity::Lambda::RuntimeStatus::stopped;
            case LambdaRuntimeStatus::unknown:
                return Database::Entity::Lambda::RuntimeStatus::unknown;
        }
        return Database::Entity::Lambda::RuntimeStatus::unknown;
    }

    LambdaRuntimeStatus Mapper::mapRuntimeStatus(const Database::Entity::Lambda::RuntimeStatus status) {
        switch (status) {
            case Database::Entity::Lambda::RuntimeStatus::starting:
                return LambdaRuntimeStatus::starting;
            case Database::Entity::Lambda::RuntimeStatus::idle:
                return LambdaRuntimeStatus::idle;
            case Database::Entity::Lambda::RuntimeStatus::running:
                return LambdaRuntimeStatus::running;
            case Database::Entity::Lambda::RuntimeStatus::failed:
                return LambdaRuntimeStatus::failed;
            case Database::Entity::Lambda::RuntimeStatus::stopped:
            case Database::Entity::Lambda::RuntimeStatus::unknown:
                return LambdaRuntimeStatus::stopped;
        }
        return LambdaRuntimeStatus::stopped;
    }
}// namespace Awsmock::Dto::Lambda