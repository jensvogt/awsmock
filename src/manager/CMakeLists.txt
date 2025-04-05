set(BINARY awsmockmgr)

set(SOURCES src/server/Manager.cpp src/server/Monitoring.cpp src/main.cpp)

if (WIN32)
    add_executable(${BINARY} WIN32 ${SOURCES})
    set_target_properties(${BINARY} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
    target_include_directories(${BINARY} PUBLIC ./include ../db/include ../dto/include ../service/include
            d:/work/Boost.Application/include ${EXTERNAL_INCLUDE_DIR})
    target_link_directories(${BINARY} PUBLIC ${EXTERNAL_LIB_DIR})
    target_link_libraries(${BINARY} awsmocksrv_static awsmockdto_static awsmockdb_static awsmockcore_static yaml-cpp
            libssl libcrypto archive magic.lib libmagic crypt32.lib prometheus-cpp-pull.lib prometheus-cpp-core.lib
            pdh.lib boost_program_options-vc143-mt-x64-1_87 ssh.lib)
    install(TARGETS ${BINARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
else ()
    add_executable(${BINARY} ${SOURCES})
    target_include_directories(${BINARY} PUBLIC ./include ../dto/include ../db/include ../core/include ../service/include)
    target_link_libraries(${BINARY} PUBLIC awsmocksrv awsmockdto awsmockdb awsmockcore mongocxx bsoncxx boost_filesystem
            boost_url boost_thread boost_locale boost_program_options boost_log boost_log_setup yaml-cpp ssl crypto
            archive magic ssh)
    install(TARGETS ${BINARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif ()

