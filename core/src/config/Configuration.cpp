//
// Created by vogje01 on 22/08/2022.
//

#include <awsmock/core/config/Configuration.h>

namespace AwsMock::Core {

    Configuration::Configuration() {
        Initialize();
    }

    Configuration::Configuration(const std::string &basename) {
        Initialize();
        SetFilename(basename);
    }

    void Configuration::Initialize() {

        // General
        DefineProperty<std::string>("awsmock.region", "AWSMOCK_REGION", "eu-central-1");
        DefineProperty<std::string>("awsmock.user", "AWSMOCK_USER", "none");
        DefineProperty<std::string>("awsmock.access.key-id", "AWSMOCK_ACCESS_ACCOUNT_KEY_ID", "none");
        DefineProperty<std::string>("awsmock.access.account-id", "AWSMOCK_ACCESS_ACCOUNT_ID", "000000000000");
        DefineProperty<std::string>("awsmock.access.client-id", "AWSMOCK_ACCESS_CLIENT_ID", "00000000");
        DefineProperty<std::string>("awsmock.access.secret-access-key", "AWSMOCK_ACCESS_SECRET_ACCESS_KEY", "none");
#ifdef _WIN32
        DefineProperty<std::string>("awsmock.data-dir", "AWSMOCK_DATA_DIR", "C:/Program Files (x86)/awsmock/data/");
        DefineProperty<std::string>("awsmock.temp-dir", "AWSMOCK_TEMP_DIR", "C:/Program Files (x86)/awsmock/tmp/");
        DefineProperty<std::string>("awsmock.backup-dir", "AWSMOCK_BACKUP_DIR", "C:/Program Files (x86)/awsmock/data/backup");
#else
        DefineProperty<std::string>("awsmock.data-dir", "AWSMOCK_DATA_DIR", "/usr/local/awsmock/data");
        DefineProperty<std::string>("awsmock.temp-dir", "AWSMOCK_TEMP_DIR", "/usr/local/awsmock/tmp");
        DefineProperty<std::string>("awsmock.backup-dir", "AWSMOCK_BACKUP_DIR", "/usr/local/awsmock/data/backup");
#endif
        DefineProperty<bool>("awsmock.json.pretty", "AWSMOCK_PRETTY", false);
        DefineProperty<bool>("awsmock.aws.signature.verify", "AWSMOCK_VERIFY_SIGNATURE", false);
        DefineProperty<bool>("awsmock.dockerized", "AWSMOCK_DOCKERIZED", false);
        DefineProperty<std::string>("awsmock.magic-file", "AWSMOCK_MAGIC_FILE", DEFAULT_MAGIC_FILE);

        // Auto load
        DefineProperty<bool>("awsmock.autoload.active", "AWSMOCK_AUTOLOAD_ACTIVE", true);
#ifdef _WIN32
        DefineProperty<std::string>("awsmock.autoload.file", "AWSMOCK_AUTOLOAD_FILE", "C:/Program Files (x86)/awsmock/init/init.json");
        DefineProperty<std::string>("awsmock.autoload.dir", "AWSMOCK_AUTOLOAD_DIR", "C:/Program Files (x86)/awsmock/init");
#else
        DefineProperty<std::string>("awsmock.autoload.file", "AWSMOCK_AUTOLOAD_FILE", "/usr/local/awsmock/init/init.json");
        DefineProperty<std::string>("awsmock.autoload.dir", "AWSMOCK_AUTOLOAD_DIR", "/usr/local/awsmock/init");
#endif

        // Gateway
        DefineProperty<bool>("awsmock.gateway.active", "AWSMOCK_GATEWAY_ACTIVE", true);
        DefineProperty<std::string>("awsmock.gateway.http.host", "AWSMOCK_GATEWAY_HOST", "localhost");
        DefineProperty<std::string>("awsmock.gateway.http.address", "AWSMOCK_GATEWAY_ADDRESS", "0.0.0.0");
        DefineProperty<int>("awsmock.gateway.http.port", "AWSMOCK_GATEWAY_PORT", 4566);
        DefineProperty<int>("awsmock.gateway.http.max-queue", "AWSMOCK_GATEWAY_MAX_QUEUE", 250);
        DefineProperty<int>("awsmock.gateway.http.max-thread", "AWSMOCK_GATEWAY_MAX_THREAD", 256);
        DefineProperty<int>("awsmock.gateway.http.max-body", "AWSMOCK_GATEWAY_MAX_BODY", 104857600);
        DefineProperty<int>("awsmock.gateway.http.timeout", "AWSMOCK_GATEWAY_TIMEOUT", 900);

        // S3
        DefineProperty<bool>("awsmock.modules.s3.active", "AWSMOCK_MODULES_S3_ACTIVE", true);
        DefineProperty<bool>("awsmock.modules.s3.backup.active", "AWSMOCK_MODULES_S3_BACKUP_ACTIVE", true);
        DefineProperty<std::string>("awsmock.modules.s3.backup.cron", "AWSMOCK_MODULES_S3_BACKUP_CRON", "0 0 0 * * ?");
        DefineProperty<int>("awsmock.modules.s3.backup.count", "AWSMOCK_MODULES_S3_BACKUP_COUNT", 5);
#ifdef _WIN32
        DefineProperty<std::string>("awsmock.modules.s3.data-dir", "AWSMOCK_MODULES_S3_DATA_DIR", "C:/Program Files (x86)/awsmock/data/s3");
#else
        DefineProperty<std::string>("awsmock.modules.s3.data-dir", "AWSMOCK_MODULES_S3_DATA_DIR", "/usr/local/awsmock/data/s3");
#endif
        DefineProperty<int>("awsmock.modules.s3.monitoring.period", "AWSMOCK_MODULES_S3_MONITORING_PERIOD", 900);
        DefineProperty<int>("awsmock.modules.s3.counter-period", "AWSMOCK_MODULES_S3_COUNTER_PERIOD", 300);
        DefineProperty<int>("awsmock.modules.s3.sync.object.period", "AWSMOCK_MODULES_S3_SYNC_OBJECT_PERIOD", 3600);
        DefineProperty<int>("awsmock.modules.s3.sync.bucket.period", "AWSMOCK_MODULES_S3_SYNC_BUCKET_PERIOD", 300);

        // SQS
        DefineProperty<bool>("awsmock.modules.sqs.active", "AWSMOCK_MODULES_SQS_ACTIVE", true);
        DefineProperty<bool>("awsmock.modules.sqs.backup.active", "AWSMOCK_MODULES_SQS_BACKUP_ACTIVE", true);
        DefineProperty<std::string>("awsmock.modules.sqs.backup.cron", "AWSMOCK_MODULES_SQS_BACKUP_CRON", "0 0 0 * * ?");
        DefineProperty<int>("awsmock.modules.sqs.backup.count", "AWSMOCK_MODULES_SQS_BACKUP_COUNT", 5);
        DefineProperty<int>("awsmock.modules.sqs.monitoring.period", "AWSMOCK_MONITORING_SQS_PERIOD", 300);
        DefineProperty<int>("awsmock.modules.sqs.reset.period", "AWSMOCK_WORKER_SQS_RESET_PERIOD", 30);
        DefineProperty<int>("awsmock.modules.sqs.counter.period", "AWSMOCK_WORKER_SQS_COUNTER_PERIOD", 30);
        DefineProperty<int>("awsmock.modules.sqs.receive-poll", "AWSMOCK_WORKER_SQS_RECEIVE_POLL", 1000);

        // SNS
        DefineProperty<bool>("awsmock.modules.sns.active", "AWSMOCK_MODULES_SNS_ACTIVE", true);
        DefineProperty<bool>("awsmock.modules.sns.backup.active", "AWSMOCK_MODULES_SNS_BACKUP_ACTIVE", true);
        DefineProperty<std::string>("awsmock.modules.sns.backup.cron", "AWSMOCK_MODULES_SNS_BACKUP_CRON", "0 0 0 * * ?");
        DefineProperty<int>("awsmock.modules.sns.backup.count", "AWSMOCK_MODULES_SNS_BACKUP_COUNT", 5);
        DefineProperty<int>("awsmock.modules.sns.timeout", "AWSMOCK_MODULES_SNS_TIMEOUT", 14);
        DefineProperty<int>("awsmock.modules.sns.monitoring.period", "AWSMOCK_SNS_MONITORING_PERIOD", 300);
        DefineProperty<int>("awsmock.modules.sns.delete.period", "AWSMOCK_SNS_DELETE_PERIOD", 300);
        DefineProperty<int>("awsmock.modules.sns.counter.period", "AWSMOCK_SNS_COUNTER_PERIOD", 300);

        // Lambda
        DefineProperty<bool>("awsmock.modules.lambda.active", "AWSMOCK_MODULES_LAMBDA_ACTIVE", true);
        DefineProperty<bool>("awsmock.modules.lambda.backup.active", "AWSMOCK_MODULES_LAMBDA_BACKUP_ACTIVE", true);
        DefineProperty<std::string>("awsmock.modules.lambda.backup.cron", "AWSMOCK_MODULES_LAMBDA_BACKUP_CRON", "0 0 0 * * ?");
        DefineProperty<int>("awsmock.modules.lambda.backup.count", "AWSMOCK_MODULES_LAMBDA_BACKUP_COUNT", 5);
        DefineProperty<int>("awsmock.modules.lambda.lifetime", "AWSMOCK_MODULES_LAMBDA_LIFETIME", 3600);
#ifdef _WIN32
        DefineProperty<std::string>("awsmock.modules.lambda.data-dir", "AWSMOCK_MODULES_LAMBDA_DATADIR", "C:/Program Files (x86)/awsmock/data/lambda");
#else
        DefineProperty<std::string>("awsmock.modules.lambda.data-dir", "AWSMOCK_MODULES_LAMBDA_DATADIR", "/Usr/local/awsmock/data/lambda");
#endif
        DefineProperty<int>("awsmock.modules.lambda.private-port", "AWSMOCK_MODULES_LAMBDA_PRIVATE_PORT", 8080);
        DefineProperty<int>("awsmock.modules.lambda.log-retention-period", "AWSMOCK_MODULES_LAMBDA_LOG_RETENTION_PERIOD", 1);
        DefineProperty<int>("awsmock.modules.lambda.remove-period", "AWSMOCK_MODULES_LAMBDA_REMOVE_PERIOD", 3600);
        DefineProperty<int>("awsmock.modules.lambda.counter-period", "AWSMOCK_MODULES_LAMBDA_COUNTER_PERIOD", 300);
        DefineProperty<std::string>("awsmock.modules.lambda.runtime.java11", "AWSMOCK_MODULES_LAMBDA_JAVA11", "public.ecr.aws/lambda/java:11");
        DefineProperty<std::string>("awsmock.modules.lambda.runtime.java17", "AWSMOCK_MODULES_LAMBDA_JAVA17", "public.ecr.aws/lambda/java:17");
        DefineProperty<std::string>("awsmock.modules.lambda.runtime.java21", "AWSMOCK_MODULES_LAMBDA_JAVA21", "public.ecr.aws/lambda/java:21");
        DefineProperty<std::string>("awsmock.modules.lambda.runtime.python3-9", "AWSMOCK_MODULES_LAMBDA_PYTHON39", "public.ecr.aws/lambda/python:3.9");
        DefineProperty<std::string>("awsmock.modules.lambda.runtime.python3-10", "AWSMOCK_MODULES_LAMBDA_PYTHON310", "public.ecr.aws/lambda/python:3.10");
        DefineProperty<std::string>("awsmock.modules.lambda.runtime.python3-11", "AWSMOCK_MODULES_LAMBDA_PYTHON311", "public.ecr.aws/lambda/python:3.11");
        DefineProperty<std::string>("awsmock.modules.lambda.runtime.python3-12", "AWSMOCK_MODULES_LAMBDA_PYTHON312", "public.ecr.aws/lambda/python:3.12");
        DefineProperty<std::string>("awsmock.modules.lambda.runtime.python3-13", "AWSMOCK_MODULES_LAMBDA_PYTHON313", "public.ecr.aws/lambda/python:3.13");
        DefineProperty<std::string>("awsmock.modules.lambda.runtime.nodejs20-x", "AWSMOCK_MODULES_LAMBDA_NODES20", "public.ecr.aws/lambda/nodejs:20");
        DefineProperty<std::string>("awsmock.modules.lambda.runtime.provided-al2", "AWSMOCK_MODULES_LAMBDA_PROVIDES_AL2", "public.ecr.aws/lambda/provided:al2");
        DefineProperty<std::string>("awsmock.modules.lambda.runtime.provided-al2023", "AWSMOCK_MODULES_LAMBDA_PROVIDED_2023", "public.ecr.aws/lambda/provided:al2023");
        DefineProperty<std::string>("awsmock.modules.lambda.runtime.provided-latest", "AWSMOCK_MODULES_LAMBDA_PROVIDED_LATEST", "public.ecr.aws/lambda/provided:latest");
        DefineProperty<std::string>("awsmock.modules.lambda.runtime.go", "AWSMOCK_MODULES_LAMBDA_GO", "public.ecr.aws/lambda/provided:al2023");

        // Transfer server
        DefineProperty<bool>("awsmock.modules.transfer.active", "AWSMOCK_MODULES_TRANSFER_ACTIVE", true);
        DefineProperty<bool>("awsmock.modules.transfer.backup.active", "AWSMOCK_MODULES_TRANSFER_BACKUP_ACTIVE", true);
        DefineProperty<std::string>("awsmock.modules.transfer.backup.cron", "AWSMOCK_MODULES_TRANSFER_BACKUP_CRON", "0 0 0 * * ?");
        DefineProperty<int>("awsmock.modules.transfer.backup.count", "AWSMOCK_MODULES_TRANSFER_BACKUP_COUNT", 5);
        DefineProperty<std::string>("awsmock.modules.transfer.bucket", "AWSMOCK_MODULES_TRANSFER_BUCKET", "transfer-server");
#ifdef _WIN32
        DefineProperty<std::string>("awsmock.modules.transfer.data-dir", "AWSMOCK_MODULES_TRANSFER_DATA_DIR", "C:/Program Files (x86)/awsmock/data/transfer");
#else
        DefineProperty<std::string>("awsmock.modules.transfer.data-dir", "AWSMOCK_MODULES_TRANSFER_DATA_DIR", "/usr/local/awsmock/data/transfer");
#endif
        DefineProperty<int>("awsmock.modules.transfer.monitoring.period", "AWSMOCK_MODULES_TRANSFER_MONITORING_PERIOD", 300);
        DefineProperty<int>("awsmock.modules.transfer.worker.period", "AWSMOCK_MODULES_TRANSFER_WORKER_PERIOD", 300);
        DefineArrayProperty<std::string>("awsmock.modules.transfer.directories", "AWSMOCK_MODULES_TRANSFER_DIRECTORIES", "incoming;outgoing");

        // FTP server
        DefineProperty<int>("awsmock.modules.transfer.ftp.pasv-min", "AWSMOCK_MODULES_TRANSFER_FTP_PASV_MIN", 6000);
        DefineProperty<int>("awsmock.modules.transfer.ftp.pasv-max", "AWSMOCK_MODULES_TRANSFER_FTP_PASV_MAX", 6100);
        DefineProperty<int>("awsmock.modules.transfer.ftp.port", "AWSMOCK_MODULES_TRANSFER_FTP_PORT", 2121);
        DefineProperty<std::string>("awsmock.modules.transfer.ftp.address", "AWSMOCK_MODULES_TRANSFER_FTP_ADDRESS", "0.0.0.0");

        // SFTP server
        DefineProperty<int>("awsmock.modules.transfer.sftp.pasv-min", "AWSMOCK_MODULES_TRANSFER_FTP_PASV_MIN", 6000);
        DefineProperty<int>("awsmock.modules.transfer.sftp.pasv-max", "AWSMOCK_MODULES_TRANSFER_FTP_PASV_MAX", 6100);
        DefineProperty<int>("awsmock.modules.transfer.sftp.port", "AWSMOCK_MODULES_TRANSFER_FTP_PORT", 22222);
        DefineProperty<std::string>("awsmock.modules.transfer.sftp.address", "AWSMOCK_MODULES_TRANSFER_FTP_ADDRESS", "0.0.0.0");

        // Cognito
        DefineProperty<bool>("awsmock.modules.cognito.active", "AWSMOCK_MODULES_COGNITO_ACTIVE", true);
        DefineProperty<bool>("awsmock.modules.cognito.backup.active", "AWSMOCK_MODULES_COGNITO_BACKUP_ACTIVE", true);
        DefineProperty<std::string>("awsmock.modules.cognito.backup.cron", "AWSMOCK_MODULES_COGNITO_BACKUP_CRON", "0 0 0 * * ?");
        DefineProperty<int>("awsmock.modules.cognito.backup.count", "AWSMOCK_MODULES_COGNITO_BACKUP_COUNT", 5);
        DefineProperty<int>("awsmock.modules.cognito.counter-period", "AWSMOCK_MODULES_COGNITO_COUNTER_PERIOD", 300);
        DefineProperty<int>("awsmock.modules.cognito.monitoring.period", "AWSMOCK_MODULES_COGNITO_MONITORING_PERIOD", 300);

        // DynamoDB
        DefineProperty<bool>("awsmock.modules.dynamodb.active", "AWSMOCK_MODULES_DYNAMODB_ACTIVE", true);
        DefineProperty<bool>("awsmock.modules.dynamodb.backup.active", "AWSMOCK_MODULES_DYNAMODB_BACKUP_ACTIVE", true);
        DefineProperty<std::string>("awsmock.modules.dynamodb.backup.cron", "AWSMOCK_MODULES_DYNAMODB_BACKUP_CRON", "0 0 0 * * ?");
        DefineProperty<int>("awsmock.modules.dynamodb.backup.count", "AWSMOCK_MODULES_DYNAMODB_BACKUP_COUNT", 5);
        DefineProperty<int>("awsmock.modules.dynamodb.monitoring.period", "AWSMOCK_MONITORING_DYNAMODB_PERIOD", 300);
        DefineProperty<int>("awsmock.modules.dynamodb.worker.period", "AWSMOCK_MONITORING_DYNAMODB_PERIOD", 300);
        DefineProperty<std::string>("awsmock.modules.dynamodb.container.host", "AWSMOCK_MODULES_DYNAMODB_CONTAINER_HOST", "localhost");
        DefineProperty<int>("awsmock.modules.dynamodb.container.port", "AWSMOCK_MODULES_DYNAMODB_CONTAINER_PORT", 8000);
        DefineProperty<std::string>("awsmock.modules.dynamodb.container.name", "AWSMOCK_MODULES_DYNAMODB_CONTAINER_NAME", "dynamodb-local");
        DefineProperty<std::string>("awsmock.modules.dynamodb.container.image-name", "AWSMOCK_MODULES_DYNAMODB_CONTAINER_IMAGE_NAME", "docker.io/library/dynamodb-local");
        DefineProperty<std::string>("awsmock.modules.dynamodb.container.image-tag", "AWSMOCK_MODULES_DYNAMODB_CONTAINER_IMAGE_TAG", "latest");
        DefineProperty<std::string>("awsmock.modules.dynamodb.container.data-dir", "AWSMOCK_MODULES_DYNAMODB_DATA_DIR", "/usr/local/awsmock/data/dynamodb");

        // SecretsManager
        DefineProperty<bool>("awsmock.modules.secretsmanager.active", "AWSMOCK_MODULES_SECRETSMANAGER_ACTIVE", true);
        DefineProperty<bool>("awsmock.modules.secretsmanager.backup.active", "AWSMOCK_MODULES_SECRETSMANAGER_BACKUP_ACTIVE", true);
        DefineProperty<std::string>("awsmock.modules.secretsmanager.backup.cron", "AWSMOCK_MODULES_SECRETSMANAGER_BACKUP_CRON", "0 0 0 * * ?");
        DefineProperty<int>("awsmock.modules.secretsmanager.backup.count", "AWSMOCK_MODULES_SECRETSMANAGER_BACKUP_COUNT", 5);
        DefineProperty<int>("awsmock.modules.secretsmanager.monitoring.period", "AWSMOCK_MODULES_SECRETSMANAGER_MONITORING_PERIOD", 300);
        DefineProperty<int>("awsmock.modules.secretsmanager.worker.period", "AWSMOCK_MODULES_SECRETSMANAGER_WORKER_PERIOD", 300);

        // KMS
        DefineProperty<bool>("awsmock.modules.kms.active", "AWSMOCK_MODULES_KMS_ACTIVE", true);
        DefineProperty<bool>("awsmock.modules.kms.backup.active", "AWSMOCK_MODULES_KMS_BACKUP_ACTIVE", true);
        DefineProperty<std::string>("awsmock.modules.kms.backup.cron", "AWSMOCK_MODULES_KMS_BACKUP_CRON", "0 0 0 * * ?");
        DefineProperty<int>("awsmock.modules.kms.backup.count", "AWSMOCK_MODULES_KMS_BACKUP_COUNT", 5);
        DefineProperty<int>("awsmock.modules.kms.monitoring.period", "AWSMOCK_SERVICE_KMS_MONITORING_PERIOD", 300);
        DefineProperty<int>("awsmock.modules.kms.remove.period", "AWSMOCK_WORKER_KMS_REMOVE_PERIOD", 300);

        // SSM
        DefineProperty<bool>("awsmock.modules.ssm.active", "AWSMOCK_MODULES_SSM_ACTIVE", true);
        DefineProperty<bool>("awsmock.modules.ssm.backup.active", "AWSMOCK_MODULES_SSM_BACKUP_ACTIVE", true);
        DefineProperty<std::string>("awsmock.modules.ssm.backup.cron", "AWSMOCK_MODULES_SSM_BACKUP_CRON", "0 0 0 * * ?");
        DefineProperty<int>("awsmock.modules.ssm.backup.count", "AWSMOCK_MODULES_SSM_BACKUP_COUNT", 5);
        DefineProperty<int>("awsmock.modules.ssm.monitoring.period", "AWSMOCK_SERVICE_SSM_MONITORING_PERIOD", 300);
        DefineProperty<int>("awsmock.modules.ssm.remove.period", "AWSMOCK_WORKER_SSM_REMOVE_PERIOD", 300);

        // Application
        DefineProperty<bool>("awsmock.modules.application.active", "AWSMOCK_MODULES_APPLICATION_ACTIVE", true);
        DefineProperty<bool>("awsmock.modules.application.backup.active", "AWSMOCK_MODULES_APPLICATION_BACKUP_ACTIVE", true);
        DefineProperty<std::string>("awsmock.modules.application.backup.cron", "AWSMOCK_MODULES_APPLICATION_BACKUP_CRON", "0 0 0 * * ?");
        DefineProperty<int>("awsmock.modules.application.backup.count", "AWSMOCK_MODULES_APPLICATION_BACKUP_COUNT", 5);
        DefineProperty<int>("awsmock.modules.application.monitoring-period", "AWSMOCK_SERVICE_APPLICATION_MONITORING_PERIOD", 300);
        DefineProperty<bool>("awsmock.modules.application.log-server", "AWSMOCK_SERVICE_APPLICATION_LOG_SERVER", false);
#ifdef _WIN32
        DefineProperty<std::string>("awsmock.modules.application.data-dir", "AWSMOCK_MODULES_APPLICATION_DATADIR", "C:/Program Files (x86)/awsmock/data/application");
#else
        DefineProperty<std::string>("awsmock.modules.application.data-dir", "AWSMOCK_MODULES_APPLICATION_DATADIR", "/usr/local/awsmock/data/application");
#endif
        DefineProperty<std::string>("awsmock.modules.application.runtime.java11", "AWSMOCK_MODULES_APPLICATION_JAVA11", "alpine/java:11.0.22-jdk");
        DefineProperty<std::string>("awsmock.modules.application.runtime.java17", "AWSMOCK_MODULES_APPLICATION_JAVA17", "openjdk:17-jdk-alpine");
        DefineProperty<std::string>("awsmock.modules.application.runtime.java21", "AWSMOCK_MODULES_APPLICATION_JAVA21", "alpine/java:21-jdk");
        DefineProperty<std::string>("awsmock.modules.application.runtime.python3-9", "AWSMOCK_MODULES_APPLICATION_PYTHON39", "python:3.9");
        DefineProperty<std::string>("awsmock.modules.application.runtime.python3-10", "AWSMOCK_MODULES_APPLICATION_PYTHON310", "python:3.10");
        DefineProperty<std::string>("awsmock.modules.application.runtime.python3-11", "AWSMOCK_MODULES_APPLICATION_PYTHON311", "python:3.11");
        DefineProperty<std::string>("awsmock.modules.application.runtime.python3-12", "AWSMOCK_MODULES_APPLICATION_PYTHON312", "python:3.12");
        DefineProperty<std::string>("awsmock.modules.application.runtime.python3-13", "AWSMOCK_MODULES_APPLICATION_PYTHON313", "python:3.13");
        DefineProperty<std::string>("awsmock.modules.application.runtime.nodejs20-x", "AWSMOCK_MODULES_APPLICATION_NODES20", "public.ecr.aws/lambda/nodejs:20");
        DefineProperty<std::string>("awsmock.modules.application.runtime.nodejs21-x", "AWSMOCK_MODULES_APPLICATION_NODES20", "public.ecr.aws/lambda/nodejs:21");
        DefineProperty<std::string>("awsmock.modules.application.runtime.provided-al2", "AWSMOCK_MODULES_APPLICATION_PROVIDES_AL2", "public.ecr.aws/lambda/provided:al2");
        DefineProperty<std::string>("awsmock.modules.application.runtime.provided-al2023", "AWSMOCK_MODULES_APPLICATION_PROVIDED_2023", "public.ecr.aws/lambda/provided:al2023");
        DefineProperty<std::string>("awsmock.modules.application.runtime.provided-latest", "AWSMOCK_MODULES_APPLICATION_PROVIDED_LATEST", "public.ecr.aws/lambda/provided:latest");
        DefineProperty<std::string>("awsmock.modules.application.runtime.go", "AWSMOCK_MODULES_APPLICATION_GO", "public.ecr.aws/lambda/provided:al2023");

        // Docker
        DefineProperty<bool>("awsmock.docker.active", "AWSMOCK_DOCKER_ACTIVE", true);
        DefineProperty<std::string>("awsmock.docker.network-mode", "AWSMOCK_DOCKER_NETWORK_MODE", "local");
        DefineProperty<std::string>("awsmock.docker.network-name", "AWSMOCK_DOCKER_NETWORK_NAME", "local");
        DefineProperty<int>("awsmock.docker.default.memory-size", "AWSMOCK_DOCKER_DEFAULT_MEMORY_SIZE", 512);
        DefineProperty<int>("awsmock.docker.default.temp-size", "AWSMOCK_DOCKER_DEFAULT_TEMP_SIZE", 10240);
        DefineProperty<int>("awsmock.docker.container.port", "AWSMOCK_DOCKER_CONTAINER_PORT", 8080);
        DefineProperty<std::string>("awsmock.docker.socket", "AWSMOCK_DOCKER_SOCKET", "/var/run/docker.sock");
        DefineProperty<int>("awsmock.docker.container.maxWaitTime", "AWSMOCK_DOCKER_CONTAINER_MAX_WAIT_TIME", 5);
        DefineProperty<int>("awsmock.docker.container.checkTime", "AWSMOCK_DOCKER_CONTAINER_CHECK_TIME", 500);

        // Podman
        DefineProperty<bool>("awsmock.podman.active", "AWSMOCK_PODMAN_ACTIVE", false);
        DefineProperty<std::string>("awsmock.podman.network-mode", "AWSMOCK_PODMAN_NETWORK_MODE", "local");
        DefineProperty<std::string>("awsmock.podman.network-name", "AWSMOCK_PODMAN_NETWORK_NAME", "local");
        DefineProperty<int>("awsmock.podman.default.memory-size", "AWSMOCK_PODMAN_DEFAULT_MEMORY_SIZE", 512);
        DefineProperty<int>("awsmock.podman.default.temp-size", "AWSMOCK_PODMAN_DEFAULT_TEMP_SIZE", 10240);
        DefineProperty<int>("awsmock.podman.container.port", "AWSMOCK_PODMAN_CONTAINER_PORT", 8080);
        DefineProperty<std::string>("awsmock.podman.socket", "AWSMOCK_PODMAN_SOCKET", "/run/podman/podman.sock");

        // Monitoring
        DefineProperty<int>("awsmock.monitoring.port", "AWSMOCK_MONITORING_PORT", 9091);
        DefineProperty<int>("awsmock.monitoring.system-period", "AWSMOCK_MONITORING_SYSTEM_PERIOD", 60);
        DefineProperty<bool>("awsmock.monitoring.prometheus", "AWSMOCK_MONITORING_PROMETHEUS", false);
        DefineProperty<bool>("awsmock.monitoring.internal", "AWSMOCK_MONITORING_INTERN", true);
        DefineProperty<int>("awsmock.monitoring.retention", "AWSMOCK_MONITORING_RETENTION", 3);
        DefineProperty<bool>("awsmock.monitoring.smooth", "AWSMOCK_MONITORING_SMOOTH", false);
        DefineProperty<int>("awsmock.monitoring.average-period", "AWSMOCK_MONITORING_AVERAGE_PERIOD", 300);

        // Database
        DefineProperty<bool>("awsmock.mongodb.active", "AWSMOCK_MONGODB_ACTIVE", true);
        DefineProperty<std::string>("awsmock.mongodb.name", "AWSMOCK_MONGODB_NAME", "awsmock");
        DefineProperty<std::string>("awsmock.mongodb.host", "AWSMOCK_MONGODB_HOST", "localhost");
        DefineProperty<int>("awsmock.mongodb.port", "AWSMOCK_MONGODB_PORT", 27017);
        DefineProperty<std::string>("awsmock.mongodb.user", "AWSMOCK_MONGODB_USER", "root");
        DefineProperty<std::string>("awsmock.mongodb.password", "AWSMOCK_MONGODB_PASSWORD", "password");
        DefineProperty<int>("awsmock.mongodb.pool-size", "AWSMOCK_MONGODB_POOL_SIZE", 256);

        // Frontend
        DefineProperty<bool>("awsmock.frontend.active", "AWSMOCK_FRONTEND_ACTIVE", true);
        DefineProperty<std::string>("awsmock.frontend.address", "AWSMOCK_FRONTEND_ADDRESS", "0.0.0.0");
        DefineProperty<int>("awsmock.frontend.port", "AWSMOCK_FRONTEND_PORT", 4567);
        DefineProperty<int>("awsmock.frontend.workers", "AWSMOCK_FRONTEND_WORKERS", 10);
        DefineProperty<int>("awsmock.frontend.timeout", "AWSMOCK_FRONTEND_TIMEOUT", 900);
#ifdef _WIN32
        DefineProperty<std::string>("awsmock.frontend.doc-root", "AWSMOCK_FRONTEND_DOC_ROOT", "C:/Program Files (x86)/awsmock/frontend");
#else
        DefineProperty<std::string>("awsmock.frontend.doc-root", "AWSMOCK_FRONTEND_DOC_ROOT", "/usr/local/awsmock/frontend");
#endif

        // Logging
        DefineProperty<std::string>("awsmock.logging.level", "AWSMOCK_LOG_LEVEL", "info");
#ifdef _WIN32
        DefineProperty<std::string>("awsmock.logging.dir", "AWSMOCK_LOG_DIR_NAME", "C:/Program Files (x86)/awsmock/log");
#else
        DefineProperty<std::string>("awsmock.logging.dir", "AWSMOCK_LOG_DIR_NAME", "/usr/local/awsmock/log");
#endif
        DefineProperty<std::string>("awsmock.logging.prefix", "AWSMOCK_LOG_FILE_PREFIX", "awsmock");
        DefineProperty<long>("awsmock.logging.file-size", "AWSMOCK_LOG_FILE_SIZE", 10485760);
        DefineProperty<int>("awsmock.logging.file-count", "AWSMOCK_LOG_FILE_COUNT", 5);
        DefineProperty<bool>("awsmock.logging.file-active", "AWSMOCK_LOG_FILE_ACTIVE", false);
        DefineProperty<bool>("awsmock.logging.console-active", "AWSMOCK_LOG_CONSOLE_ACTIVE", true);
        DefineProperty<bool>("awsmock.logging.websocket-active", "AWSMOCK_LOG_WEBSOCKET_ACTIVE", false);
    }

