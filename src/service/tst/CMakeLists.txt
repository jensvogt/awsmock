set(BINARY srv_tst)
set(STATIC_LIB awsmocksrv)

set(SQS_SOURCES SQSServiceTests.cpp SQSServerCliTest.cpp SQSServerJavaTests.cpp)
set(SNS_SOURCES SNSServiceTests.cpp SNSServerJavaTests.cpp SNSServerCliTest.cpp)
set(S3_SOURCES ../btst/S3ServiceCliTests.cpp ../btst/S3ServiceJavaTests.cpp)
set(LAMBDA_SOURCES ../btst/LambdaServiceTests.cpp ../btst/LambdaServiceCliTests.cpp)
set(DOCKER_SOURCES DockerServiceTests.cpp)
set(COGNITO_SOURCES CognitoServiceTests.cpp CognitoServerCliTest.cpp CognitoServerJavaTests.cpp)
set(DYNAMODB_SOURCES DynamoDbServerJavaTests.cpp DynamoDbServerCliTest.cpp DynamoDbServiceTests.cpp)
set(KMS_SOURCES KMSServiceTests.cpp KMSServerJavaTests.cpp KMSServiceCliTests.cpp)
set(SSM_SOURCES SSMServerCliTest.cpp)
set(TRANSFER_SOURCES TransferServerCliTest.cpp)
set(MAIN_SOURCES main.cpp ModuleServiceTests.cpp TestBase.cpp)

# Includes
include_directories(../include ../../core/include ../../db/include ../../dto/include ${BSONCXX_INCLUDE_DIRS} ${MONGOCXX_INCLUDE_DIRS})

add_executable(${BINARY} ${SQS_SOURCES} ${SNS_SOURCES} ${S3_SOURCES} ${DOCKER_SOURCES} ${LAMBDA_SOURCES}
        ${COGNITO_SOURCES} ${DYNAMODB_SOURCES} ${KMS_SOURCES} ${SSM_SOURCES} ${TRANSFER_SOURCES} ${MAIN_SOURCES})
target_link_libraries(${BINARY} PUBLIC ${STATIC_LIB} awsmockcore awsmockdto awsmockdb boost_locale boost_program_options
        boost_log boost_log_setup boost_process ssl crypto archive magic gtest gmock ssh)

add_test(NAME ${BINARY} COMMAND ${BINARY} WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/lambda/java-basic-1.0-SNAPSHOT.jar DESTINATION /tmp)
