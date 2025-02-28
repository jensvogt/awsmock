set(BINARY awsmockmgr)

set(SOURCES src/server/Manager.cpp src/server/Monitoring.cpp src/main.cpp)

# External includes
include_directories(./include ../core/include ../db/include ../dto/include ../service/include ${INC_BSON_CXX}
        ${INC_MONGO_CXX} ${EXT_INCLUDE})
link_directories(../core ../dto ../db ../service ${EXT_LIB})

add_executable(${BINARY} ${SOURCES})
add_library(${BINARY}_lib STATIC ${SOURCES})
target_link_libraries(${BINARY} PUBLIC awsmocksrv awsmockdto awsmockdb awsmockcore ${LIB_BOOST_PROGRAM_OPTIONS}
        ${LIB_BOOST_THREAD})

install(TARGETS ${BINARY} DESTINATION ${BIN_INSTALL_DIR})
