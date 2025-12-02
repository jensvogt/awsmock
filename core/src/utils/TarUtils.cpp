//
// Created by vogje01 on 18/05/2023.
//

#include <awsmock/core/TarUtils.h>

namespace AwsMock::Core {

#ifdef _WIN32

    void TarUtils::TarDirectory(const std::string &tarFile, const std::string &directory) {

        const std::wstring output_filename_w = StringUtils::ConvertToWideString(tarFile);
        const std::wstring directory_w = StringUtils::ConvertToWideString(directory);

        // Create a new archive write handle
        archive *a = archive_write_new();
        if (!a) {
            log_error << "Failed to create archive handle.";
            return;
        }

        // Set the format to PAX (POSIX.1-2001/ustar extension), which automatically handles
        // long file names and large files. This is the key for long path support.
        int r = archive_write_set_format_pax(a);
        if (r != ARCHIVE_OK) {
            log_error << "Failed to set PAX format: " << archive_error_string(a);
            archive_write_free(a);
            return;
        }

        // Use archive_write_open_filename_w to handle the wide-character output path on Windows
        // This allows the output file itself to potentially exceed MAX_PATH.
        r = archive_write_open_filename_w(a, output_filename_w.c_str());
        if (r != ARCHIVE_OK) {
            log_error << "Failed to open output file: " << archive_error_string(a);
            archive_write_free(a);
            return;
        }

        log_debug << "Creating archive: " << output_filename_w;

        const boost::filesystem::recursive_directory_iterator dir(directory);
        const boost::filesystem::recursive_directory_iterator end;
        int count = 0;

        try {

            // Use recursive_directory_iterator for deep traversal
            for (const auto &entry: std::filesystem::recursive_directory_iterator(directory_w, std::filesystem::directory_options::skip_permission_denied)) {

                // Skip tar file
                if (StringUtils::EqualsIgnoreCase(tarFile, entry.path().string())) {
                    continue;
                }

                // Get paths
                std::wstring current_path_w = FileUtils::NormalizePathForLongPaths(entry.path().wstring());

                // Get the path relative to the root for the TAR entry name
                std::wstring relative_path_w = std::filesystem::relative(entry.path(), directory_w).wstring();

                // Skip the directory '.' itself
                if (relative_path_w.empty() || relative_path_w == L".") continue;

                // Stat the file using the wide-character stat function
                struct _stat64 st{};
                if (const int retStat = _wstat64(current_path_w.c_str(), &st); retStat != 0) {
                    log_warning << "Failed to stat file: " << current_path_w << ", error: " << retStat << ". Skipping.";
                    continue;
                }

                archive_entry *tar_entry = archive_entry_new();
                if (!tar_entry) {
                    log_error << "Failed to create archive entry.";
                    continue;
                }

                // Set the wide path name in the entry
                archive_entry_copy_pathname_w(tar_entry, relative_path_w.c_str());

                // Set metadata from stat
                archive_entry_set_mode(tar_entry, st.st_mode);
                archive_entry_set_size(tar_entry, st.st_size);
                archive_entry_set_mtime(tar_entry, st.st_mtime, 0);

                // Add entry to the archive
                r = archive_write_header(a, tar_entry);
                if (r != ARCHIVE_OK) {
                    log_warning << "Warning: Skipping entry due to header write error: " << archive_error_string(a);
                    archive_entry_free(tar_entry);
                    continue;
                }
                count++;

                // For regular files, stream the data
                if (entry.is_regular_file()) {
                    FILE *fd = nullptr;

                    if (const errno_t err = _wfopen_s(&fd, current_path_w.c_str(), L"rb"); err == 0 && fd) {
                        char buff[8192];
                        size_t bytes_read;

                        while ((bytes_read = fread(buff, 1, sizeof(buff), fd)) > 0) {
                            r = static_cast<int>(archive_write_data(a, buff, bytes_read));
                            if (r < ARCHIVE_OK) {
                                log_error << "Failed to write data to archive: " << archive_error_string(a);
                                break;
                            }
                        }
                        fclose(fd);
                    } else {
                        log_warning << "Failed to open file for read: " << current_path_w << ". Skipping data.";
                    }
                }
                archive_entry_free(tar_entry);
            }
        } catch (const std::filesystem::filesystem_error &ex) {
            // Catch critical errors like directory access failures
            log_error << "Filesystem error during traversal: " << ex.what();
            archive_write_close(a);
            archive_write_free(a);
            return;
        }

        // Cleanup
        r = archive_write_close(a);
        if (r != ARCHIVE_OK) {
            log_error << "Failed to close archive: " << archive_error_string(a);
            archive_write_free(a);
            return;
        }
        archive_write_free(a);
        log_info << "Successfully archived, name: " << tarFile << ", count: " << count;
    }

#else

