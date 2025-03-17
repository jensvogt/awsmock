set(BINARY awsmockmgr)

set(SOURCES src/server/Manager.cpp src/server/Monitoring.cpp src/main.cpp)

# External includes
include_directories(./include ../core/include ../db/include ../dto/include ../service/include
        ${BSON_INCLUDE_DIRS} ${BSONCXX_INCLUDE_DIRS} ${MONGOCXX_INCLUDE_DIRS})

if (WIN32)
    add_executable(${BINARY} WIN32 ${SOURCES})
    set_target_properties(${BINARY} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
    target_link_libraries(${BINARY} awsmocksrv_static awsmockdto_static awsmockdb_static awsmockcore_static
            $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>
            $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>
            Boost::locale Boost::filesystem Boost::url yaml-cpp::yaml-cpp OpenSSL::SSL OpenSSL::Crypto
            LibArchive::LibArchive prometheus-cpp::pull prometheus-cpp::core unofficial::libmagic::libmagic)
else ()
    add_executable(${BINARY} ${SOURCES})
    target_link_libraries(${BINARY} PUBLIC awsmocksrv awsmockdto awsmockdb awsmockcore
            $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>
            $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>
            Boost::locale Boost::filesystem Boost::url Boost::program_options yaml-cpp::yaml-cpp OpenSSL::SSL OpenSSL::Crypto
            LibArchive::LibArchive prometheus-cpp::pull prometheus-cpp::core unofficial::libmagic::libmagic)
endif ()

install(TARGETS ${BINARY} DESTINATION ${BIN_INSTALL_DIR})
