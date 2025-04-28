set(BINARY dto_tst)
set(STATIC_LIB awsmockdto)

set(S3_SOURCES s3/S3DtoTest.cpp)
set(SQS_SOURCES sqs/MessageAttributeTest.cpp)
set(TRANSFER_SOURCES transfer/CreateServerRequestTest.cpp transfer/CreateServerResponseTest.cpp transfer/CreateUserRequestTest.cpp transfer/CreateServerResponseTest.cpp)
set(MAIN_SOURCES main.cpp)

# Includes
include_directories(../include ../../core/include ../../db/include ${BSONCXX_INCLUDE_DIRS} ${MONGOCXX_INCLUDE_DIRS})

add_executable(${BINARY} ${S3_SOURCES} ${SQS_SOURCES} ${TRANSFER_SOURCES} ${MAIN_SOURCES})
target_link_libraries(${BINARY} PUBLIC ${STATIC_LIB} awsmockcore awsmockdto awsmockdb mongocxx bsoncxx boost_filesystem
        boost_url boost_thread boost_locale boost_program_options boost_log boost_log_setup boost_process ssl crypto
        archive magic gtest gmock)

add_test(NAME ${BINARY} COMMAND ${BINARY} WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
