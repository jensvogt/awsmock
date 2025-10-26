set(BINARY awslocal)
set(SOURCES src/AwsLocal.cpp src/main.cpp)

if (WIN32)
    add_executable(${BINARY} WIN32 ${SOURCES})
    set_target_properties(${BINARY} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
    target_include_directories(${BINARY} PUBLIC ./include ../core/include ../dto/include ${BSONCXX_INCLUDE_DIR}
            ${MONGOCXX_INCLUDE_DIR} ${BOOST_INCLUDE_DIR} ${LIBMAGIC_INCLUDE_DIR} ${LIBARCHIVE_INCLUDE_DIR}
            ${LIBJWT_INCLUDE_DIR} ${LIBSSL_INCLUDE_DIR} ${LIBPROMETHEUS_INCLUDE_DIR} ${LIBSSH_INCLUDE_DIR}
            ${LIBSSL_LIB_DIR})
    target_link_directories(${BINARY} PUBLIC ${MONGOCXX_LIB_DIR} ${BOOST_LIB_DIR} ${LIBSSH_LIB_DIR} ${LIBSSL_LIB_DIR}
            ${LIBARCHIVE_LIB_DIR} ${LIBMAGIC_LIB_DIR} ${LIBPROMETHEUS_LIB_DIR})
    target_link_libraries(${BINARY} awsmocksrv_static awsmockdto_static awsmockdb_static awsmockcore_static
            libssl.lib libcrypto.lib archive.lib magic.lib prometheus-cpp-pull.lib prometheus-cpp-core.lib
            bsoncxx-v_noabi-rhi-x64-v143-md.lib mongocxx-v_noabi-rhi-x64-v143-md.lib bson2.lib mongoc2.lib
            "$<$<CONFIG:RELEASE>:libboost_filesystem-vc143-mt-x64-1_89.lib>"
            "$<$<CONFIG:DEBUG>:libboost_filesystem-vc143-mt-gd-x64-1_89.lib>"
            "$<$<CONFIG:RELEASE>:libboost_url-vc143-mt-x64-1_89.lib>"
            "$<$<CONFIG:DEBUG>:libboost_url-vc143-mt-gd-x64-1_89.lib>"
            "$<$<CONFIG:RELEASE>:libboost_thread-vc143-mt-x64-1_89.lib>"
            "$<$<CONFIG:DEBUG>:libboost_thread-vc143-mt-gd-x64-1_89.lib>"
            "$<$<CONFIG:RELEASE>:libboost_locale-vc143-mt-x64-1_89.lib>"
            "$<$<CONFIG:DEBUG>:libboost_locale-vc143-mt-gd-x64-1_89.lib>"
            "$<$<CONFIG:RELEASE>:libboost_program_options-vc143-mt-x64-1_89.lib>"
            "$<$<CONFIG:DEBUG>:libboost_program_options-vc143-mt-gd-x64-1_89.lib>"
            "$<$<CONFIG:RELEASE>:libboost_log-vc143-mt-x64-1_89.lib>"
            "$<$<CONFIG:DEBUG>:libboost_log-vc143-mt-gd-x64-1_89.lib>"
            "$<$<CONFIG:RELEASE>:libboost_log_setup-vc143-mt-x64-1_89.lib>"
            "$<$<CONFIG:DEBUG>:libboost_log_setup-vc143-mt-gd-x64-1_89.lib>"
            "$<$<CONFIG:RELEASE>:libboost_process-vc143-mt-x64-1_89.lib>"
            "$<$<CONFIG:DEBUG>:libboost_process-vc143-mt-gd-x64-1_89.lib>"
            "$<$<CONFIG:RELEASE>:libboost_json-vc143-mt-x64-1_89.lib>"
            "$<$<CONFIG:DEBUG>:libboost_json-vc143-mt-gd-x64-1_89.lib>"
            "$<$<CONFIG:RELEASE>:libboost_coroutine-vc143-mt-x64-1_89.lib>"
            "$<$<CONFIG:DEBUG>:libboost_coroutine-vc143-mt-gd-x64-1_89.lib>"
            "$<$<CONFIG:RELEASE>:libboost_context-vc143-mt-x64-1_89.lib>"
            "$<$<CONFIG:DEBUG>:libboost_context-vc143-mt-gd-x64-1_89.lib>"
            ssh.lib crypt32.lib Iphlpapi.lib pdh.lib ntdll.lib shlwapi.lib)
else ()
    add_executable(${BINARY} ${SOURCES})
    target_include_directories(${BINARY} PUBLIC ./include ${EXTERNAL_INCLDUE_DIR})
    target_link_libraries(${BINARY} PUBLIC awsmockcore boost_program_options boost_process)
    if (APPLE)
        add_link_options("-rpath /usr/local/awsmock/lib")
        set_target_properties(${BINARY} PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${CMAKE_INSTALL_PREFIX}/lib;${EXTERNAL_LIB_DIR}")
    endif ()
endif ()

install(TARGETS ${BINARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

