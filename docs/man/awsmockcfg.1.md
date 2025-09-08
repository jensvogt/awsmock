---
title: awsmockcfg(1)
section: 1
version: 1.0.1
builddate: 1.0.1
date: $builddate$
header: awsmockcfg AwsMock configuration
footer: awsmockcfg $version$
---

## NAME

```awsmockcfg``` AwsMock configuration

## DESCRIPTION

AwsMock can be configured in several ways: first is the default configuration file:
```/usr/local/awsmock/etc/awsmock.json```.
It defines the main configuration for all AwsMock modules.
The default configuration file can be changed using the ```--config``` options of the ```awsmockmgr```.

Each of the values can be overwritten by environment variables, whereas the environment variable has the same name
written in capital letters and the dots replaced by underscores:

```
awsmock.log.level=debug
```

becomes:

```
AWSMOCK_LOG_LEVEL=debug
```

## VARIABLES

The following lists show all variables in the configuration file, their corresponding environment variable, the
meaning of the variable, and the default value.

### General variables

```
awsmock.region                                    AWSMOCK_REGION                                    AWS region, default: eu-central-1
awsmock.user                                      AWSMOCK_USER                                      AWS user, default: none
awsmock.access.key-id                             AMWSMOCK_KEY_ID                                   AWS secret-key ID, default: none
awsmock.access.account-id                         AMWSMOCK_ACCOUNT_ID                               AWS account ID, default: 000000000000
awsmock.access.client-id                          AWSMOCK_CLIENT_ID                                 AWS client ID, default: 00000000
awsmock.access.secret-access-key                  AWSMOCK_SECRET_ACCESS_KEY                         AWS secret access key, default: none
awsmock.data.dir                                  AWSMOCK_DATA_DIR                                  base data directory, default /usr/local/awsmock/data
awsmock.temp.dir                                  AWSMOCK_TEMP_DIR                                  temporary data directory, default /usr/local/awsmock/tmp
awsmock.backup.dir                                AWSMOCK_BACKUP_DIR                                backup directory,, default /usr/local/awsmock/data/backup
awsmock.shm-size                                  AWSMOCK_SHM_SIZE                                  shared memory size in bytes, default: 65536
awsmock.json.pretty                               AWSMOCK_JSON_PRETTY                               pretty print output for XML and JSON, default: false
awsmock.aws.signature.verify                      AWSMOCK_SIGNATURE_VERIFY                          check AWS sginature with each request, default: false
awsmock.dockerized                                AWSMOCK_DOCKERIZED                                true when running as docker container, default: false
awsmock.magic-file                                AWSMOCK_MAGIC_FILE                                libmagic database, default: /usr/local/awsmock/etc/magic.mgc
awsmock.autoload.active                           AWSMOCK_AUTO_LOAD_ACTIVE                          if true, auto load infrastructure, default: true
awsmock.autoload.file                             AWSMOCK_AUTO_LOAD_FILE                            infrastruture JSON file name, default: /usr/local/awsmock/init/init.json
awsmock.autoload.dir                              AWSMOCK_AUTO_LOAD_DIR                             load all JSON files from directory, default: /usr/local/awsmock/init
```

### Frontend variables

```
awsmock.frontend.active                           AWSMOCK_FRONTEND_ACTIVE                           activation flag, default: true
awsmock.frontend.address                          AWSMOCK_FRONTEND_ADDRESS                          listener address, default: 0.0.0.0
awsmock.frontend.port                             AWSMOCK_FRONTEND_PORT                             port, default: 4567
awsmock.frontend.workers                          AWSMOCK_FRONTEND_WORKERS                          worker thread, default: 10
awsmock.frontend.timeout                          AWSMOCK_FRONTEND_TIMEOUT                          request timeout in seconds, default: 900
awsmock.frontend.doc-root                         AWSMOCK_FRONTEND_DOC_ROOT                         root directory, default: /usr/local/awsmock/frontend
```

### Gateway variables

