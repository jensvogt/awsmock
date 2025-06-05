
set(LIBRARY awsmockdto)
set(LIBRARY_STATIC awsmockdto_static)

set(COMMON_SOURCES src/common/UserAgent.cpp src/common/S3ClientCommand.cpp src/common/SQSClientCommand.cpp
        src/common/Services.cpp src/module/model/Infrastructure.cpp src/common/SNSClientCommand.cpp
        src/common/DynamoDbClientCommand.cpp src/common/SecretsManagerClientCommand.cpp
        src/common/KMSClientCommand.cpp src/common/LambdaClientCommand.cpp src/common/SSMClientCommand.cpp
        src/common/MonitoringClientCommand.cpp)
set(SQS_SOURCES src/sqs/CreateQueueRequest.cpp src/sqs/CreateQueueResponse.cpp src/sqs/model/MessageAttribute.cpp
        src/sqs/ListQueuesRequest.cpp src/sqs/ListQueuesResponse.cpp src/sqs/GetQueueUrlRequest.cpp
        src/sqs/GetQueueAttributesRequest.cpp src/sqs/GetQueueAttributesResponse.cpp src/sqs/GetQueueUrlResponse.cpp
        src/sqs/DeleteQueueRequest.cpp src/sqs/ChangeMessageVisibilityRequest.cpp src/sqs/DeleteMessageBatchEntry.cpp
        src/sqs/DeleteMessageBatchRequest.cpp src/sqs/DeleteQueueResponse.cpp src/sqs/model/EventNotification.cpp
        src/sqs/model/EventRecord.cpp src/sqs/SendMessageBatchRequest.cpp src/sqs/SendMessageBatchResponse.cpp
        src/sqs/model/MessageEntry.cpp src/sqs/model/MessageSuccessful.cpp src/sqs/model/MessageFailed.cpp
        src/sqs/ListQueueArnsResponse.cpp src/sqs/ListMessagesRequest.cpp src/sqs/ListMessagesResponse.cpp
        src/sqs/ListQueueTagsRequest.cpp src/sqs/ListQueueTagsResponse.cpp src/sqs/model/BatchResultErrorEntry.cpp
        src/sqs/model/DeleteMessageBatchResultEntry.cpp src/sqs/DeleteMessageBatchResponse.cpp src/sqs/mapper/Mapper.cpp
        src/sqs/DeleteAttributeRequest.cpp)
set(SNS_SOURCES src/sns/mapper/Mapper.cpp)
set(S3_SOURCES src/s3/CreateBucketRequest.cpp src/s3/CreateBucketResponse.cpp src/s3/PutBucketVersioningRequest.cpp
        src/s3/DeleteObjectRequest.cpp src/s3/DeleteObjectsRequest.cpp src/s3/DeleteObjectsResponse.cpp
        src/s3/CompleteMultipartUploadResult.cpp src/s3/CopyObjectRequest.cpp src/s3/CopyObjectResponse.cpp
        src/s3/MoveObjectRequest.cpp src/s3/MoveObjectResponse.cpp src/s3/CreateBucketConstraint.cpp
        src/s3/model/UserIdentity.cpp src/s3/GetMetadataRequest.cpp src/s3/GetMetadataResponse.cpp
        src/s3/ListBucketRequest.cpp src/s3/ListBucketResponse.cpp src/s3/DeleteBucketRequest.cpp
        src/s3/CreateMultipartUploadRequest.cpp src/s3/CreateMultipartUploadResult.cpp
        src/s3/ListObjectVersionsRequest.cpp src/s3/ListObjectVersionsResponse.cpp src/s3/model/FilterRule.cpp
        src/s3/PutBucketNotificationConfigurationRequest.cpp src/s3/PutBucketNotificationConfigurationResponse.cpp
        src/s3/model/QueueConfiguration.cpp src/s3/model/TopicConfiguration.cpp src/s3/mapper/Mapper.cpp
        src/s3/model/LambdaConfiguration.cpp src/s3/PutBucketEncryptionRequest.cpp src/s3/model/ObjectVersion.cpp
        src/s3/model/RestoreStatus.cpp src/s3/model/ObjectVersion.cpp src/s3/model/RestoreStatus.cpp
        src/s3/model/ObjectVersion.cpp src/s3/model/RestoreStatus.cpp src/s3/UploadPartCopyRequest.cpp
        src/s3/UploadPartCopyResponse.cpp src/s3/GetBucketRequest.cpp src/s3/GetBucketResponse.cpp
        src/s3/UpdateBucketRequest.cpp src/s3/model/Bucket.cpp src/s3/PurgeBucketRequest.cpp)
set(DOCKER_SOURCES src/docker/model/Port.cpp src/docker/model/Container.cpp src/docker/model/Image.cpp
        src/docker/ListContainerResponse.cpp src/docker/ListImageResponse.cpp src/docker/model/LogConfig.cpp
        src/docker/CreateContainerRequest.cpp src/docker/model/Filters.cpp src/docker/CreateContainerResponse.cpp
        src/docker/CreateNetworkRequest.cpp src/docker/CreateNetworkResponse.cpp src/docker/model/Network.cpp
        src/docker/ListNetworkResponse.cpp src/docker/PruneContainerResponse.cpp src/docker/model/Platform.cpp
        src/docker/model/State.cpp src/docker/InspectContainerResponse.cpp src/docker/model/HostConfig.cpp)
