set(BINARY srv_tst)

# Includes
include_directories(../include ../../core/include ../../db/include ../../dto/include ${BSONCXX_INCLUDE_DIRS} ${MONGOCXX_INCLUDE_DIRS})

# Libraries
set(LIBRARIES awsmockcore awsmockdto awsmockdb awsmocksrv mongocxx bsoncxx boost_locale boost_program_options boost_log
        boost_log_setup boost_process boost_unit_test_framework boost_coroutine boost_context ssl crypto archive magic ssh)

# KMS
add_executable(srv_kms_service KMSServiceTests.cpp TestBase.cpp)
target_link_libraries(srv_kms_service PUBLIC ${LIBRARIES})
add_executable(srv_kms_cli KMSServiceCliTests.cpp TestBase.cpp)
target_link_libraries(srv_kms_cli PUBLIC ${LIBRARIES})
add_executable(srv_kms_java KMSServiceJavaTests.cpp TestBase.cpp)
target_link_libraries(srv_kms_java PUBLIC ${LIBRARIES})

# Secrets manager
add_executable(srv_secret_service SecretsManagerServiceTests.cpp TestBase.cpp)
target_link_libraries(srv_secret_service PUBLIC ${LIBRARIES})
#add_executable(srv_secret_cli SecretsManagerServiceCliTests.cpp TestBase.cpp)
#target_link_libraries(srv_secret_cli PUBLIC ${LIBRARIES})
#add_executable(srv_secret_java SecretsManagerServiceJavaTests.cpp TestBase.cpp)
#target_link_libraries(srv_secret_java PUBLIC ${LIBRARIES})

# SQS
add_executable(srv_sqs_service SQSServiceTests.cpp TestBase.cpp)
target_link_libraries(srv_sqs_service PUBLIC ${LIBRARIES})
add_executable(srv_sqs_cli SQSServiceCliTests.cpp TestBase.cpp)
target_link_libraries(srv_sqs_cli PUBLIC ${LIBRARIES})
add_executable(srv_sqs_java SQSServiceJavaTests.cpp TestBase.cpp)
target_link_libraries(srv_sqs_java PUBLIC ${LIBRARIES})

# SNS
add_executable(srv_sns_service SNSServiceTests.cpp TestBase.cpp)
target_link_libraries(srv_sns_service PUBLIC ${LIBRARIES})
add_executable(srv_sns_cli SNSServiceCliTests.cpp TestBase.cpp)
target_link_libraries(srv_sns_cli PUBLIC ${LIBRARIES})
add_executable(srv_sns_java SNSServiceJavaTests.cpp TestBase.cpp)
target_link_libraries(srv_sns_java PUBLIC ${LIBRARIES})

# S3
add_executable(srv_s3_service S3ServiceTests.cpp TestBase.cpp)
target_link_libraries(srv_s3_service PUBLIC ${LIBRARIES})
add_executable(srv_s3_cli S3ServiceCliTests.cpp TestBase.cpp)
target_link_libraries(srv_s3_cli PUBLIC ${LIBRARIES})
add_executable(srv_s3_java S3ServiceJavaTests.cpp TestBase.cpp)
target_link_libraries(srv_s3_java PUBLIC ${LIBRARIES})

# Lambda
add_executable(srv_lambda_service LambdaServiceTests.cpp TestBase.cpp)
target_link_libraries(srv_lambda_service PUBLIC ${LIBRARIES})
add_executable(srv_lambda_cli LambdaServiceCliTests.cpp TestBase.cpp)
target_link_libraries(srv_lambda_cli PUBLIC ${LIBRARIES})

add_test(NAME srv_kms_service COMMAND srv_kms_service WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
add_test(NAME srv_kms_cli COMMAND srv_kms_cli WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
#add_test(NAME srv_kms_java COMMAND srv_kms_java WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
add_test(NAME srv_secret_service COMMAND srv_secret_service WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
#add_test(NAME srv_secret_cli COMMAND srv_secret_cli WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
#add_test(NAME srv_secret_java COMMAND srv_secret_java WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
add_test(NAME srv_sqs_service COMMAND srv_sqs_service WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
add_test(NAME srv_sqs_cli COMMAND srv_sqs_cli WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
add_test(NAME srv_sqs_java COMMAND srv_sqs_java WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
add_test(NAME srv_sns_service COMMAND srv_sns_service WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
add_test(NAME srv_sns_cli COMMAND srv_sns_cli WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
add_test(NAME srv_sns_java COMMAND srv_sns_java WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
add_test(NAME srv_s3_service COMMAND srv_s3_service WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
add_test(NAME srv_s3_cli COMMAND srv_s3_cli WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
add_test(NAME srv_s3_java COMMAND srv_s3_java WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
add_test(NAME srv_lambda_service COMMAND srv_lambda_service WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
add_test(NAME srv_lambda_cli COMMAND srv_lambda_cli WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/lambda/java-basic-1.0-SNAPSHOT.jar DESTINATION /tmp)
