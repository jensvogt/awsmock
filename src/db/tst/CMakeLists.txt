set(BINARY db_tst)
set(STATIC_LIB awsmockdb)

set(SQS_SOURCES SQSDatabaseTests.cpp SQSMemoryDbTests.cpp)
set(SNS_SOURCES SNSDatabaseTests.cpp SNSMemoryDbTests.cpp)
set(S3_SOURCES S3DatabaseTests.cpp S3MemoryDbTests.cpp)
set(MODULE_SOURCES ModuleDatabaseTests.cpp ModuleMemoryDbTests.cpp)
set(LAMBDA_SOURCES LambdaDatabaseTests.cpp LambdaMemoryDbTests.cpp)
set(COGNITO_SOURCES CognitoDatabaseTests.cpp CognitoMemoryDbTests.cpp)
set(SECRETSMANAGER_SOURCES SecretsManagerDatabaseTests.cpp)
set(SSM_SOURCES SSMDatabaseTests.cpp)
set(TRANSFER_SOURCES TransferDatabaseTest.cpp TransferMemoryDbTest.cpp)

# Includes
include_directories(../include ../../core/include ${BSONCXX_INCLUDE_DIRS} ${MONGOCXX_INCLUDE_DIRS})

add_executable(${BINARY} DatabaseTests.cpp ${SQS_SOURCES} ${S3_SOURCES} ${SNS_SOURCES} ${MODULE_SOURCES}
        ${LAMBDA_SOURCES} ${TRANSFER_SOURCES} ${COGNITO_SOURCES} ${SECRETSMANAGER_SOURCES} ${SSM_SOURCES} main.cpp)

target_link_libraries(${BINARY} PUBLIC ${STATIC_LIB} awsmockcore mongocxx bsoncxx boost_filesystem boost_program_options
        boost_url boost_thread boost_locale yaml-cpp ssl crypto archive magic gtest gmock)

add_test(NAME ${BINARY} COMMAND ${BINARY})
