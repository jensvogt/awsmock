set(BINARY awsmockmgr)

set(SOURCES src/server/Manager.cpp src/server/Monitoring.cpp src/main.cpp)

# External includes
include_directories(./include ../core/include ../db/include ../dto/include ../service/include ${INC_BSON_CXX}
        ${INC_MONGO_CXX} ${EXT_INCLUDE})
link_directories(../core ../dto ../db ../service ${EXT_LIB})

add_library(${BINARY}_lib STATIC ${SOURCES})

if (WIN32)
    add_executable(${BINARY} WIN32 ${SOURCES})
    set_target_properties(${BINARY} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
    target_link_libraries(${BINARY} awsmocksrv_static awsmockdto_static awsmockdb_static awsmockcore_static
            ${LIB_MONGO_CXX} ${LIB_BSON_CXX} ${LIB_BOOST_LOCALE} ${LIB_BOOST_FILESYSTEM} ${LIB_BOOST_URL} ${LIB_YAML}
            ${LIB_GTEST} ${LIB_SSL} ${LIB_CRYPTO} ${LIB_ARCHIVE} ${LIB_PROMETHEUS} ${LIB_MAGIC})
else ()
    add_executable(${BINARY} ${SOURCES})
    target_link_libraries(${BINARY} PUBLIC awsmocksrv awsmockdto awsmockdb awsmockcore ${LIB_BOOST_THREAD}
            ${LIB_BOOST_PROGRAM_OPTIONS})
endif ()

install(TARGETS ${BINARY} DESTINATION ${BIN_INSTALL_DIR})