    std::string Configuration::GetFilename() const {
        if (_filename.empty()) {
            log_error << "Configuration filename not set";
            throw CoreException("Configuration filename not set");
        }
        return _filename;
    }

    void Configuration::SetFilename(const std::string &filename) {

        // Check file name length
        if (filename.empty()) {
            log_error << "Empty configuration filename";
            return;
        }

        // Check file
        if (!FileUtils::FileExists(filename)) {
            log_warning << "Configuration file '" << filename << "' does not exist. Will use defaults.";
            return;
        }

        // Save file name
        _filename = filename;

        // Parse JSON file
        read_json(_filename, _treeConfiguration);

        // Reapply environment settings
        ApplyEnvSettings();
    }

    std::string Configuration::GetAppName() {
        return "awsmock";
    }

    std::string Configuration::GetVersion() {
        return APP_VERSION;
    }

    std::string Configuration::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    void Configuration::Dump() const {
        std::ostringstream oss;
        write_json(oss, _treeConfiguration);
        std::cerr << oss.str() << std::endl;
    }

    void Configuration::WriteFile(const std::string &filename) const {
        write_json(filename, _treeConfiguration);
    }

    bool Configuration::HasValue(const std::string &key) const {
        return HasProperty(key);
    }

    void Configuration::AddToEnvList(const std::string &key, const std::string &value) { _envList[key] = value; }

