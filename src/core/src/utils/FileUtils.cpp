#include <awsmock/core/FileUtils.h>

namespace AwsMock::Core {

    const std::map<std::string, std::string> FileUtils::MimeTypes = {
            {".aac", "audio/aac"},
            {".abw", "application/x-abiword"},
            {".apng", "application/apng"},
            {".aec", "application/x-freearc"},
            {".avif", "image/avif"},
            {".avi", "video/x-msvideo"},
            {".azw", "application/vnd.amazon.ebook"},
            {".bin", "application/octet-stream"},
            {".bmp", "application/bmp"},
            {".bz", "application/x-bzip"},
            {".bz2", "application/x-bzip"},
            {".cda", "application/x-cdf"},
            {".csh", "application/x-csh"},
            {".css", "text/css"},
            {".csv", "text/csv"},
            {".doc", "application/ms-word"},
            {".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
            {".eot", "application/vnd.ms-fontobject"},
            {".epub", "application/epub+zip"},
            {".gz", "application/gzip"},
            {".gif", "image/gif"},
            {".ico", "image/vnd.microsoft.icon"},
            {".ics", "text/calendar"},
            {".html", "test/html"},
            {".html", "test/html"},
            {".jar", "application/java-archive"},
            {".jpg", "image/jpeg"},
            {".jpeg", "image/jpeg"},
            {".js", "application/javascript"},
            {".json", "application/json"},
            {".jsonld", "application/ld+json"},
            {".mid", "audio/midi"},
            {".midi", "audio/midi"},
            {".mjs", "text/javascript"},
            {".mp3", "audio/mpeg"},
            {".mp4", "video/mp4"},
            {".mpeg", "video/mpeg"},
            {".mpkg", "application/vnd.apple.installer+xml"},
            {".odp", "application/vnd.oasis.opendocument.presentation"},
            {".ods", "application/vnd.oasis.opendocument.spreadsheet"},
            {".oga", "audio/ogg"},
            {".ogv", "video/ogg"},
            {".ogx", "application/ogg"},
            {".opus", "audio/ogg"},
            {".otf", "font/otf"},
            {".pdf", "application/pdf"},
            {".png", "image/png"},
            {".php", "application/x-httpd-php"},
            {".ppt", "application/vnd.ms-powerpoint"},
            {".pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
            {".rar", "application/vnd.rar"},
            {".rtf", "application/rtf"},
            {".sh", "application/x-sh"},
            {".svg", "image/svg+xml"},
            {".tar", "application/x-tar"},
            {".tif", "image/tiff"},
            {".tiff", "image/tiff"},
            {".ts", "video/mp2t"},
            {".ttf", "font/ttf"},
            {".txt", "text/plain"},
            {".vsd", "application/vnd.visio"},
            {".wav", "audio/wav"},
            {".weba", "audio/webm"},
            {".webm", "video/webm"},
            {".webp", "image/webp"},
            {".woff", "font/woff"},
            {".woff2", "font/woff2"},
            {".xhtml", "application/xhtml+xml"},
            {".xls", "application/vnd.ms-excel"},
            {".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
            {".xml", "application/xml"},
            {".xul", "application/vnd.mozilla.xul+xml"},
            {".zip", "application/zip"},
            {".3gp", "video/3gpp"},
            {".3g2", "video/3gpp2"},
            {".7z", "application/x-7z-compressed"},
    };

    std::string FileUtils::GetBasename(const std::string &fileName) {
        const std::filesystem::path path(fileName);
        return path.filename().replace_extension().string();
    }

    std::string FileUtils::GetExtension(const std::string &fileName) {
        const std::filesystem::path path(fileName);
        return path.extension().string().substr(1);
    }

    bool FileUtils::FileNameContainsString(const std::string &fileName, const std::string &pattern) {
        const std::filesystem::path path(fileName);
        return GetBasename(fileName).find(pattern) != std::string::npos;
    }

    std::string FileUtils::GetTempFile(const std::string &extension) {
        const boost::filesystem::path temp = boost::filesystem::temp_directory_path().append(boost::filesystem::unique_path());
        return temp.native() + "." + extension;
    }

    std::string FileUtils::GetTempFile(const std::string &dir, const std::string &extension) {
        DirUtils::EnsureDirectory(dir);
        const boost::filesystem::path temp = boost::filesystem::unique_path();
        return dir + "/" + temp.string() + "." + extension;
    }

    std::string FileUtils::GetParentPath(const std::string &fileName) {
        const std::filesystem::path path(fileName);
        return path.parent_path();
    }

    long FileUtils::FileSize(const std::string &fileName) {
        if (FileExists(fileName)) {
            return static_cast<long>(std::filesystem::file_size({fileName.c_str()}));
        }
        return -1;
    }

    void FileUtils::MoveTo(const std::string &sourceFileName, const std::string &targetFileName, const bool createDir) {
        try {
            if (createDir) {
                create_directories(boost::filesystem::path(GetParentPath(targetFileName)));
            }
            boost::filesystem::rename(boost::filesystem::path(sourceFileName), boost::filesystem::path(targetFileName));
        } catch (std::exception &e) {
            log_error << e.what();
        }
    }

    void FileUtils::CopyTo(const std::string &sourceFileName, const std::string &targetFileName, const bool createDir) {

        if (createDir) {
            create_directories(boost::filesystem::path(GetParentPath(targetFileName)));
        }
        copy_file(boost::filesystem::path(sourceFileName), boost::filesystem::path(targetFileName), boost::filesystem::copy_options::none);
    }

    long FileUtils::AppendBinaryFiles(const std::string &outFile, const std::string &inDir, const std::vector<std::string> &files) {

        const int dest = open(outFile.c_str(), O_WRONLY | O_CREAT, 0644);

        size_t copied = 0;
        for (auto &it: files) {

            const int source = open(it.c_str(), O_RDONLY, 0);
            struct stat stat_source{};
            fstat(source, &stat_source);
            copied += sendfile(dest, source, nullptr, stat_source.st_size);

            close(source);
        }
        close(dest);
        return static_cast<long>(copied);
    }

    long FileUtils::AppendTextFiles(const std::string &outFile, const std::string &inDir, const std::vector<std::string> &files) {

        long copied = 0;
        std::ofstream ofs(outFile, std::ios::out | std::ios::trunc);
        for (auto &it: files) {
            std::ifstream ifs(it, std::ios::in);
            copied = boost::iostreams::copy(ifs, ofs);
            ofs.flush();
            ifs.close();
        }
        ofs.close();
        return copied;
    }

    std::string FileUtils::CreateTempFile(const std::string &extension) {
        std::string tempFilename = GetTempFile(extension);
        std::ofstream tempFile;
        tempFile.open(tempFilename);
        tempFile << StringUtils::GenerateRandomString(100);
        tempFile.close();
        return tempFilename;
    }

    std::string FileUtils::CreateTempFile(const std::string &extension, const int numBytes) {
        std::string tempFilename = GetTempFile(extension);
        std::ofstream tempFile;
        tempFile.open(tempFilename);
        tempFile << StringUtils::GenerateRandomString(numBytes);
        tempFile.close();
        return tempFilename;
    }

    std::string FileUtils::CreateTempFile(const std::string &dirName, const std::string &extension, const int numBytes) {
        std::string tmpFileName = GetTempFile(dirName, extension);
        std::ofstream tempFile;
        tempFile.open(tmpFileName);
        tempFile << StringUtils::GenerateRandomString(numBytes);
        tempFile.close();
        return tmpFileName;
    }

    std::string FileUtils::CreateTempFile(const std::string &extension, const std::string &content) {
        std::string tempFilename = GetTempFile(extension);
        std::ofstream tempFile;
        tempFile.open(tempFilename);
        tempFile << content;
        tempFile.close();
        return tempFilename;
    }

    bool FileUtils::FileExists(const std::string &fileName) {
        return std::filesystem::exists(fileName);
    }

    std::string FileUtils::StripBasePath(const std::string &fileName) {
        return GetBasename(fileName) + "." + GetExtension(fileName);
    }

    std::string FileUtils::GetOwner(const std::string &fileName) {

        struct stat info{};
        stat(fileName.c_str(), &info);
        if (const passwd *pw = getpwuid(info.st_uid)) {
            return pw->pw_name;
        }
        return {};
    }

    void FileUtils::DeleteFile(const std::string &fileName) {
        if (fileName.empty()) {
            return;
        }
        if (std::filesystem::exists(fileName)) {
            std::filesystem::remove(fileName);
        }
    }

    bool FileUtils::Touch(const std::string &fileName) {
        const int fd = open(fileName.c_str(), O_WRONLY | O_CREAT | O_NOCTTY | O_NONBLOCK, 0666);
        if (fd < 0) {
            log_error << "Could not open file: " << fileName;
            return false;
        }
        if (int rc = utimensat(AT_FDCWD, fileName.c_str(), nullptr, 0)) {
            log_error << "Could not touch file: " << fileName;
            return false;
        }
        close(fd);
        return true;
    }

    std::string FileUtils::GetContentType(const std::string &path) {
        if (const std::string extension = boost::filesystem::path(path).extension().string(); MimeTypes.contains(extension)) {
            return MimeTypes.at(extension);
        }
        return GetContentTypeMagicFile(path);
    }

    std::string FileUtils::ReadFile(const std::string &fileName) {
        boost::filesystem::path pat(fileName);

        // Open the stream to 'lock' the file.
        std::ifstream f(fileName, std::ios::in | std::ios::binary);

        // Obtain the size of the file.
        const auto sz = boost::filesystem::file_size(fileName);

        // Create a buffer.
        std::string result(sz, '\0');

        // Read the whole file into the buffer.
        f.read(result.data(), sz);

        return result;
    }

    std::string FileUtils::GetContentTypeMagicFile(const std::string &path) {

        if (!FileExists(path)) {
            return "application/octet-stream";
        }

        // allocate magic cookie
        magic_set *const magic = magic_open(MAGIC_MIME_TYPE);
        if (magic == nullptr) {
            log_error << "Could not open libmagic";
        }

        // load the default magic database (indicated by nullptr)
        if (magic_load(magic, nullptr) != 0) {
            log_error << "Could not load libmagic";
        }

        // compile the default magic database (indicated by nullptr)
        if (magic_compile(magic, nullptr) != 0) {
            log_error << "Could not compile libmagic";
        }

        // get description of the filename argument
        const char *mime = magic_file(magic, path.c_str());
        if (mime == nullptr) {
            log_error << "Could not get mime type";
            mime = "application/octet-stream";
        } else {
            log_debug << "Found content-type: " << mime;
        }
        std::string result = {mime};

        // Free magic cookie and mime
        magic_close(magic);

        return result;
    }

    std::string FileUtils::GetContentTypeMagicString(const std::string &content) {

        // allocate magic cookie
        magic_set *const magic = magic_open(MAGIC_MIME_TYPE);
        if (magic == nullptr) {
            log_error << "Could not open libmagic";
        }

        // load the default magic database (indicated by nullptr)
        if (magic_load(magic, nullptr) != 0) {
            log_error << "Could not load libmagic";
        }

        // compile the default magic database (indicated by nullptr)
        if (magic_compile(magic, nullptr) != 0) {
            log_error << "Could not compile libmagic";
        }

        // get description of the filename argument
        const char *mime = magic_buffer(magic, content.data(), content.size());
        if (mime == nullptr) {
            log_error << "Could not get mime type";
            mime = "application/octet-stream";
        } else {
            log_debug << "Found content-type: " << mime;
        }
        std::string result = {mime};

        // Free magic cookie and mime
        magic_close(magic);

        return result;
    }

    void FileUtils::StripChunkSignature(const std::string &path) {

        std::string line;
        std::ifstream fin;
        std::string tempFile = GetTempFile("bin");

        fin.open(path);

        // Contents of path must be copied to a temp file then renamed back to the path file
        std::ofstream temp;
        temp.open(tempFile);

        // Write all lines to temp other than the line marked for erasing
        while (getline(fin, line)) {
            if (!StringUtils::ContainsIgnoreCase(line, "chunk-signature")) {
                temp << line;
            }
        }

        temp.close();
        fin.close();

        // Required conversion for remove and rename functions
        DeleteFile(path);
        CopyTo(tempFile, path);
        DeleteFile(tempFile);
    }

}// namespace AwsMock::Core