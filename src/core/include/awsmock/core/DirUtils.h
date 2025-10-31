//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWS_MOCK_CORE_DIRECTORY_UTILS_H
#define AWS_MOCK_CORE_DIRECTORY_UTILS_H

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define BOOST_ASIO_NO_WIN32_LEAN_AND_MEAN
#include <boost/asio.hpp>
#include <windows.h>
#endif


// C++ standard includes
#include <string>

// Boost includes
#include <boost/filesystem.hpp>

// AwsMock includes
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/StringUtils.h>

namespace AwsMock::Core {

    /**
     * @brief Substring comparison
     */
    struct SubstringCompare {

        std::string _delimiter;

        /**
         * @brief Constructor
         *
         * @param delimiter delimiter character
         */
        explicit SubstringCompare(const std::string &delimiter) : _delimiter(delimiter) {}

        /**
         * @brief Comparison
         *
         * @param a comparison value
         * @param b comparison value
         */
        bool operator()(std::string const &a, std::string const &b) const {

            const std::string sa = StringUtils::Split(a, _delimiter)[1];
            const std::string sb = StringUtils::Split(b, _delimiter)[1];

            return std::stol(sa) < std::stol(sb);
        }
    };

    /**
     * @brief Directory utilities.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class DirUtils {

    public:

        /**
         * @brief Returns a thread safe temp directory name.
         *
         * @return thread safe temp directory name
         */
        static std::string GetTempDir();

        /**
         * @brief Returns relative path for the given directory.
         *
         * @param dir directory name
         * @return relative path to dir
         */
        static std::string RelativePath(const std::string &dir);

        /**
         * @brief Creates an empty temporary directory within the given parent directory
         *
         * @param parent parent directory for the temporary directory
         * @return thread safe temp directory name
         */
        static std::string CreateTempDir(const std::string &parent = {});

        /**
         * @brief Checks the existence of a directory
         *
         * @param dirName name of the directory.
         * @return true, if the directory exists otherwise false.
         */
        static bool DirectoryExists(const std::string &dirName);

        /**
         * @brief Count the number of files in the geven directory.
         *
         * @param dirName name of the directory.
         * @param recursive recursively file counting.
         * @return total number of files.
         */
        static long DirectoryCountFiles(const std::string &dirName, bool recursive = true);

        /**
         * @brief Checks whether the directory is empty.
         *
         * @param dirName name of the directory.
         * @return true, if directory is empty.
         */
        static bool DirectoryEmpty(const std::string &dirName);

        /**
         * @brief Creates a directory tree recursively
         *
         * @param dirName name of the directory.
         * @param recursive if true, create also all parent directories.
         */
        static void MakeDirectory(const std::string &dirName, bool recursive = true);

        /**
         * @brief Ensures that a directory exists.
         *
         * <p>If it does not exist, it will be created.</p>
         *
         * @param dirName name of the directory.
         */
        static void EnsureDirectory(const std::string &dirName);

        /**
         * @brief Checks whether the given file name is a directory.
         *
         * @param dirName name of the directory.
         * @return true if the supplied file name is actually a directory.
         */
        static bool IsDirectory(const std::string &dirName);

        /**
         * @brief Get a list of files
         *
         * @param dirName name of the directory
         * @param recursive recursively list files
         * @return vector of file names
         */
        static std::vector<std::string> ListFiles(const std::string &dirName, bool recursive = true);

        /**
         * @brief Get a sorted list of files with the given prefix.
         *
         * @param dirName name of the directory
         * @param prefix file name prefix
         * @return vector of file names
         */
        static std::vector<std::string> ListFilesByPrefix(const std::string &dirName, const std::string &prefix);

        /**
         * @brief Get a sorted list of files with the given extension.
         *
         * @param dirName name of the directory
         * @param extension file extension
         * @param sorting sort by file name, default: false
         * @return vector of file names
         */
        static std::vector<std::string> ListFilesByExtension(const std::string &dirName, const std::string &extension, bool sorting = false);

        /**
         * @brief Get a list of files with the given pattern.
         *
         * @param dirName name of the directory
         * @param pattern regular expression which must match the filename
         * @param recursive recursively list files
         * @param sorting sort by file name, default: false
         * @return vector of file names
         */
        static std::vector<std::string> ListFilesByPattern(const std::string &dirName, const std::string &pattern, bool recursive = true, bool sorting = false);

        /**
         * @brief Deletes a whole directory tree.
         *
         * @param dirName directory name.
         */
        static void DeleteDirectory(const std::string &dirName);

        /**
         * @brief Deletes all files in the directory, but not the directory itself.
         *
         * @param dirName directory name.
         */
        static void DeleteFilesInDirectory(const std::string &dirName);
    };

}// namespace AwsMock::Core

#endif// AWS_MOCK_CORE_DIRECTORY_UTILS_H