    void TarUtils::TarDirectory(const std::string &tarFile, const std::string &directory) {

        int err = 0;
        log_trace << "Create gzipped tarfile, tarFile: " << tarFile << ", directory: " << directory;
        archive *a = archive_write_new();
        // Windows does not support compression
        archive_write_add_filter_gzip(a);
        archive_write_set_format_gnutar(a);
        err = archive_write_open_filename(a, tarFile.c_str());
        if (err != ARCHIVE_OK) {
            log_error << "Could not open tar file, path: " << tarFile << ", directory: " << directory << ", error: " << archive_error_string(a);
            return;
        }

        boost::filesystem::recursive_directory_iterator dir(directory);
        const boost::filesystem::recursive_directory_iterator end;
        int count = 0;
        while (dir != end) {
            if (dir->path() != tarFile) {
                std::string filename = dir->path().string();
                WriteFile(a, filename, directory, dir->is_directory(), dir->is_symlink());
                count++;
            }
            ++dir;
        }
        archive_write_close(a);
        archive_write_free(a);
    }

    void TarUtils::WriteFile(archive *archive, std::string &fileName, const std::string &directory, const bool isDir, const bool isLink) {

        int err = 0;
        std::string entryName = fileName;
        StringUtils::Replace(entryName, directory, "");
        log_trace << "Removed directory, entryName: " << entryName << ", filename: " << fileName << ", directory: " << directory;

        struct stat st{};
        stat(fileName.c_str(), &st);
        archive_entry *entry = archive_entry_new();
        archive_entry_set_pathname(entry, entryName.c_str());
        archive_entry_set_size(entry, st.st_size);
        archive_entry_copy_stat(entry, &st);
        std::string link;
        if (isDir) {
            archive_entry_set_filetype(entry, S_IFDIR);
            archive_entry_set_perm(entry, 0755);
            archive_entry_set_uid(entry, 0);
            archive_entry_set_gid(entry, 0);
            log_trace << "Is directory";
        } else if (isLink) {
            link = Readsymlink(fileName);
            archive_entry_set_filetype(entry, AE_IFLNK);
            archive_entry_set_symlink(entry, link.c_str());
            archive_entry_set_perm(entry, 0644);
            archive_entry_set_uid(entry, 0);
            archive_entry_set_gid(entry, 0);
            log_trace << "Is link";
        } else {
            archive_entry_set_filetype(entry, AE_IFREG);
            archive_entry_set_perm(entry, 0755);
            archive_entry_set_uid(entry, 0);
            archive_entry_set_gid(entry, 0);
            log_trace << "Is regular file";
        }
        err = archive_write_header(archive, entry);
        if (err != ARCHIVE_OK) {
            log_error << "Could not write header, filename: " << fileName << ", error: " << archive_error_string(archive);
            return;
        }
        log_trace << "Wrote header";
#ifdef _WIN32
        const int fd = open(fileName.c_str(), O_RDONLY | O_BINARY);
#else
        const int fd = open(fileName.c_str(), O_RDONLY);
#endif
        if (fd >= 0) {
            char buff[8192];
            long len = read(fd, buff, sizeof(buff));
            while (len > 0) {
                archive_write_data(archive, buff, len);
                len = read(fd, buff, sizeof(buff));
                log_trace << "File written to archive, name: " << entryName;
            }
            close(fd);
        } else {
            log_error << "Cannot open file: " << fileName;
        }
        log_trace << "Finished writing file: " << fileName;
        archive_entry_free(entry);
    }

    std::string TarUtils::Readsymlink(const std::string &path) {
        size_t len;
        if (char buf[1024]; (len = readlink(path.c_str(), buf, sizeof(buf) - 1)) != -1) {
            buf[len] = '\0';
            return {buf};
        }
        return "";
    }

    int TarUtils::CopyData(archive *ar, archive *aw) {
        const void *buff;
        size_t size;
        la_int64_t offset;

        for (;;) {
            int r = archive_read_data_block(ar, &buff, &size, &offset);
            if (r == ARCHIVE_EOF)
                return (ARCHIVE_OK);
            if (r < ARCHIVE_OK)
                return (r);
            r = static_cast<int>(archive_write_data_block(aw, buff, size, offset));
            if (r < ARCHIVE_OK) {
                log_error << archive_error_string(aw);
                return (r);
            }
        }
    }
#endif

}// namespace AwsMock::Core
