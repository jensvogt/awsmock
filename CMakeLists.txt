cmake_minimum_required(VERSION 3.22)

project(awsmock LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -------------------------------------------------------------------------
# Global link flags
# -------------------------------------------------------------------------

if (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    add_definitions(-D_WIN32_WINNT=0x0A00)
    add_compile_options(/MP8 /FS "/Zc:__cplusplus" /EHsc "$<$<CONFIG:RELEASE>:/MT>" "$<$<CONFIG:DEBUG>:/MTd>"
            /D "_WIN32" "$<$<CONFIG:RELEASE>:/O2iy>" "$<$<CONFIG:DEBUG>:/Od>" /bigobj)
endif ()

# -------------------------------------------------------------------------
# Dependencies
# -------------------------------------------------------------------------
find_package(LibArchive REQUIRED)
find_package(bsoncxx CONFIG REQUIRED)
find_package(mongocxx CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED COMPONENTS filesystem thread process date_time program_options json url chrono log log_setup)
find_package(jwt-cpp CONFIG REQUIRED)
find_package(OpenSSL REQUIRED COMPONENTS SSL Crypto)
find_package(libssh CONFIG REQUIRED)
if (WIN32)
    set(EXTERNAL_INC_DIR ${CMAKE_SOURCE_DIR}/extern/win32/include)
    set(EXTERNAL_LIB_DIR ${CMAKE_SOURCE_DIR}/extern/win32/lib)
    find_package(utf8proc REQUIRED)
    find_package(Boost REQUIRED COMPONENTS system)
elseif (UNIX)
    pkg_check_modules(LIBMAGIC REQUIRED libmagic)
    include_directories(${LIBMAGIC_INCLUDE_DIRS})
    link_directories(${LIBMAGIC_LIBRARY_DIRS})
    add_definitions(${LIBMAGIC_CFLAGS_OTHER})
endif ()

# -------------------------------------------------------------------------
# Build AWSMock Core library
# -------------------------------------------------------------------------

set(CORE_SOURCES core/src/utils/StringUtils.cpp core/src/utils/FileUtils.cpp core/src/utils/DirUtils.cpp
        core/src/utils/DateTimeUtils.cpp core/src/utils/CryptoUtils.cpp core/src/utils/AwsUtils.cpp
        core/src/utils/SystemUtils.cpp core/src/utils/TarUtils.cpp core/src/utils/RandomUtils.cpp
        core/src/utils/TestUtils.cpp core/src/utils/HttpUtils.cpp core/src/utils/NumberUtils.cpp
        core/src/utils/MemoryMappedFile.cpp core/src/utils/XmlUtils.cpp core/src/utils/UnixSocket.cpp
        core/src/utils/HttpSocket.cpp core/src/utils/JwtUtils.cpp core/src/scheduler/PeriodicTask.cpp
        core/src/scheduler/Scheduler.cpp core/src/utils/BackupUtils.cpp core/src/config/Configuration.cpp
        core/src/utils/SrpUtils.cpp core/src/utils/DomainSocket.cpp core/src/scheduler/CronTask.cpp
        core/src/utils/WindowsSocket.cpp core/src/utils/CronUtils.cpp core/src/scheduler/OneTimeTask.cpp
        core/src/utils/ZipUtils.cpp core/src/exception/CoreException.cpp core/src/exception/DatabaseException.cpp
        core/src/exception/ServiceException.cpp core/src/exception/JsonException.cpp core/src/exception/NotFoundException.cpp
        core/src/exception/ForbiddenException.cpp core/src/exception/UnauthorizedException.cpp
        core/src/exception/BadRequestException.cpp core/src/logging/LogStream.cpp core/src/logging/LogWebsocketSink.cpp
        core/src/logging/LogStream.cpp core/src/logging/LogWebsocketSink.cpp core/src/monitoring/MonitoringCollector.cpp)
set(SOURCES ${UTILS_SOURCES} ${EXCEPTION_SOURCES} ${LOGGING_SOURCES} ${MONITORING_SOURCES})

add_library(awsmockcore STATIC ${CORE_SOURCES})
target_include_directories(awsmockcore PRIVATE ${CMAKE_SOURCE_DIR}/core/include)
target_link_libraries(awsmockcore PRIVATE $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>)
target_link_libraries(awsmockcore PRIVATE $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>)
target_link_libraries(awsmockcore PUBLIC OpenSSL::SSL OpenSSL::Crypto Boost::boost Boost::log Boost::log_setup
        Boost::thread Boost::process Boost::date_time Boost::url Boost::chrono LibArchive::LibArchive magic)
if (WIN32)
    target_include_directories(awsmockcore PRIVATE ${EXTERNAL_INC_DIR})
    target_link_libraries(awsmockcore PUBLIC Boost::system)
    target_link_directories(awsmockcore PUBLIC ${EXTERNAL_LIB_DIR})
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
        dto/src/s3/mapper/Mapper.cpp dto/src/lambda/mapper/Mapper.cpp dto/src/cognito/mapper/Mapper.cpp
        dto/src/transfer/mapper/Mapper.cpp dto/src/dynamodb/mapper/Mapper.cpp dto/src/secretsmanager/mapper/Mapper.cpp
        dto/src/ssm/mapper/Mapper.cpp dto/src/apps/mapper/Mapper.cpp dto/src/apigateway/mapper/Mapper.cpp
        dto/src/module/model/Module.cpp dto/src/module/mapper/Mapper.cpp dto/src/module/ExportInfrastructureResponse.cpp
        dto/src/module/ListModuleNamesResponse.cpp dto/src/module/ImportInfrastructureRequest.cpp
        dto/src/monitoring/mapper/Mapper.cpp)

add_library(awsmockdto STATIC ${DTO_SOURCES})
target_include_directories(awsmockdto PRIVATE ${CMAKE_SOURCE_DIR}/core/include ${CMAKE_SOURCE_DIR}/dto/include
        ${CMAKE_SOURCE_DIR}/db/include)
target_link_libraries(awsmockdto PUBLIC awsmockcore awsmockdb OpenSSL::SSL OpenSSL::Crypto Boost::boost Boost::log
        Boost::log_setup Boost::thread Boost::process Boost::date_time Boost::url Boost::chrono Boost::json
        LibArchive::LibArchive)
target_link_libraries(awsmockdto PRIVATE $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>)
target_link_libraries(awsmockdto PRIVATE $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>)

# -------------------------------------------------------------------------
# Build AWSMock Database library
# -------------------------------------------------------------------------

set(DB_SOURCES db/src/entity/sqs/MessageAttribute.cpp db/src/entity/sqs/Message.cpp db/src/repository/SQSDatabase.cpp
        db/src/memorydb/SQSMemoryDb.cpp db/src/entity/sqs/Queue.cpp db/src/entity/sqs/QueueAttribute.cpp
        db/src/entity/sqs/RedrivePolicy.cpp db/src/entity/sns/MessageAttribute.cpp db/src/entity/sns/Message.cpp
        db/src/entity/sns/Topic.cpp db/src/entity/sns/TopicAttribute.cpp db/src/entity/sns/Subscription.cpp
        db/src/repository/SNSDatabase.cpp db/src/memorydb/SNSMemoryDb.cpp db/src/entity/s3/Bucket.cpp
        db/src/entity/s3/BucketNotification.cpp db/src/entity/s3/Object.cpp db/src/repository/S3Database.cpp
        db/src/memorydb/S3MemoryDb.cpp db/src/entity/s3/FilterRule.cpp db/src/entity/s3/QueueNotification.cpp
        db/src/entity/s3/TopicNotification.cpp db/src/entity/s3/LambdaNotification.cpp db/src/entity/s3/BucketEncryption.cpp
        db/src/entity/lambda/Tags.cpp db/src/entity/lambda/Environment.cpp db/src/entity/lambda/Lambda.cpp
        db/src/entity/lambda/Code.cpp db/src/entity/lambda/EphemeralStorage.cpp db/src/repository/LambdaDatabase.cpp
        db/src/memorydb/LambdaMemoryDb.cpp db/src/entity/lambda/Instance.cpp db/src/entity/lambda/EventSourceMapping.cpp
        db/src/entity/lambda/LambdaResult.cpp db/src/repository/TransferDatabase.cpp db/src/entity/transfer/User.cpp
        db/src/entity/transfer/Transfer.cpp db/src/memorydb/TransferMemoryDb.cpp db/src/entity/cognito/UserPool.cpp
        db/src/entity/cognito/User.cpp db/src/entity/cognito/UserPoolClient.cpp db/src/entity/cognito/Group.cpp
        db/src/memorydb/CognitoMemoryDb.cpp db/src/repository/CognitoDatabase.cpp db/src/entity/dynamodb/Item.cpp
        db/src/entity/dynamodb/Table.cpp db/src/entity/dynamodb/AttributeValue.cpp db/src/entity/dynamodb/ProvisionedThroughput.cpp
        db/src/repository/DynamoDbDatabase.cpp db/src/memorydb/DynamoDbMemoryDb.cpp db/src/entity/secretsmanager/Secret.cpp
        db/src/entity/secretsmanager/SecretVersion.cpp db/src/entity/secretsmanager/RotationRules.cpp
        db/src/repository/SecretsManagerDatabase.cpp db/src/memorydb/SecretsManagerMemoryDb.cpp db/src/entity/kms/Key.cpp
        db/src/repository/KMSDatabase.cpp db/src/memorydb/KMSMemoryDb.cpp db/src/entity/ssm/Parameter.cpp
        db/src/repository/SSMDatabase.cpp db/src/memorydb/SSMMemoryDb.cpp db/src/entity/module/Module.cpp
        db/src/repository/ModuleDatabase.cpp db/src/memorydb/ModuleMemoryDb.cpp db/src/entity/apps/Application.cpp
        db/src/repository/ApplicationDatabase.cpp db/src/memorydb/ApplicationMemoryDb.cpp db/src/utils/MongoUtils.cpp
        db/src/utils/ConnectionPool.cpp db/src/utils/SqsUtils.cpp db/src/repository/ApiGatewayDatabase.cpp
        db/src/memorydb/ApiGatewayMemoryDb.cpp db/src/entity/apigateway/ApiKey.cpp db/src/entity/apigateway/Rest.cpp
        db/src/entity/monitoring/Counter.cpp db/src/repository/MonitoringDatabase.cpp db/src/repository/Database.cpp
        db/src/utils/TestUtils.cpp)

add_library(awsmockdb STATIC ${DB_SOURCES})
target_include_directories(awsmockdb PRIVATE ${CMAKE_SOURCE_DIR}/core/include ${CMAKE_SOURCE_DIR}/dto/include
        ${CMAKE_SOURCE_DIR}/db/include)
target_link_libraries(awsmockdb PUBLIC awsmockcore OpenSSL::SSL OpenSSL::Crypto Boost::boost Boost::log Boost::log_setup
        Boost::thread Boost::process Boost::date_time Boost::url Boost::chrono Boost::json LibArchive::LibArchive)
target_link_libraries(awsmockdb PRIVATE $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>)
target_link_libraries(awsmockdb PRIVATE $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>)

# -------------------------------------------------------------------------
# Build AWSMock Service library
# -------------------------------------------------------------------------

set(SRV_SOURCES srv/src/common/AbstractHandler.cpp srv/src/common/AbstractServer.cpp srv/src/s3/S3Server.cpp
        srv/src/s3/S3Handler.cpp srv/src/s3/S3Service.cpp srv/src/s3/S3HashCreator.cpp srv/src/sqs/SQSServer.cpp
        srv/src/sqs/SQSHandler.cpp srv/src/sqs/SQSService.cpp srv/src/sns/SNSServer.cpp srv/src/sns/SNSHandler.cpp
        srv/src/sns/SNSService.cpp srv/src/lambda/LambdaServer.cpp srv/src/lambda/LambdaHandler.cpp
        srv/src/lambda/LambdaService.cpp srv/src/lambda/LambdaCreator.cpp srv/src/lambda/LambdaExecutor.cpp
        srv/src/cognito/CognitoHandler.cpp srv/src/cognito/CognitoHandler.cpp srv/src/cognito/CognitoService.cpp
        srv/src/cognito/CognitoServer.cpp srv/src/transfer/TransferServer.cpp srv/src/transfer/TransferHandler.cpp
        srv/src/transfer/TransferService.cpp srv/src/dynamodb/DynamoDbServer.cpp srv/src/dynamodb/DynamoDbHandler.cpp
        srv/src/dynamodb/DynamoDbService.cpp srv/src/secretsmanager/SecretsManagerServer.cpp
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

add_library(awsmocksrv STATIC ${SRV_SOURCES})
target_include_directories(awsmocksrv PRIVATE ${CMAKE_SOURCE_DIR}/core/include ${CMAKE_SOURCE_DIR}/dto/include
        ${CMAKE_SOURCE_DIR}/db/include ${CMAKE_SOURCE_DIR}/srv/include)
target_link_libraries(awsmocksrv PUBLIC awsmockcore awsmockdb awsmockdto OpenSSL::SSL OpenSSL::Crypto
        Boost::boost Boost::log Boost::log_setup Boost::thread Boost::process Boost::date_time Boost::url Boost::chrono
        LibArchive::LibArchive ssh)
target_link_libraries(awsmocksrv PRIVATE $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>)
target_link_libraries(awsmocksrv PRIVATE $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>)

# -------------------------------------------------------------------------
# AWSMock Manager (main program)
# -------------------------------------------------------------------------

add_executable(awsmockmgr manager/src/server/Manager.cpp manager/src/server/Monitoring.cpp manager/src/WindowsService.cpp
        manager/src/main.cpp)
target_include_directories(awsmockmgr PRIVATE ${CMAKE_SOURCE_DIR}/manager/include ${CMAKE_SOURCE_DIR}/core/include
        ${CMAKE_SOURCE_DIR}/dto/include ${CMAKE_SOURCE_DIR}/db/include ${CMAKE_SOURCE_DIR}/srv/include)
target_link_libraries(awsmockmgr PUBLIC awsmockcore awsmockdb awsmockdto awsmocksrv OpenSSL::SSL OpenSSL::Crypto
        Boost::boost Boost::log Boost::log_setup Boost::thread Boost::process Boost::date_time Boost::url
        Boost::program_options Boost::chrono LibArchive::LibArchive ssh magic)
target_link_libraries(awsmockmgr PRIVATE $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>)
target_link_libraries(awsmockmgr PRIVATE $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>)
if (WIN32)
    target_link_directories(awsmockmgr PUBLIC ${EXTERNAL_LIB_DIR})
    target_link_libraries(awsmockmgr PUBLIC Boost::system pcre2-posix pcre2-8 crypt32.lib Iphlpapi.lib pdh.lib ntdll.lib shlwapi.lib)
endif ()

# -------------------------------------------------------------------------
# AWSMock Controller
# -------------------------------------------------------------------------

add_executable(awsmockctl controller/src/Controller.cpp controller/src/main.cpp)
target_include_directories(awsmockctl PRIVATE ${CMAKE_SOURCE_DIR}/controller/include ${CMAKE_SOURCE_DIR}/core/include
        ${CMAKE_SOURCE_DIR}/dto/include ${CMAKE_SOURCE_DIR}/db/include ${CMAKE_SOURCE_DIR}/srv/include)
target_link_libraries(awsmockctl PRIVATE $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>)
target_link_libraries(awsmockctl PRIVATE $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>)
target_link_libraries(awsmockctl PUBLIC awsmockcore awsmockdb awsmockdto awsmocksrv OpenSSL::SSL OpenSSL::Crypto
        Boost::boost Boost::log Boost::log_setup Boost::thread Boost::process Boost::date_time Boost::url
        Boost::program_options Boost::chrono LibArchive::LibArchive magic)
if (WIN32)
    target_link_directories(awsmockctl PUBLIC ${EXTERNAL_LIB_DIR})
    target_link_libraries(awsmockctl PUBLIC Boost::system pcre2-posix pcre2-8 shlwapi.lib)
endif ()

# -------------------------------------------------------------------------
# AWSMock awslocal
# -------------------------------------------------------------------------

add_executable(awslocal awslocal/src/AwsLocal.cpp awslocal/src/main.cpp)
target_include_directories(awslocal PRIVATE ${CMAKE_SOURCE_DIR}/awslocal/include ${CMAKE_SOURCE_DIR}/core/include
        ${CMAKE_SOURCE_DIR}/dto/include)
target_link_libraries(awslocal PUBLIC awsmockcore awsmockdto OpenSSL::SSL OpenSSL::Crypto Boost::boost Boost::log
        Boost::log_setup Boost::thread Boost::process Boost::date_time Boost::url Boost::program_options Boost::chrono
        LibArchive::LibArchive magic)
if (WIN32)
    target_link_directories(awslocal PUBLIC ${EXTERNAL_LIB_DIR})
    target_link_libraries(awslocal PUBLIC shlwapi.lib)
endif ()

# -------------------------------------------------------------------------
# Build documentation
# -------------------------------------------------------------------------
add_subdirectory(docs)
