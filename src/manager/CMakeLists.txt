set(BINARY awsmockmgr)

set(SOURCES src/server/Manager.cpp src/server/Monitoring.cpp src/main.cpp)

# External includes
include_directories(./include ../core/include ../db/include ../dto/include ../service/include
        ${BSON_INCLUDE_DIRS} ${BSONCXX_INCLUDE_DIRS} ${MONGOCXX_INCLUDE_DIRS} "d:/work/Boost.Application/include" ${Boost_INCLUDE_DIRS})

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
    install(RUNTIME_DEPENDENCY_SET libAwsMockCtl
            PRE_EXCLUDE_REGEXES
            [=[api-ms-]=]
            [=[ext-ms-]=]
            [[kernel32\.dll]]
            [[libc\.so\..*]] [[libgcc_s\.so\..*]] [[libm\.so\..*]] [[libstdc\+\+\.so\..*]]
            POST_EXCLUDE_REGEXES
            [=[.*(\\|/)system32(\\|/).*\.dll]=]
            [=[^/(lib|usr/lib|usr/local/lib/lib64)]=])
else ()
    add_executable(${BINARY} ${SOURCES})
    target_link_libraries(${BINARY} PUBLIC awsmocksrv awsmockdto awsmockdb awsmockcore
            $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>
            $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>
            Boost::locale Boost::filesystem Boost::url Boost::program_options yaml-cpp::yaml-cpp OpenSSL::SSL OpenSSL::Crypto
            LibArchive::LibArchive prometheus-cpp::pull prometheus-cpp::core unofficial::libmagic::libmagic)
    install(TARGETS ${BINARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin RUNTIME_DEPENDENCY_SET libAwsMockMgr)
    install(RUNTIME_DEPENDENCY_SET libAwsMockMgr
            PRE_EXCLUDE_REGEXES
            [=[api-ms-]=]
            [=[ext-ms-]=]
            [[kernel32\.dll]] [[libc\.so\..*]]
            [[libgcc_s\.so\..*]]
            [[libm\.so\..*]]
            [[libstdc\+\+\.so\..*]]
            POST_EXCLUDE_REGEXES
            [=[.*(\\|/)system32(\\|/).*\.dll]=]
            [=[^/(lib|usr/lib|usr/local/lib/lib64)]=]
            DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
endif ()