set(LAMBDA_SOURCES src/lambda/ListTagsResponse.cpp src/lambda/ListFunctionResponse.cpp src/lambda/model/Function.cpp
        src/lambda/model/Code.cpp src/lambda/CreateTagRequest.cpp src/lambda/CreateFunctionRequest.cpp
        src/lambda/CreateFunctionResponse.cpp src/lambda/GetFunctionResponse.cpp src/lambda/model/Tags.cpp
        src/lambda/model/UserIdentity.cpp src/lambda/mapper/Mapper.cpp src/lambda/model/AccountLimit.cpp
        src/lambda/model/AccountUsage.cpp src/lambda/AccountSettingsResponse.cpp src/lambda/model/Configuration.cpp
        src/lambda/CreateEventSourceMappingsRequest.cpp src/lambda/CreateEventSourceMappingsResponse.cpp
        src/lambda/ListEventSourceMappingsRequest.cpp src/lambda/ListEventSourceMappingsResponse.cpp
        src/lambda/model/EventSourceMapping.cpp src/lambda/DeleteFunctionRequest.cpp)
set(COGNITO_SOURCES src/cognito/ListUserPoolRequest.cpp src/cognito/ListUserPoolResponse.cpp
        src/common/CognitoClientCommand.cpp src/cognito/ListUsersRequest.cpp src/cognito/ListUsersResponse.cpp
        src/cognito/model/LambdaConfig.cpp src/cognito/model/CustomEmailSender.cpp src/cognito/model/CustomSmsSender.cpp
        src/cognito/PreTokenGenerationConfig.cpp src/cognito/mapper/Mapper.cpp src/cognito/ListGroupRequest.cpp
        src/cognito/ListGroupsResponse.cpp src/cognito/ListUsersInGroupRequest.cpp src/cognito/ListUsersInGroupResponse.cpp
        src/cognito/model/UserContextData.cpp src/cognito/SignUpRequest.cpp src/cognito/SignUpResponse.cpp
        src/cognito/ListUserPoolClientsRequest.cpp src/cognito/ListUserPoolClientsResponse.cpp
        src/cognito/UpdateUserPoolRequest.cpp src/cognito/UpdateUserPoolClientRequest.cpp
        src/cognito/UpdateUserPoolDomainRequest.cpp src/cognito/UpdateUserPoolDomainResponse.cpp
        src/cognito/model/ValidationData.cpp src/cognito/ResponseToAuthChallengeRequest.cpp
        src/cognito/ResponseToAuthChallengeResponse.cpp)
set(TRANSFER_SOURCES src/transfer/ListUsersRequest.cpp src/transfer/ListUsersResponse.cpp src/transfer/CreateUserRequest.cpp
        src/transfer/CreateUserResponse.cpp src/transfer/DeleteServerRequest.cpp src/transfer/StartServerRequest.cpp
        src/transfer/StopServerRequest.cpp src/transfer/mapper/Mapper.cpp src/transfer/GetServerDetailsRequest.cpp
        src/transfer/GetServerDetailsResponse.cpp src/transfer/DeleteUserRequest.cpp src/transfer/DeleteProtocolRequest.cpp)
set(DYNAMODB_SOURCES src/dynamodb/mapper/Mapper.cpp)
set(SSM_SOURCES src/ssm/mapper/Mapper.cpp src/ssm/PutParameterRequest.cpp src/ssm/PutParameterResponse.cpp
        src/ssm/model/Parameter.cpp src/ssm/GetParameterRequest.cpp src/ssm/GetParameterResponse.cpp
        src/ssm/DeleteParameterRequest.cpp src/ssm/model/ParameterFilter.cpp src/ssm/DescribeParametersRequest.cpp
        src/ssm/DescribeParametersResponse.cpp)
set(MODULE_SOURCES src/module/model/Module.cpp src/module/model/GatewayConfig.cpp src/module/mapper/Mapper.cpp
        src/module/ExportInfrastructureRequest.cpp src/module/ExportInfrastructureResponse.cpp
        src/module/CleanInfrastructureRequest.cpp src/module/ListModuleNamesResponse.cpp
        src/module/ImportInfrastructureRequest.cpp)
set(MONITORING_SOURCES src/monitoring/mapper/Mapper.cpp)

set(SOURCES ${COMMON_SOURCES} ${SQS_SOURCES} ${SNS_SOURCES} ${S3_SOURCES} ${DOCKER_SOURCES} ${LAMBDA_SOURCES}
        ${COGNITO_SOURCES} ${TRANSFER_SOURCES} ${DYNAMODB_SOURCES} ${KMS_SOURCES} ${SSM_SOURCES} ${MODULE_SOURCES}
        ${MONITORING_SOURCES})

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
            include/awsmock/dto/secretsmanager/model/VersionStage.h)
    target_link_libraries(${LIBRARY} PUBLIC awsmockcore awsmockdb mongocxx bsoncxx boost_filesystem
            boost_url boost_thread boost_locale boost_json ssl crypto archive magic)
    install(TARGETS ${LIBRARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
endif ()
