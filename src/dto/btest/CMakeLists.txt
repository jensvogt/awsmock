set(BINARY dto_tst)

# Libraries
set(LIBRARIES awsmockcore awsmockdto awsmockdb mongocxx bsoncxx boost_program_options boost_log boost_log_setup
        boost_unit_test_framework ssl crypto archive magic ssh)

# S3
add_executable(${BINARY} S3DtoTest.cpp TestBase.cpp)
target_include_directories(${BINARY} PRIVATE ../include ../../core/include ../../db/include ${BSONCXX_INCLUDE_DIR}
        ${MONGOCXX_INCLUDE_DIR} ${EXTERNAL_INCLUDE_DIR})
target_link_directories(${BINARY} PRIVATE ${BSONCXX_LIB_DIR} ${MONGOCXX_LIB_DIR} ${EXTERNAL_LIB_DIR})
target_link_libraries(${BINARY} PUBLIC ${LIBRARIES})

add_test(NAME ${BINARY} COMMAND dto_s3 WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
