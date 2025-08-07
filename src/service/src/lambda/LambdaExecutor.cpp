//
// Created by root on 10/9/23.
//

#include "awsmock/dto/lambda/model/LambdaResult.h"


#include <awsmock/service/lambda/LambdaExecutor.h>
#include <boost/asio/detached.hpp>
#include <boost/asio/spawn.hpp>

namespace AwsMock::Service {

    Database::Entity::Lambda::LambdaResult LambdaExecutor::Invocation(const Database::Entity::Lambda::Lambda &lambda, const std::string &containerId, const std::string &host, const int port, const std::string &payload) {

        // Initialize shared memory
        auto _segment = boost::interprocess::managed_shared_memory(boost::interprocess::open_only, SHARED_MEMORY_SEGMENT_NAME);
        Database::LambdaCounterMapType *_lambdaCounterMap = _segment.find<Database::LambdaCounterMapType>(Database::LAMBDA_COUNTER_MAP_NAME).first;

        // Monitoring
        Monitoring::MetricServiceTimer measure(LAMBDA_INVOCATION_TIMER, "function_name", lambda.function);
        Monitoring::MetricService::instance().IncrementCounter(LAMBDA_INVOCATION_COUNT, "function_name", lambda.function);
        log_debug << "Sending lambda invocation request, function: " << lambda.function << " endpoint: " << host << ":" << port;
        log_trace << "Sending lambda invocation request, payload: " << payload;

        // Prepare resultSend request to lambda docker container
        Database::Entity::Lambda::LambdaResult lambdaResult;
        lambdaResult.containerId = containerId;

        // Send request to lambda docker container
        const system_clock::time_point start = system_clock::now();
        const Core::HttpSocketResponse response = Core::HttpSocket::SendJson(http::verb::post, host, port, "/2015-03-31/functions/function/invocations", payload);
        const long runtime = std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now() - start).count();

        // Get lambda log messages
        const std::string logs = ContainerService::instance().GetContainerLogs(containerId, start);

        // Save results
        Database::LambdaDatabase::instance().SetAverageRuntime(lambda.oid, runtime);
        Database::LambdaDatabase::instance().SetInstanceStatus(containerId, Database::Entity::Lambda::InstanceIdle);
        lambdaResult.status = response.statusCode;
        lambdaResult.httpStatusCode = Core::HttpUtils::StatusCodeToString(response.statusCode);
        lambdaResult.lambdaStatus = response.statusCode == http::status::ok ? Database::Entity::Lambda::InstanceSuccess : Database::Entity::Lambda::InstanceFailed;
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
        return lambdaResult;
    }

    void LambdaExecutor::SpawnDetached(const Database::Entity::Lambda::Lambda &lambda, const std::string &containerId, const std::string &host, const int port, const std::string &payload) {
        boost::asio::spawn(_ioc, [lambda, containerId, host, port, payload](boost::asio::yield_context) {
                        Invocation(lambda, containerId, host, port, payload);
                        log_info << "Lambda invocation started, lambda: " << lambda.function << ", containerId: " << containerId; }, boost::asio::detached);
        _ioc.poll();
    }

}// namespace AwsMock::Service