# custom-triplets/x64-linux-release.cmake
set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CMAKE_SYSTEM_NAME Linux)

# This is the "Magic" line that skips the debug build
set(VCPKG_BUILD_TYPE release)