```
awsmock.gateway.active                            AWSMOCK_MODULES_GATEWAY_ACTIVE                    activation flag, default: true
awsmock.gateway.http.host                         AWSMOCK_MODULES_GATEWAY_HOST                      host, default: localhost
awsmock.gateway.http.address                      AWSMOCK_MODULES_GATEWAY_ADDRESS                   listener IP address, default: 0.0.0.0
awsmock.gateway.http.port                         AWSMOCK_MODULES_GATEWAY_PORT                      port, default: 4566
awsmock.gateway.http.max-queue                    AWSMOCK_MODULES_GATEWAY_MAX_QUEUE                 maximal queue length, default: 250
awsmock.gateway.http.max-thread                   AWSMOCK_MODULES_GATEWAY_MAX_THREAD                maximal threads, default: 256
awsmock.gateway.http.max-body                     AWSMOCK_MODULES_GATEWAY_MAX_BODY                  maximal request body size in bytes, default: 104857600
awsmock.gateway.timeout                           AWSMOCK_MODULES_GATEWAY_TIMEOUT                   HTTP request timeout in seconds, default: 900
```

### S3 module variables

```
awsmock.modules.s3.active                         AWSMOCK_MODULES_S3_ACTIVE                         activation flag, default: true
awsmock.modules.s3.backup.active                  AWSMOCK_MODULES_S3_BACKUP_ACTIVE                  backup activation flag, default: true
awsmock.modules.s3.backup.cron                    AWSMOCK_MODULES_S3_BACKUP_CRON                    backup cron expression, default: 0 0 0 * * ?
awsmock.modules.s3.data-dir                       AWSMOCK_MODULES_S3_DATA_DIR                       data directory, default: /usr/local/awsmock/data/s3
awsmock.modules.s3.monitoring.period              AWSMOCK_MODULES_S3_MONITORING_PERIOD              monitoring period in seconds, default: 900
awsmock.modules.s3.counter.period                 AWSMOCK_MODULES_S3_COUNTER_PERIOD                 monitoring counter period in seconds, default: 300
awsmock.modules.s3.sync.period                    AWSMOCK_MODULES_S3_SYNCH_PERIOD                   synchronize buckets and objects period in seconds, default: 3600
```

### SQS module variables

```
awsmock.modules.sqs.active                        AWSMOCK_MODULES_SQS_ACTIVE                        activation flag, default: true
awsmock.modules.sqs.backup.active                 AWSMOCK_MODULES_SQS_BACKUP_ACTIVE                 backup activation flag, default: true
awsmock.modules.sqs.backup.cron                   AWSMOCK_MODULES_SQS_BACKUP_CRON                   backup cron expression, default: 0 0 0 * * ?
awsmock.modules.sqs.monitoring.period             AWSMOCK_MODULES_SQS_MONITORING_PERIOD             monitoring period in seconds, default: 300
awsmock.modules.sqs.reset.period                  AWSMOCK_MODULES_SQS_RESET_PERIOD                  message reset period in seconds, default: 30
awsmock.modules.sqs.counter.period                AWSMOCK_MODULES_SQS_COUNTER_PERIOD                monitoring counter period in seconds, default: 300
awsmock.modules.sqs.receive-poll                  AWSMOCK_MODULES_SQS_RECEIVE_POLL                  message receive poll period in milliseconds, default: 1000
```

### SNS module variables

```
awsmock.modules.sns.active                        AWSMOCK_MODULES_SNS_ACTIVE                        activation flag, default: true
awsmock.modules.sns.backup.active                 AWSMOCK_MODULES_SNS_BACKUP_ACTIVE                 backup activation flag, default: true
awsmock.modules.sns.backup.cron                   AWSMOCK_MODULES_SNS_BACKUP_CRON                   backup cron expression, default: 0 0 0 * * ?
awsmock.modules.sns.monitoring.period             AWSMOCK_MODULES_SNS_MONITORING_PERIOD             monitoring timeout period in seconds, default: 300
awsmock.modules.sns.delete.period                 AWSMOCK_MODULES_SNS_DELETE_PERIOD                 message delete period in seconds, default: 300
awsmock.modules.sns.counter.period                AWSMOCK_MODULES_SNS_COUNTER_PERIOD                monitoring counter period in seconds, default: 300
awsmock.modules.sns.timeout                       AWSMOCK_MODULES_SNS_TIMEOUT                       message retention period in days, default: 14
```

