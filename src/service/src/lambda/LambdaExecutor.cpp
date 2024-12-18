//
// Created by root on 10/9/23.
//

#include <awsmock/service/lambda/LambdaExecutor.h>

namespace AwsMock::Service {

    void LambdaExecutor::operator()(const std::string &oid, const std::string &containerId, const std::string &host, const int port, const std::string &payload, const std::string &functionName) const {

        Monitoring::MetricServiceTimer measure(LAMBDA_INVOCATION_TIMER);
        Monitoring::MetricService::instance().IncrementCounter(LAMBDA_INVOCATION_COUNT);
        log_debug << "Sending lambda invocation request, function: " << functionName << " endpoint: " << host << ":" << port;
        log_debug << "Sending lambda invocation request, payload: " << payload;

        // Set status
        Database::LambdaDatabase::instance().SetInstanceStatus(containerId, Database::Entity::Lambda::InstanceRunning);
        Database::LambdaDatabase::instance().SetLastInvocation(oid, system_clock::now());
        const system_clock::time_point start = system_clock::now();

        // Send request to lambda docker container
        const Core::HttpSocketResponse response = Core::HttpSocket::SendJson(http::verb::post, host, port, "/2015-03-31/functions/function/invocations", payload);
        if (response.statusCode != http::status::ok) {
            log_error << "HTTP error, httpStatus: " << response.statusCode << " body: " << response.body << " payload: " << payload;
            Database::LambdaDatabase::instance().SetInstanceStatus(containerId, Database::Entity::Lambda::InstanceFailed);
        }

        // Reset status
        Database::LambdaDatabase::instance().SetInstanceStatus(containerId, Database::Entity::Lambda::InstanceIdle);
        Database::LambdaDatabase::instance().SetAverageRuntime(oid, std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now() - start).count());
        log_debug << "Lambda invocation finished, function: " << functionName << " httpStatus: " << response.statusCode;
        log_info << "Lambda invocation finished, lambda: " << functionName << " output: " << response.body;
    }

}// namespace AwsMock::Service