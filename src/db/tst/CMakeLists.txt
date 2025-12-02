set(BINARY db_tst)
set(STATIC_LIB awsmockdb)

add_executable(${BINARY} main.cpp)
target_include_directories(${BINARY} PRIVATE ../include ../../core/include ${BSONCXX_INCLUDE_DIR}
        ${MONGOCXX_INCLUDE_DIR} ${EXTERNAL_INCLUDE_DIR})
target_link_directories(${BINARY} PRIVATE ${BSONCXX_LIB_DIR} ${MONGOCXX_LIB_DIR} ${EXTERNAL_LIB_DIR})
target_link_libraries(${BINARY} PUBLIC ${STATIC_LIB} awsmockcore mongocxx bsoncxx boost_filesystem boost_program_options
        boost_process boost_locale boost_log boost_log_setup boost_json boost_unit_test_framework ssl crypto archive magic)

add_test(NAME ${BINARY} COMMAND ${BINARY})