### Lambda module variables

```
awsmock.modules.lambda.active                     AWSMOCK_MODULES_LAMBDA_ACTIVE                     activation flag, default: true
awsmock.modules.lambda.backup.active              AWSMOCK_MODULES_LAMBDA_BACKUP_ACTIVE              backup activation flag, default: true
awsmock.modules.lambda.backup.cron                AWSMOCK_MODULES_LAMBDA_BACKUP_CRON                backup cron expression, default: 0 0 0 * * ?
awsmock.modules.lambda.lifetime                   AWSMOCK_MODULES_LAMBDA_LIFETIME                   function lifetime perios in seconds, default: 3600
awsmock.modules.lambda.data-dir                   AWSMOCK_MODULES_LAMBDA_DATA_DIR                   function data directory, default: /usr/local/awsmock/data/lambda
awsmock.modules.lambda.log-retention period       AWSMOCK_MODULES_LAMBDA_LOG_RETENTION_PERIOD       log file retention period days, default: 1
awsmock.modules.lambda.remove.period              AWSMOCK_MODULES_LAMBDA_REMOVE_PERIOD              removal period in seconds, default: 3600
awsmock.modules.lambda.counter.period             AWSMOCK_MODULES_LAMBDA_COUNTER_PERIOD             monitoring counter period in seconds, default: 300
awsmock.modules.lambda.runtime.java11             AWSMOCK_MODULES_LAMBDA_RUNTIME_JAVA11             Java 11 image, default: pubic.ecr.aws/lambda/java:11
awsmock.modules.lambda.runtime.java17             AWSMOCK_MODULES_LAMBDA_RUNTIME_JAVA17             Java 17 image, default: pubic.ecr.aws/lambda/java:17
awsmock.modules.lambda.runtime.java21             AWSMOCK_MODULES_LAMBDA_RUNTIME_JAVA21             Java 21 image, default: pubic.ecr.aws/lambda/java:21
awsmock.modules.lambda.runtime.python3-9          AWSMOCK_MODULES_LAMBDA_RUNTIME_PYTHON39           Python 3.8 image, default: pubic.ecr.aws/lambda/python:3.8
awsmock.modules.lambda.runtime.python3-10         AWSMOCK_MODULES_LAMBDA_RUNTIME_PYTHON310          Python 3.8 image, default: pubic.ecr.aws/lambda/python:3.10
awsmock.modules.lambda.runtime.python3-11         AWSMOCK_MODULES_LAMBDA_RUNTIME_PYTHON311          Python 3.8 image, default: pubic.ecr.aws/lambda/python:3.11
awsmock.modules.lambda.runtime.python3-12         AWSMOCK_MODULES_LAMBDA_RUNTIME_PYTHON312          Python 3.8 image, default: pubic.ecr.aws/lambda/python:3.12
awsmock.modules.lambda.runtime.python3-12         AWSMOCK_MODULES_LAMBDA_RUNTIME_PYTHON313          Python 3.8 image, default: pubic.ecr.aws/lambda/python:3.13
awsmock.modules.lambda.runtime.nodejs18-x         AWSMOCK_MODULES_LAMBDA_RUNTIME_NODEJS18x          nodeJS 18.x image, default: pubic.ecr.aws/lambda/nodejs:18
awsmock.modules.lambda.runtime.nodejs20-x         AWSMOCK_MODULES_LAMBDA_RUNTIME_NODEJS20x          nodeJS 18.x image, default: pubic.ecr.aws/lambda/nodejs:20
awsmock.modules.lambda.runtime.nodejs22-x         AWSMOCK_MODULES_LAMBDA_RUNTIME_NODEJS22x          nodeJS 18.x image, default: pubic.ecr.aws/lambda/nodejs:22
awsmock.modules.lambda.runtime.al2                AWSMOCK_MODULES_LAMBDA_RUNTIME_AL2                Amazon Linux v2, default: pubic.ecr.aws/lambda/provided:al2
awsmock.modules.lambda.runtime.al2023             AWSMOCK_MODULES_LAMBDA_RUNTIME_AL2023             Amazon Linux v2023, default: pubic.ecr.aws/lambda/provided:al2023
awsmock.modules.lambda.runtime.latest             AWSMOCK_MODULES_LAMBDA_RUNTIME_LATEST             Amazon Linux latest version, default: pubic.ecr.aws/lambda/provided:latest
awsmock.modules.lambda.runtime.go                 AWSMOCK_MODULES_LAMBDA_RUNTIME_GO                 Go image, default: pubic.ecr.aws/lambda/provided:al2023
```

