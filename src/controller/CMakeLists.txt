set(BINARY awsmockctl)
set(SOURCES src/Controller.cpp src/main.cpp)

include_directories(include ../core/include ../dto/include ../db/include ${INC_BSON_CXX} ${INC_MONGO_CXX} /usr/local/include)
link_directories(../core /usr/local/lib)

add_executable(${BINARY} ${SOURCES})

# Check systemd
find_library(SYSTEMD NAMES systemd HINTS /usr/lib/x86_64-linux-gnu)
if (SYSTEMD)
    # systemd available
    add_definitions(-DHAS_SYSTEMD)
    target_link_libraries(${BINARY} PUBLIC awsmockcore awsmockdto awsmockdb mongocxx bsoncxx boost_program_options boost_thread boost_filesystem boost_url yaml-cpp
            pthread crypto ssl z archive systemd magic)
else ()
    # systemd not available
    target_link_libraries(${BINARY} PUBLIC awsmockcore awsmockdto awsmockdb mongocxx bsoncxx boost_program_options boost_filesystem boost_thread boost_url yaml-cpp
            pthread crypto ssl z archive magic)
endif ()

add_library(${BINARY}_lib STATIC ${SOURCES})

install(TARGETS ${BINARY} DESTINATION bin)
