
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
        src/secretsmanager/SecretsManagerService.cpp src/secretsmanager/SecretsManagerMonitoring.cpp)
set(KMS_SOURCES src/kms/KMSServer.cpp src/kms/KMSHandler.cpp src/kms/KMSWorker.cpp src/kms/KMSService.cpp
        src/kms/KMSMonitoring.cpp src/kms/KMSCreator.cpp)
set(SSM_SOURCES src/ssm/SSMServer.cpp src/ssm/SSMHandler.cpp src/ssm/SSMService.cpp)
set(FTP_SOURCES src/ftpserver/Filesystem.cpp src/ftpserver/FtpSession.cpp src/ftpserver/FtpServer.cpp
        src/ftpserver/FtpServerImpl.cpp src/ftpserver/UserDatabase.cpp)
set(GATEWAY_SOURCES src/gateway/GatewayServer.cpp src/gateway/GatewaySession.cpp src/gateway/GatewayListener.cpp
        src/gateway/GatewayWorker.cpp src/gateway/GatewayRouter.cpp)
set(DOCKER_SOURCES src/container/ContainerService.cpp)
set(MODULE_SOURCES src/module/ModuleService.cpp src/module/ModuleHandler.cpp src/module/ModuleMonitoring.cpp
        src/module/ModuleMap.cpp)
set(MONITORING_SOURCES src/monitoring/MetricService.cpp src/monitoring/MetricSystemCollector.cpp
        src/monitoring/MonitoringHandler.cpp src/monitoring/MonitoringServer.cpp src/monitoring/MonitoringService.cpp)
set(FRONTEND_SOURCES src/frontend/FrontendWorker.cpp src/frontend/FrontendServer.cpp)

set(SOURCES ${COMMON_SOURCES} ${S3_SOURCES} ${SQS_SOURCES} ${SNS_SOURCES} ${TRANSFER_SOURCES} ${FTP_SOURCES}
        ${LAMBDA_SOURCES} ${COGNITO_SOURCES} ${DYNAMODB_SOURCES} ${KMS_SOURCES} ${SSM_SOURCES} ${SECRETMANAGER_SOURCES}
        ${DOCKER_SOURCES} ${GATEWAY_SOURCES} ${MODULE_SOURCES} ${MONITORING_SOURCES} ${FRONTEND_SOURCES})

# Test directory
if (NOT WIN32)
    add_subdirectory(tst)
endif ()

# Includes
include_directories(./include ../core/include ../db/include ../dto/include ${INC_BSON_CXX} ${INC_MONGO_CXX} ${EXT_INCLUDE})
link_directories(${EXT_LIB})

# Shared library
if (NOT WIN32)
    add_library(${LIBRARY} SHARED ${SOURCES})
    target_link_libraries(${LIBRARY} PUBLIC awsmockcore awsmockdb awsmockdto ${LIB_BSON_CXX} ${LIB_MONGO_CXX}
            ${LIB_BOOST_LOCALE} ${LIB_BOOST_FILESYSTEM} ${LIB_BOOST_URL} ${LIB_YAML} ${LIB_GTEST} ${LIB_SSL} ${LIB_CRYPTO}
            ${LIB_ARCHIVE} ${LIB_PROMETHEUS} ${LIB_MAGIC})
endif ()

# Static library
add_library(${LIBRARY_STATIC} STATIC ${SOURCES})
if (WIN32)
    target_link_libraries(${LIBRARY_STATIC} PUBLIC ${STATIC_LIB} awsmockcore_static awsmockdb_static awsmockdto_static
            ${LIB_BSON_CXX} ${LIB_MONGO_CXX} ${LIB_BOOST_LOCALE} ${LIB_BOOST_FILESYSTEM} ${LIB_BOOST_URL} ${LIB_BOOST_PROCESS}
            ${LIB_YAML} ${LIB_GTEST} ${LIB_SSL} ${LIB_CRYPTO} ${LIB_ARCHIVE} ${LIB_PROMETHEUS} ${LIB_MAGIC} wbemuuid.lib)
else ()
    target_link_libraries(${LIBRARY_STATIC} PUBLIC ${STATIC_LIB} awsmockcore_static awsmockdb_static awsmockdto_static
            ${LIB_BSON_CXX} ${LIB_MONGO_CXX} ${LIB_BOOST_LOCALE} ${LIB_BOOST_FILESYSTEM} ${LIB_BOOST_URL} ${LIB_BOOST_PROCESS}
            ${LIB_YAML} ${LIB_GTEST} ${LIB_SSL} ${LIB_CRYPTO} ${LIB_ARCHIVE} ${LIB_PROMETHEUS} ${LIB_MAGIC})
endif ()

if (NOT WIN32)
    install(TARGETS ${LIBRARY} DESTINATION ${LIB_INSTALL_DIR})
endif ()
