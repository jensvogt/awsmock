set(BINARY core_tst)
set(STATIC_LIB awsmockcore)

set(SOURCES CryptoTests.cpp FileUtilsTests.cpp DirUtilsTests.cpp StringUtilsTests.cpp ConfigurationTests.cpp RandomUtilsTests.cpp AwsUtilsTests.cpp
        HttpUtilsTests.cpp SystemUtilsTests.cpp XmlUtilsTests.cpp JwtUtilsTests.cpp TarUtilsTests.cpp DateTimeUtilsTests.cpp main.cpp)

add_executable(${BINARY} ${SOURCES})
if (WIN32)
    include_directories(../include ${INC_BSON_CXX} ${INC_MONGO_CXX} D:/work/lib/include)
    link_directories(../core D:/work/lib/lib)
    target_link_libraries(${BINARY} ${LIB_BSON_CXX} ${LIB_MONGO_CXX} ${LIB_BOOST_LOCALE} ${LIB_BOOST_FILESYSTEM} ${LIB_BOOST_URL} yaml-cpp gtest ${LIB_SSL} ${LIB_CRYPTO}
            ${LIB_ARCHIVE} prometheus-cpp-core prometheus-cpp-pull)
else ()
    include_directories(${BINARY} PRIVATE ../include /usr/local/include ${INC_BSON_CXX} ${INC_MONGO_CXX})
    link_directories(${BINARY} PRIVATE /usr/local/lib)
    target_link_libraries(${BINARY} PUBLIC ${STATIC_LIB} ${LIB_BSON_CXX} ${LIB_MONGO_CXX} ${LIB_BOOST_LOCALE} ${LIB_BOOST_FILESYSTEM} ${LIB_BOOST_URL} yaml-cpp gtest pthread ssl crypto
            ${LIB_ARCHIVE} prometheus-cpp-core prometheus-cpp-pull magic)
endif ()

add_test(NAME ${BINARY} COMMAND ${BINARY} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

