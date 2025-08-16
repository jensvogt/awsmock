
set(LIBRARY awsmockdto)
set(LIBRARY_STATIC awsmockdto_static)

set(COMMON_SOURCES src/common/UserAgent.cpp src/common/S3ClientCommand.cpp src/common/SQSClientCommand.cpp
        src/common/Services.cpp src/module/model/Infrastructure.cpp src/common/SNSClientCommand.cpp
        src/common/DynamoDbClientCommand.cpp src/common/SecretsManagerClientCommand.cpp
        src/common/KMSClientCommand.cpp src/common/LambdaClientCommand.cpp src/common/SSMClientCommand.cpp
        src/common/MonitoringClientCommand.cpp src/common/CognitoClientCommand.cpp src/common/ApplicationClientCommand.cpp)
set(SQS_SOURCES src/sqs/mapper/Mapper.cpp)
set(SNS_SOURCES src/sns/mapper/Mapper.cpp)
set(S3_SOURCES src/s3/PutBucketVersioningRequest.cpp src/s3/MoveObjectRequest.cpp src/s3/MoveObjectResponse.cpp
        src/s3/model/UserIdentity.cpp src/s3/CreateMultipartUploadRequest.cpp src/s3/CreateMultipartUploadResult.cpp
        src/s3/ListObjectVersionsRequest.cpp src/s3/ListObjectVersionsResponse.cpp src/s3/model/FilterRule.cpp
        src/s3/PutBucketNotificationConfigurationRequest.cpp src/s3/PutBucketNotificationConfigurationResponse.cpp
        src/s3/model/QueueConfiguration.cpp src/s3/model/TopicConfiguration.cpp src/s3/mapper/Mapper.cpp
        src/s3/model/LambdaConfiguration.cpp src/s3/PutBucketEncryptionRequest.cpp src/s3/model/ObjectVersion.cpp
        src/s3/model/RestoreStatus.cpp src/s3/model/ObjectVersion.cpp src/s3/model/RestoreStatus.cpp
        src/s3/model/ObjectVersion.cpp src/s3/model/RestoreStatus.cpp src/s3/UploadPartCopyRequest.cpp
        src/s3/UploadPartCopyResponse.cpp src/s3/UpdateBucketRequest.cpp src/s3/model/Bucket.cpp
        src/s3/PurgeBucketRequest.cpp)
set(DOCKER_SOURCES src/docker/model/Port.cpp src/docker/model/Container.cpp src/docker/model/Image.cpp
        src/docker/ListContainerResponse.cpp src/docker/ListImageResponse.cpp src/docker/model/LogConfig.cpp
        src/docker/CreateContainerRequest.cpp src/docker/model/Filters.cpp src/docker/CreateContainerResponse.cpp
        src/docker/CreateNetworkRequest.cpp src/docker/CreateNetworkResponse.cpp src/docker/model/Network.cpp
        src/docker/ListNetworkResponse.cpp src/docker/PruneContainerResponse.cpp src/docker/model/Platform.cpp
        src/docker/model/State.cpp src/docker/InspectContainerResponse.cpp src/docker/model/HostConfig.cpp)
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
set(MODULE_SOURCES src/module/model/Module.cpp src/module/model/GatewayConfig.cpp src/module/mapper/Mapper.cpp
        src/module/ExportInfrastructureResponse.cpp
        src/module/CleanInfrastructureRequest.cpp src/module/ListModuleNamesResponse.cpp
        src/module/ImportInfrastructureRequest.cpp)
set(MONITORING_SOURCES src/monitoring/mapper/Mapper.cpp)

set(SOURCES ${COMMON_SOURCES} ${SQS_SOURCES} ${SNS_SOURCES} ${S3_SOURCES} ${DOCKER_SOURCES} ${LAMBDA_SOURCES}
        ${COGNITO_SOURCES} ${TRANSFER_SOURCES} ${DYNAMODB_SOURCES} ${KMS_SOURCES} ${SSM_SOURCES} ${MODULE_SOURCES}
        ${SECRETSMANAGER_SOURCES} ${APPS_SOURCES} ${MONITORING_SOURCES})

# Test directory
if (NOT WIN32)
    #    add_subdirectory(tst)
endif ()

# Includes
include_directories(include ../core/include ../db/include ${BSONCXX_INCLUDE_DIR} ${MONGOCXX_INCLUDE_DIR} ${EXTERNAL_INCLUDE_DIR})

# Shared library
if (WIN32)
    add_library(${LIBRARY_STATIC} STATIC ${SOURCES})
else ()
    add_library(${LIBRARY} SHARED ${SOURCES}
            include/awsmock/dto/secretsmanager/model/VersionStage.h
            src/secretsmanager/mapper/Mapper.cpp
            include/awsmock/dto/secretsmanager/mapper/Mapper.h)
    target_link_libraries(${LIBRARY} PUBLIC awsmockcore awsmockdb mongocxx bsoncxx boost_filesystem
            boost_url boost_thread boost_locale boost_json ssl crypto archive magic)
    install(TARGETS ${LIBRARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
endif ()
