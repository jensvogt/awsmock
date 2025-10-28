
set(LIBRARY awsmockdb)
set(LIBRARY_STATIC awsmockdb_static)

set(SQS_SOURCES src/entity/sqs/MessageAttribute.cpp src/entity/sqs/Message.cpp src/repository/SQSDatabase.cpp
        src/memorydb/SQSMemoryDb.cpp src/entity/sqs/Queue.cpp src/entity/sqs/QueueAttribute.cpp
        src/entity/sqs/RedrivePolicy.cpp)
set(SNS_SOURCES src/entity/sns/MessageAttribute.cpp src/entity/sns/Message.cpp src/entity/sns/Topic.cpp
        src/entity/sns/TopicAttribute.cpp src/entity/sns/Subscription.cpp src/repository/SNSDatabase.cpp
        src/memorydb/SNSMemoryDb.cpp)
set(S3_SOURCES src/entity/s3/Bucket.cpp src/entity/s3/BucketNotification.cpp src/entity/s3/Object.cpp
        src/repository/S3Database.cpp src/memorydb/S3MemoryDb.cpp src/entity/s3/FilterRule.cpp
        src/entity/s3/QueueNotification.cpp src/entity/s3/TopicNotification.cpp src/entity/s3/LambdaNotification.cpp
        src/entity/s3/BucketEncryption.cpp)
set(LAMBDA_SOURCES src/entity/lambda/Tags.cpp src/entity/lambda/Environment.cpp src/entity/lambda/Lambda.cpp
        src/entity/lambda/Code.cpp src/entity/lambda/EphemeralStorage.cpp src/repository/LambdaDatabase.cpp
        src/memorydb/LambdaMemoryDb.cpp src/entity/lambda/Instance.cpp src/entity/lambda/EventSourceMapping.cpp
        src/entity/lambda/LambdaResult.cpp)
set(TRANSFER_SOURCES src/repository/TransferDatabase.cpp src/entity/transfer/User.cpp src/entity/transfer/Transfer.cpp
        src/memorydb/TransferMemoryDb.cpp)
set(COGNITO_SOURCES src/entity/cognito/UserPool.cpp src/entity/cognito/User.cpp src/entity/cognito/UserPoolClient.cpp
        src/entity/cognito/Group.cpp src/memorydb/CognitoMemoryDb.cpp src/repository/CognitoDatabase.cpp)
set(DYNAMODB_SOURCES src/entity/dynamodb/Item.cpp src/entity/dynamodb/Table.cpp src/entity/dynamodb/AttributeValue.cpp
        src/entity/dynamodb/ProvisionedThroughput.cpp src/repository/DynamoDbDatabase.cpp
        src/memorydb/DynamoDbMemoryDb.cpp)
set(SECRETSMANAGER_SOURCES src/entity/secretsmanager/Secret.cpp src/entity/secretsmanager/SecretVersion.cpp
        src/entity/secretsmanager/RotationRules.cpp src/repository/SecretsManagerDatabase.cpp
        src/memorydb/SecretsManagerMemoryDb.cpp)
set(KMS_SOURCES src/entity/kms/Key.cpp src/repository/KMSDatabase.cpp src/memorydb/KMSMemoryDb.cpp)
set(SSM_SOURCES src/entity/ssm/Parameter.cpp src/repository/SSMDatabase.cpp src/memorydb/SSMMemoryDb.cpp)
set(MODULE_SOURCES src/entity/module/Module.cpp src/repository/ModuleDatabase.cpp src/memorydb/ModuleMemoryDb.cpp)
set(APPS_SOURCES src/entity/apps/Application.cpp src/repository/ApplicationDatabase.cpp src/memorydb/ApplicationMemoryDb.cpp)
set(UTIL_SOURCES src/utils/MongoUtils.cpp src/utils/ConnectionPool.cpp src/utils/SqsUtils.cpp)
set(API_GATEWAY_SOURCES src/repository/ApiGatewayDatabase.cpp src/memorydb/ApiGatewayMemoryDb.cpp
        src/entity/apigateway/ApiKey.cpp src/entity/apigateway/Rest.cpp)
set(MONITORING_SOURCES src/entity/monitoring/Counter.cpp src/repository/MonitoringDatabase.cpp)

set(SOURCES src/repository/Database.cpp src/utils/TestUtils.cpp ${SQS_SOURCES} ${SNS_SOURCES} ${S3_SOURCES}
        ${LAMBDA_SOURCES} ${TRANSFER_SOURCES} ${COGNITO_SOURCES} ${DYNAMODB_SOURCES} ${KMS_SOURCES} ${MODULE_SOURCES}
        ${SECRETSMANAGER_SOURCES} ${SSM_SOURCES} ${APPS_SOURCES} ${API_GATEWAY_SOURCES} ${UTIL_SOURCES}
        ${MONITORING_SOURCES})

# Test directory
if (NOT WIN32)
    add_subdirectory(tst)
endif ()

# Includes
include_directories(./include ../core/include ${LIBRARY} PUBLIC ./include ${BSONCXX_INCLUDE_DIR} ${MONGOCXX_INCLUDE_DIR}
        ${BOOST_INCLUDE_DIR} ${LIBMAGIC_INCLUDE_DIR} ${LIBARCHIVE_INCLUDE_DIR} ${LIBJWT_INCLUDE_DIR}
        ${LIBSSL_INCLUDE_DIR})

# Shared library
if (WIN32)
    add_library(${LIBRARY_STATIC} STATIC ${SOURCES})
    target_link_directories(${LIBRARY_STATIC} PUBLIC ${MONGOCXX_LIB_DIR} ${BSONCXX_LIB_DIR} ${BOOST_LIB_DIR} ${LIBSSL_LIB_DIR}
            ${LIBARCHIVE_LIB_DIR} ${LIBMAGIC_LIB_DIR})
    target_link_libraries(${LIBRARY_STATIC} PUBLIC awsmockcore_static bsoncxx-v_noabi-rhi-x64-v143-md.lib
            mongocxx-v_noabi-rhi-x64-v143-md.lib boost_filesystem-vc143-mt-x64-1_89.lib
            boost_url-vc143-mt-x64-1_89.lib boost_locale-vc143-mt-x64-1_89.lib boost_log-vc143-mt-x64-1_89.lib
            boost_log_setup-vc143-mt-x64-1_89.lib boost_process-vc143-mt-x64-1_89.lib libssl.lib libcrypto.lib
            archive.lib magic.lib)
    #install(TARGETS ${LIBRARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
else ()
    add_library(${LIBRARY} SHARED ${SOURCES})
    target_link_libraries(${LIBRARY} awsmockcore mongocxx bsoncxx boost_filesystem boost_url boost_thread boost_locale
            boost_json ssl crypto archive magic)
    install(TARGETS ${LIBRARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
endif ()
