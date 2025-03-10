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
include_directories(../include ../../core/include ${INC_BSON_CXX} ${INC_MONGO_CXX} ${EXT_INCLUDE})
link_directories(../core ${EXT_LIB})

add_executable(${BINARY} DatabaseTests.cpp ${SQS_SOURCES} ${S3_SOURCES} ${SNS_SOURCES} ${MODULE_SOURCES}
        ${LAMBDA_SOURCES} ${TRANSFER_SOURCES} ${COGNITO_SOURCES} ${SECRETSMANAGER_SOURCES} ${SSM_SOURCES} main.cpp)

target_link_libraries(${BINARY} PUBLIC ${STATIC_LIB} awsmockcore ${LIB_BOOST_THREAD})

add_test(NAME ${BINARY} COMMAND ${BINARY})
