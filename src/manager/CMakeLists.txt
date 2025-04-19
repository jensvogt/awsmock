set(BINARY awsmockmgr)

set(SOURCES src/server/Manager.cpp src/server/Monitoring.cpp src/main.cpp)

if (WIN32)
    add_executable(${BINARY} WIN32 ${SOURCES})
    set_target_properties(${BINARY} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
    target_include_directories(${BINARY} PUBLIC ./include ../db/include ../dto/include ../service/include
            d:/work/Boost.Application/include ${EXTERNAL_INCLUDE_DIR})
    target_link_directories(${BINARY} PUBLIC ${EXTERNAL_LIB_DIR})
    target_link_libraries(${BINARY} awsmocksrv_static awsmockdto_static awsmockdb_static awsmockcore_static yaml-cpp.lib
            libssl.lib libcrypto.lib archive_static.lib libmagic.lib prometheus-cpp-pull.lib prometheus-cpp-core.lib
            "$<$<CONFIG:RELEASE>:libboost_filesystem-vc143-mt-s-x64-1_88.lib>"
            "$<$<CONFIG:DEBUG>:libboost_filesystem-vc143-mt-sgd-x64-1_88.lib>"
            "$<$<CONFIG:RELEASE>:libboost_url-vc143-mt-s-x64-1_88.lib>"
            "$<$<CONFIG:DEBUG>:libboost_url-vc143-mt-sgd-x64-1_88.lib>"
            "$<$<CONFIG:RELEASE>:libboost_thread-vc143-mt-s-x64-1_88.lib>"
            "$<$<CONFIG:DEBUG>:libboost_thread-vc143-mt-sgd-x64-1_88.lib>"
            "$<$<CONFIG:RELEASE>:libboost_locale-vc143-mt-s-x64-1_88.lib>"
            "$<$<CONFIG:DEBUG>:libboost_locale-vc143-mt-sgd-x64-1_88.lib>"
            "$<$<CONFIG:RELEASE>:libboost_program_options-vc143-mt-s-x64-1_88.lib>"
            "$<$<CONFIG:DEBUG>:libboost_program_options-vc143-mt-sgd-x64-1_88.lib>"
            "$<$<CONFIG:RELEASE>:libboost_log-vc143-mt-s-x64-1_88.lib>"
            "$<$<CONFIG:DEBUG>:libboost_log-vc143-mt-sgd-x64-1_88.lib>"
            "$<$<CONFIG:RELEASE>:libboost_log_setup-vc143-mt-s-x64-1_88.lib>"
            "$<$<CONFIG:DEBUG>:libboost_log_setup-vc143-mt-sgd-x64-1_88.lib>"
            "$<$<CONFIG:RELEASE>:libboost_process-vc143-mt-s-x64-1_88.lib>"
            "$<$<CONFIG:DEBUG>:libboost_process-vc143-mt-sgd-x64-1_88.lib>"
            ssh.lib crypt32.lib Iphlpapi.lib pdh.lib ntdll.lib shlwapi.lib)
    install(TARGETS ${BINARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
else ()
    add_executable(${BINARY} ${SOURCES})
    target_include_directories(${BINARY} PUBLIC ./include ../dto/include ../db/include ../core/include ../service/include)
    target_link_libraries(${BINARY} PUBLIC awsmocksrv awsmockdto awsmockdb awsmockcore mongocxx bsoncxx boost_filesystem
            boost_url boost_thread boost_locale boost_program_options boost_log boost_log_setup boost_process
            yaml-cpp ssl crypto archive magic ssh)
    if (APPLE)
        add_link_options("-rpath /usr/local/lib")
        set_target_properties(${BINARY} PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${CMAKE_INSTALL_PREFIX}/lib;${EXTERNAL_LIB_DIR}")
    endif ()
    install(TARGETS ${BINARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif ()

