//
// Created by vogje01 on 4/12/26.
//

#ifndef AWSMOCK_CORE_MAGIC_DETECTOR_H
#define AWSMOCK_CORE_MAGIC_DETECTOR_H

// Libmagic includes
#include <magic.h>

// Awsmock includes
#include <awsmock/core/logging/LogStream.h>

namespace AwsMock::Core {

    class MagicDetector {

      public:

        MagicDetector() : _magic(magic_open(MAGIC_MIME_TYPE)) {
            if (!_magic) {
                log_error << "Failed to open libmagic";
                return;
            }
            const auto magicFile = Configuration::instance().GetValue<std::string>("awsmock.magic-file");
            if (magic_load(_magic, magicFile.c_str()) != 0) {
                magic_close(_magic);
                log_error << "Failed to load magic db";
                return;
            }
        }
        ~MagicDetector() { magic_close(_magic); }

        static MagicDetector &instance() {
            static MagicDetector detector;// thread-safe since C++11
            return detector;
        }

        // Delete copy/move
        MagicDetector(const MagicDetector &) = delete;
        MagicDetector &operator=(const MagicDetector &) = delete;
        MagicDetector(MagicDetector &&) = delete;
        MagicDetector &operator=(MagicDetector &&) = delete;

        // Detect from file path
        [[nodiscard]]
        std::string fromFile(const std::string &path) const {
            std::lock_guard lock(_mutex);
            const char *result = magic_file(_magic, path.c_str());
            return result ? result : "application/octet-stream";
        }

        // Detect from string content
        [[nodiscard]]
        std::string fromContent(const std::string &content) const {
            std::lock_guard lock(_mutex);
            const char *result = magic_buffer(_magic, content.data(), content.size());
            return result ? result : "application/octet-stream";
        }

      private:

        magic_t _magic;
        mutable std::mutex _mutex;
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_MAGIC_DETECTOR_H
