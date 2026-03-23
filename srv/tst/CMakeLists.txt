set(BINARY service_tst)
set(STATIC_LIB awsmocksrv)
set(CMAKE_BUILD_TYPE "Debug")

set(SOURCES Main.cpp DynamoDbServiceTest.cpp)

add_executable(${BINARY} ${SOURCES})
target_include_directories(${BINARY} PUBLIC ${CMAKE_SOURCE_DIR}/core/include ${CMAKE_SOURCE_DIR}/db/include
        ${CMAKE_SOURCE_DIR}/dto/include ${CMAKE_SOURCE_DIR}/srv/include)
target_link_libraries(${BINARY} PRIVATE awsmockcore awsmockdb awsmockdto awsmocksrv)
target_link_libraries(${BINARY} PRIVATE $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>)
target_link_libraries(${BINARY} PRIVATE $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>)
target_link_libraries(${BINARY} PRIVATE OpenSSL::SSL OpenSSL::Crypto Boost::boost Boost::log Boost::log_setup
        Boost::thread Boost::process Boost::date_time Boost::url Boost::unit_test_framework Boost::chrono Boost::locale
        LibArchive::LibArchive magic)

add_test(NAME ${BINARY} COMMAND ${BINARY} WORKING_DIRECTORY ..)
