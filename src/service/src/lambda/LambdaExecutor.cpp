//
// Created by root on 10/9/23.
//

#include <awsmock/service/lambda/LambdaExecutor.h>

namespace AwsMock::Service {

    Database::Entity::Lambda::LambdaResult LambdaExecutor::Invocation(Database::Entity::Lambda::Lambda &lambda, std::string &containerId, std::string &host, const int port, std::string &payload) const {
        boost::interprocess::named_mutex _mutex{boost::interprocess::open_or_create, containerId.c_str()};

        Monitoring::MetricServiceTimer measure(LAMBDA_INVOCATION_TIMER, "function_name", lambda.function);
        _metricService.IncrementCounter(LAMBDA_INVOCATION_COUNT, "function_name", lambda.function);

        // Initialize shared memory
        auto _segment = boost::interprocess::managed_shared_memory(boost::interprocess::open_only, SHARED_MEMORY_SEGMENT_NAME);
        Database::LambdaCounterMapType *lambdaCounterMap = _segment.find<Database::LambdaCounterMapType>(Database::LAMBDA_COUNTER_MAP_NAME).first;

        // Monitoring
        log_debug << "Sending lambda invocation request, function: " << lambda.function << " endpoint: " << host << ":" << port;
        log_trace << "Sending lambda invocation request, payload: " << payload;

        // Prepare resultSend request to lambda docker container
        Database::Entity::Lambda::LambdaResult lambdaResult;
        lambdaResult.containerId = containerId;

        // Send request to lambda docker container
        Core::HttpSocketResponse response;
        const system_clock::time_point start = system_clock::now();
        {
            boost::interprocess::scoped_lock lock(_mutex, boost::interprocess::defer_lock);
            response = Core::HttpSocket::SendJson(http::verb::post, host, port, "/2015-03-31/functions/function/invocations", payload);
        }
        const long runtime = std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now() - start).count();

        // Get lambda log messages
        log_info << "Getting lambda logs, containerId: " << containerId;
        const std::string logs = _containerService.GetContainerLogs(containerId, start);

        // Save results
        _lambdaDatabase.SetAverageRuntime(lambda.oid, runtime);
        _lambdaDatabase.SetInstanceStatus(containerId, Database::Entity::Lambda::InstanceIdle);
        lambdaResult.status = response.statusCode;
        lambdaResult.httpStatusCode = Core::HttpUtils::StatusCodeToString(response.statusCode);
        lambdaResult.lambdaStatus = response.statusCode == http::status::ok ? Database::Entity::Lambda::InstanceSuccess : Database::Entity::Lambda::InstanceFailed;
        lambdaResult.requestBody = payload;
        lambdaResult.responseBody = response.body;
        lambdaResult.logMessages = Core::StringUtils::RemoveColorCoding(logs);
        lambdaResult.lambdaName = lambda.function;
        lambdaResult.lambdaArn = lambda.arn;
        lambdaResult.duration = runtime;
        _lambdaDatabase.CreateLambdaResult(lambdaResult);

        (*lambdaCounterMap)[lambda.arn].invocations++;
        (*lambdaCounterMap)[lambda.arn].averageRuntime += runtime;

        log_debug << "Lambda invocation finished, lambda: " << lambda.function << " output: " << response.body;
        log_info << "Lambda invocation finished, lambda: " << lambda.function;
        return lambdaResult;
    }

}// namespace AwsMock::Service