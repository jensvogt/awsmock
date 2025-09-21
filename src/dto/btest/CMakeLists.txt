set(BINARY dto_tst)

# Includes
include_directories(../include ../../core/include ../../db/include ${BSONCXX_INCLUDE_DIRS} ${MONGOCXX_INCLUDE_DIRS})

# Libraries
set(LIBRARIES awsmockcore awsmockdto awsmockdb mongocxx bsoncxx boost_locale boost_program_options boost_log
        boost_log_setup boost_process boost_unit_test_framework boost_coroutine boost_context ssl crypto archive magic ssh)

# S3
add_executable(dto_s3 S3DtoTest.cpp TestBase.cpp)
target_link_libraries(dto_s3 PUBLIC ${LIBRARIES})

add_test(NAME dto_s3 COMMAND dto_s3 WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
