set(BINARY awsmockctl)
set(SOURCES src/Controller.cpp src/main.cpp)

if (WIN32)
    add_executable(${BINARY} WIN32 ${SOURCES})
    set_target_properties(${BINARY} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
    target_include_directories(${BINARY} PUBLIC include ../core/include ../dto/include ../db/include
            ${BSONCXX_INCLUDE_DIR} ${MONGOCXX_INCLUDE_DIR} ${EXTERNAL_INCLUDE_DIR})
    target_link_directories(${BINARY} PUBLIC ${EXTERNAL_LIB_DIR})
    target_link_libraries(${BINARY} awsmocksrv_static awsmockdto_static awsmockdb_static awsmockcore_static awsmockcore_static
            bsoncxx-v_noabi-rhs-x64-v143-md.lib mongocxx-v_noabi-rhs-x64-v143-md.lib bson-1.0.lib mongoc-1.0.lib
            libssl.lib libcrypto.lib archive_static.lib libmagic.lib prometheus-cpp-pull.lib prometheus-cpp-core.lib
            libboost_filesystem-vc143-mt-s-x64-1_88.lib libboost_url-vc143-mt-s-x64-1_88.lib
            libboost_thread-vc143-mt-s-x64-1_88.lib libboost_locale-vc143-mt-s-x64-1_88.lib
            libboost_program_options-vc143-mt-s-x64-1_88.lib libboost_log-vc143-mt-s-x64-1_88.lib
            libboost_log_setup-vc143-mt-s-x64-1_88.lib libboost_process-vc143-mt-s-x64-1_88.lib ssh.lib crypt32.lib
            Iphlpapi.lib pdh.lib ntdll.lib shlwapi.lib)
else ()
    add_executable(${BINARY} ${SOURCES})
    # Check systemd
    find_library(SYSTEMD NAMES systemd HINTS /usr/lib/x86_64-linux-gnu)
    if (SYSTEMD)
        # systemd available
        add_definitions(-DHAS_SYSTEMD)
        target_include_directories(${BINARY} PUBLIC include ../core/include ../dto/include ../db/include
                ${BSONCXX_INCLUDE_DIR} ${MONGOCXX_INCLUDE_DIR})
        target_link_libraries(${BINARY} PUBLIC awsmockcore awsmockdto awsmockdb mongocxx bsoncxx boost_filesystem
                boost_url boost_thread boost_locale boost_program_options boost_log boost_log_setup boost_process
                ssl crypto archive magic systemd)
    else ()
        # systemd not available
        target_link_libraries(${BINARY} PUBLIC awsmockcore awsmockdto awsmockdb mongocxx bsoncxx boost_filesystem
                boost_url boost_thread boost_locale boost_program_options boost_log boost_log_setup boost_process
                ssl crypto archive magic)
        target_include_directories(${BINARY} PUBLIC include ../core/include ../dto/include ../db/include
                ${BSONCXX_INCLUDE_DIR} ${MONGOCXX_INCLUDE_DIR})
    endif ()
    if (APPLE)
        add_link_options("-rpath /usr/local/awsmock/lib")
        set_target_properties(${BINARY} PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${CMAKE_INSTALL_PREFIX}/lib;${EXTERNAL_LIB_DIR}")
    endif ()
endif ()

install(TARGETS ${BINARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

