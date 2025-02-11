set(BINARY awsmockmgr)

set(SOURCES src/server/Manager.cpp src/server/Monitoring.cpp src/main.cpp)

# External includes
include_directories(./include ../core/include ../db/include ../dto/include ../service/include ${INC_BSON_CXX} ${INC_MONGO_CXX} /usr/local/include)
link_directories(../core ../dto ../db ../service /usr/local/lib)

add_executable(${BINARY} ${SOURCES})
add_library(${BINARY}_lib STATIC ${SOURCES})

target_link_libraries(${BINARY} PUBLIC awsmocksrv awsmockdto awsmockdb awsmockcore mongocxx bsoncxx boost_program_options boost_filesystem boost_thread boost_url
        boost_locale yaml-cpp prometheus-cpp-pull prometheus-cpp-core pthread crypto ssl archive z magic)

install(TARGETS ${BINARY} DESTINATION bin)
