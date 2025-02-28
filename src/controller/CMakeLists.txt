set(BINARY awsmockctl)
set(SOURCES src/Controller.cpp src/main.cpp)

include_directories(include ../core/include ../dto/include ../db/include ${INC_BSON_CXX} ${INC_MONGO_CXX} ${EXT_INCLUDE})
link_directories(../core ${EXT_LIB})

add_executable(${BINARY} ${SOURCES})

if (WIN32)
    target_link_libraries(${BINARY} PUBLIC awsmockcore awsmockdto awsmockdb)
else ()
    # Check systemd
    find_library(SYSTEMD NAMES systemd HINTS /usr/lib/x86_64-linux-gnu)
    if (SYSTEMD)
        # systemd available
        add_definitions(-DHAS_SYSTEMD)
        target_link_libraries(${BINARY} PUBLIC awsmockcore awsmockdto awsmockdb boost_program_options systemd)
    else ()
        # systemd not available
        target_link_libraries(${BINARY} PUBLIC awsmockcore awsmockdto awsmockdb boost_program_options)
    endif ()
endif ()
add_library(${BINARY}_lib STATIC ${SOURCES})

install(TARGETS ${BINARY} DESTINATION ${BIN_INSTALL_DIR})

