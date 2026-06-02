//
// Created by vogje01 on 4/12/26.
//

#pragma once

// Libmagic includes
#include <magic.h>

// Awsmock includes
#include <awsmock/core/logging/LogStream.h>

namespace Awsmock::Core {

    class MagicDetector {

      public:

        MagicDetector() : _magic(magic_open(MAGIC_MIME_TYPE)) {
            if (!_magic) {
                log_error << "Failed to open libmagic";
                return;
            }
            const auto magicFile = Configuration::instance().get<std::string>("awsmock.magic-file");
            if (magic_load(_magic, magicFile.c_str()) != 0) {
                log_error << "Failed to load magic db";
                magic_close(_magic);
                _magic = nullptr;
            }
        }

        ~MagicDetector() {
            if (_magic) {
                magic_close(_magic);
            }
        }

        static MagicDetector &instance() {
            static MagicDetector detector;
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
            if (!_magic) {
                return "application/octet-stream";
            }
            const char *result = magic_file(_magic, path.c_str());
            return result ? result : "application/octet-stream";
        }

        // Detect from string content
        [[nodiscard]]
        std::string fromContent(const std::string &content) const {
            std::lock_guard lock(_mutex);
            if (!_magic) {
                return "application/octet-stream";
            }
            const char *result = magic_buffer(_magic, content.data(), content.size());
            return result ? result : "application/octet-stream";
        }

      private:

        magic_t _magic;
        mutable std::mutex _mutex;
        mutable logger_t _logger{boost::log::keywords::channel = "Core"};
    };

}// namespace Awsmock::Core