### KMS module variables

```
awsmock.modules.kms.active                        AWSMOCK_MODULES_KMS_ACTIVE                        module activation, default: true
awsmock.modules.kms.backup.active                 AWSMOCK_MODULES_KMS_BACKUP_ACTIVE                 backup activation flag, default: true
awsmock.modules.kms.backup.cron                   AWSMOCK_MODULES_KMS_BACKUP_CRON                   backup cron expression, default: 0 0 0 * * ?
awsmock.modules.kms.monitoring.period             AWSMOCK_MODULES_KMS_PERIOD                        monitoring polling period in seconds, default: 300
awsmock.modules.kms.remove.period                 AWSMOCK_MODULES_KMS_REMOVE_PERIOD                 key removal period in seconds, default: 300 
```

### SSM module variables

```
awsmock.modules.ssm.active                        AWSMOCK_MODULES_SSM_ACTIVE                        module activation, default: true
awsmock.modules.ssm.backup.active                 AWSMOCK_MODULES_SSM_BACKUP_ACTIVE                 backup activation flag, default: true
awsmock.modules.ssm.backup.cron                   AWSMOCK_MODULES_SSM_BACKUP_CRON                   backup cron expression, default: 0 0 0 * * ?
awsmock.modules.ssm.monitoring.period             AWSMOCK_MODULES_SSM_PERIOD                        monitoring polling period in seconds, default: 300
awsmock.modules.ssm.remove.period                 AWSMOCK_MODULES_SSM_REMOVE_PERIOD                 parameter removal period in seconds, default: 300 
```

### Application module variables

```
awsmock.modules.application.active                AWSMOCK_MODULES_APPLICATION_ACTIVE               module activation, default: true
awsmock.modules.application.backup.active         AWSMOCK_MODULES_APPLICATION_BACKUP_ACTIVE        backup activation flag, default: true
awsmock.modules.application.backup.cron           AWSMOCK_MODULES_APPLICATION_BACKUP_CRON          backup cron expression, default: 0 0 0 * * ?
awsmock.modules.application.monitoring.period     AWSMOCK_MODULES_APPLICATION_PERIOD               monitoring polling period in seconds, default: 300
awsmock.modules.application.remove.period         AWSMOCK_MODULES_APPLICATION_REMOVE_PERIOD        parameter removal period in seconds, default: 300 
```

### Secrets Manager module variables

```
awsmock.modules.secretsmanager.active             AWSMOCK_MODULES_SECRETSMANAGER_ACTIVE             module activation, default: true
awsmock.modules.secretsmanager.backup.active      AWSMOCK_MODULES_SECRETSMANAGER_BACKUP_ACTIVE      backup activation flag, default: true
awsmock.modules.secretsmanager.backup.cron        AWSMOCK_MODULES_SECRETSMANAGER_BACKUP_CRON        backup cron expression, default: 0 0 0 * * ?
awsmock.modules.secretsmanager.monitoring.period  AWSMOCK_MODULES_SECRETSMANAGER_MONIOTRING_PERIOD  monitoring polling period in seconds, default: 300
awsmock.modules.secretsmanager.worker.period      AWSMOCK_MODULES_SECRETSMANAGER_WORKER_PERIOD      parameter remove period in seconds, default: 300 
```

