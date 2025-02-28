set(BINARY srv_tst)
set(STATIC_LIB awsmocksrv)

set(SQS_SOURCES SQSServiceTests.cpp SQSServerCliTest.cpp SQSServerJavaTests.cpp)
set(SNS_SOURCES SNSServiceTests.cpp SNSServerJavaTests.cpp SNSServerCliTest.cpp)
set(S3_SOURCES S3ServiceTests.cpp S3ServerCliTest.cpp S3ServerJavaTests.cpp)
set(LAMBDA_SOURCES LambdaServiceTests.cpp LambdaServerCliTest.cpp)
set(DOCKER_SOURCES DockerServiceTests.cpp)
set(COGNITO_SOURCES CognitoServiceTests.cpp CognitoServerCliTest.cpp CognitoServerJavaTests.cpp)
set(DYNAMODB_SOURCES DynamodbServerJavaTests.cpp DynamoDbServerCliTest.cpp DynamoDbServiceTests.cpp)
set(KMS_SOURCES KMSServiceTests.cpp KMSServerJavaTests.cpp KMSServerCliTest.cpp)
set(SSM_SOURCES SSMServerCliTest.cpp)
set(TRANSFER_SOURCES TransferServerCliTest.cpp)
set(MAIN_SOURCES main.cpp ModuleServiceTests.cpp TestBase.cpp)

# Includes
include_directories(../include ../../core/include ../../db/include ../../dto/include ${INC_BSON_CXX} ${INC_MONGO_CXX}
        ${EXT_INCLUDE})
link_directories(../../core ../../db ../../dto ${EXT_LIB})

add_executable(${BINARY} ${SQS_SOURCES} ${SNS_SOURCES} ${S3_SOURCES} ${DOCKER_SOURCES} ${LAMBDA_SOURCES} ${COGNITO_SOURCES} ${DYNAMODB_SOURCES} ${KMS_SOURCES}
        ${SSM_SOURCES} ${TRANSFER_SOURCES} ${MAIN_SOURCES})
target_link_libraries(${BINARY} PUBLIC ${STATIC_LIB} awsmockcore awsmockdto awsmockdb ${LIB_BOOST_THREAD})

add_test(NAME ${BINARY} COMMAND ${BINARY} WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
