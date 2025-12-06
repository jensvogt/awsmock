//
// Created by vogje01 on 18/05/2023.
//

#include <awsmock/core/ZipUtils.h>

namespace AwsMock::Core {

#ifdef _WIN32
    void ZipUtils::Unzip(const std::string &zipFile, const std::string &directory) {

        archive_entry *entry;
        constexpr int flags = ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS | ARCHIVE_EXTRACT_OWNER;

        // Normalize paths before passing them to the Windows subsystem
        const std::wstring archivePathW = StringUtils::ConvertToWideString(zipFile);
        const std::wstring outputPathW = StringUtils::ConvertToWideString(directory);
        const std::wstring normalizedArchivePath = FileUtils::NormalizePathForLongPaths(archivePathW);
        const std::wstring normalizedOutputPath = FileUtils::NormalizePathForLongPaths(outputPathW);

        archive *a = archive_read_new();
        if (a == nullptr) {
            log_error << "Libarchive Error: archive_read_new failed.";
            return;
        }

        archive_read_support_filter_all(a);
        archive_read_support_format_all(a);

        archive *ext = archive_write_disk_new();
        archive_write_disk_set_options(ext, flags);

        // Use the wide-character open function
        int r = archive_read_open_filename_w(a, normalizedArchivePath.c_str(), BUFSIZ);
        if (r != ARCHIVE_OK) {
            log_error << "Libarchive Error: Cannot open file: " << archive_error_string(a);
            archive_read_free(a);
            archive_write_free(ext);
            return;
        }

        // Loop through all entries in the archive
        while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
            // Get the entry path as a wide string
            const wchar_t *entryPath = archive_entry_pathname_w(entry);

            // Construct the full output path using wide strings
            std::wstringstream fullPathStream;
            fullPathStream << normalizedOutputPath << L"\\" << entryPath;
            std::wstring fullPathW = fullPathStream.str();

            // Convert the wide-character long path back to a multi-byte string for the ANSI entry setter.
            // Libarchive's disk writer (if compiled with long-path support) should handle the \\?\ prefix
            // even if set via the ANSI API, as the wide-char open was used.
            std::string fullPathA = StringUtils::ConvertToNarrowString(fullPathW);
            archive_entry_set_pathname(entry, fullPathA.c_str());
            archive_entry_set_perm(entry, 0777);
            archive_entry_set_uid(entry, 0);
            archive_entry_set_gid(entry, 0);

            r = archive_write_header(ext, entry);
            if (r == ARCHIVE_OK) {
                r = CopyData(a, ext);// Copy file data
                if (r != ARCHIVE_OK) {
                    archive_write_finish_entry(ext);
                    break;
                }
            } else if (r == ARCHIVE_FATAL) {
                log_error << "Libarchive Fatal Error writing header: " << archive_error_string(ext);
                break;
            }

            r = archive_write_finish_entry(ext);
            if (r != ARCHIVE_OK) {
                log_error << "Libarchive Error finishing entry: " << archive_error_string(ext);
                break;
            }
        }

        // Clean up
        archive_read_close(a);
        archive_read_free(a);
        archive_write_close(ext);
        archive_write_free(ext);
    }

#else
    void ZipUtils::Unzip(const std::string &zipFile, const std::string &directory) {

        log_trace << "Unzipping started, zipFile: " << zipFile << ", directory: " << directory;
        archive_entry *entry;
        int r;

        // Select which attributes we want to restore.
        constexpr int flags = ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS;

        archive *a = archive_read_new();

        archive_read_support_filter_all(a);
        archive_read_support_format_all(a);

        archive *ext = archive_write_disk_new();
        archive_write_disk_set_options(ext, flags);
        archive_write_disk_set_standard_lookup(ext);
        if ((r = archive_read_open_filename(a, zipFile.c_str(), BUFSIZ)) != 0) {
            log_error << "Could not open ZIP file, path: " << zipFile << ", directory: " << directory;
            return;
        }
        // Loop through all entries in the archive
        while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {

            // Prepend the output path to the entry's path
            std::string fullPath = directory + "/" + archive_entry_pathname(entry);
            archive_entry_set_pathname(entry, fullPath.c_str());

            r = archive_write_header(ext, entry);
            if (r == ARCHIVE_OK) {
                r = CopyData(a, ext);// Copy file data
                if (r != ARCHIVE_OK) {
                    // Error during data copy
                    archive_write_finish_entry(ext);
                    log_error << "Error during data copy: " << archive_error_string(a);
                    break;
                }
            } else if (r == ARCHIVE_FATAL) {
                // Non-recoverable error, stop extraction
                log_error << "Libarchive Fatal Error writing header: " << archive_error_string(ext);
                break;
            }

            r = archive_write_finish_entry(ext);
            if (r != ARCHIVE_OK) {
                log_error << "Libarchive Error finishing entry: " << archive_error_string(ext);
                break;
            }
        }
        archive_read_close(a);
        archive_read_free(a);
        archive_write_close(ext);
        archive_write_free(ext);
    }
#endif

    int ZipUtils::CopyData(archive *ar, archive *aw) {
        const void *buff;
        size_t size;
#if defined(_WIN32) || defined(BOOST_OS_MACOS)
        long long offset;

#else
        long long offset;
#endif

        for (;;) {
            // Read data block from the archive
            long r = archive_read_data_block(ar, &buff, &size, &offset);
            if (r == ARCHIVE_EOF)
                return ARCHIVE_OK;
            if (r != ARCHIVE_OK) {
                log_error << "Error reading data block, error: " << archive_error_string(ar);
                return r;
            }

            // Write data block to the disk
            r = archive_write_data(aw, buff, size);
            if (r < 0) {
                log_error << "Error writing data block, error: " << archive_error_string(ar);
                return r;
            }
        }
    }

}// namespace AwsMock::Core