### Transfer server module variables

```
awsmock.modules.transfer.active                   AWSMOCK_MODULES_TRANSFER_ACTIVE                   module activation, default: true
awsmock.modules.transfer.backup.active            AWSMOCK_MODULES_TRANSFER_BACKUP_ACTIVE            backup activation flag, default:true
awsmock.modules.transfer.backup.cron              AWSMOCK_MODULES_TRANSFER_BACKUP_CRON              backup cron expression, default: 0 0 0 * * ?
awsmock.modules.transfer.bucket                   AWSMOCK_MODULES_TRANSFER_BUCKET                   S3 bucket for the transfer server, default: transfer-server
awsmock.modules.transfer.data-dir                 AWSMOCK_MODULES_TRANSFER_DATA_DIR                 data directory, default: /usr/local/awsmock/data/transfer
awsmock.modules.transfer.ftp.port                 AWSMOCK_MODULES_TRANSFER_FTP_PORT                 port for the transfer FTP server, default: 2121
awsmock.modules.transfer.monitoring-period        AWSMOCK_MODULES_TRANSFER_PERIOD                   polling period
awsmock.modules.transfer.ftp.address              AWSMOCK_MODULES_TRANSFER_FTP_ADDRESS              FTP server listen address, default: 0.0.0.0
awsmock.modules.transfer.ftp.port                 AWSMOCK_MODULES_TRANSFER_FTP_PORT                 port for the FTP server, default: 2121
awsmock.modules.transfer.ftp.pasv-min             AWSMOCK_MODULES_TRANSFER_FTP_PASV_MIN             minimal port used for passive mode, default: 6000
awsmock.modules.transfer.ftp.pasv-max             AWSMOCK_MODULES_TRANSFER_FTP_PASV_MAX             maximal port used for passive mode, default: 6100
awsmock.modules.transfer.sftp.address             AWSMOCK_MODULES_TRANSFER_SFTP_ADDRESS             SFTP server listen address, default: 0.0.0.0
awsmock.modules.transfer.sftp.port                AWSMOCK_MODULES_TRANSFER_SFTP_PORT                port for the SFTP server, default: 2222
awsmock.modules.transfer.sftp.pasv-min            AWSMOCK_MODULES_TRANSFER_SFTP_PASV_MIN            minimal port used for passive mode, default: 6000
awsmock.modules.transfer.sftp.pasv-max            AWSMOCK_MODULES_TRANSFER_SFTP_PASV_MAX            maximal port used for passive mode, default: 6100
```

### Cognito module variables

```
awsmock.modules.cognito.active                    AWSMOCK_MODULES_COGNITO_ACTIVE                    activation flag, default: true
awsmock.modules.cognito.backup.active             AWSMOCK_MODULES_COGNITO_BACKUP_ACTIVE             backup activation flag, default: true
awsmock.modules.cognito.backup.cron               AWSMOCK_MODULES_COGNITO_BACKUP_CRON               backup cron expression, default: 0 0 0 * * ?
awsmock.modules.cognito.monitoring.period         AWSMOCK_MODULES_COGNITO_MONITORING_PERIOD         monitoring timeout period in seconds, default: 300.
awsmock.modules.cognito.counter.period            AWSMOCK_MODULES_COGNITO_COUNTER_PERIOD            monitoring counter timeout period in seconds, default: 300.
```

### DynamoDB module variables

