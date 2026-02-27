set(BINARY core_tst)
set(STATIC_LIB awsmockcore)
set(CMAKE_BUILD_TYPE "Debug")

set(SOURCES Main.cpp CryptoUtilsTest.cpp AwsUtilsTests.cpp ConfigurationTests.cpp CronUtilsTests.cpp DateTimeUtilsTests.cpp)

add_executable(${BINARY} ${SOURCES})
target_include_directories(${BINARY} PUBLIC ${CMAKE_SOURCE_DIR}/core/include)
target_link_libraries(${BINARY} PRIVATE awsmockcore)
target_link_libraries(${BINARY} PRIVATE $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>)
target_link_libraries(${BINARY} PRIVATE $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>)
target_link_libraries(${BINARY} PRIVATE OpenSSL::SSL OpenSSL::Crypto Boost::boost Boost::log Boost::log_setup
        Boost::thread Boost::process Boost::date_time Boost::url Boost::unit_test_framework Boost::chrono Boost::locale
        LibArchive::LibArchive magic)

add_test(NAME ${BINARY} COMMAND ${BINARY} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/images/7337529778404.tif DESTINATION /tmp)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/images/9783911244381.jpg DESTINATION /tmp)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/pdf/dummy.pdf DESTINATION /tmp)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/lambda/java-basic-1.0-SNAPSHOT.jar DESTINATION /tmp)
