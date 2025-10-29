//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_CORE_ZIP_UTILS_H
#define AWSMOCK_CORE_ZIP_UTILS_H

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
#include <Windows.h>
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

namespace AwsMock::Core {

    /**
     * @brief TAR compression utilities
     *
     * @par
     * If you have problems using long path names (Java sometimes has long path names) on Windows, set the following registry key using PowerShell:
     * New-ItemProperty-Path "HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem" -Name "LongPathsEnabled" -Value 1 -PropertyType DWORD -Force
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ZipUtils {

      public:

        /**
         * @brief Constructor
         */
        ZipUtils() = default;

        /**
         * @brief Unzip directory
         *
         * @param zipFile file name to unzip
         * @param directory output directory
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
        static std::string ReadSymlink(const std::string &path);

        /**
         * @brief Copy data from A to B
         *
         * @param ar source file name
         * @param aw destination file name
         * @return success of -1
         */
        static int CopyData(archive *ar, archive *aw);
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_ZIP_UTILS_H