```
awsmock.modules.dynamodb.active                   AWSMOCK_MODULES_DYNAMODB_ACTIVE                   activation flag, default: true
awsmock.modules.dynamodb.data-dir                 AWSMOCK_MODULES_DYNAMODB_DATA_DIR                 data directory, default: /usr/local/awsmock/data/dynamodb
awsmock.modules.dynamodb.backup.active            AWSMOCK_MODULES_DYNAMODB_BACKUP_ACTIVE            backup activation flag, default: true
awsmock.modules.dynamodb.backup.cron              AWSMOCK_MODULES_DYNAMODB_BACKUP_CRON              backup cron expression, default: 0 0 0 * * ?
awsmock.modules.dynamodb.monitoring.period        AWSMOCK_MODULES_DYNAMODB_MONITORING_PERIOD        monitoring timeout period in milliseconds, default: 300
awsmock.modules.dynamodb.worker.period            AWSMOCK_MODULES_DYNAMODB_WORKER_PERIOD            worker threads period in seconds, default: 300
awsmock.modules.dynamodb.container.host           AWSMOCK_MODULES_DYNAMODB_CONTAINER_HOST           dynamodb docker container host, default: localhost
awsmock.modules.dynamodb.container.port           AWSMOCK_MODULES_DYNAMODB_CONTAINER_PORT           dynamodb docker container port, default: 8000
awsmock.modules.dynamodb.container.name           AWSMOCK_MODULES_DYNAMODB_CONTAINER_NAME           dynamodb docker container name, default: dynamodb-local
awsmock.modules.dynamodb.container.image-name     AWSMOCK_MODULES_DYNAMODB_CONTAINER_IMAGE_NAME     dynamodb docker container image name, default: amazon/dynamodb-local
awsmock.modules.dynamodb.container.image-tag      AWSMOCK_MODULES_DYNAMODB_CONTAINER_IMAGE_NAME     dynamodb docker container image tag, default: latest
```

### Docker variables

```
awsmock.docker.active                             AWSMOCK_DOCKER_ACTIVE                             activation flag, default: true
awsmock.docker.network-mode                       AWSMOCK_DOCKER_NETWORK_MODE                       network mode, default: local
awsmock.docker.network-name                       AWSMOCK_DOCKER_NETWORK_NAME                       network name, default: local
awsmock.docker.default.memory-size                AWSMOCK_DOCKER_DEFAULT_MEMORY_SIZE                default memory size in kilobytes, default: 512
awsmock.docker.default.temp-size                  AWSMOCK_DOCKER_DEFAULT_TEMP_SIZE                  default /tmp file system size kilobytes, default: 10240
awsmock.docker.container.port                     AWSMOCK_DOCKER_CONTAINER_PORT                     container default port, default: 8080
awsmock.docker.container.maxWaitTime              AWSMOCK_DOCKER_CONTAINER_MAX_WAIT_TIME            container max wait time in seconds, default: 5
awsmock.docker.container.checkTime                AWSMOCK_DOCKER_CONTAINER_CHECK_TIME               container check time in milliseconds, default: 500
awsmock.docker.socket                             AWSMOCK_DOCKER_SOCKET                             docker daemon socket, default: /var/run/docker.sock
```

### Podman variables

```
awsmock.podman.active                             AWSMOCK_PODMAN_ACTIVE                             activation flag, default: true
awsmock.podman.network-mode                       AWSMOCK_PODMAN_NETWORK_MODE                       network mode, default: local
awsmock.podman.network-name                       AWSMOCK_PODMAN_NETWORK_NAME                       network name, default: local
awsmock.podman.default.memory-size                AWSMOCK_PODMAN_DEFAULT_MEMORY_SIZE                default memory size in kilobytes, default: 512
awsmock.podman.default.temp-size                  AWSMOCK_PODMAN_DEFAULT_TEMP_SIZE                  default /tmp file system size kilobytes, default: 10240
awsmock.podman.container.port                     AWSMOCK_PODMAN_CONTAINER_PORT                     container default port, default: 8080
awsmock.podman.container.maxWaitTime              AWSMOCK_PODMAN_CONTAINER_MAX_WAIT_TIME            container max wait time in seconds, default: 5
awsmock.podman.container.checkTime                AWSMOCK_PODMAN_CONTAINER_CHECK_TIME               container check time in milliseconds, default: 500
awsmock.podman.socket                             AWSMOCK_PODMAN_SOCKET                             podman daemon socket, default: /var/run/podman.sock
```

