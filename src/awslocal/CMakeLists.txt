set(BINARY awslocal)
set(SOURCES src/AwsLocal.cpp src/main.cpp)

add_executable(${BINARY} ${SOURCES})

if (WIN32)
    set_target_properties(${BINARY} PROPERTIES WIN32_EXECUTABLE ON)
    set_target_properties(${BINARY} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
    target_include_directories(${BINARY} PUBLIC ./include ${BSONCXX_INCLUDE_DIR} ${MONGOCXX_INCLUDE_DIR})
    target_link_libraries(${BINARY} awsmockcore_static Boost::filesystem Boost::url Boost::thread Boost::locale
            Boost::program_options)
    install(TARGETS ${BINARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin RUNTIME_DEPENDENCY_SET libAwsMockLocal)
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
    target_link_libraries(${BINARY} PUBLIC awsmockcore mongocxx bsoncxx boost_filesystem boost_url boost_thread
            boost_locale boost_program_options yaml-cpp ssl crypto archive magic)
    target_include_directories(${BINARY} PUBLIC ./include ${BSONCXX_INCLUDE_DIR} ${MONGOCXX_INCLUDE_DIR})
    install(TARGETS ${BINARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin RUNTIME_DEPENDENCY_SET libAwsMockLocal)
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

