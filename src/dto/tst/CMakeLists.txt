set(BINARY dto_tst)
set(STATIC_LIB awsmockdto)

set(SQS_SOURCES sqs/MessageAttributeTest.cpp)
set(TRANSFER_SOURCES transfer/CreateServerRequestTest.cpp transfer/CreateServerResponseTest.cpp transfer/CreateUserRequestTest.cpp transfer/CreateServerResponseTest.cpp)
set(MAIN_SOURCES main.cpp)

# Includes
include_directories(../include ../../core/include ../../db/include ${INC_BSON_CXX} ${INC_MONGO_CXX} ${EXT_INCLUDE})
link_directories(../../core ../../db ../../dto ${EXT_LIB})

add_executable(${BINARY} ${SQS_SOURCES} ${TRANSFER_SOURCES} ${MAIN_SOURCES})
target_link_libraries(${BINARY} PUBLIC ${STATIC_LIB} awsmockcore awsmockdto awsmockdb ${LIB_BOOST_THREAD})

add_test(NAME ${BINARY} COMMAND ${BINARY} WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
