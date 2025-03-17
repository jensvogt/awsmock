set(BINARY awslocal)
set(SOURCES src/AwsLocal.cpp src/main.cpp)

include_directories(include ../core/include ${EXT_INCLUDE})
link_directories(../core ${EXT_LIB})

add_executable(${BINARY} ${SOURCES})

if (WIN32)
    set_target_properties(${BINARY} PROPERTIES WIN32_EXECUTABLE ON)
    set_target_properties(${BINARY} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
    target_link_libraries(${BINARY} awsmockcore_static ${LIB_BOOST_PROCESS})
else ()
    target_link_libraries(${BINARY} PUBLIC awsmockcore ${LIB_BOOST_PROCESS} ${LIB_BOOST_THREAD}
            ${LIB_BOOST_PROGRAM_OPTIONS})
endif ()

install(TARGETS ${BINARY} DESTINATION ${BIN_INSTALL_DIR})

