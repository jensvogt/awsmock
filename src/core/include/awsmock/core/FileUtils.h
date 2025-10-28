//
// Created by vogje01 on 22/08/2022.
//

#ifndef AWS_MOCK_CORE_FILE_UTILS_H
#define AWS_MOCK_CORE_FILE_UTILS_H

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define BOOST_ASIO_NO_WIN32_LEAN_AND_MEAN
#include <boost/asio.hpp>
#include <windows.h>
#endif

// Standard C includes
#include <magic.h>

// Standard C++ includes
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#ifndef _WIN32
#include <pwd.h>
#endif

#ifdef __APPLE__
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#elif __linux__
#include <sys/sendfile.h>
#elif _WIN32
#include "accctrl.h"
#include "aclapi.h"
#include <stdio.h>
#include <tchar.h>
#pragma comment(lib, "advapi32.lib")
#endif

// Boost includes
#include <boost/asio/streambuf.hpp>
#include <boost/beast/core/file.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/iostreams/copy.hpp>

// AwsMock includes
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/FieldAlloc.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/SystemUtils.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/core/logging/LogStream.h>

#define BUFFER_LEN 8092
#define DEFAULT_MIME_TYPE "application/octet-stream"

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
         * @brief Returns the first line of a file
         *
         * @param filePath directory to create the file
         * @return first line
         */
        static std::string GetFirstLine(const std::string &filePath);

        /**
         * @brief Removes the first line of a file
         *
         * @param filePath directory to create the file
         * @param skip number of bytes to skip
         */
        static void RemoveFirstBytes(const std::string &filePath, int skip);

        /**
         * @brief Creates a temporary file containing a random string
         *
         * @param extension file name extension
         * @return file name
         */
        static std::string CreateTempFile(const std::string &extension);

        /**
         * @brief Creates a temporary file containing a specific content string
         *
         * @param extension file name extension
         * @param content content of the file
         * @return file name
         */
        static std::string CreateTempFile(const std::string &extension, const std::string &content);

        /**
         * @brief Creates a temporary file containing a numBytes long random string
         *
         * @param extension file name extension
         * @param numBytes length of the file
         * @return file name
         */
        static std::string CreateTempFile(const std::string &extension, int numBytes);

        /**
         * @brief Creates a temporary file containing a numBytes long random string in the given directory.
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
         * @return true, if the file exists otherwise false.
         */
        static bool FileExists(const std::string &fileName) {
            return std::filesystem::exists(fileName) && std::filesystem::is_regular_file(fileName);
        }

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
         * @param fileName name of the file to get the owner from
         */
        static std::string GetOwner(const std::string &fileName);

        /**
         * @brief Delete the file with the given file name
         *
         * @param fileName name of the file to delete
         */
        static void RemoveFile(const std::string &fileName);

        /**
         * @brief Read a whole file into a string
         *
         * @param fileName name of the file
         * @return string with file content
         */
        static std::string ReadFile(const std::string &fileName);

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
         * @brief Get the Mime type using libmagic
         *
         * @param path absolute file path
         * @return content type, as mime type
         */
        static std::string GetContentTypeMagicFile(const std::string &path);

        /**
         * @brief Get the Mime type by extension
         *
         * @param path absolute file path
         * @param realPath real file path
         * @return content type, as mime type
         */
        static std::string GetContentType(const std::string &path, const std::string &realPath);

        /**
         * @brief Get the Mime type using libmagic from a string
         *
         * @param content content string
         * @return content type, as mime type
         */
        static std::string GetContentTypeMagicString(const std::string &content);

        /**
         * @brief Checks whether the file is base64 encoded.
         *
         * @param filePath file path
         * @return true if the file is base64 encoded
         */
        static bool IsBase64(const std::string &filePath);

        /**
         * @brief Decodes a Base64 encoded file
         *
         * @param filePath file path
         */
        static void Base64DecodeFile(const std::string &filePath);

        /**
         * @brief File path separator
         *
         * @return file path separator
         */
        static std::string SetFileSeparator(std::string &filePath);

        /**
         * @brief Remove the last n bytes from the file
         *
         * @param filename name of the file
         * @param size number of bytes to remove
         */
        static void RemoveLastBytes(const std::string &filename, long size);

        /**
         * @brief Copies a file from source to destination.
         *
         * @param inputFile input filename
         * @param outputFile output filename
         * @return number of bytes copied
         */
        static long StreamCopier(const std::string &inputFile, const std::string &outputFile);

        /**
         * @brief Copies a part of a file from source to destination.
         *
         * @param inputFile input filename
         * @param outputFile output filename
         * @param start bytes from the beginning of the file
         * @param length number of bytes to copy
         * @return number of bytes copied
         */
        static long StreamCopier(const std::string &inputFile, const std::string &outputFile, long start, long length);

        /**
         * @brief Copies a file from source to destination.
         *
         * @param inputFile input filename
         * @param outputFile output filename
         * @param count number of bytes to copy
         * @return number of bytes copied
         */
        static long StreamCopier(const std::string &inputFile, const std::string &outputFile, long count);

        /**
         * @brief Copies all from an input stream to an output stream.
         *
         * @param is input stream
         * @param os output stream
         * @return number of bytes copied
         */
        static long StreamCopier(std::istream &is, std::ostream &os);

        /**
         * @brief Copies maxSize bytes from an input stream to an output stream.
         *
         * @param istream input stream
         * @param ostream output stream
         * @param count number of bytes to copy
         * @return number of bytes copied
         */
        static long StreamCopier(std::istream &istream, std::ostream &ostream, long count);

        /**
         * @brief Copies maxSize bytes from an input stream to an output stream.
         *
         * @param istream input stream
         * @param ostream output stream
         * @param start number of bytes from the beginning of the file
         * @param count number of bytes to copy
         * @return number of bytes copied
         */
        static long StreamCopier(std::istream &istream, std::ostream &ostream, long start, long count);

#ifdef _WIN32
        /**
         * @brief Prepends the Windows extended-length path prefix to support paths
         * longer than MAX_PATH (260 characters).
         *
         * @par
         * The resulting path starts with "\\?\". Note: This only works with wide-character (Unicode) APIs.
         *
         * @param path The path to normalize.
         * @return The normalized path as a wide string.
         */
        static std::wstring NormalizePathForLongPaths(const std::wstring& path);
#endif

        /**
         * @brief File path separator
         *
         * @return file path separator
         */
        static std::string separator();

        /**
         * Mime types from file extensions
         */
        const static std::map<std::string, std::string> MimeTypes;

        template<typename... Ts>
        static std::string appendPath(Ts &&...args) {
            std::ostringstream oss;
            (oss << ... << std::forward<Ts>(args));
            return oss.str();
        }
    };

    inline std::string FileUtils::SetFileSeparator(std::string &filePath) {
        boost::replace_all(filePath, "\\", "/");
        return filePath;
    }

    inline std::string FileUtils::separator() {
#ifdef _WIN32
        return "\\";
#else
        return "/";
#endif
    }


}// namespace AwsMock::Core

#endif// AWS_MOCK_CORE_FILE_UTILS_H
