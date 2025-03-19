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

target_link_libraries(${BINARY} PUBLIC ${STATIC_LIB} awsmockcore
        $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>
        $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>
        Boost::filesystem Boost::url Boost::thread Boost::locale yaml-cpp::yaml-cpp OpenSSL::SSL OpenSSL::SSL
        LibArchive::LibArchive unofficial::libmagic::libmagic GTest::gtest GTest::gtest_main GTest::gmock
        GTest::gmock_main)

add_test(NAME ${BINARY} COMMAND ${BINARY})
