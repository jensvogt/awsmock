set(BINARY awsmockctl)
set(SOURCES src/Controller.cpp src/main.cpp)

if (WIN32)
    include_directories(include ../core/include ../dto/include ../db/include ${INC_BSON_CXX} ${INC_MONGO_CXX} D:/work/lib/include)
    link_directories(../core d:/work/lib/lib)
else ()
    include_directories(include ../core/include ../dto/include ../db/include ${INC_BSON_CXX} ${INC_MONGO_CXX} /usr/local/include)
    link_directories(../core /usr/local/lib)
endif ()

add_executable(${BINARY} ${SOURCES})
if (APPLE)
    set_target_properties(${BINARY} PROPERTIES INSTALL_RPATH "/usr/local/lib")
endif ()

# Check systemd
if (WIN32)
    target_link_libraries(${BINARY} PUBLIC awsmockcore awsmockdto awsmockdb)
else ()
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
install(TARGETS ${BINARY} DESTINATION /usr/local/awsmock/bin)
