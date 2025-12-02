set(BINARY awslocal)
set(SOURCES src/AwsLocal.cpp src/main.cpp)

if (WIN32)
    add_executable(${BINARY} WIN32 ${SOURCES})
    set_target_properties(${BINARY} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
    target_include_directories(${BINARY} PUBLIC ./include ../core/include ${BOOST_INCLUDE_DIR})
    target_link_directories(${BINARY} PUBLIC ${BOOST_LIB_DIR})
    target_link_libraries(${BINARY} awsmockcore_static
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
            "$<$<CONFIG:RELEASE>:libboost_json-vc143-mt-x64-1_89.lib>"
            "$<$<CONFIG:DEBUG>:libboost_json-vc143-mt-gd-x64-1_89.lib>"
            "$<$<CONFIG:RELEASE>:libboost_coroutine-vc143-mt-x64-1_89.lib>"
            "$<$<CONFIG:DEBUG>:libboost_coroutine-vc143-mt-gd-x64-1_89.lib>"
            "$<$<CONFIG:RELEASE>:libboost_context-vc143-mt-x64-1_89.lib>"
            "$<$<CONFIG:DEBUG>:libboost_context-vc143-mt-gd-x64-1_89.lib>"
            ssh.lib crypt32.lib Iphlpapi.lib pdh.lib ntdll.lib shlwapi.lib)
else ()
    add_executable(${BINARY} ${SOURCES})
    target_include_directories(${BINARY} PRIVATE ./include ../core/include ${EXTERNAL_INCLDUE_DIR})
    target_link_libraries(${BINARY} PRIVATE awsmockcore boost_process boost_program_options boost_log boost_log_setup)
    if (APPLE)
        add_link_options("-rpath /usr/local/awsmock/lib")
        set_target_properties(${BINARY} PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${CMAKE_INSTALL_PREFIX}/lib;${EXTERNAL_LIB_DIR}")
    endif ()
endif ()

install(TARGETS ${BINARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
