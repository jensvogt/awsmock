set(BINARY core_tst)
set(STATIC_LIB awsmockcore)

set(SOURCES CryptoTests.cpp FileUtilsTests.cpp DirUtilsTests.cpp StringUtilsTests.cpp ConfigurationTests.cpp RandomUtilsTests.cpp AwsUtilsTests.cpp
        HttpUtilsTests.cpp SystemUtilsTests.cpp XmlUtilsTests.cpp JwtUtilsTests.cpp TarUtilsTests.cpp DateTimeUtilsTests.cpp main.cpp)

add_executable(${BINARY} ${SOURCES})
include_directories(../include ${INC_BSON_CXX} ${INC_MONGO_CXX} ${EXT_INCLUDE})
link_directories(${EXT_LIB})
#target_link_libraries(${BINARY} ${LIB_BSON_CXX} ${LIB_MONGO_CXX} ${LIB_BOOST_LOCALE} ${LIB_BOOST_FILESYSTEM} ${LIB_BOOST_URL} ${LIB_YAML} ${LIB_GTEST} ${LIB_SSL} ${LIB_CRYPTO} ${LIB_ARCHIVE} ${LIB_PROMETHEUS}${LIB_ARCHIVE} ${LIB_MAGIC})

add_test(NAME ${BINARY} COMMAND ${BINARY} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

