set(BINARY awsmockmgr)

set(SOURCES src/server/Manager.cpp src/server/Monitoring.cpp src/main.cpp)

# External includes
if (WIN32)
    include_directories(./include ../core/include ../db/include ../dto/include ../service/include ${INC_BSON_CXX} ${INC_MONGO_CXX} D:/work/lib/include)
    link_directories(../core ../dto ../db ../service D:/work/lib/lib)
else ()
    include_directories(./include ../core/include ../db/include ../dto/include ../service/include ${INC_BSON_CXX} ${INC_MONGO_CXX} /usr/local/include)
    link_directories(../core ../dto ../db ../service /usr/local/lib)
endif ()

add_executable(${BINARY} ${SOURCES})
add_library(${BINARY}_lib STATIC ${SOURCES})
if (WIN32)
    target_link_libraries(${BINARY} PUBLIC awsmocksrv awsmockdto awsmockdb awsmockcore)
else ()
    target_link_libraries(${BINARY} PUBLIC awsmocksrv awsmockdto awsmockdb awsmockcore ${LIB_BOOST_PROGRAM_OPTIONS})
endif ()

install(TARGETS ${BINARY} DESTINATION bin)
