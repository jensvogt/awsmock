set(BINARY awsmockmgr)

set(SOURCES src/server/Manager.cpp src/server/Monitoring.cpp src/main.cpp)

if (WIN32)
    add_executable(${BINARY} WIN32 ${SOURCES})
    set_target_properties(${BINARY} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
    target_include_directories(${BINARY} PUBLIC "d:/work/Boost.Application/include" ${Boost_INCLUDE_DIRS})
    target_link_libraries(${BINARY} awsmocksrv_static awsmockdto_static awsmockdb_static awsmockcore_static
            $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>
            $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>
            Boost::locale Boost::filesystem Boost::program_options Boost::url Boost::thread yaml-cpp::yaml-cpp OpenSSL::SSL
            OpenSSL::Crypto LibArchive::LibArchive prometheus-cpp::pull prometheus-cpp::core
            unofficial::libmagic::libmagic)
    install(TARGETS ${BINARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin RUNTIME_DEPENDENCY_SET libAwsMockMgr)
else ()
    add_executable(${BINARY} ${SOURCES})
    target_include_directories(${BINARY} PUBLIC ./include ../dto/include ../db/include ../core/include ../service/include)
    target_link_libraries(${BINARY} PUBLIC awsmocksrv awsmockdto awsmockdb awsmockcore mongocxx bsoncxx boost_filesystem
            boost_url boost_thread boost_locale boost_program_options yaml-cpp ssl crypto archive magic ssh)
    install(TARGETS ${BINARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif ()

