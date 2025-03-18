set(BINARY awslocal)
set(SOURCES src/AwsLocal.cpp src/main.cpp)

include_directories(include ../core/include ${BSONCXX_INCLUDE_DIRS} ${MONGOCXX_INCLUDE_DIRS})

add_executable(${BINARY} ${SOURCES})

if (WIN32)
    set_target_properties(${BINARY} PROPERTIES WIN32_EXECUTABLE ON)
    set_target_properties(${BINARY} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
    target_link_libraries(${BINARY} awsmockcore_static ${LIB_BOOST_PROCESS})
    install(TARGETS ${BINARY} DESTINATION ${CMAKE_INSTALL_PREFIX} RUNTIME_DEPENDENCY_SET libAwsMockLocal)
    install(RUNTIME_DEPENDENCY_SET libAwsMockLocal
            PRE_EXCLUDE_REGEXES
            [=[api-ms-]=]
            [=[ext-ms-]=]
            [[kernel32\.dll]]
            [[libc\.so\..*]] [[libgcc_s\.so\..*]] [[libm\.so\..*]] [[libstdc\+\+\.so\..*]]
            POST_EXCLUDE_REGEXES
            [=[.*(\\|/)system32(\\|/).*\.dll]=]
            [=[^/(lib|usr/lib|usr/local/lib/lib64)]=])
else ()
    target_link_libraries(${BINARY} PUBLIC awsmockcore
            $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>
            $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>
            Boost::filesystem Boost::url Boost::thread Boost::locale Boost::program_options yaml-cpp::yaml-cpp
            OpenSSL::SSL OpenSSL::SSL prometheus-cpp::core prometheus-cpp::pull LibArchive::LibArchive
            unofficial::libmagic::libmagic)
    install(TARGETS ${BINARY} DESTINATION ${CMAKE_INSTALL_PREFIX} RUNTIME_DEPENDENCY_SET libAwsMockLocal)
    install(RUNTIME_DEPENDENCY_SET libAwsMockLocal
            PRE_EXCLUDE_REGEXES
            [=[api-ms-]=]
            [=[ext-ms-]=]
            [[kernel32\.dll]]
            [[libc\.so\..*]] [[libgcc_s\.so\..*]] [[libm\.so\..*]] [[libstdc\+\+\.so\..*]]
            POST_EXCLUDE_REGEXES
            [=[.*(\\|/)system32(\\|/).*\.dll]=]
            [=[^/(lib|usr/lib|usr/local/lib/lib64)]=] DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
endif ()

