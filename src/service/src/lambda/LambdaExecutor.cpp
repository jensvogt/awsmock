//
// Created by root on 10/9/23.
//

#include "awsmock/service/container/ContainerService.h"


#include <awsmock/service/lambda/LambdaExecutor.h>

namespace AwsMock::Service {

    void LambdaExecutor::operator()(const Database::Entity::Lambda::Lambda &lambda, const std::string &containerId, const std::string &host, const int port, const std::string &payload, const std::string &functionName, const std::string &receiptHandle) const {

        // Initialize shared memory
        auto _segment = boost::interprocess::managed_shared_memory(boost::interprocess::open_only, SHARED_MEMORY_SEGMENT_NAME);
        Database::LambdaCounterMapType *_lambdaCounterMap = _segment.find<Database::LambdaCounterMapType>(Database::LAMBDA_COUNTER_MAP_NAME).first;

        Monitoring::MetricServiceTimer measure(LAMBDA_INVOCATION_TIMER, "function_name", functionName);
        Monitoring::MetricService::instance().IncrementCounter(LAMBDA_INVOCATION_COUNT, "function_name", functionName);
        log_debug << "Sending lambda invocation request, function: " << functionName << " endpoint: " << host << ":" << port;
        log_trace << "Sending lambda invocation request, payload: " << payload;

        // Set status
        Database::LambdaDatabase::instance().SetInstanceStatus(containerId, Database::Entity::Lambda::InstanceRunning);
        Database::LambdaDatabase::instance().SetLastInvocation(lambda.oid, system_clock::now());
        const system_clock::time_point start = system_clock::now();
        (*_lambdaCounterMap)[lambda.arn].invocations++;

        // Send request to lambda docker container
        Database::Entity::Lambda::LambdaResult result;
        result.containerId = containerId;
        const Core::HttpSocketResponse response = Core::HttpSocket::SendJson(http::verb::post, host, port, "/2015-03-31/functions/function/invocations", payload);

        // Calculate runtime
        const long runtime = std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now() - start).count();
        Database::LambdaDatabase::instance().SetAverageRuntime(lambda.oid, runtime);

        if (response.statusCode != http::status::ok) {

            log_error << "HTTP error, httpStatus: " << response.statusCode << ", responseBody: " << response.body << ", requestBody: " << payload;
            Database::LambdaDatabase::instance().SetInstanceStatus(containerId, Database::Entity::Lambda::InstanceFailed);

            result.httpStatusCode = Core::HttpUtils::StatusCodeToString(response.statusCode);
            result.lambdaStatus = Database::Entity::Lambda::InstanceFailed;

        } else {

            // Reset status
            Database::LambdaDatabase::instance().SetInstanceStatus(containerId, Database::Entity::Lambda::InstanceIdle);
            log_debug << "Lambda invocation finished, function: " << functionName << " httpStatus: " << response.statusCode;

            if (Core::StringUtils::Contains(response.body, "success") && !receiptHandle.empty()) {
                const long deleted = Database::SQSDatabase::instance().DeleteMessage(receiptHandle);
                log_info << "SQS messages deleted, count: " << deleted;
            }
            result.httpStatusCode = Core::HttpUtils::StatusCodeToString(response.statusCode);
            result.lambdaStatus = Database::Entity::Lambda::InstanceIdle;
        }

        // Get log messages
        const std::string logs = ContainerService::instance().GetContainerLogs(containerId, start);

        // Save results
        result.requestBody = payload;
        result.responseBody = response.body;
        result.logMessages = Core::StringUtils::RemoveColorCoding(logs);
        result.lambdaName = functionName;
        result.lambdaArn = lambda.arn;
        result.duration = runtime;
        Database::LambdaDatabase::instance().CreateLambdaResult(result);

        (*_lambdaCounterMap)[lambda.arn].invocations++;
        (*_lambdaCounterMap)[lambda.arn].averageRuntime += runtime;

        log_debug << "Lambda invocation finished, lambda: " << functionName << " output: " << response.body;
        log_info << "Lambda invocation finished, lambda: " << functionName;
    }

}// namespace AwsMock::Service