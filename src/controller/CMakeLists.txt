set(BINARY awsmockctl)
set(SOURCES src/Controller.cpp src/main.cpp)

if (WIN32)
    add_executable(${BINARY} WIN32 ${SOURCES})
    set_target_properties(${BINARY} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
    target_include_directories(${BINARY} PUBLIC include ../core/include ../dto/include ../db/include
            ${BSONCXX_INCLUDE_DIR} ${MONGOCXX_INCLUDE_DIR} ${EXTERNAL_INCLUDE_DIR})
    target_link_directories(${BINARY} PUBLIC ${EXTERNAL_LIB_DIR})
    target_link_libraries(${BINARY} awsmocksrv_static awsmockdto_static awsmockdb_static awsmockcore_static yaml-cpp
            boost_program_options-vc143-mt-x64-1_87 boost_log-vc143-mt-x64-1_87 libssl libcrypto archive magic.lib
            prometheus-cpp-pull.lib prometheus-cpp-core.lib libmagic crypt32.lib pdh.lib)
    install(TARGETS ${BINARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
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
                boost_url boost_thread boost_locale boost_program_options boost_log boost_log_setup yaml-cpp ssl crypto
                archive magic systemd)
        install(TARGETS ${BINARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    else ()
        # systemd not available
        target_link_libraries(${BINARY} PUBLIC awsmockcore awsmockdto awsmockdb mongocxx bsoncxx boost_filesystem
                boost_url boost_thread boost_locale boost_program_options boost_log boost_log_setup yaml-cpp ssl crypto
                archive magic)
        target_include_directories(${BINARY} PUBLIC include ../core/include ../dto/include ../db/include
                ${BSONCXX_INCLUDE_DIR} ${MONGOCXX_INCLUDE_DIR})
        install(TARGETS ${BINARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    endif ()
endif ()


