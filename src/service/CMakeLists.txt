set(LIBRARY awsmocksrv)
set(LIBRARY_STATIC awsmocksrv_static)

set(COMMON_SOURCES src/common/AbstractHandler.cpp src/common/AbstractServer.cpp)
set(S3_SOURCES src/s3/S3Server.cpp src/s3/S3Handler.cpp src/s3/S3Service.cpp src/s3/S3HashCreator.cpp)
set(SQS_SOURCES src/sqs/SQSServer.cpp src/sqs/SQSHandler.cpp src/sqs/SQSService.cpp)
set(SNS_SOURCES src/sns/SNSServer.cpp src/sns/SNSHandler.cpp src/sns/SNSService.cpp)
set(LAMBDA_SOURCES src/lambda/LambdaServer.cpp src/lambda/LambdaHandler.cpp src/lambda/LambdaService.cpp
        src/lambda/LambdaCreator.cpp src/lambda/LambdaExecutor.cpp)
set(COGNITO_SOURCES src/cognito/CognitoHandler.cpp src/cognito/CognitoHandler.cpp src/cognito/CognitoService.cpp
        src/cognito/CognitoServer.cpp)
set(TRANSFER_SOURCES src/transfer/TransferServer.cpp src/transfer/TransferHandler.cpp src/transfer/TransferService.cpp)
set(DYNAMODB_SOURCES src/dynamodb/DynamoDbServer.cpp src/dynamodb/DynamoDbHandler.cpp src/dynamodb/DynamoDbService.cpp)
set(SECRETMANAGER_SOURCES src/secretsmanager/SecretsManagerServer.cpp src/secretsmanager/SecretsManagerHandler.cpp
        src/secretsmanager/SecretsManagerService.cpp src/secretsmanager/SecretRotation.cpp)
set(KMS_SOURCES src/kms/KMSServer.cpp src/kms/KMSHandler.cpp src/kms/KMSService.cpp
        src/kms/KMSCreator.cpp)
set(SSM_SOURCES src/ssm/SSMServer.cpp src/ssm/SSMHandler.cpp src/ssm/SSMService.cpp)
set(FTP_SOURCES src/ftpserver/Filesystem.cpp src/ftpserver/FtpSession.cpp src/ftpserver/FtpServer.cpp
        src/ftpserver/FtpServerImpl.cpp src/ftpserver/UserDatabase.cpp src/sftpserver/SftpServer.cpp
        src/sftpserver/SftpUser.cpp)
set(GATEWAY_SOURCES src/gateway/GatewayServer.cpp src/gateway/GatewaySession.cpp src/gateway/GatewayListener.cpp
        src/gateway/GatewayRouter.cpp)
set(CONTAINER_SOURCES src/container/ContainerService.cpp src/container/ContainerHandler.cpp)
set(MODULE_SOURCES src/module/ModuleService.cpp src/module/ModuleHandler.cpp src/module/ModuleMonitoring.cpp
        src/module/ModuleMap.cpp)
set(APPLICATION_SOURCES src/application/ApplicationHandler.cpp src/application/ApplicationServer.cpp src/application/ApplicationService.cpp
        src/application/ApplicationCreator.cpp src/application/ApplicationLogServer.cpp src/application/ApplicationLogSession.cpp)
set(API_GATEWAY_SOURCES src/apigateway/ApiGatewayService.cpp src/apigateway/ApiGatewayHandler.cpp)
set(MONITORING_SOURCES src/monitoring/MetricService.cpp src/monitoring/MetricSystemCollector.cpp
        src/monitoring/MonitoringHandler.cpp src/monitoring/MonitoringServer.cpp src/monitoring/MonitoringService.cpp)
set(FRONTEND_SOURCES src/frontend/FrontendWorker.cpp src/frontend/FrontendServer.cpp)

set(SOURCES ${COMMON_SOURCES} ${S3_SOURCES} ${SQS_SOURCES} ${SNS_SOURCES} ${TRANSFER_SOURCES} ${FTP_SOURCES}
        ${LAMBDA_SOURCES} ${COGNITO_SOURCES} ${DYNAMODB_SOURCES} ${KMS_SOURCES} ${SSM_SOURCES} ${SECRETMANAGER_SOURCES}
        ${CONTAINER_SOURCES} ${GATEWAY_SOURCES} ${APPLICATION_SOURCES} ${API_GATEWAY_SOURCES} ${MODULE_SOURCES}
        ${MONITORING_SOURCES} ${FRONTEND_SOURCES})

# Test directory
if (NOT WIN32)
    add_subdirectory(btst)
endif ()

# Library
if (WIN32)
    add_library(${LIBRARY_STATIC} STATIC ${SOURCES})
    target_include_directories(${LIBRARY} PRIVATE include ../core/include ../db/include ../dto/include
            ${BSONCXX_INCLUDE_DIR} ${MONGOCXX_INCLUDE_DIR} ${EXTERNAL_INCLUDE_DIR})
    target_link_directories(${LIBRARY_STATIC} PUBLIC ${MONGOCXX_LIB_DIR} ${BSONCXX_LIB_DIR} ${EXTERNAL_LIB_DIR})
    target_link_libraries(${LIBRARY_STATIC} PUBLIC awsmockcore_static awsmockdb_static awsmockdto_static
            mongocxx-v_noabi-rhi-x64-v145-md.lib bsoncxx-v_noabi-rhi-x64-v145-md.lib boost_url-vc143-mt-x64-1_89.lib
            boost_filesystem-vc143-mt-x64-1_89.lib boost_locale-vc143-mt-x64-1_89.lib boost_log-vc143-mt-x64-1_89.lib
            boost_log_setup-vc143-mt-x64-1_89.lib boost_process-vc143-mt-x64-1_89.lib libssl.lib libcrypto.lib
            archive.lib magic.lib ssh.lib prometheus-cpp-core.lib prometheus-cpp-pull.lib pdh.lib)
else ()
    add_library(${LIBRARY} SHARED ${SOURCES})
    target_include_directories(${LIBRARY} PRIVATE include ../core/include ../db/include ../dto/include
            ${BSONCXX_INCLUDE_DIR} ${MONGOCXX_INCLUDE_DIR} ${EXTERNAL_INCLUDE_DIR})
    target_link_directories(${LIBRARY} PUBLIC ${MONGOCXX_LIB_DIR} ${BSONCXX_LIB_DIR} ${EXTERNAL_LIB_DIR})
    target_link_libraries(${LIBRARY} PRIVATE awsmockcore awsmockdb awsmockdto boost_locale boost_thread boost_filesystem
            boost_log boost_log_setup boost_context ssl crypto archive magic ssh)
    install(TARGETS ${LIBRARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
endif ()
