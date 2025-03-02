set(BINARY awsmockmgr)

set(SOURCES src/server/Manager.cpp src/server/Monitoring.cpp src/main.cpp)

# External includes
include_directories(./include ../core/include ../db/include ../dto/include ../service/include ${INC_BSON_CXX}
        ${INC_MONGO_CXX} ${EXT_INCLUDE})
link_directories(../core ../dto ../db ../service ${EXT_LIB})

add_executable(${BINARY} ${SOURCES})
add_library(${BINARY}_lib STATIC ${SOURCES})
if (WIN32)
    target_link_libraries(${BINARY} PUBLIC awsmocksrv_static awsmockdto_static awsmockdb_static awsmockcore_static)
else ()
    target_link_libraries(${BINARY} PUBLIC awsmocksrv awsmockdto awsmockdb awsmockcore)
endif ()

install(TARGETS ${BINARY} DESTINATION ${BIN_INSTALL_DIR})
