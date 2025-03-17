//
// Created by vogje01 on 11/2/23.
//

#include <awsmock/core/MemoryMappedFile.h>

namespace AwsMock::Core {

    boost::mutex MemoryMappedFile::_mutex;

    bool MemoryMappedFile::OpenFile(const std::string &filename) {
#ifdef WIN32
        // TODO: Fix windows port
        return true;
#else
        _fileSize = FileUtils::FileSize(filename);

        int fd = open(filename.c_str(), O_RDONLY);
        if (fd < 0) {
            log_error << "Could not open file for memory mapping";
            return false;
        }

        _start = mmap(nullptr, _fileSize, PROT_READ, MAP_SHARED, fd, 0);
        _membuffer = (char *) _start;

        if (_membuffer == MAP_FAILED) {
            close(fd);
            log_error << "Could not memory map file: " << filename;
            return false;
        }
        close(fd);

        _mapped = true;
        return _mapped;
#endif
    }

    void MemoryMappedFile::CloseFile() {
        boost::mutex::scoped_lock lock(_mutex);
#ifdef WIN32
#else
        if (munmap(_start, _fileSize) < 0) {
            log_error << "Could not unmap file";
        }
#endif
        log_debug << "Memory mapped file closed";
        _mapped = false;
    }

    long MemoryMappedFile::ReadChunk(long start, long length, char *buffer) {
        if (start + length > _fileSize) {
            length = _fileSize - start;
        }
        boost::mutex::scoped_lock lock(_mutex);
        log_debug << "start: " << start << " length: " << length << " size: " << _fileSize;
        memcpy(buffer, _membuffer + start, length);
        return length;
    }

}// namespace AwsMock::Core