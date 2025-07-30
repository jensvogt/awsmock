//
// Created by vogje01 on 11/2/23.
//

#ifndef AWS_MOCK_CORE_MEMORY_MAPPED_FILE_H
#define AWS_MOCK_CORE_MEMORY_MAPPED_FILE_H

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define BOOST_ASIO_NO_WIN32_LEAN_AND_MEAN
#include <boost/asio.hpp>
#include <windows.h>
#endif


// C includes
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#ifndef _WIN32
#include <sys/mman.h>
#include <unistd.h>
#endif

// C++ includes
#include <string>

// Boost includes
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

// AwsMock includes
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/logging/LogStream.h>

namespace AwsMock::Core {

    /**
     * @brief Memory mapped file utility.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class MemoryMappedFile {

      public:

        /**
         * @brief Constructor
         */
        MemoryMappedFile() : _start(nullptr), _membuffer(nullptr), _mapped(false) {}

        /**
         * @brief Singleton
         *
         * @return singleton instance
         */
        static MemoryMappedFile &instance() {
            static MemoryMappedFile memoryMappedFile;
            return memoryMappedFile;
        }

        /**
         * @brief Opens a file and prepares the memory map
         *
         * @param filename name of the file.
         * @return true on success
         */
        bool OpenFile(const std::string &filename);

        /**
         * @brief Release all resources
         */
        void CloseFile();

        /**
         * @brief Copy a chunk of data from the memory mapped file the provided output buffer
         *
         * @param start start index
         * @param end end index
         * @param buffer char buffer
         * @return number of bytes actually read
         */
        long ReadChunk(long start, long end, char *buffer);

        /**
         * @brief Returns true in case the file is mapped already.
         *
         * @return true, in case file is already mapped.
         */
        [[nodiscard]] bool IsMapped() const { return _mapped; }

      private:

        typedef int FileHandle;

        /**
         * Start pointer
         */
        void *_start;

        /**
         * Char buffer
         */
        char *_membuffer;

        /**
         * Mapped flag
         */
        bool _mapped;

        /**
         * Start pointer
        */
        std::string _filename;

        /**
         * File handle
         */
        FileHandle _file{};

        /**
         * File size
         */
        long _fileSize = 0;

        /**
         * Mutex
         */
        static boost::mutex _mutex;
    };

}// namespace AwsMock::Core

#endif// AWS_MOCK_CORE_MEMORY_MAPPED_FILE_H
