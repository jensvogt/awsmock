cmake_minimum_required(VERSION 3.22)

project(awsmock LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -------------------------------------------------------------------------
# Global link flags
# -------------------------------------------------------------------------

if (!APPLE)
    #    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -pthread")
    #    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static -pthread")
endif ()
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_BINARY_DIR}/conan_toolchain.cmake" CACHE STRING "")

if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0A00)
    add_compile_options(/MP16 /FS "/Zc:__cplusplus" /EHsc "$<$<CONFIG:RELEASE>:/MD>" "$<$<CONFIG:DEBUG>:/MDd>" /D "_WIN32")
    add_compile_options("$<$<CONFIG:RELEASE>:/O2iy>" "$<$<CONFIG:DEBUG>:/Od>" /bigobj)
    add_compile_options(/FS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(CMAKE_SYSTEM_NAME Windows)
    set(CMAKE_GENERATOR_PLATFORM x64)
    set(EXTERNAL_LIB_DIR ${CMAKE_SOURCE_DIR}/extern/win32/lib)
    set(EXTERNAL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/extern/win32/include)
    set(EXTERNAL_LIBS boost_log-vc145-mt-x64-1_89.lib boost_log_setup-vc145-mt-x64-1_89.lib boost_process-vc145-mt-x64-1_89.lib
            boost_url-vc145-mt-x64-1_89.lib boost_filesystem-vc145-mt-x64-1_89.lib boost_thread-vc145-mt-x64-1_89.lib
            magic.lib libssl.lib libcrypto.lib archive.lib)
endif ()

#find_package(LibArchive)
#find_package(mongocxx)
#find_package(Boost COMPONENTS Boost::json Boost::headers Boost::system Boost::serialization)
#find_package(jwt-cpp)
#find_package(libssh)
#find_package(OpenSSL COMPONENTS SSL Crypto)

# -------------------------------------------------------------------------
# Build AWSMock Core library
# -------------------------------------------------------------------------

set(UTILS_SOURCES core/src/utils/StringUtils.cpp core/src/utils/FileUtils.cpp core/src/utils/DirUtils.cpp core/src/utils/DateTimeUtils.cpp
        core/src/utils/CryptoUtils.cpp core/src/utils/AwsUtils.cpp core/src/utils/SystemUtils.cpp core/src/utils/TarUtils.cpp
        core/src/utils/RandomUtils.cpp core/src/utils/TestUtils.cpp core/src/utils/HttpUtils.cpp core/src/utils/NumberUtils.cpp
        core/src/utils/MemoryMappedFile.cpp core/src/utils/XmlUtils.cpp core/src/utils/UnixSocket.cpp core/src/utils/HttpSocket.cpp
        core/src/utils/JwtUtils.cpp core/src/scheduler/PeriodicTask.cpp core/src/scheduler/Scheduler.cpp core/src/utils/BackupUtils.cpp
        core/src/config/Configuration.cpp core/src/utils/SrpUtils.cpp core/src/utils/DomainSocket.cpp core/src/scheduler/CronTask.cpp
        core/src/utils/WindowsSocket.cpp core/src/utils/CronUtils.cpp core/src/scheduler/OneTimeTask.cpp core/src/utils/ZipUtils.cpp)
set(EXCEPTION_SOURCES core/src/exception/CoreException.cpp core/src/exception/DatabaseException.cpp
        core/src/exception/ServiceException.cpp core/src/exception/JsonException.cpp core/src/exception/NotFoundException.cpp
        core/src/exception/ForbiddenException.cpp core/src/exception/UnauthorizedException.cpp
        core/src/exception/BadRequestException.cpp)
set(LOGGING_SOURCES core/src/logging/LogStream.cpp core/src/logging/LogWebsocketSink.cpp core/src/logging/LogStream.cpp
        core/src/logging/LogWebsocketSink.cpp)
set(MONITORING_SOURCES core/src/monitoring/MonitoringCollector.cpp)
set(SOURCES ${UTILS_SOURCES} ${EXCEPTION_SOURCES} ${LOGGING_SOURCES} ${MONITORING_SOURCES})

#add_library(awsmockcore STATIC ${SOURCES})
if (APPLE)
    target_include_directories(awsmockcore PUBLIC ${EXTERNAL_INCLUDE_DIR} ${EXTERNAL_INCLUDE_DIR}/bsoncxx/v_noabi
            ${EXTERNAL_INCLUDE_DIR}/mongocxx/v_noabi "/opt/homebrew/opt/openssl@3/include"
            "/opt/homebrew/opt/libmagic/include" "/opt/homebrew/opt/libarchive/include" "/opt/homebrew/include")
    target_link_directories(awsmockcore PUBLIC ${EXTERNAL_LIB_DIR} "/opt/homebrew/opt/openssl@3/lib" "/opt/homebrew/opt/libmagic/lib"
            "/opt/homebrew/opt/libarchive/lib" "/opt/homebrew/lib")
    target_link_libraries(awsmockcore PUBLIC boost_log-vc145-mt-x64-1_89.lib boost_log_setup boost_thread boost_chrono boost_container
            boost_date_time boost_url boost_exception boost_filesystem boost_regex pthread)
    target_compile_options(awsmockcore PRIVATE -Wno-deprecated-declarations)

elseif (WIN32)

    add_library(awsmockcore SHARED ${SOURCES})
    target_include_directories(awsmockcore PRIVATE ${CMAKE_SOURCE_DIR}/core/include ${EXTERNAL_INCLUDE_DIR}
            ${EXTERNAL_INCLUDE_DIR}/bsoncxx/v_noabi ${EXTERNAL_INCLUDE_DIR}/mongocxx/v_noabi)
    target_link_directories(awsmockcore PUBLIC ${EXTERNAL_LIB_DIR})
    target_link_libraries(awsmockcore PUBLIC ${EXTERNAL_LIBS})

else ()
    target_include_directories(awsmockcore PUBLIC core/include ${EXTERNAL_INCLUDE_DIR}
            ${EXTERNAL_INCLUDE_DIR}/bsoncxx/v_noabi ${EXTERNAL_INCLUDE_DIR}/mongocxx/v_noabi)
    link_directories(awsmockcore ${EXTERNAL_LIB_DIR})
    target_link_libraries(awsmockcore PUBLIC boost_log boost_log_setup boost_thread boost_chrono
            boost_container boost_date_time boost_url boost_exception boost_filesystem boost_regex boost_process
            pthread rt)
    target_compile_options(awsmockcore PRIVATE -Wno-deprecated-declarations)
endif ()


# -------------------------------------------------------------------------
# Build AWSMock DTO library
# -------------------------------------------------------------------------

set(DTO_SOURCES dto/src/common/UserAgent.cpp dto/src/common/S3ClientCommand.cpp dto/src/common/SQSClientCommand.cpp
        dto/src/common/Services.cpp dto/src/module/model/Infrastructure.cpp dto/src/common/SNSClientCommand.cpp
        dto/src/common/DynamoDbClientCommand.cpp dto/src/common/SecretsManagerClientCommand.cpp
        dto/src/common/KMSClientCommand.cpp dto/src/common/LambdaClientCommand.cpp dto/src/common/SSMClientCommand.cpp
        dto/src/common/MonitoringClientCommand.cpp dto/src/common/CognitoClientCommand.cpp
        dto/src/common/ApiGatewayClientCommand.cpp dto/src/common/ApplicationClientCommand.cpp
        dto/src/common/ContainerClientCommand.cpp dto/src/sqs/mapper/Mapper.cpp dto/src/sns/mapper/Mapper.cpp
        dto/src/s3/mapper/Mapper.cpp dto/src/lambda/CreateTagRequest.cpp dto/src/lambda/CreateFunctionRequest.cpp
        dto/src/lambda/CreateFunctionResponse.cpp dto/src/lambda/GetFunctionResponse.cpp dto/src/lambda/mapper/Mapper.cpp
        dto/src/lambda/ListEventSourceMappingsRequest.cpp dto/src/lambda/ListEventSourceMappingsResponse.cpp
        dto/src/cognito/mapper/Mapper.cpp dto/src/transfer/ListUsersRequest.cpp dto/src/transfer/ListUsersResponse.cpp
        dto/src/transfer/CreateUserRequest.cpp dto/src/transfer/CreateUserResponse.cpp
        dto/src/transfer/DeleteServerRequest.cpp dto/src/transfer/StartServerRequest.cpp
        dto/src/transfer/StopServerRequest.cpp dto/src/transfer/mapper/Mapper.cpp
        dto/src/transfer/GetServerDetailsRequest.cpp dto/src/transfer/GetServerDetailsResponse.cpp
        dto/src/transfer/DeleteUserRequest.cpp dto/src/transfer/DeleteProtocolRequest.cpp dto/src/dynamodb/mapper/Mapper.cpp
        dto/src/secretsmanager/mapper/Mapper.cpp dto/src/ssm/mapper/Mapper.cpp dto/src/apps/mapper/Mapper.cpp
        dto/src/apigateway/mapper/Mapper.cpp dto/src/module/model/Module.cpp dto/src/module/model/GatewayConfig.cpp
        dto/src/module/mapper/Mapper.cpp dto/src/module/ExportInfrastructureResponse.cpp
        dto/src/module/CleanInfrastructureRequest.cpp dto/src/module/ListModuleNamesResponse.cpp
        dto/src/module/ImportInfrastructureRequest.cpp dto/src/monitoring/mapper/Mapper.cpp)

add_library(awsmockdto SHARED ${DTO_SOURCES})
if (APPLE)
    target_include_directories(awsmockdto PUBLIC core/include dto/include db/include ${EXTERNAL_INCLUDE_DIR}
            ${EXTERNAL_INCLUDE_DIR}/bsoncxx/v_noabi ${EXTERNAL_INCLUDE_DIR}/mongocxx/v_noabi
            "/opt/homebrew/opt/openssl@3/include" "/opt/homebrew/opt/libmagic/include"
            "/opt/homebrew/opt/libarchive/include" "/opt/homebrew/include")
    target_link_directories(awsmockdto PUBLIC ${EXTERNAL_LIB_DIR} "/opt/homebrew/opt/openssl@3/lib"
            "/opt/homebrew/opt/libmagic/lib" "/opt/homebrew/opt/libarchive/lib" "/opt/homebrew/lib")
    target_link_libraries(awsmockdto PUBLIC boost_log boost_log_setup boost_thread boost_chrono boost_container
            boost_date_time boost_url boost_exception boost_filesystem boost_regex boost_thread pthread)
    target_compile_options(awsmockdto PRIVATE -Wno-deprecated-declarations)

elseif (WIN32)

    target_include_directories(awsmockdto PUBLIC core/include dto/include db/include ${EXTERNAL_INCLUDE_DIR}
            ${EXTERNAL_INCLUDE_DIR}/bsoncxx/v_noabi ${EXTERNAL_INCLUDE_DIR}/mongocxx/v_noabi)
    target_link_directories(awsmockdto PUBLIC ${EXTERNAL_LIB_DIR})
    target_link_libraries(awsmockdto PUBLIC awsmockcore awsmockdb mongocxx-v_noabi-rhi-md.lib mongoc2.lib
            bsoncxx-v_noabi-rhi-md.lib bson2.lib boost_url-vc145-mt-x64-1_89.lib boost_filesystem-vc145-mt-x64-1_89.lib
            boost_locale-vc145-mt-x64-1_89.lib boost_log-vc145-mt-x64-1_89.lib boost_log_setup-vc145-mt-x64-1_89.lib
            boost_process-vc145-mt-x64-1_89.lib boost_json-vc145-mt-x64-1_89.lib libssl.lib libcrypto.lib archive.lib magic.lib ssh.lib pdh.lib)

else ()
    target_include_directories(awsmockdto PUBLIC core/include dto/include db/include ${EXTERNAL_INCLUDE_DIR}
            ${EXTERNAL_INCLUDE_DIR}/bsoncxx/v_noabi ${EXTERNAL_INCLUDE_DIR}/mongocxx/v_noabi)
    target_link_directories(awsmockdto PUBLIC ${EXTERNAL_LIB_DIR})
    target_link_libraries(awsmockdto PUBLIC boost_log boost_log_setup boost_thread boost_chrono boost_container
            boost_date_time boost_url boost_exception boost_filesystem boost_regex boost_thread pthread rt)
    target_compile_options(awsmockdto PRIVATE -Wno-deprecated-declarations)
endif ()

# -------------------------------------------------------------------------
# Build AWSMock Database library
# -------------------------------------------------------------------------

set(SQS_SOURCES db/src/entity/sqs/MessageAttribute.cpp db/src/entity/sqs/Message.cpp db/src/repository/SQSDatabase.cpp
        db/src/memorydb/SQSMemoryDb.cpp db/src/entity/sqs/Queue.cpp db/src/entity/sqs/QueueAttribute.cpp
        db/src/entity/sqs/RedrivePolicy.cpp)
set(SNS_SOURCES db/src/entity/sns/MessageAttribute.cpp db/src/entity/sns/Message.cpp db/src/entity/sns/Topic.cpp
        db/src/entity/sns/TopicAttribute.cpp db/src/entity/sns/Subscription.cpp db/src/repository/SNSDatabase.cpp
        db/src/memorydb/SNSMemoryDb.cpp)
set(S3_SOURCES db/src/entity/s3/Bucket.cpp db/src/entity/s3/BucketNotification.cpp db/src/entity/s3/Object.cpp
        db/src/repository/S3Database.cpp db/src/memorydb/S3MemoryDb.cpp db/src/entity/s3/FilterRule.cpp
        db/src/entity/s3/QueueNotification.cpp db/src/entity/s3/TopicNotification.cpp db/src/entity/s3/LambdaNotification.cpp
        db/src/entity/s3/BucketEncryption.cpp)
set(LAMBDA_SOURCES db/src/entity/lambda/Tags.cpp db/src/entity/lambda/Environment.cpp db/src/entity/lambda/Lambda.cpp
        db/src/entity/lambda/Code.cpp db/src/entity/lambda/EphemeralStorage.cpp db/src/repository/LambdaDatabase.cpp
        db/src/memorydb/LambdaMemoryDb.cpp db/src/entity/lambda/Instance.cpp db/src/entity/lambda/EventSourceMapping.cpp
        db/src/entity/lambda/LambdaResult.cpp)
set(TRANSFER_SOURCES db/src/repository/TransferDatabase.cpp db/src/entity/transfer/User.cpp db/src/entity/transfer/Transfer.cpp
        db/src/memorydb/TransferMemoryDb.cpp)
set(COGNITO_SOURCES db/src/entity/cognito/UserPool.cpp db/src/entity/cognito/User.cpp db/src/entity/cognito/UserPoolClient.cpp
        db/src/entity/cognito/Group.cpp db/src/memorydb/CognitoMemoryDb.cpp db/src/repository/CognitoDatabase.cpp)
set(DYNAMODB_SOURCES db/src/entity/dynamodb/Item.cpp db/src/entity/dynamodb/Table.cpp db/src/entity/dynamodb/AttributeValue.cpp
        db/src/entity/dynamodb/ProvisionedThroughput.cpp db/src/repository/DynamoDbDatabase.cpp
        db/src/memorydb/DynamoDbMemoryDb.cpp)
set(SECRETSMANAGER_SOURCES db/src/entity/secretsmanager/Secret.cpp db/src/entity/secretsmanager/SecretVersion.cpp
        db/src/entity/secretsmanager/RotationRules.cpp db/src/repository/SecretsManagerDatabase.cpp
        db/src/memorydb/SecretsManagerMemoryDb.cpp)
set(KMS_SOURCES db/src/entity/kms/Key.cpp db/src/repository/KMSDatabase.cpp db/src/memorydb/KMSMemoryDb.cpp)
set(SSM_SOURCES db/src/entity/ssm/Parameter.cpp db/src/repository/SSMDatabase.cpp db/src/memorydb/SSMMemoryDb.cpp)
set(MODULE_SOURCES db/src/entity/module/Module.cpp db/src/repository/ModuleDatabase.cpp db/src/memorydb/ModuleMemoryDb.cpp)
set(APPS_SOURCES db/src/entity/apps/Application.cpp db/src/repository/ApplicationDatabase.cpp db/src/memorydb/ApplicationMemoryDb.cpp)
set(UTIL_SOURCES db/src/utils/MongoUtils.cpp db/src/utils/ConnectionPool.cpp db/src/utils/SqsUtils.cpp)
set(API_GATEWAY_SOURCES db/src/repository/ApiGatewayDatabase.cpp db/src/memorydb/ApiGatewayMemoryDb.cpp
        db/src/entity/apigateway/ApiKey.cpp db/src/entity/apigateway/Rest.cpp)
set(MONITORING_SOURCES db/src/entity/monitoring/Counter.cpp db/src/repository/MonitoringDatabase.cpp)
set(SOURCES db/src/repository/Database.cpp db/src/utils/TestUtils.cpp ${SQS_SOURCES} ${SNS_SOURCES} ${S3_SOURCES}
        ${LAMBDA_SOURCES} ${TRANSFER_SOURCES} ${COGNITO_SOURCES} ${DYNAMODB_SOURCES} ${KMS_SOURCES} ${MODULE_SOURCES}
        ${SECRETSMANAGER_SOURCES} ${SSM_SOURCES} ${APPS_SOURCES} ${API_GATEWAY_SOURCES} ${UTIL_SOURCES}
        ${MONITORING_SOURCES})

add_library(awsmockdb SHARED ${SOURCES})
if (APPLE)
    target_include_directories(awsmockdb PUBLIC ${EXTERNAL_INCLUDE_DIR} ${EXTERNAL_INCLUDE_DIR}/bsoncxx/v_noabi
            ${EXTERNAL_INCLUDE_DIR}/mongocxx/v_noabi "/opt/homebrew/opt/openssl@3/include"
            "/opt/homebrew/opt/libmagic/include" "/opt/homebrew/opt/libarchive/include" "/opt/homebrew/include")
    target_link_directories(awsmockdb PUBLIC ${EXTERNAL_LIB_DIR} "/opt/homebrew/opt/openssl@3/lib" "/opt/homebrew/opt/libmagic/lib"
            "/opt/homebrew/opt/libarchive/lib" "/opt/homebrew/lib")
    target_link_libraries(awsmockdb PUBLIC awsmockcore mongocxx bsoncxx mongoc2 bson2
            boost_log boost_log_setup boost_thread boost_chrono boost_container
            boost_date_time boost_url boost_exception boost_filesystem boost_regex boost_thread ssl crypto
            magic lzma bz2 lz4 z zstd pthread)
    target_compile_options(awsmockdb PRIVATE -Wno-deprecated-declarations)

elseif (WIN32)

    target_include_directories(awsmockdb PUBLIC ${CMAKE_SOURCE_DIR}/db/include ${CMAKE_SOURCE_DIR}/core/include
            ${CMAKE_SOURCE_DIR}/dto/include ${EXTERNAL_INCLUDE_DIR} ${EXTERNAL_INCLUDE_DIR}/bsoncxx/v_noabi
            ${EXTERNAL_INCLUDE_DIR}/mongocxx/v_noabi)
    target_link_directories(awsmockdb PUBLIC ${EXTERNAL_LIB_DIR})
    target_link_libraries(awsmockdb PUBLIC awsmockcore mongocxx-v_noabi-rhi-md.lib mongoc2.lib bsoncxx-v_noabi-rhi-md.lib
            bson2.lib boost_url-vc145-mt-x64-1_89.lib boost_filesystem-vc145-mt-x64-1_89.lib
            boost_locale-vc145-mt-x64-1_89.lib boost_log-vc145-mt-x64-1_89.lib
            boost_log_setup-vc145-mt-x64-1_89.lib boost_process-vc145-mt-x64-1_89.lib libssl.lib libcrypto.lib
            archive.lib magic.lib ssh.lib pdh.lib)

else ()
    target_include_directories(awsmockdb PUBLIC core/include dto/include db/include ${EXTERNAL_INCLUDE_DIR}
            ${EXTERNAL_INCLUDE_DIR}/bsoncxx/v_noabi ${EXTERNAL_INCLUDE_DIR}/mongocxx/v_noabi)
    target_link_directories(awsmockdb PUBLIC ${EXTERNAL_LIB_DIR})
    target_link_libraries(awsmockdb PUBLIC awsmockdto awsmockcore mongocxx-static bsoncxx-static mongoc2 bson2 boost_log
            boost_log_setup boost_thread boost_chrono boost_container boost_date_time boost_url
            boost_exception boost_filesystem boost_regex boost_thread ssl crypto magic lzma bz2 lz4 z zstd icuuc icui18n
            icudata pthread rt)
    target_compile_options(awsmockdb PRIVATE -Wno-deprecated-declarations)
endif ()

# -------------------------------------------------------------------------
# Build AWSMock Service library
# -------------------------------------------------------------------------

set(SRV_SOURCES srv/src/common/AbstractHandler.cpp srv/src/common/AbstractServer.cpp
        srv/src/s3/S3Server.cpp srv/src/s3/S3Handler.cpp srv/src/s3/S3Service.cpp srv/src/s3/S3HashCreator.cpp
        srv/src/sqs/SQSServer.cpp srv/src/sqs/SQSHandler.cpp srv/src/sqs/SQSService.cpp srv/src/sns/SNSServer.cpp
        srv/src/sns/SNSHandler.cpp srv/src/sns/SNSService.cpp srv/src/lambda/LambdaServer.cpp
        srv/src/lambda/LambdaHandler.cpp srv/src/lambda/LambdaService.cpp srv/src/lambda/LambdaCreator.cpp
        srv/src/lambda/LambdaExecutor.cpp srv/src/cognito/CognitoHandler.cpp srv/src/cognito/CognitoHandler.cpp
        srv/src/cognito/CognitoService.cpp srv/src/cognito/CognitoServer.cpp srv/src/transfer/TransferServer.cpp
        srv/src/transfer/TransferHandler.cpp srv/src/transfer/TransferService.cpp srv/src/dynamodb/DynamoDbServer.cpp
        srv/src/dynamodb/DynamoDbHandler.cpp srv/src/dynamodb/DynamoDbService.cpp srv/src/secretsmanager/SecretsManagerServer.cpp
        srv/src/secretsmanager/SecretsManagerHandler.cpp srv/src/secretsmanager/SecretsManagerService.cpp
        srv/src/secretsmanager/SecretRotation.cpp srv/src/kms/KMSServer.cpp srv/src/kms/KMSHandler.cpp
        srv/src/kms/KMSService.cpp srv/src/kms/KMSCreator.cpp srv/src/ssm/SSMServer.cpp srv/src/ssm/SSMHandler.cpp
        srv/src/ssm/SSMService.cpp srv/src/ftpserver/Filesystem.cpp srv/src/ftpserver/FtpSession.cpp
        srv/src/ftpserver/FtpServer.cpp srv/src/ftpserver/FtpServerImpl.cpp srv/src/ftpserver/UserDatabase.cpp
        srv/src/sftpserver/SftpServer.cpp srv/src/sftpserver/SftpUser.cpp srv/src/gateway/GatewayServer.cpp
        srv/src/gateway/GatewaySession.cpp srv/src/gateway/GatewayListener.cpp srv/src/gateway/GatewayRouter.cpp
        srv/src/container/ContainerService.cpp srv/src/container/ContainerHandler.cpp srv/src/module/ModuleService.cpp
        srv/src/module/ModuleHandler.cpp srv/src/module/ModuleMonitoring.cpp srv/src/module/ModuleMap.cpp
        srv/src/application/ApplicationHandler.cpp srv/src/application/ApplicationServer.cpp
        srv/src/application/ApplicationService.cpp srv/src/application/ApplicationCreator.cpp
        srv/src/application/ApplicationLogServer.cpp srv/src/application/ApplicationLogSession.cpp
        srv/src/apigateway/ApiGatewayService.cpp srv/src/apigateway/ApiGatewayHandler.cpp
        srv/src/monitoring/MetricService.cpp srv/src/monitoring/MetricSystemCollector.cpp
        srv/src/monitoring/MonitoringHandler.cpp srv/src/monitoring/MonitoringServer.cpp
        srv/src/monitoring/MonitoringService.cpp srv/src/frontend/FrontendWorker.cpp srv/src/frontend/FrontendServer.cpp
        srv/src/frontend/FrontendSession.cpp)

add_library(awsmocksrv SHARED ${SRV_SOURCES})
if (APPLE)
    target_include_directories(awsmocksrv PUBLIC ${EXTERNAL_INCLUDE_DIR} ${EXTERNAL_INCLUDE_DIR}/bsoncxx/v_noabi
            ${EXTERNAL_INCLUDE_DIR}/mongocxx/v_noabi "/opt/homebrew/opt/openssl@3/include"
            "/opt/homebrew/opt/libmagic/include" "/opt/homebrew/opt/libarchive/include" "/opt/homebrew/include")
    target_link_directories(awsmocksrv PUBLIC ${EXTERNAL_LIB_DIR} "/opt/homebrew/opt/openssl@3/lib" "/opt/homebrew/opt/libmagic/lib"
            "/opt/homebrew/opt/libarchive/lib" "/opt/homebrew/lib")
    target_link_libraries(awsmocksrv PUBLIC awsmockdb awsmockdto awsmockcore boost_log boost_log_setup boost_thread
            boost_chrono boost_container boost_date_time boost_url boost_exception boost_filesystem boost_regex
            boost_context boost_thread archive xml2 lzma bz2 lz4 z zstd nettle gmp ssh ssl crypto)
    target_compile_options(awsmocksrv PRIVATE -Wno-deprecated-declarations)

elseif (WIN32)

    target_include_directories(awsmocksrv PUBLIC ${CMAKE_SOURCE_DIR}/core/include ${CMAKE_SOURCE_DIR}/dto/include
            ${CMAKE_SOURCE_DIR}/db/include ${CMAKE_SOURCE_DIR}/srv/include ${EXTERNAL_INCLUDE_DIR}
            ${EXTERNAL_INCLUDE_DIR}/bsoncxx/v_noabi ${EXTERNAL_INCLUDE_DIR}/mongocxx/v_noabi)
    target_link_directories(awsmocksrv PUBLIC ${EXTERNAL_LIB_DIR})
    target_link_libraries(awsmocksrv PUBLIC awsmockcore awsmockdb awsmockdto
            mongocxx-v_noabi-rhi-md.lib bsoncxx-v_noabi-rhi-md.lib boost_url-vc145-mt-x64-1_89.lib
            boost_filesystem-vc145-mt-x64-1_89.lib boost_json-vc145-mt-x64-1_89.lib boost_log-vc145-mt-x64-1_89.lib
            boost_log_setup-vc145-mt-x64-1_89.lib boost_process-vc145-mt-x64-1_89.lib boost_context-vc145-mt-x64-1_89.lib
            libssl.lib libcrypto.lib archive.lib magic.lib ssh.lib Iphlpapi.lib pdh.lib ntdll.lib shlwapi.lib)

else ()
    target_include_directories(awsmocksrv PUBLIC core/include dto/include db/include srv/include ${EXTERNAL_INCLUDE_DIR}
            ${EXTERNAL_INCLUDE_DIR}/bsoncxx/v_noabi ${EXTERNAL_INCLUDE_DIR}/mongocxx/v_noabi)
    target_link_directories(awsmocksrv PUBLIC ${EXTERNAL_LIB_DIR})
    target_link_libraries(awsmocksrv PUBLIC awsmockdb awsmockdto awsmockcore boost_log boost_log_setup boost_thread
            boost_chrono boost_container boost_date_time boost_url boost_exception boost_filesystem boost_regex
            boost_context boost_thread ssh resolv xml2 ssl crypto nettle gmp lzma bz2 lz4 z zstd icuuc icui18n icudata magic archive acl pthread rt)
    target_compile_options(awsmocksrv PRIVATE -Wno-deprecated-declarations)
endif ()

# -------------------------------------------------------------------------
# AWSMock Manager (main program)
# -------------------------------------------------------------------------

if (APPLE)
    add_executable(awsmockmgr manager/src/server/Manager.cpp manager/src/server/Monitoring.cpp manager/src/main.cpp)
    target_include_directories(awsmockmgr PUBLIC ${EXTERNAL_INCLUDE_DIR} ${EXTERNAL_INCLUDE_DIR}/bsoncxx/v_noabi
            ${EXTERNAL_INCLUDE_DIR}/mongocxx/v_noabi "/opt/homebrew/opt/openssl@3/include"
            "/opt/homebrew/opt/libmagic/include" "/opt/homebrew/opt/libarchive/include" "/opt/homebrew/include"
            "/opt/homebrew/opt/icu4c/include")
    target_link_directories(awsmockmgr PUBLIC ${EXTERNAL_LIB_DIR} "/opt/homebrew/opt/openssl@3/lib" "/opt/homebrew/opt/libmagic/lib"
            "/opt/homebrew/opt/libarchive/lib" "/opt/homebrew/lib")
    target_link_libraries(awsmockmgr PRIVATE awsmocksrv awsmockdto awsmockdb awsmockcore mongocxx-static bsoncxx-static
            mongoc2 bson2
            -Wl,-force_load,${EXTERNAL_LIB_DIR}/libboost_log.a
            -Wl,-force_load,${EXTERNAL_LIB_DIR}/libboost_date_time.a
            -Wl,-force_load,${EXTERNAL_LIB_DIR}/libboost_thread.a
            /opt/homebrew/opt/icu4c/lib/libicuuc.a /opt/homebrew/opt/icu4c/lib/libicui18n.a
            /opt/homebrew/opt/icu4c/lib/libicudata.a boost_log_setup boost_chrono boost_exception boost_regex
            boost_program_options boost_process boost_json boost_filesystem archive xml2 nettle gmp lzma bz2 lz4 z zstd
            magic ssl ssh crypto pthread resolv "-framework CoreFoundation" "-framework Security"
            "-framework SystemConfiguration" -Wl,-rpath,/usr/local/lib)
    target_compile_options(awsmockmgr PRIVATE -Wno-deprecated-declarations)

elseif (WIN32)

    add_executable(awsmockmgr WIN32 manager/src/server/Manager.cpp manager/src/server/Monitoring.cpp
            manager/src/WindowsService.cpp manager/src/main.cpp)
    set_target_properties(awsmockmgr PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
    target_include_directories(awsmockmgr PUBLIC ${CMAKE_SOURCE_DIR}/manager/include ${CMAKE_SOURCE_DIR}/core/include
            ${CMAKE_SOURCE_DIR}/srv/include ${CMAKE_SOURCE_DIR}/dto/include ${EXTERNAL_INCLUDE_DIR}
            ${EXTERNAL_INCLUDE_DIR}/bsoncxx/v_noabi ${EXTERNAL_INCLUDE_DIR}/mongocxx/v_noabi)
    target_link_directories(awsmockmgr PUBLIC ${EXTERNAL_LIB_DIR})
    target_link_libraries(awsmockmgr PUBLIC awsmockdto awsmockdb awsmockcore awsmocksrv
            mongocxx-v_noabi-rhi-md.lib mongoc2.lib bsoncxx-v_noabi-rhi-md.lib bson2.lib
            boost_filesystem-vc145-mt-x64-1_89.lib boost_url-vc145-mt-x64-1_89.lib
            boost_thread-vc145-mt-x64-1_89.lib boost_locale-vc145-mt-x64-1_89.lib
            boost_program_options-vc145-mt-x64-1_89.lib boost_log-vc145-mt-x64-1_89.lib
            boost_log_setup-vc145-mt-x64-1_89.lib boost_json-vc145-mt-x64-1_89.lib
            ssh.lib crypt32.lib Iphlpapi.lib pdh.lib ntdll.lib shlwapi.lib)
else ()
    add_executable(awsmockmgr manager/src/server/Manager.cpp manager/src/server/Monitoring.cpp manager/src/main.cpp)
    target_include_directories(awsmockmgr PUBLIC manager/include core/include dto/include db/include srv/include
            ${EXTERNAL_INCLUDE_DIR} ${EXTERNAL_INCLUDE_DIR}/bsoncxx/v_noabi ${EXTERNAL_INCLUDE_DIR}/mongocxx/v_noabi)
    target_link_directories(awsmockmgr PUBLIC ${EXTERNAL_LIB_DIR})
    target_link_libraries(awsmockmgr PUBLIC -Wl,--start-group awsmocksrv awsmockdto awsmockdb awsmockcore
            mongocxx-static bsoncxx-static mongoc2 bson2 boost_log boost_log_setup boost_thread boost_chrono
            boost_date_time boost_exception boost_regex boost_program_options boost_process boost_json boost_filesystem
            ssh ssl crypto archive xml2 nettle gmp lzma bz2 lz4 z zstd icuuc icui18n icudata magic acl pthread rt resolv
            ssh -Wl,--end-group)
    target_compile_options(awsmockmgr PRIVATE -Wno-deprecated-declarations)
endif ()

# -------------------------------------------------------------------------
# AWSMock Controller
# -------------------------------------------------------------------------

if (APPLE)
    add_executable(awsmockctl controller/src/Controller.cpp controller/src/main.cpp)
    target_link_libraries(awsmockctl PRIVATE awsmockdto awsmockdb awsmockcore mongocxx-static bsoncxx-static mongoc2 bson2
            -Wl,-force_load,${EXTERNAL_LIB_DIR}/libboost_log.a -Wl,-force_load,${EXTERNAL_LIB_DIR}/libboost_date_time.a
            -Wl,-force_load,${EXTERNAL_LIB_DIR}/libboost_thread.a boost_log_setup boost_chrono boost_exception
            boost_regex boost_program_options boost_process boost_json boost_filesystem archive xml2 nettle gmp lzma
            bz2 lz4 z zstd magic ssl crypto pthread resolv -Wl,-rpath,/usr/local/lib)
    target_include_directories(awsmockctl PRIVATE /opt/homebrew/opt/icu4c/include)
    target_link_libraries(awsmockctl PRIVATE /opt/homebrew/opt/icu4c/lib/libicuuc.a /opt/homebrew/opt/icu4c/lib/libicui18n.a
            /opt/homebrew/opt/icu4c/lib/libicudata.a)
    target_compile_options(awsmockctl PRIVATE -Wno-deprecated-declarations)

elseif (WIN32)

    add_executable(awsmockctl WIN32 controller/src/Controller.cpp controller/src/main.cpp)
    set_target_properties(awsmockctl PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
    target_include_directories(awsmockctl PUBLIC ${CMAKE_SOURCE_DIR}/controller/include ${CMAKE_SOURCE_DIR}/core/include
            ${CMAKE_SOURCE_DIR}/dto/include ${EXTERNAL_INCLUDE_DIR})
    target_link_directories(awsmockctl PUBLIC ${EXTERNAL_LIB_DIR})
    target_link_libraries(awsmockctl awsmockdto awsmockcore mongocxx bsoncxx mongoc2.lib bson2.lib
            libboost_filesystem-vc145-mt-x64-1_89.lib libboost_url-vc145-mt-x64-1_89.lib
            libboost_thread-vc145-mt-x64-1_89.lib libboost_locale-vc145-mt-x64-1_89.lib
            libboost_program_options-vc145-mt-x64-1_89.lib libboost_log-vc145-mt-x64-1_89.lib
            libboost_log_setup-vc145-mt-x64-1_89.lib libboost_json-vc145-mt-x64-1_89.lib
            ssh.lib crypt32.lib Iphlpapi.lib pdh.lib ntdll.lib shlwapi.lib)
else ()
    add_executable(awsmockctl controller/src/Controller.cpp controller/src/main.cpp)
    target_include_directories(awsmockctl PUBLIC controller/include core/include dto/include ${EXTERNAL_INCLUDE_DIR}
            ${EXTERNAL_INCLUDE_DIR}/bsoncxx/v_noabi ${EXTERNAL_INCLUDE_DIR}/mongocxx/v_noabi)
    target_link_directories(awsmockctl PUBLIC ${EXTERNAL_LIB_DIR})
    target_link_libraries(awsmockctl PRIVATE awsmockdto awsmockdb awsmockcore -Wl,--start-group mongocxx-static
            bsoncxx-static mongoc2 bson2 boost_log boost_log_setup boost_thread boost_chrono boost_date_time
            boost_exception boost_regex boost_program_options boost_process boost_json boost_filesystem boost_thread
            archive xml2 nettle gmp lzma bz2 lz4 z zstd icuuc icui18n icudata magic ssl crypto acl pthread rt resolv
            -Wl,--end-group)
    target_compile_options(awsmockctl PRIVATE -Wno-deprecated-declarations)
endif ()

# -------------------------------------------------------------------------
# AWSMock awslocal
# -------------------------------------------------------------------------

if (APPLE)

    add_executable(awslocal awslocal/src/AwsLocal.cpp awslocal/src/main.cpp)
    target_include_directories(awslocal PRIVATE awslocal/include /opt/homebrew/opt/icu4c/include)
    target_link_libraries(awslocal PRIVATE awsmockcore -Wl,-force_load,${EXTERNAL_LIB_DIR}/libboost_log.a
            -Wl,-force_load,${EXTERNAL_LIB_DIR}/libboost_date_time.a -Wl,-force_load,${EXTERNAL_LIB_DIR}/libboost_thread.a
            boost_log_setup boost_chrono boost_exception boost_regex boost_program_options boost_process boost_json
            boost_filesystem archive xml2 nettle gmp lzma bz2 lz4 z zstd magic ssl crypto pthread resolv
            -Wl,-rpath,/usr/local/lib)
    target_link_libraries(awslocal PRIVATE /opt/homebrew/opt/icu4c/lib/libicuuc.a /opt/homebrew/opt/icu4c/lib/libicui18n.a
            /opt/homebrew/opt/icu4c/lib/libicudata.a)
    target_compile_options(awslocal PRIVATE -Wno-deprecated-declarations)

elseif (WIN32)

    add_executable(awslocal WIN32 awslocal/src/AwsLocal.cpp awslocal/src/main.cpp)
    set_target_properties(awslocal PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
    target_include_directories(awslocal PUBLIC ${CMAKE_SOURCE_DIR}/awslocal/include ${CMAKE_SOURCE_DIR}/core/include
            ${EXTERNAL_INCLUDE_DIR})
    target_link_directories(awslocal PUBLIC ${EXTERNAL_LIB_DIR})
    target_link_libraries(awslocal awsmockcore boost_log-vc145-mt-x64-1_89.lib boost_log_setup-vc145-mt-x64-1_89.lib
            boost_program_options-vc145-mt-x64-1_89.lib boost_process-vc145-mt-x64-1_89.lib boost_json-vc145-mt-x64-1_89.lib
            ssh.lib crypt32.lib Iphlpapi.lib pdh.lib ntdll.lib shlwapi.lib)
else ()
    add_executable(awslocal awslocal/src/AwsLocal.cpp awslocal/src/main.cpp)
    target_include_directories(awslocal PUBLIC ${CMAKE_SOURCE_DIR}/awslocal/include ${CMAKE_SOURCE_DIR}core/include
            ${CMAKE_SOURCE_DIR}dto/include ${EXTERNAL_INCLUDE_DIR} ${EXTERNAL_INCLUDE_DIR}/bsoncxx/v_noabi
            ${EXTERNAL_INCLUDE_DIR}/mongocxx/v_noabi)
    target_link_directories(awslocal PUBLIC ${EXTERNAL_LIB_DIR})
    target_link_libraries(awslocal PRIVATE -Wl,--start-group awsmockcore boost_log boost_log_setup boost_chrono
            boost_date_time boost_exception boost_regex boost_process boost_program_options boost_json boost_filesystem
            boost_thread archive xml2 nettle gmp lzma bz2 lz4 z zstd icuuc icui18n icudata magic ssl crypto acl pthread
            rt resolv -Wl,--end-group)
    target_compile_options(awslocal PRIVATE -Wno-deprecated-declarations)
endif ()

# -------------------------------------------------------------------------
# Build documentation
# -------------------------------------------------------------------------
add_subdirectory(docs)

# -------------------------------------------------------------------------
# Install
# -------------------------------------------------------------------------

#install(TARGETS awsmockmgr DESTINATION bin)
#install(DIRECTORY ${EXTERNAL_ROOT}/lib DESTINATION .)
#install(DIRECTORY ${EXTERNAL_ROOT}/include DESTINATION .)
