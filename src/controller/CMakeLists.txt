set(BINARY awsmockctl)
set(SOURCES src/Controller.cpp src/main.cpp)

include_directories(include ../core/include ../dto/include ../db/include ${BSONCXX_INCLUDE_DIRS} ${MONGOCXX_INCLUDE_DIRS})

if (WIN32)
    add_executable(${BINARY} WIN32 ${SOURCES})
    set_target_properties(${BINARY} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
    target_link_libraries(${BINARY} awsmockcore_static awsmockdto_static awsmockdb_static
            $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>
            $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>
            Boost::filesystem Boost::url Boost::thread Boost::locale Boost::program_options yaml-cpp::yaml-cpp
            OpenSSL::SSL OpenSSL::SSL prometheus-cpp::core prometheus-cpp::pull LibArchive::LibArchive
            unofficial::libmagic::libmagic)
else ()
    add_executable(${BINARY} ${SOURCES})
    # Check systemd
    find_library(SYSTEMD NAMES systemd HINTS /usr/lib/x86_64-linux-gnu)
    if (SYSTEMD)
        # systemd available
        add_definitions(-DHAS_SYSTEMD)
        target_link_libraries(${BINARY} PUBLIC awsmockcore awsmockdto awsmockdb
                $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>
                $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>
                Boost::filesystem Boost::url Boost::thread Boost::locale Boost::program_options yaml-cpp::yaml-cpp
                OpenSSL::SSL OpenSSL::SSL prometheus-cpp::core prometheus-cpp::pull LibArchive::LibArchive
                unofficial::libmagic::libmagic systemd)
        install(TARGETS ${BINARY} DESTINATION ${CMAKE_INSTALL_PREFIX} RUNTIME_DEPENDENCY_SET libAwsMockCtl)
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
        # systemd not available
        target_link_libraries(${BINARY} PUBLIC awsmockcore awsmockdto awsmockdb
                $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>
                $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>
                Boost::filesystem Boost::url Boost::thread Boost::locale Boost::program_options yaml-cpp::yaml-cpp
                OpenSSL::SSL OpenSSL::SSL prometheus-cpp::core prometheus-cpp::pull LibArchive::LibArchive
                unofficial::libmagic::libmagic)
        install(TARGETS ${BINARY} DESTINATION ${CMAKE_INSTALL_PREFIX} RUNTIME_DEPENDENCY_SET libAwsMockCtl)
        install(RUNTIME_DEPENDENCY_SET libAwsMockCtl
                PRE_EXCLUDE_REGEXES
                [=[api-ms-]=]
                [=[ext-ms-]=]
                [[kernel32\.dll]]
                [[libc\.so\..*]] [[libgcc_s\.so\..*]] [[libm\.so\..*]] [[libstdc\+\+\.so\..*]]
                POST_EXCLUDE_REGEXES
                [=[.*(\\|/)system32(\\|/).*\.dll]=]
                [=[^/(lib|usr/lib|usr/local/lib/lib64)]=] DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
    endif ()
endif ()


