set(BINARY core_tst)
set(STATIC_LIB awsmockcore)

set(SOURCES CryptoTests.cpp FileUtilsTests.cpp DirUtilsTests.cpp StringUtilsTests.cpp ConfigurationTests.cpp RandomUtilsTests.cpp AwsUtilsTests.cpp
        HttpUtilsTests.cpp SystemUtilsTests.cpp XmlUtilsTests.cpp JwtUtilsTests.cpp TarUtilsTests.cpp main.cpp)

add_executable(${BINARY} ${SOURCES})
target_include_directories(${BINARY} PRIVATE ../include /usr/local/include ${INC_BSON_CXX} ${INC_MONGO_CXX})
target_link_libraries(${BINARY} PUBLIC ${STATIC_LIB} ${LIB_BSON_CXX} ${LIB_MONGO_CXX} boost_locale boost_filesystem boost_url yaml-cpp gtest pthread ssl crypto
        archive z prometheus-cpp-core prometheus-cpp-pull magic)

add_test(NAME ${BINARY} COMMAND ${BINARY})

