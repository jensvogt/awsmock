set(BINARY db_tst)
set(STATIC_LIB awsmockdb)

# Includes
include_directories(../include ../../core/include ${BSONCXX_INCLUDE_DIRS} ${MONGOCXX_INCLUDE_DIRS})

add_executable(${BINARY} main.cpp)

target_link_libraries(${BINARY} PUBLIC ${STATIC_LIB} awsmockcore mongocxx bsoncxx boost_filesystem boost_program_options
        boost_url boost_thread boost_locale boost_log boost_log_setup boost_process boost_json boost_unit_test_framework
        ssl crypto archive magic)

add_test(NAME ${BINARY} COMMAND ${BINARY})
