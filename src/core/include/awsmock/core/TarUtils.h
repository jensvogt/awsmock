//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_CORE_TAR_UTILS_H
#define AWSMOCK_CORE_TAR_UTILS_H

#ifdef WIN32
#ifndef NOMINMAX
# define NOMINMAX
#endif
#endif

// C++ standard includes
#include <string>

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
         * @param removeDir remove name of the directory from filename
         * @param isDir directory flag
         * @param isLink link flag, needed to preserve links
         */
        static void WriteFile(archive *archive, const std::string &fileName, const std::string &removeDir, bool isDir, bool isLink);

        /**
         * @brief Read a symbolic link.
         *
         * @param path path to link.
         * @return target path.
         */
        static std::string Readsymlink(const std::string &path);

        static int CopyData(archive *ar, archive *aw);
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_TAR_UTILS_H