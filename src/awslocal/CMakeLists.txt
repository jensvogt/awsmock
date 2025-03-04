set(BINARY awslocal)
set(SOURCES src/AwsLocal.cpp src/main.cpp)

include_directories(include ../core/include ${EXT_INCLUDE})
link_directories(../core ${EXT_LIB})

add_executable(${BINARY} ${SOURCES})

if (WIN32)
    set_target_properties(${BINARY} PROPERTIES WIN32_EXECUTABLE ON)
    set_target_properties(${BINARY} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
    target_link_libraries(${BINARY} awsmockcore_static)
else ()
    target_link_libraries(${BINARY} PUBLIC awsmockcore boost_program_options)
endif ()

install(TARGETS ${BINARY} DESTINATION ${BIN_INSTALL_DIR})

