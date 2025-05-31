set(BINARY srv_tst)
set(STATIC_LIB awsmocksrv)

# Includes
include_directories(../include ../../core/include ../../db/include ../../dto/include ${BSONCXX_INCLUDE_DIRS} ${MONGOCXX_INCLUDE_DIRS})

# Libraries
set(LIBRARIES awsmockcore awsmockdto awsmockdb boost_locale boost_program_options boost_log boost_log_setup
        boost_process boost_unit_test_framework ssl crypto archive magic ssh)

# KMS
add_executable(srv_kms_service KMSServiceTests.cpp TestBase.cpp)
target_link_libraries(srv_kms_service PUBLIC ${STATIC_LIB} ${LIBRARIES})
add_executable(srv_kms_cli KMSServiceCliTests.cpp TestBase.cpp)
target_link_libraries(srv_kms_cli PUBLIC ${STATIC_LIB} ${LIBRARIES})
add_executable(srv_kms_java KMSServiceJavaTests.cpp TestBase.cpp)
target_link_libraries(srv_kms_java PUBLIC ${STATIC_LIB} ${LIBRARIES})

# Secrets manager
add_executable(srv_secret_service SecretsManagerServiceTests.cpp TestBase.cpp)
target_link_libraries(srv_secret_service PUBLIC ${STATIC_LIB} ${LIBRARIES})
#add_executable(srv_secret_cli SecretsManagerServiceCliTests.cpp TestBase.cpp)
#target_link_libraries(srv_secret_cli PUBLIC ${STATIC_LIB} ${LIBRARIES})
#add_executable(srv_secret_java SecretsManagerServiceJavaTests.cpp TestBase.cpp)
#target_link_libraries(srv_secret_java PUBLIC ${STATIC_LIB} ${LIBRARIES})

# SQS
add_executable(srv_sqs_service SQSServiceTests.cpp TestBase.cpp)
target_link_libraries(srv_sqs_service PUBLIC ${STATIC_LIB} ${LIBRARIES})
add_executable(srv_sqs_cli SQSServiceCliTests.cpp TestBase.cpp)
target_link_libraries(srv_sqs_cli PUBLIC ${STATIC_LIB} ${LIBRARIES})
add_executable(srv_sqs_java SQSServiceJavaTests.cpp TestBase.cpp)
target_link_libraries(srv_sqs_java PUBLIC ${STATIC_LIB} ${LIBRARIES})

# SQS
add_executable(srv_sns_service SNSServiceTests.cpp TestBase.cpp)
target_link_libraries(srv_sns_service PUBLIC ${STATIC_LIB} ${LIBRARIES})

#add_test(NAME srv_kms_service COMMAND srv_kms_service WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
add_test(NAME srv_kms_cli COMMAND srv_kms_cli WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
#add_test(NAME srv_kms_java COMMAND srv_kms_java WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
add_test(NAME srv_secret_service COMMAND srv_secret_service WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
#add_test(NAME srv_secret_cli COMMAND srv_secret_cli WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
#add_test(NAME srv_secret_java COMMAND srv_secret_java WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
add_test(NAME srv_sqs_service COMMAND srv_sqs_service WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
add_test(NAME srv_sqs_cli COMMAND srv_sqs_cli WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
add_test(NAME srv_sqs_java COMMAND srv_sqs_java WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
add_test(NAME srv_sns_service COMMAND srv_sns_service WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})

#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/lambda/java-basic-1.0-SNAPSHOT.jar DESTINATION /tmp)
