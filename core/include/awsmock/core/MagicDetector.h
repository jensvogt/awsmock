//
// Created by vogje01 on 4/12/26.
//

#pragma once

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
            const auto magicFile = Configuration::instance().get<std::string>("awsmock.magic-file");
            if (magic_load(_magic, magicFile.c_str()) != 0) {
                magic_close(_magic);
                log_error << "Failed to load magic db";
                return;
            }
        }

        ~MagicDetector() { magic_close(_magic); }

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
            //#ifdef _WIN32
            //            return detectMimeTypeWin(path);
            //#else
            std::lock_guard lock(_mutex);
            const char *result = magic_file(_magic, path.c_str());
            return result ? result : "application/octet-stream";
            //#endif
        }

        // Detect from string content
        [[nodiscard]]
        std::string fromContent(const std::string &content) const {
            std::lock_guard lock(_mutex);
            const char *result = magic_buffer(_magic, content.data(), content.size());
            return result ? result : "application/octet-stream";
        }

#ifdef _WIN32
        static std::string detectMimeTypeWin(const std::string &filePath) {
            // Windows fallback — extension-based detection
            // libmagic is unreliable on Windows without correct .mgc
            static const std::unordered_map<std::string, std::string> mimeMap = {
                {".jpg", "image/jpeg"},
                {".jpeg", "image/jpeg"},
                {".png", "image/png"},
                {".pdf", "application/pdf"},
                {".json", "application/json"},
                {".xml", "application/xml"},
                {".txt", "text/plain"},
                {".zip", "application/zip"},
                {".gz", "application/gzip"},
            };
            const std::filesystem::path p(filePath);
            std::string ext = p.extension().string();
            std::ranges::transform(ext, ext.begin(), tolower);
            const auto it = mimeMap.find(ext);
            return it != mimeMap.end() ? it->second : "application/octet-stream";
#endif
        }

    private:
        magic_t _magic;
        mutable std::mutex _mutex;
    };

} // namespace AwsMock::Core
