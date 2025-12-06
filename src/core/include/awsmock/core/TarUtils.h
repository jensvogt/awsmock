//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_CORE_TAR_UTILS_H
#define AWSMOCK_CORE_TAR_UTILS_H

#undef BOOST_FILESYSTEM_DYN_LINK

// C++ standard includes
#include <fcntl.h>
#include <string>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <io.h>
#define BOOST_ASIO_NO_WIN32_LEAN_AND_MEAN
#include <boost/asio.hpp>
#include <windows.h>
#endif

// Boost includes
#include <boost/filesystem/directory.hpp>

// Archive includes
#include <archive.h>
#include <archive_entry.h>

// AwsMock includes
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/StringUtils.h>

#ifdef _WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <filesystem>
constexpr int TAR_BLOCKSIZE = 512;
constexpr int TAR_NAME_MAX = 100;
constexpr int USTAR_MAGIC_LEN = 6;
constexpr int USTAR_VERSION_LEN = 2;

// --- Standard USTAR TAR Header (512 bytes) ---
// Note: Fields must be character arrays to match the fixed 512-byte layout.
struct TarHeader {
    char name[100];     // File name
    char mode[8];       // File mode (octal)
    char uid[8];        // Owner's user ID (octal)
    char gid[8];        // Owner's group ID (octal)
    char size[12];      // File size in bytes (octal)
    char mtime[12];     // Last modification time (octal)
    char chksum[8];     // Checksum of header (octal)
    char typeflag;      // Type of file ('0' for file, '5' for directory, 'x' for PAX extended header)
    char linkname[100]; // Target link name
    char magic[USTAR_MAGIC_LEN];  // USTAR magic string "ustar"
    char version[USTAR_VERSION_LEN]; // Version "00"
    char uname[32];     // Owner user name
    char gname[32];     // Owner group name
    char devmajor[8];   // Device major number
    char devminor[8];   // Device minor number
    char prefix[155];   // Prefix for long filenames (up to 256 chars total path)
    char padding[12];   // Padding to 512 bytes
};
#endif

namespace AwsMock::Core {

    /**
     * @brief TAR compression utilities
     *
     * @par
     * If you have problems using long path names (Java sometimes has long path names) on Windows, set the following registry key using powershell:
     * New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem" -Name "LongPathsEnabled" -Value 1 -PropertyType DWORD -Force
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class TarUtils {

      public:

        /**
         * @brief Constructor
         */
        TarUtils() = default;

        /**
         * @brief Archive a whole directory tree to a tar file.
         *
         * @param tarFile name of the tar file
         * @param directory directory name
         */
        static void TarDirectory(const std::string &tarFile, const std::string &directory);

      private:

#ifndef _WIN32

        /**
         * @brief Writes archive single file to the Tar archive.
         *
         * @param archive tar archive.
         * @param fileName name of the file to write
         * @param directory remove name of the directory from filename
         * @param isDir directory flag
         * @param isLink link flag, needed to preserve links
         */
        static void WriteFile(archive *archive, std::string &fileName, const std::string &directory, bool isDir, bool isLink);

        /**
         * @brief Read a symbolic link.
         *
         * @param path path to link.
         * @return target path.
         */
        static std::string Readsymlink(const std::string &path);

        /**
         * @brief Copy data from A to B
         *
         * @param ar source file name
         * @param aw destination file name
         * @return success of -1
         */
        static int CopyData(archive *ar, archive *aw);

#else

        /**
         * @brief Creates a TAR archive using libarchive, supporting long paths (>260 chars) via wstring.
         *
         * @param output_filename_w The wide-character name of the output TAR file.
         * @param source_path_w The wide-character path of the source file to archive.
         * @param archive_path_w The wide-character path to use inside the archive (the long path).
         * @return true if the archive was created successfully, false otherwise.
         */
        static bool create_tar_with_long_path(const std::wstring& output_filename_w, const std::wstring& source_path_w, const std::wstring& archive_path_w);

#endif
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_TAR_UTILS_H