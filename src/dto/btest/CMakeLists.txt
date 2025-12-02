set(BINARY dto_tst)

# Libraries
set(LIBRARIES)

# S3
add_executable(${BINARY} S3DtoTest.cpp TestBase.cpp)
target_include_directories(${BINARY} PRIVATE ../include ../../core/include ../../db/include ${BSONCXX_INCLUDE_DIR}
        ${MONGOCXX_INCLUDE_DIR} ${EXTERNAL_INCLUDE_DIR})
target_link_directories(${BINARY} PRIVATE ${EXTERNAL_LIB_DIR})
target_link_libraries(${BINARY} PUBLIC awsmockcore awsmockdto mongocxx bsoncxx boost_process boost_program_options
        boost_log boost_log_setup boost_unit_test_framework ssl crypto archive magic ssh)

add_test(NAME ${BINARY} COMMAND dto_s3 WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
