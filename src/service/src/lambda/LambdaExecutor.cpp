//
// Created by root on 10/9/23.
//

#include <awsmock/service/lambda/LambdaExecutor.h>

namespace AwsMock::Service {

    void LambdaExecutor::operator()(const Database::Entity::Lambda::Lambda &lambda, const std::string &containerId, const std::string &host, const int port, const std::string &payload, const std::string &functionName, const std::string &receiptHandle) const {

        Monitoring::MetricServiceTimer measure(LAMBDA_INVOCATION_TIMER, "function_name", functionName);
        Monitoring::MetricService::instance().IncrementCounter(LAMBDA_INVOCATION_COUNT, "function_name", functionName);
        log_debug << "Sending lambda invocation request, function: " << functionName << " endpoint: " << host << ":" << port;
        log_trace << "Sending lambda invocation request, payload: " << payload;

        // Set status
        Database::LambdaDatabase::instance().SetInstanceStatus(containerId, Database::Entity::Lambda::InstanceRunning);
        Database::LambdaDatabase::instance().SetLastInvocation(lambda.oid, system_clock::now());
        const system_clock::time_point start = system_clock::now();

        // Send request to lambda docker container
        Database::Entity::Lambda::LambdaResult result;
        const Core::HttpSocketResponse response = Core::HttpSocket::SendJson(http::verb::post, host, port, "/2015-03-31/functions/function/invocations", payload);
        if (response.statusCode != http::status::ok) {

            log_error << "HTTP error, httpStatus: " << response.statusCode << ", responseBody: " << response.body << ", requestBody: " << payload;
            Database::LambdaDatabase::instance().SetInstanceStatus(containerId, Database::Entity::Lambda::InstanceFailed);
            Database::LambdaDatabase::instance().SetAverageRuntime(lambda.oid, std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now() - start).count());

            result.httpStatusCode = Core::HttpUtils::StatusCodeToString(response.statusCode);
            result.lambdaStatus = Database::Entity::Lambda::InstanceFailed;

        } else {

            // Reset status
            Database::LambdaDatabase::instance().SetInstanceStatus(containerId, Database::Entity::Lambda::InstanceIdle);
            Database::LambdaDatabase::instance().SetAverageRuntime(lambda.oid, std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now() - start).count());
            log_debug << "Lambda invocation finished, function: " << functionName << " httpStatus: " << response.statusCode;

            if (Core::StringUtils::Contains(response.body, "success") && !receiptHandle.empty()) {
                const long deleted = Database::SQSDatabase::instance().DeleteMessage(receiptHandle);
                log_info << "SQS messages deleted, count: " << deleted;
            }
            result.httpStatusCode = Core::HttpUtils::StatusCodeToString(response.statusCode);
            result.lambdaStatus = Database::Entity::Lambda::InstanceIdle;
        }

        // Save results
        result.requestBody = payload;
        result.responseBody = response.body;
        result.lambdaName = functionName;
        result.lambdaArn = lambda.arn;
        result.runtime = lambda.runtime;
        Database::LambdaDatabase::instance().CreateLambdaResult(result);

        log_debug << "Lambda invocation finished, lambda: " << functionName << " output: " << response.body;
        log_info << "Lambda invocation finished, lambda: " << functionName;
    }

}// namespace AwsMock::Service