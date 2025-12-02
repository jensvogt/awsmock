include(CMakeFindDependencyMacro)
find_dependency(mongoc 2.1.2)
find_dependency(bsoncxx 4.2.0)
include("${CMAKE_CURRENT_LIST_DIR}/mongocxx_targets.cmake")
