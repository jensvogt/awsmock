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
#include <awsmock/core/LogStream.h>
#include <awsmock/core/StringUtils.h>

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

        /**
         * @brief Unzip file to directory
         *
         * @param zipFile name of the tar file
         * @param directory directory name
         */
        static void Unzip(const std::string &zipFile, const std::string &directory);

      private:

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

#ifdef _WIN32

        /**
         * @brief Prepare the log file name on Windows machines
         *
         * @param filename file name
         * @return file with "\\?\" prepended
         */
        static std::string PrepareFilename(std::string &filename);
#endif
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_TAR_UTILS_H