### Monitoring variables

```
awsmock.monitoring.active                         AWSMOCK_MONITORING_ACTIVE                         activation flag, default: true
awsmock.monitoring.port                           AWSMOCK_MONITORING_PROT                           prometheus port, default: 9091
awsmock.monitoring.period                         AWSMOCK_MONITORING_PERIOD                         monitoring period in seconds, default: 60
awsmock.monitoring.prometheus                     AWSMOCK_MONITORING_PROMETHEUS                     use prometheus monitoring, default: false
awsmock.monitoring.internal                       AWSMOCK_MONITORING_INTERNAL                       use internal monitoring, default: true
awsmock.monitoring.retention                      AWSMOCK_MONITORING_RETENTION                      counter retention period in days, default: 3
awsmock.monitoring.smooth                         AWSMOCK_MONITORING_SMOOTH                         smoothening counters by averaging over time, default: false
awsmock.monitoring.average                        AWSMOCK_MONITORING_AVERAGE                        average period in seconds, default: 300
```

### Mongo DB variables

```
awsmock.mongodb.active                            AWSMOCK_MONGODB_ACTIVE                            activation flag, default: true
awsmock.mongodb.name                              AWSMOCK_MONGODB_NAME                              database name, default: awsmock
awsmock.mongodb.host                              AWSMOCK_MONGODB_HOST                              database hostname, default: localhost
awsmock.mongodb.port                              AWSMOCK_MONGODB_PORT                              database port, default: 27017
awsmock.mongodb.user                              AWSMOCK_MONGODB_USER                              database root user, default: root
awsmock.mongodb.password                          AWSMOCK_MONGODB_PASSWORD                          database root password, default: password
awsmock.mongodb.pool-size                         AWSMOCK_MONGODB_POOL_SIZE                         connection pool size, default: 256
```

### Logging variables

```
awsmock.logging.console-active                    AWSMOCK_LOG_CONSOLE_ACTIVE                        console appender activation flag
awsmock.logging.file-active                       AWSMOCK_LOG_FILE_ACTIVE                           file appender activation flag
awsmock.logging.level                             AWSMOCK_LOG_LEVEL                                 logging level, default: info, valid: trace, debug, info, warning, error, fatal
awsmock.logging.dir                               AWSMOCK_LOG_DIR                                   logging file directory, default: /usr/local/awsmock/log
awsmock.logging.prefix                            AWSMOCK_LOG_PREFIX                                prefix for the log files, default: awsmock
awsmock.logging.file-size                         AWSMOCK_LOG_FILE_SIZE                             maximal file size for log files in bytes, default: 10385760
awsmock.logging.file-count                        AWSMOCK_LOG_FILE_COUNT                            maximal number of log files, default: 5
```

## EXAMPLES

To switch of the database support and set the logging level to ```debug```:

```
export AWSMOCK_MONGODB_ACTIVE=false
export AWSMOCK_LOGGING_LEVEL=debug
/usr/bin/awsmockmgr --config ./awsmock.json
```

this will take the value from the given JSON file, but the database activation flag and the logging level will be
overwritten by the environment variables. Finally, the log level will be set to ```debug```.

## AUTHOR

Jens Vogt <jens.vogt@opitz-consulting.com>

## VERSION

$version$ ($builddate$)

## BUGS

Bugs and enhancement requests can be reported and filed at https://github.com/jensvogt/awsmock/issues

## SEE ALSO

```awsmockctl(1)```, ```awsmockmgr(1)```, ```awslocal(1)```, ```awsmocks3(1)```, ```awsmocksqs(1)```,
```awsmocksns(1)```, ```awsmocklambda(1)```, ```awsmockdynamodb(1)```, ```awsmockcognito(1)```,
```awsmocktransfer(1)```, ```awsmocksecretsmanager(1)```, ```awsmocksqs(1)```,```awsmockssm(1)```, ```awsmockapi(1)```