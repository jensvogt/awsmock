//
// Created by vogje01 on 22/08/2022.
//

#ifndef AWS_MOCK_CORE_FILE_UTILS_H
#define AWS_MOCK_CORE_FILE_UTILS_H

// Standard C includes
#include <sys/sendfile.h>

// Standard C++ includes
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <pwd.h>
#include <string>

// Boost includes
#include <boost/asio/streambuf.hpp>
#include <boost/beast/core/file.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/iostreams/copy.hpp>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/StringUtils.h>

#define BUFFER_LEN 8092

namespace AwsMock::Core {

    /**
     * @brief File utilities.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class FileUtils {

      public:

        /**
         * @brief Extracts the base name (without extension) from the given file name.
         *
         * @param fileName
         * @return base name of the filename
         */
        static std::string GetBasename(const std::string &fileName);

        /**
         * @brief Returns the file extension.
         *
         * @param fileName name fo the file
         * @return file extension in lower case letters
         */
        static std::string GetExtension(const std::string &fileName);

        /**
         * @brief Returns the parent directory name.
         *
         * @param fileName name fo the file
         * @return parent directory name
         */
        static std::string GetParentPath(const std::string &fileName);

        /**
         * @brief Check whether the file has the given pattern
         *
         * @param fileName file name to check
         * @param pattern file name pattern
         * @return true in case the file matches the pattern
         */
        static bool FileNameContainsString(const std::string &fileName, const std::string &pattern);

        /**
         * @brief Returns a thread safe temp file name.
         *
         * @param extension desired file extension
         * @return thread safe temp file name
         */
        static std::string GetTempFile(const std::string &extension);

        /**
         * @brief Returns a thread safe temp file name.
         *
         * @param dir directory to create the file
         * @param extension desired file extension
         * @return thread safe temp file name
         */
        static std::string GetTempFile(const std::string &dir, const std::string &extension);

        /**
         * @brief Creates an temporary file containing an random string
         *
         * @param extension file name extension
         * @return file name
         */
        static std::string CreateTempFile(const std::string &extension);

        /**
         * @brief Creates an temporary file containing an specific content string
         *
         * @param extension file name extension
         * @param content content of the file
         * @return file name
         */
        static std::string CreateTempFile(const std::string &extension, const std::string &content);

        /**
         * @brief Creates an temporary file containing a numBytes long random string
         *
         * @param extension file name extension
         * @param numBytes length of the file
         * @return file name
         */
        static std::string CreateTempFile(const std::string &extension, int numBytes);

        /**
         * @brief Creates an temporary file containing a numBytes long random string in the given directory.
         *
         * @param dirName name of the directory
         * @param extension file name extension
         * @param numBytes length of the file
         * @return file name
         */
        static std::string CreateTempFile(const std::string &dirName, const std::string &extension, int numBytes);

        /**
         * @brief Check whether a file exists.
         *
         * @param fileName name of the file.
         * @return true, if file exists otherwise false.
         */
        static bool FileExists(const std::string &fileName);

        /**
         * @brief Returns the size of the given in bytes.
         *
         * @param fileName name of the file.
         * @return size of the in bytes.
         */
        static long FileSize(const std::string &fileName);

        /**
         * @brief Moves a file to a new directory.
         *
         * @param sourceFileName name of the source file.
         * @param targetFileName name of the target file.
         * @param createDir if true, create directory tree if it does not exist.
         */
        static void MoveTo(const std::string &sourceFileName, const std::string &targetFileName, bool createDir = true);

        /**
         * @brief Copies a file to a new directory.
         *
         * @param sourceFileName name of the source file.
         * @param targetFileName name of the target file.
         * @param createDir if true, create directory tree if it does not exist.
         */
        static void CopyTo(const std::string &sourceFileName, const std::string &targetFileName, bool createDir = true);

        /**
         * @brief Append several binary files to a single output file.
         *
         * <p>The out file will be truncated, before its used </p>
         *
         * @param outFile output file name
         * @param inDir input directory
         * @param files string vector of binary files to append to output file
         */
        static long AppendBinaryFiles(const std::string &outFile, const std::string &inDir, const std::vector<std::string> &files);

        /**
         * @brief Append several text files to a single output file.
         *
         * <p>The out file will be truncated, before its used </p>
         *
         * @param outFile output file name
         * @param inDir input directory
         * @param files string vector of text files to append to output file
         * @return output file size
         */
        static long AppendTextFiles(const std::string &outFile, const std::string &inDir, const std::vector<std::string> &files);

        /**
         * @brief Strips the path from the filename
         *
         * @param fileName name of the file
         * @return file name without base path
         */
        static std::string StripBasePath(const std::string &fileName);

        /**
         * @brief Returns the file owner.
         *
         * @param fileName name of the file to delete
         */
        static std::string GetOwner(const std::string &fileName);

        /**
         * @brief Delete the file with the given file name
         *
         * @param fileName name of the file to delete
         */
        static void DeleteFile(const std::string &fileName);

        /**
         * @brief Unzip the given file to the given directory.
         *
         * @param zipFile zip file to decompress.
         * @param dirName output directory.
         */
        static void UnzipFiles(const std::string &zipFile, const std::string &dirName);

        /**
         * @brief Unzip the given file to the given directory.
         *
         * @param zipFile zip file to decompress.
         * @param dirName directory to zip.
         */
        static void ZipFiles(const std::string &zipFile, const std::string &dirName);

        /**
         * @brief Touch an existing file
         *
         * @param fileName absolute file name of the file
         */
        static bool Touch(const std::string &fileName);

        /**
         * @brief Strip chunk signatures from file
         *
         * @param path absolute file path
         */
        static void StripChunkSignature(const std::string &path);

        /**
         * @brief File path separator
         *
         * @return file path separator
         */
        static std::string separator();
    };

    inline std::string FileUtils::separator() {
        return "/";
    }

}// namespace AwsMock::Core

#endif// AWS_MOCK_CORE_FILE_UTILS_H
