//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/service/dynamodb/DynamoDbServer.h>

namespace AwsMock::Service {

    DynamoDbServer::DynamoDbServer() : AbstractServer("dynamodb", 10), _module("dynamodb"), _dockerService(DockerService::instance()) {

        // Get HTTP configuration values
        Core::Configuration &configuration = Core::Configuration::instance();
        _region = configuration.getString("awsmock.region");
        _port = configuration.getInt("awsmock.service.dynamodb.port", DYNAMODB_DEFAULT_PORT);
        _host = configuration.getString("awsmock.service.dynamodb.host", DYNAMODB_DEFAULT_HOST);
        _maxQueueLength = configuration.getInt("awsmock.service.dynamodb.max.queue", DYNAMODB_DEFAULT_QUEUE);
        _maxThreads = configuration.getInt("awsmock.service.dynamodb.max.threads", DYNAMODB_DEFAULT_THREADS);
        _requestTimeout = configuration.getInt("awsmock.service.dynamodb.timeout", DYNAMODB_DEFAULT_TIMEOUT);
        _workerPeriod = configuration.getInt("awsmock.service.dynamodb.worker.period", DYNAMODB_DEFAULT_WORKER_PERIOD);
        _monitoringPeriod = configuration.getInt("awsmock.service.dynamodb.monitoring.period", DYNAMODB_DEFAULT_MONITORING_PERIOD);
        _dockerHost = configuration.getString("awsmock.dynamodb.host", DYNAMODB_DOCKER_HOST);
        _dockerPort = configuration.getInt("awsmock.dynamodb.port", DYNAMODB_DOCKER_PORT);

        // Sleeping period
        _period = configuration.getInt("awsmock.worker.dynamodb.period", 10000);
        log_debug << "DynamoDB server period: " << _period;

        // Docker module
        _dockerService = DockerService::instance();
        log_debug << "DynamoDbServer initialized";

        // Monitoring
        _dynamoDbMonitoring = std::make_shared<DynamoDbMonitoring>(_monitoringPeriod);

        // Worker
        _dynamoDbWorker = std::make_shared<DynamoDbWorker>(_workerPeriod);

        // Start DynamoDb docker image
        StartLocalDynamoDb();
    }

    void DynamoDbServer::Initialize() {

        // Check module active
        if (!IsActive("dynamodb")) {
            log_info << "DynamoDb module inactive";
            return;
        }
        log_info << "DynamoDb server started";

        // Start HTTP manager
        //StartHttpServer(_maxQueueLength, _maxThreads, _requestTimeout, _host, _port, new DynamoDbRequestHandlerFactory(_configuration));

        // Cleanup
        CleanupContainers();

        // Set running
        SetRunning();
    }

    void DynamoDbServer::Run() {
    }

    void DynamoDbServer::Shutdown() {
        _dynamoDbMonitoring->Stop();
        StopHttpServer();
    }

    void DynamoDbServer::CleanupContainers() {
        _dockerService.PruneContainers();
        log_debug << "Docker containers cleaned up";
    }

    void DynamoDbServer::StartLocalDynamoDb() {
        log_debug << "Starting DynamoDB docker image";

        // Check docker image
        if (!_dockerService.ImageExists(DYNAMODB_DOCKER_IMAGE, DYNAMODB_DOCKER_TAG)) {
            _dockerService.BuildImage(DYNAMODB_DOCKER_IMAGE, DYNAMODB_DOCKER_TAG, DYNAMODB_DOCKER_FILE);
        }

        // Check container image
        if (!_dockerService.ContainerExists(DYNAMODB_DOCKER_IMAGE, DYNAMODB_DOCKER_TAG)) {
            _dockerService.CreateContainer(DYNAMODB_DOCKER_IMAGE, DYNAMODB_DOCKER_TAG, _dockerPort, _dockerPort);
        }

        // Get docker container
        Dto::Docker::Container container = _dockerService.GetContainerByName(DYNAMODB_DOCKER_IMAGE, DYNAMODB_DOCKER_TAG);

        // Start docker container, in case it is not already running.
        if (container.state != "running") {
            _dockerService.StartDockerContainer(container.id);
            log_info << "Docker containers for DynamoDB started";
        } else {
            log_info << "Docker containers for DynamoDB already running";
        }
    }

    void DynamoDbServer::StopLocalDynamoDb() {
        log_debug << "Starting DynamoDB docker image";

        // Check docker image
        if (!_dockerService.ImageExists(DYNAMODB_DOCKER_IMAGE, DYNAMODB_DOCKER_TAG)) {
            throw Core::ServiceException("Image does not exist", Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
        }

        // Check container image
        if (!_dockerService.ContainerExists(DYNAMODB_DOCKER_IMAGE, DYNAMODB_DOCKER_TAG)) {
            throw Core::ServiceException("Container does not exist", Poco::Net::HTTPServerResponse::HTTP_INTERNAL_SERVER_ERROR);
        }

        // Get docker container
        Dto::Docker::Container container = _dockerService.GetContainerByName(DYNAMODB_DOCKER_IMAGE, DYNAMODB_DOCKER_TAG);

        // Stop docker container, in case it is running.
        if (container.state == "running") {
            _dockerService.StopContainer(container);
            log_info << "Docker containers for DynamoDB stopped";
        } else {
            log_info << "Docker containers for DynamoDB not running";
        }
    }

}// namespace AwsMock::Service
