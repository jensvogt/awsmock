//
// Created by vogje01 on 1/5/25.
//

#include "TestBase.h"

namespace AwsMock::Service {

    void TestBase::StartGateway() {

        // Define endpoint. This is the endpoint of the SQS server, not the gateway
        const std::string _port = _configuration.GetValue<std::string>("awsmock.gateway.http.port");
        const std::string _host = _configuration.GetValue<std::string>("awsmock.gateway.http.host");
        const std::string _address = _configuration.GetValue<std::string>("awsmock.gateway.http.address");

        // Set test config
        _endpoint = "http://" + _host + ":" + _port;

        // Start gateway server
        _gatewayServer = std::make_shared<GatewayServer>(_ios);
        _thread = boost::thread([&]() {
            _ios.run();
        });
    }

    void TestBase::StartGateway(const int port) {

        // Define endpoint. This is the endpoint of the SQS server, not the gateway
        _configuration.SetValue<int>("awsmock.gateway.http.port", port + 1);
        _configuration.SetValue<std::string>("awsmock.gateway.http.host", "localhost");

        // Start the gateway
        StartGateway();
    }

    Core::HttpSocketResponse TestBase::SendGetCommand(const std::string &url, const std::string &payload, const int port) {
        Core::HttpSocketResponse response = Core::HttpSocket::SendJson(http::verb::get, "localhost", port, url, payload);
        log_debug << "Status: " << response.statusCode << " body: " << response.body;
        return response;
    }

    Core::HttpSocketResponse TestBase::SendPutCommand(const std::string &url, const std::string &payload, const int port) {
        Core::HttpSocketResponse response = Core::HttpSocket::SendJson(http::verb::post, "localhost", port, url, payload);
        log_debug << "Status: " << response.statusCode << " body: " << response.body;
        return response;
    }

    Core::HttpSocketResponse TestBase::SendPostCommand(const std::string &url, const std::string &payload, const int port) {
        Core::HttpSocketResponse response = Core::HttpSocket::SendJson(http::verb::post, "localhost", port, url, payload);
        log_debug << "Status: " << response.statusCode << " body: " << response.body;
        return response;
    }

    Core::HttpSocketResponse TestBase::SendDeleteCommand(const std::string &url, const std::string &payload, const int port) {
        Core::HttpSocketResponse response = Core::HttpSocket::SendJson(http::verb::delete_, "localhost", port, url, payload);
        log_debug << "Status: " << response.statusCode << " body: " << response.body;
        return response;
    }

    void TestBase::StopGateway() {
        _ios.stop();
    }

}// namespace AwsMock::Service