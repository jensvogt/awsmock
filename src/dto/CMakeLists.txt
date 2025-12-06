
set(LIBRARY awsmockdto)
set(LIBRARY_STATIC awsmockdto_static)

set(COMMON_SOURCES src/common/UserAgent.cpp src/common/S3ClientCommand.cpp src/common/SQSClientCommand.cpp
        src/common/Services.cpp src/module/model/Infrastructure.cpp src/common/SNSClientCommand.cpp
        src/common/DynamoDbClientCommand.cpp src/common/SecretsManagerClientCommand.cpp
        src/common/KMSClientCommand.cpp src/common/LambdaClientCommand.cpp src/common/SSMClientCommand.cpp
        src/common/MonitoringClientCommand.cpp src/common/CognitoClientCommand.cpp src/common/ApiGatewayClientCommand.cpp
        src/common/ApplicationClientCommand.cpp)
set(SQS_SOURCES src/sqs/mapper/Mapper.cpp)
set(SNS_SOURCES src/sns/mapper/Mapper.cpp)
set(S3_SOURCES src/s3/mapper/Mapper.cpp)
set(LAMBDA_SOURCES
        src/lambda/CreateTagRequest.cpp src/lambda/CreateFunctionRequest.cpp src/lambda/CreateFunctionResponse.cpp
        src/lambda/GetFunctionResponse.cpp src/lambda/mapper/Mapper.cpp src/lambda/ListEventSourceMappingsRequest.cpp
        src/lambda/ListEventSourceMappingsResponse.cpp)
set(COGNITO_SOURCES src/cognito/mapper/Mapper.cpp)
set(TRANSFER_SOURCES src/transfer/ListUsersRequest.cpp src/transfer/ListUsersResponse.cpp src/transfer/CreateUserRequest.cpp
        src/transfer/CreateUserResponse.cpp src/transfer/DeleteServerRequest.cpp src/transfer/StartServerRequest.cpp
        src/transfer/StopServerRequest.cpp src/transfer/mapper/Mapper.cpp src/transfer/GetServerDetailsRequest.cpp
        src/transfer/GetServerDetailsResponse.cpp src/transfer/DeleteUserRequest.cpp src/transfer/DeleteProtocolRequest.cpp)
set(DYNAMODB_SOURCES src/dynamodb/mapper/Mapper.cpp)
set(SECRETSMANAGER_SOURCES src/secretsmanager/mapper/Mapper.cpp)
set(SSM_SOURCES src/ssm/mapper/Mapper.cpp)
set(APPS_SOURCES src/apps/mapper/Mapper.cpp)
set(API_GATEWAY_SOURCES src/apigateway/mapper/Mapper.cpp)
set(MODULE_SOURCES src/module/model/Module.cpp src/module/model/GatewayConfig.cpp src/module/mapper/Mapper.cpp
        src/module/ExportInfrastructureResponse.cpp
        src/module/CleanInfrastructureRequest.cpp src/module/ListModuleNamesResponse.cpp
        src/module/ImportInfrastructureRequest.cpp)
set(MONITORING_SOURCES src/monitoring/mapper/Mapper.cpp)

set(SOURCES ${COMMON_SOURCES} ${SQS_SOURCES} ${SNS_SOURCES} ${S3_SOURCES} ${LAMBDA_SOURCES} ${COGNITO_SOURCES}
        ${TRANSFER_SOURCES} ${DYNAMODB_SOURCES} ${KMS_SOURCES} ${SSM_SOURCES} ${MODULE_SOURCES}
        ${SECRETSMANAGER_SOURCES} ${APPS_SOURCES} ${API_GATEWAY_SOURCES} ${MONITORING_SOURCES})

# Test directory
if (NOT WIN32)
    add_subdirectory(btest)
endif ()

# Includes
include_directories(include ../core/include ../db/include ${LIBRARY} PUBLIC ./include ${BSONCXX_INCLUDE_DIR} ${MONGOCXX_INCLUDE_DIR}
        ${BOOST_INCLUDE_DIR} ${LIBMAGIC_INCLUDE_DIR} ${LIBARCHIVE_INCLUDE_DIR} ${LIBJWT_INCLUDE_DIR}
        ${LIBSSL_INCLUDE_DIR})

# Shared library
if (WIN32)
    add_library(${LIBRARY_STATIC} STATIC ${SOURCES})
    target_link_directories(${LIBRARY_STATIC} PUBLIC ${MONGOCXX_LIB_DIR} ${BSONCXX_LIB_DIR} ${BOOST_LIB_DIR} ${LIBSSL_LIB_DIR}
            ${LIBARCHIVE_LIB_DIR} ${LIBMAGIC_LIB_DIR})
    target_link_libraries(${LIBRARY_STATIC} PUBLIC awsmockcore_static awsmockdb_static mongocxx-v_noabi-rhi-x64-v143-md.lib
            bsoncxx-v_noabi-rhi-x64-v143-md.lib boost_filesystem-vc143-mt-x64-1_89.lib boost_url-vc143-mt-x64-1_89.lib
            boost_locale-vc143-mt-x64-1_89.lib boost_log-vc143-mt-x64-1_89.lib boost_log_setup-vc143-mt-x64-1_89.lib
            boost_process-vc143-mt-x64-1_89.lib libssl.lib libcrypto.lib archive.lib magic.lib)
    #install(TARGETS ${LIBRARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
else ()
    add_library(${LIBRARY} SHARED ${SOURCES})
    target_link_libraries(${LIBRARY} PUBLIC awsmockcore awsmockdb mongocxx bsoncxx boost_filesystem
            boost_url boost_thread boost_locale boost_json ssl crypto archive magic)
    install(TARGETS ${LIBRARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
endif ()
