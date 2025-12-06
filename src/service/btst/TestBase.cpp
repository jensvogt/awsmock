//
// Created by vogje01 on 1/5/25.
//

#include "TestBase.h"

#include "awsmock/core/TestUtils.h"
#include "awsmock/utils/TestUtils.h"

//std::unique_ptr<boost::interprocess::managed_shared_memory> shm;
AwsMock::Database::ConnectionPool &pool = AwsMock::Database::ConnectionPool::instance();

namespace AwsMock::Service {
    void TestBase::StartGateway() {

        // Define endpoint. This is the endpoint of the SQS server, not the gateway
        const auto _port = _configuration.GetValue<std::string>("awsmock.gateway.http.port");
        const auto _host = _configuration.GetValue<std::string>("awsmock.gateway.http.host");
        const auto _address = _configuration.GetValue<std::string>("awsmock.gateway.http.address");

        // Set test config
        _endpoint = "http://" + _host + ":" + _port;

        // Start the gateway server
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

    /**
     * Initialize database
     */
    bool TestBase::InitUnitTests() {

        Core::TestUtils::CreateTestConfigurationFile();
        Core::LogStream::Initialize();
        Core::LogStream::RemoveConsoleLogs();

        InitializeDatabase();
        InitializeShm();

        // Create services
        Database::TestUtils::CreateServices();

        // Initialize monitoring
        Monitoring::MetricService::instance().Initialize();
        return true;
    }

    void TestBase::InitializeDatabase() {

        // Get database variables
        const auto name = Core::Configuration::instance().GetValue<std::string>("awsmock.mongodb.name");
        const auto host = Core::Configuration::instance().GetValue<std::string>("awsmock.mongodb.host");
        const auto user = Core::Configuration::instance().GetValue<std::string>("awsmock.mongodb.user");
        const auto password = Core::Configuration::instance().GetValue<std::string>("awsmock.mongodb.password");
        const int _port = Core::Configuration::instance().GetValue<int>("awsmock.mongodb.port");
        const int poolSize = Core::Configuration::instance().GetValue<int>("awsmock.mongodb.pool-size");

        // MongoDB URL
        mongocxx::uri _uri("mongodb://" + user + ":" + password + "@" + host + ":" + std::to_string(_port) + "/?maxPoolSize=" + std::to_string(poolSize));

        // Options
        pool = std::move(Database::ConnectionPool::instance());
        pool.Configure();
    }

    void TestBase::InitializeShm() {

        // As Awsmock is not running under root set shared memory permissions
        //boost::interprocess::permissions unrestricted_permissions;
        //unrestricted_permissions.set_unrestricted();

        // Create a managed shared memory segment.
        //boost::interprocess::shared_memory_object::remove(MONITORING_SEGMENT_NAME);
        //shm = std::make_unique<boost::interprocess::managed_shared_memory>(boost::interprocess::open_or_create, MONITORING_SEGMENT_NAME, 65000, nullptr, unrestricted_permissions);
    }

    void TestBase::StartContainer() {

        // Create a docker service
        ContainerService containerService = AwsMock::Service::ContainerService::instance();

        // Check image
        if (!containerService.ImageExists(TEST_IMAGE_NAME, TEST_CONTAINER_VERSION)) {
            containerService.CreateImage(TEST_IMAGE_NAME, TEST_CONTAINER_VERSION, TEST_IMAGE_NAME);
        }

        // Check container
        if (!containerService.ContainerExistsByImageName(TEST_IMAGE_NAME, TEST_CONTAINER_VERSION)) {
            Dto::Docker::CreateContainerResponse response = containerService.CreateContainer(TEST_IMAGE_NAME, TEST_CONTAINER_VERSION, TEST_CONTAINER_NAME, 10100, 10100);
        }

        // Start the docker container, in case it is not already running.
        if (const Dto::Docker::Container container = containerService.GetFirstContainerByImageName(TEST_IMAGE_NAME, TEST_CONTAINER_VERSION); !container.state.running) {
            containerService.StartDockerContainer(container.id, container.names.at(0));
            log_info << "Test docker container started";
        } else {
            log_info << "Test docker container already running";
        }
    }

    void TestBase::StopGateway() {
        _ios.stop();
    }

}// namespace AwsMock::Service

// entry point:
int main(const int argc, char *argv[]) {

    return boost::unit_test::unit_test_main(&AwsMock::Service::TestBase::InitUnitTests, argc, argv);
}
