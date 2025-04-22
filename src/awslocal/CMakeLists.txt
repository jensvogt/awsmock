set(BINARY awslocal)
set(SOURCES src/AwsLocal.cpp src/main.cpp)

if (WIN32)
    add_executable(${BINARY} WIN32 ${SOURCES})
    set_target_properties(${BINARY} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
    target_include_directories(${BINARY} PUBLIC ./include ${EXTERNAL_INCLDUE_DIR})
    target_link_directories(${BINARY} PUBLIC ${EXTERNAL_LIB_DIR})
    target_link_libraries(${BINARY} awsmocksrv_static awsmockdto_static awsmockdb_static awsmockcore_static awsmockcore_static
            libboost_filesystem-vc143-mt-s-x64-1_88.lib libboost_url-vc143-mt-s-x64-1_88.lib
            libboost_thread-vc143-mt-s-x64-1_88.lib libboost_locale-vc143-mt-s-x64-1_88.lib
            libboost_program_options-vc143-mt-s-x64-1_88.lib libboost_log-vc143-mt-s-x64-1_88.lib
            libboost_log_setup-vc143-mt-s-x64-1_88.lib libboost_process-vc143-mt-s-x64-1_88.lib
            ssh.lib crypt32.lib yaml-cpp.lib libssl.lib libcrypto.lib archive_static.lib libmagic.lib
            prometheus-cpp-pull.lib prometheus-cpp-core.lib Iphlpapi.lib pdh.lib ntdll.lib shlwapi.lib)
else ()
    add_executable(${BINARY} ${SOURCES})
    target_include_directories(${BINARY} PUBLIC ./include ${EXTERNAL_INCLDUE_DIR})
    target_link_libraries(${BINARY} PUBLIC awsmockcore boost_program_options boost_process)
endif ()

install(TARGETS ${BINARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

