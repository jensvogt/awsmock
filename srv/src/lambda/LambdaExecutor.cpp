//
// Created by root on 10/9/23.
//

#include <awsmock/service/lambda/LambdaExecutor.h>

namespace AwsMock::Service {

    Database::Entity::Lambda::LambdaResult LambdaExecutor::Invocation(Database::Entity::Lambda::Lambda &lambda, Database::Entity::Lambda::Instance &instance, std::string &payload) const {
        Monitoring::MonitoringTimer measure(LAMBDA_INVOCATION_TIMER, LAMBDA_INVOCATION_COUNT, "function_name", lambda.function);
        log_debug << "Sending lambda invocation request, function: " << lambda.function << " endpoint: " << instance.hostName << ":" << instance.hostPort;

        // Send request to lambda docker container
        const system_clock::time_point start = system_clock::now();
        Core::HttpSocketResponse response = Core::HttpSocket::SendJson(http::verb::post, instance.hostName, instance.hostPort, "/2015-03-31/functions/function/invocations", payload);
        const long runtime = std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now() - start).count();

        // Get lambda log messages
        log_info << "Getting lambda logs, containerId: " << instance.containerId;
        std::string logs = _containerService.GetContainerLogs(instance.containerId, start);

        // update lambda
        lambda.invocations++;
        lambda.averageRuntime = static_cast<long>(std::ceil((lambda.averageRuntime + runtime) / lambda.invocations));
        _lambdaDatabase.SetInstanceValues(instance.containerId, Database::Entity::Lambda::InstanceIdle);
        _lambdaDatabase.SetLambdaValues(lambda, lambda.invocations, lambda.averageRuntime);

        // Sanitize logs
        logs = Core::StringUtils::RemoveColorCoding(logs);
        logs = Core::StringUtils::SanitizeUtf8(logs);
        logs = Core::StringUtils::RemoveUnprintableAscii(logs);

        // Prepare resultSend request to lambda docker container
        Database::Entity::Lambda::LambdaResult lambdaResult;
        lambdaResult.instanceId = instance.instanceId;
        lambdaResult.containerId = instance.containerId;
        lambdaResult.status = response.statusCode;
        lambdaResult.httpStatusCode = Core::HttpUtils::StatusCodeToString(response.statusCode);
        lambdaResult.lambdaStatus = response.statusCode == http::status::ok ? Database::Entity::Lambda::InstanceSuccess : Database::Entity::Lambda::InstanceFailed;
        lambdaResult.requestBody = payload;
        lambdaResult.responseBody = response.body;
        lambdaResult.logMessages = logs;
        lambdaResult.lambdaName = lambda.function;
        lambdaResult.lambdaArn = lambda.arn;
        lambdaResult.duration = runtime;
        _lambdaDatabase.CreateLambdaResult(lambdaResult);

        // Set Counters
        _monitoringCollector.SetGauge(LAMBDA_RUNTIME_TIMER, "function_name", lambda.function, runtime);

        log_debug << "Lambda invocation finished, lambda: " << lambda.function << " output: " << response.body;
        log_info << "Lambda invocation finished, lambda: " << lambda.function;
        return lambdaResult;
    }

}// namespace AwsMock::Service