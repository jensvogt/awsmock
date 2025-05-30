set(BINARY srv_tst)
set(STATIC_LIB awsmocksrv)

set(SQS_SOURCES SQSServiceTests.cpp SQSServerCliTest.cpp SQSServiceJavaTests.cpp)
set(SNS_SOURCES SNSServiceTests.cpp SNSServerJavaTests.cpp SNSServerCliTest.cpp)
set(S3_SOURCES S3ServiceTests.cpp S3ServerCliTest.cpp S3ServerJavaTests.cpp)
set(LAMBDA_SOURCES LambdaServiceTests.cpp LambdaServerCliTest.cpp)
set(DOCKER_SOURCES DockerServiceTests.cpp)
set(COGNITO_SOURCES CognitoServiceTests.cpp CognitoServerCliTest.cpp CognitoServerJavaTests.cpp)
set(DYNAMODB_SOURCES DynamoDbServerJavaTests.cpp DynamoDbServerCliTest.cpp DynamoDbServiceTests.cpp)
set(KMS_SOURCES KMSServiceTests.cpp KMSServerJavaTests.cpp KMSServiceCliTests.cpp)
set(SSM_SOURCES SSMServerCliTest.cpp)
set(TRANSFER_SOURCES TransferServerCliTest.cpp)
set(MAIN_SOURCES main.cpp TestBase.cpp)

# Includes
include_directories(../include ../../core/include ../../db/include ../../dto/include ${BSONCXX_INCLUDE_DIRS} ${MONGOCXX_INCLUDE_DIRS})

add_executable(${BINARY} ${MAIN_SOURCES})
target_link_libraries(${BINARY} PUBLIC ${STATIC_LIB} awsmockcore awsmockdto awsmockdb boost_locale boost_program_options
        boost_log boost_log_setup boost_process boost_unit_test_framework ssl crypto archive magic ssh)

add_test(NAME ${BINARY} COMMAND ${BINARY} WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})

#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/lambda/java-basic-1.0-SNAPSHOT.jar DESTINATION /tmp)
