# vcpkg-ports/unofficial-libmagic/portfile.cmake
vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO mcmilk/unofficial-libmagic
        REF master
        SHA512  # optional, can omit if using HEAD
)

vcpkg_configure_cmake(
        SOURCE_PATH ${SOURCE_PATH}
        PREFER_NINJA
        OPTIONS
        "-DBUILD_SHARED_LIBS=ON"
        "-DENABLE_INSTALL=ON"
)

# Patch: Remove strndup definition on Windows
file(GLOB_RECURSE PATCH_FILES "${SOURCE_PATH}/*.c" "${SOURCE_PATH}/*.h")
foreach (f ${PATCH_FILES})
    file(READ ${f} CONTENT)
    string(REPLACE "char *strndup(" "//char *strndup(" CONTENT_MOD)
    file(WRITE ${f} "${CONTENT_MOD}")
endforeach ()

vcpkg_install_cmake()
vcpkg_fixup_cmake_targets(CONFIG_PATH lib/cmake/unofficial-libmagic)
