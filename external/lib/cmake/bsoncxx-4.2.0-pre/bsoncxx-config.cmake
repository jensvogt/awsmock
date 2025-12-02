include(CMakeFindDependencyMacro)
find_dependency(bson 2.1.2)
include("${CMAKE_CURRENT_LIST_DIR}/bsoncxx_targets.cmake")