    void Configuration::ApplyEnvSettings() {
        for (const auto &[fst, snd]: _envList) {
            SetValue<std::string>(fst, snd);
        }
    }

    bool Configuration::HasProperty(const std::string &key) const {
        return _treeConfiguration.get_child(key).get_value_optional<std::string>().has_value();
    }

    std::string Configuration::ReplaceEnvironmentVariables(const std::string &value) {

        std::string val = value;
        if (StringUtils::Contains(value, "$")) {
            size_t offset = 0;
            static std::regex envRegex(R"(\$(\w+|\{\w+\}))", std::regex::ECMAScript);
            const std::string matchText = value;
            std::sregex_token_iterator matchIter(matchText.begin(), matchText.end(), envRegex, {0, 1});
            for (const std::sregex_token_iterator end; matchIter != end; ++matchIter) {
                const std::string match = matchIter->str();
                std::string envVarName = (++matchIter)->str();

                // Remove matching braces
                if (envVarName.front() == '{' && envVarName.back() == '}') {
                    envVarName.erase(envVarName.begin());
                    envVarName.erase(envVarName.end() - 1);
                }

                // Search for env var and replace if found
                if (SystemUtils::HasEnvironmentVariable(envVarName)) {
                    std::string temp = SystemUtils::GetEnvironmentVariableValue(envVarName);

                    // Since we're manipulating the string, do a new find instead of using original match info
                    if (const size_t pos = value.find(match, offset); pos != std::string::npos) {
                        val.replace(pos, match.length(), temp);
                        offset = pos + value.length();
                    }
                } else {
                    offset += match.length();
                }
            }
        }
        return val;
    }

    std::ostream &operator<<(std::ostream &os, const Configuration &s) {
        std::ostringstream oss;
        write_json(oss, s._treeConfiguration);
        os << "Configuration=" << oss.str();
        return os;
    }
}// namespace AwsMock::Core
