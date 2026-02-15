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
        return temp.string() + "." + extension;
    }

    std::string FileUtils::GetTempFile(const std::string &dir, const std::string &extension) {
        DirUtils::EnsureDirectory(dir);
        const boost::filesystem::path temp = boost::filesystem::unique_path();
        return dir + separator() + temp.string() + "." + extension;
    }

    std::string FileUtils::GetParentPath(const std::string &fileName) {
        const std::filesystem::path path(fileName);
        return path.parent_path().string();
    }

    std::string FileUtils::GetFirstLine(const std::string &filePath) {
        std::ifstream fileHandler(filePath);
        std::string line;
        getline(fileHandler, line);
        fileHandler.close();
        return line;
    }

    void FileUtils::RemoveFirstBytes(const std::string &filePath, const int skip) {
        const std::string tmpFile = GetTempFile("tmp");
        std::ofstream ofs(tmpFile);
        std::ifstream ifs(filePath);
        ifs.seekg(skip, std::ios::beg);
        ofs << ifs.rdbuf();
        ifs.close();
        ofs.close();
        RemoveFile(filePath);
        MoveTo(tmpFile, filePath);
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
        copy_file(boost::filesystem::path(sourceFileName), boost::filesystem::path(targetFileName), boost::filesystem::copy_options::overwrite_existing);
    }

    long FileUtils::AppendBinaryFiles(const std::string &outFile, const std::string &inDir, const std::vector<std::string> &files) {

        size_t copied = 0;

#if __APPLE__
        const int dest = open(outFile.c_str(), O_WRONLY | O_CREAT, 0644);
        for (auto &it: files) {
            const int source = open(it.c_str(), O_RDONLY, 0);
            struct stat stat_source{};
            fstat(source, &stat_source);
            copied += sendfile(dest, source, 0, &stat_source.st_size, nullptr, 0);

            close(source);
        }
        close(dest);
#elif __linux__
        const int dest = open(outFile.c_str(), O_WRONLY | O_CREAT, 0644);
        for (auto &it: files) {
            const int source = open(it.c_str(), O_RDONLY, 0);
            struct stat stat_source{};
            fstat(source, &stat_source);
            copied += sendfile(dest, source, nullptr, stat_source.st_size);

            close(source);
        }
        close(dest);
#else
        FILE *dest = fopen(outFile.c_str(), "wb");
        for (auto &it: files) {
            char buffer[BUFFER_LEN];
            FILE *src = fopen(it.c_str(), "rb");

            size_t n;
            while ((n = fread(buffer, 1, BUFFER_LEN, src)) > 0) {
                fwrite(buffer, 1, n, dest);
                copied += n;
            }

            fclose(src);
        }
        fclose(dest);
#endif
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

    /*bool FileUtils::FileExists(const std::string &fileName) {
        return std::filesystem::exists(fileName) && std::filesystem::is_regular_file(fileName);
    }*/

    std::string FileUtils::StripBasePath(const std::string &fileName) {
        return GetBasename(fileName) + "." + GetExtension(fileName);
    }

    std::string FileUtils::GetOwner(const std::string &fileName) {

#ifdef WIN32
        DWORD dwRtnCode = 0;
        PSID pSidOwner = nullptr;
        BOOL bRtnBool = TRUE;
        LPTSTR AcctName = nullptr;
        LPTSTR DomainName = nullptr;
        DWORD dwAcctName = 1, dwDomainName = 1;
        SID_NAME_USE eUse = SidTypeUnknown;
        PSECURITY_DESCRIPTOR pSD = nullptr;


        // Get the handle of the file object.
        const HANDLE hFile = CreateFile(TEXT(fileName.c_str()), GENERIC_READ, FILE_SHARE_READ, nullptr,
                                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

        // Check GetLastError for CreateFile error code.
        if (hFile == INVALID_HANDLE_VALUE) {
            DWORD dwErrorCode = 0;

            dwErrorCode = GetLastError();
            log_error << "CreateFile error: " << dwErrorCode;
            return {};
        }

        // Get the owner SID of the file.
        dwRtnCode = GetSecurityInfo(hFile, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, &pSidOwner, nullptr, nullptr, nullptr, &pSD);

        // Check GetLastError for GetSecurityInfo error condition.
        if (dwRtnCode != ERROR_SUCCESS) {
            DWORD dwErrorCode = 0;

            dwErrorCode = GetLastError();
            log_error << "GetSecurityInfo error: " << dwErrorCode;
            return {};
        }

        // First call to LookupAccountSid to get the buffer sizes.
        bRtnBool = LookupAccountSid(nullptr, pSidOwner, AcctName, (LPDWORD) &dwAcctName, DomainName, (LPDWORD) &dwDomainName, &eUse);

        // Reallocate memory for the buffers.
        AcctName = static_cast<LPTSTR>(GlobalAlloc(GMEM_FIXED, dwAcctName));

        // Check GetLastError for GlobalAlloc error condition.
        if (AcctName == nullptr) {
            DWORD dwErrorCode = 0;

            dwErrorCode = GetLastError();
            log_error << "GlobalAlloc error: " << dwErrorCode;
            return {};
        }

        DomainName = static_cast<LPTSTR>(GlobalAlloc(GMEM_FIXED, dwDomainName));

        // Check GetLastError for GlobalAlloc error condition.
        if (DomainName == nullptr) {
            DWORD dwErrorCode = 0;

            dwErrorCode = GetLastError();
            log_error << "GlobalAlloc error: " << dwErrorCode;
            return {};
        }

        // Second call to LookupAccountSid to get the account name.
        bRtnBool = LookupAccountSid(nullptr, pSidOwner, AcctName, &dwAcctName, DomainName, &dwDomainName, &eUse);

        // Check GetLastError for LookupAccountSid error condition.
        if (bRtnBool == FALSE) {
            DWORD dwErrorCode = 0;

            dwErrorCode = GetLastError();

            if (dwErrorCode == ERROR_NONE_MAPPED) {
                _tprintf(TEXT("Account owner not found for specified SID.\n"));
            } else {
                _tprintf(TEXT("Error in LookupAccountSid.\n"));
            }
            return {};
        }
        if (bRtnBool == TRUE) {

            // Print the account name.
            log_debug << "Account owner: " << AcctName;
            return AcctName;
        }
#else
        struct stat info{};
        stat(fileName.c_str(), &info);
        if (const passwd *pw = getpwuid(info.st_uid)) {
            return pw->pw_name;
        }
#endif
        return {};
    }

    void FileUtils::RemoveFile(const std::string &fileName) {
        if (fileName.empty()) {
            log_error << "Empty filename";
            return;
        }
        if (!std::filesystem::exists(fileName)) {
            log_warning << "File does not exist: " << fileName;
            return;
        }
        std::filesystem::remove(fileName);
    }

    bool FileUtils::Touch(const std::string &fileName) {
#ifdef WIN32
        // TODO: Fix windows port
        return true;
#else
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
#endif
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

    std::string FileUtils::GetContentType(const std::string &path, const std::string &realPath) {
        if (const std::string extension = boost::filesystem::path(realPath).extension().string(); !extension.empty() && MimeTypes.contains(extension)) {
            return MimeTypes.at(extension);
        }
        return GetContentTypeMagicFile(path);
    }

    std::string FileUtils::GetContentTypeMagicFile(const std::string &path) {

        if (!FileExists(path)) {
            log_error << "Target path not found, path: " << path;
            return DEFAULT_MIME_TYPE;
        }

        const auto magicFile = Configuration::instance().GetValue<std::string>("awsmock.magic-file");

        if (!FileExists(magicFile)) {
            log_error << "Magic database not found, path: " << magicFile;
            return DEFAULT_MIME_TYPE;
        }

        // Allocate magic cookie
        magic_set *const magic = magic_open(MAGIC_MIME_TYPE);
        if (magic == nullptr) {
            log_error << "Could not open libmagic";
            return DEFAULT_MIME_TYPE;
        }

        // Load the default magic database (indicated by nullptr)
        if (magic_load(magic, magicFile.c_str()) != 0) {
            log_error << "Could not load libmagic mime types, fileName: " << magicFile << ", error: " << magic_error(magic);
            return DEFAULT_MIME_TYPE;
        }

        // Get a description of the filename argument
        const char *mime = magic_file(magic, path.c_str());
        if (mime == nullptr) {
            log_error << "Could not get mime type";
            mime = DEFAULT_MIME_TYPE;
        } else {
            log_debug << "Found content-type: " << mime;
        }
        std::string result = {mime};

        // Free magic cookie and mime
        magic_close(magic);
        return result;
    }

    std::string FileUtils::GetContentTypeMagicString(const std::string &content) {

        if (content.empty()) {
            return DEFAULT_MIME_TYPE;
        }

        const auto magicFile = Configuration::instance().GetValue<std::string>("awsmock.magic-file");

        // Allocate magic cookie
        magic_set *const magic = magic_open(MAGIC_MIME_TYPE);
        if (magic == nullptr) {
            log_error << "Could not open libmagic";
            return DEFAULT_MIME_TYPE;
        }

        // Load the default magic database (indicated by nullptr)
        if (magic_load(magic, magicFile.c_str()) != 0) {
            log_error << "Could not load libmagic mime types, fileName: " << magicFile;
            return DEFAULT_MIME_TYPE;
        }

        // Get the description of the filename argument
        const char *mime = magic_buffer(magic, content.data(), content.size());
        if (mime == nullptr) {
            log_error << "Could not get mime type";
            mime = DEFAULT_MIME_TYPE;
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
        RemoveFile(path);
        CopyTo(tempFile, path);
        RemoveFile(tempFile);
    }

    bool FileUtils::IsBase64(const std::string &filePath) {
        std::fstream s(filePath);
        char buff[1000]{};
        s.read(buff, 1000);
        return Crypto::IsBase64({buff, 1000});
    }

    void FileUtils::Base64DecodeFile(const std::string &filePath) {
        std::ifstream in(filePath);
        std::string line;
        in >> line;
        RemoveFile(filePath);

        std::string decoded = Crypto::Base64Decode({line});

        std::string outFilepath = GetTempFile("b64");
        std::ofstream out(outFilepath, std::ios::binary);
        out << decoded;
        out.close();
        MoveTo(outFilepath, filePath);
    }

    void FileUtils::RemoveLastBytes(const std::string &filename, const long size) {
        const std::filesystem::path p(filename);
        std::filesystem::resize_file(p, std::filesystem::file_size(p) - size);
    }

    long FileUtils::StreamCopier(const std::string &inputFile, const std::string &outputFile) {
        std::ifstream ifs(inputFile, std::ios::binary);
        std::ofstream ofs(outputFile, std::ios::binary);
        long count = StreamCopier(ifs, ofs);
        ifs.close();
        ofs.close();
        return count;
    }

    long FileUtils::StreamCopier(const std::string &inputFile, const std::string &outputFile, long start, long length) {
        std::ifstream ifs(inputFile, std::ios::binary);
        std::ofstream ofs(outputFile, std::ios::binary);
        long count = StreamCopier(ifs, ofs, start, length);
        ifs.close();
        ofs.close();
        return count;
    }

    long FileUtils::StreamCopier(const std::string &inputFile, const std::string &outputFile, long count) {
        std::ifstream ifs(inputFile, std::ios::binary);
        std::ofstream ofs(outputFile, std::ios::binary);
        long copied = StreamCopier(ifs, ofs, count);
        ifs.close();
        ofs.close();
        return copied;
    }

    long FileUtils::StreamCopier(std::istream &is, std::ostream &os) {
        is.seekg(0, std::ios::end);
        const long count = is.tellg();
        is.seekg(0, std::ios::beg);
        return StreamCopier(is, os, count);
    }

    long FileUtils::StreamCopier(std::istream &istream, std::ostream &ostream, long count) {
        long copied = 0;
        char buffer[BUFFER_LEN];
        if (count < BUFFER_LEN) {
            istream.read(buffer, count);
            ostream.write(buffer, count);
            copied = count;
        } else {
            while (count > BUFFER_LEN) {
                istream.read(buffer, BUFFER_LEN);
                ostream.write(buffer, BUFFER_LEN);
                count -= BUFFER_LEN;
                copied += BUFFER_LEN;
            }
            istream.read(buffer, count);
            ostream.write(buffer, count);
            copied += count;
        }
        return copied;
    }

    long FileUtils::StreamCopier(std::istream &istream, std::ostream &ostream, const long start, long count) {
        long copied = 0;
        istream.seekg(start, std::ios::beg);
        char buffer[BUFFER_LEN];
        if (count < BUFFER_LEN) {
            istream.read(buffer, count);
            ostream.write(buffer, count);
            copied = count;
        } else {
            while (count > BUFFER_LEN) {
                istream.read(buffer, BUFFER_LEN);
                ostream.write(buffer, BUFFER_LEN);
                count -= BUFFER_LEN;
                copied += BUFFER_LEN;
            }
            istream.read(buffer, count);
            ostream.write(buffer, count);
            copied += count;
        }
        return copied;
    }


#ifdef _WIN32
    std::wstring FileUtils::NormalizePathForLongPaths(const std::wstring &path) {
        // The prefix must be applied only to absolute paths.
        // If the path already starts with the prefix, do nothing.
        if (path.rfind(L"\\\\?\\", 0) == 0) {
            return path;
        }

        // Convert forward slashes to backslashes as required by the \\?\ prefix
        std::wstring normalizedPath = path;
        std::ranges::replace(normalizedPath, L'/', L'\\');

        // Add the prefix for long path support
        // For example: C:\Folder\file.txt becomes \\?\C:\Folder\file.txt
        if (normalizedPath.size() > 2 && normalizedPath[1] == L':' && normalizedPath[2] == L'\\') {
            return L"\\\\?\\" + normalizedPath;
        }

        // UNC paths (\\server\share\) become \\?\UNC\server\share
        if (normalizedPath.size() > 2 && normalizedPath.rfind(L"\\\\", 0) == 0) {
            return L"\\\\?\\UNC" + normalizedPath.substr(1);
        }

        // For simplicity, we only handle absolute drive paths and UNC paths here.
        return normalizedPath;
    }
#endif

}// namespace AwsMock::Core