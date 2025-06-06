set(BINARY core_tst)
set(STATIC_LIB awsmockcore)
set(CMAKE_BUILD_TYPE "Debug")

set(SOURCES Main.cpp)

add_executable(${BINARY} ${SOURCES})
target_include_directories(${BINARY} PUBLIC ../include ${BSONCXX_INCLUDE_DIR} ${MONGOCXX_INCLUDE_DIR})
target_link_directories(${BINARY} PUBLIC ${EXTERNAL_LIB_DIR})
target_link_libraries(${BINARY} awsmockcore boost_filesystem boost_url boost_thread boost_locale boost_program_options
        boost_log boost_log_setup boost_process boost_unit_test_framework ssl crypto archive magic)
target_compile_options(${BINARY} PRIVATE -coverage)
target_link_options(${BINARY} PRIVATE -coverage)

add_test(NAME ${BINARY} COMMAND ${BINARY} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/images/7337529778404.tif DESTINATION /tmp)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/images/9783911244381.jpg DESTINATION /tmp)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/pdf/dummy.pdf DESTINATION /tmp)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/lambda/java-basic-1.0-SNAPSHOT.jar DESTINATION /tmp)
