//
// Created by root on 10/9/23.
//

#include <awsmock/service/lambda/LambdaExecutor.h>

namespace AwsMock::Service {

    void LambdaExecutor::Invocation(const Database::Entity::Lambda::Lambda &lambda, const std::string &containerId, const std::string &host, const int port, const std::string &payload, Database::Entity::Lambda::LambdaResult &lambdaResult) {

        // Initialize shared memory
        auto _segment = boost::interprocess::managed_shared_memory(boost::interprocess::open_only, SHARED_MEMORY_SEGMENT_NAME);
        Database::LambdaCounterMapType *_lambdaCounterMap = _segment.find<Database::LambdaCounterMapType>(Database::LAMBDA_COUNTER_MAP_NAME).first;

        Monitoring::MetricServiceTimer measure(LAMBDA_INVOCATION_TIMER, "function_name", lambda.function);
        Monitoring::MetricService::instance().IncrementCounter(LAMBDA_INVOCATION_COUNT, "function_name", lambda.function);
        log_debug << "Sending lambda invocation request, function: " << lambda.function << " endpoint: " << host << ":" << port;
        log_trace << "Sending lambda invocation request, payload: " << payload;

        // Set status
        Database::LambdaDatabase::instance().SetInstanceStatus(containerId, Database::Entity::Lambda::InstanceRunning);
        Database::LambdaDatabase::instance().SetLastInvocation(lambda.oid, system_clock::now());
        const system_clock::time_point start = system_clock::now();
        (*_lambdaCounterMap)[lambda.arn].invocations++;

        // Send request to lambda docker container
        lambdaResult.containerId = containerId;
        const Core::HttpSocketResponse response = Core::HttpSocket::SendJson(http::verb::post, host, port, "/2015-03-31/functions/function/invocations", payload);
        lambdaResult.status = response.statusCode;

        // Calculate runtime
        const long runtime = std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now() - start).count();
        Database::LambdaDatabase::instance().SetAverageRuntime(lambda.oid, runtime);

        if (response.statusCode != http::status::ok) {

            log_error << "HTTP error, httpStatus: " << response.statusCode << ", responseBody: " << response.body << ", requestBody: " << payload;
            Database::LambdaDatabase::instance().SetInstanceStatus(containerId, Database::Entity::Lambda::InstanceFailed);

            lambdaResult.httpStatusCode = Core::HttpUtils::StatusCodeToString(response.statusCode);
            lambdaResult.lambdaStatus = Database::Entity::Lambda::InstanceFailed;

        } else {

            // Reset status
            Database::LambdaDatabase::instance().SetInstanceStatus(containerId, Database::Entity::Lambda::InstanceIdle);
            lambdaResult.httpStatusCode = Core::HttpUtils::StatusCodeToString(response.statusCode);
            lambdaResult.lambdaStatus = Database::Entity::Lambda::InstanceIdle;
            log_debug << "Lambda invocation finished, function: " << lambda.function << " httpStatus: " << response.statusCode;
        }

        // Get log messages
        const std::string logs = ContainerService::instance().GetContainerLogs(containerId, start);

        // Save results
        lambdaResult.requestBody = payload;
        lambdaResult.responseBody = response.body;
        lambdaResult.logMessages = Core::StringUtils::RemoveColorCoding(logs);
        lambdaResult.lambdaName = lambda.function;
        lambdaResult.lambdaArn = lambda.arn;
        lambdaResult.duration = runtime;
        Database::LambdaDatabase::instance().CreateLambdaResult(lambdaResult);

        (*_lambdaCounterMap)[lambda.arn].invocations++;
        (*_lambdaCounterMap)[lambda.arn].averageRuntime += runtime;

        log_debug << "Lambda invocation finished, lambda: " << lambda.function << " output: " << response.body;
        log_info << "Lambda invocation finished, lambda: " << lambda.function;
        std::lock_guard lock(_mutex);
        _exitConfirm = 1;
        _condition.notify_all();
    }

    void LambdaExecutor::SpawnDetached(const Database::Entity::Lambda::Lambda &lambda, const std::string &containerId, const std::string &host, const int port, const std::string &payload, Database::Entity::Lambda::LambdaResult &lambdaResult) {
        std::thread(&LambdaExecutor::Invocation, this, std::ref(lambda), std::ref(containerId), std::ref(host), port, std::ref(payload), std::ref(lambdaResult)).detach();
        log_info << "Lambda invocation started, lambda: " << lambda.function;
    }

    // add support functions instead of doing these things in your normal code
    void LambdaExecutor::WaitForFinish() {
        std::unique_lock lock(_mutex);
        _condition.wait(lock, [this] { return _exitConfirm == 1; });
    }
}// namespace AwsMock::Service