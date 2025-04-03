set(BINARY awslocal)
set(SOURCES src/AwsLocal.cpp src/main.cpp)


if (WIN32)
    add_executable(${BINARY} WIN32 ${SOURCES})
    set_target_properties(${BINARY} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
    target_include_directories(${BINARY} PUBLIC ./include ${EXTERNAL_INCLDUE_DIR})
    target_link_directories(${BINARY} PUBLIC ${EXTERNAL_LIB_DIR})
    target_link_libraries(${BINARY} awsmocksrv_static awsmockdto_static awsmockdb_static awsmockcore_static
            yaml-cpp libssl libcrypto archive magic.lib prometheus-cpp-pull.lib
            prometheus-cpp-core.lib libmagic crypt32.lib pdh.lib)
else ()
    add_executable(${BINARY} ${SOURCES})
    target_include_directories(${BINARY} PUBLIC ./include ${EXTERNAL_INCLDUE_DIR})
    target_link_libraries(${BINARY} PUBLIC awsmockcore boost_program_options boost_process)
endif ()

install(TARGETS ${BINARY} DESTINATION ${BIN_INSTALL_DIR})

