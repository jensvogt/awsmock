//
// Created by vogje01 on 18/05/2023.
//

#include <awsmock/core/TarUtils.h>

namespace AwsMock::Core {

    void TarUtils::Unzip(const std::string &zipFile, const std::string &directory) {

        log_trace << "Unzipping started, zipFile: " << zipFile << ", directory: " << directory;
        archive_entry *entry;
        int r;

        // Select which attributes we want to restore.
        int flags = ARCHIVE_EXTRACT_TIME;
        flags |= ARCHIVE_EXTRACT_PERM;
        flags |= ARCHIVE_EXTRACT_ACL;
        flags |= ARCHIVE_EXTRACT_FFLAGS;

        archive *a = archive_read_new();
        archive_read_support_format_all(a);
        archive_read_support_filter_all(a);
        archive *ext = archive_write_disk_new();
        archive_write_disk_set_options(ext, flags);
        archive_write_disk_set_standard_lookup(ext);
        if ((r = archive_read_open_filename(a, zipFile.c_str(), 10240)) != 0) {
            log_error << "Could not open ZIP file, path: " << zipFile << ", directory: " << directory;
            return;
        }
        for (;;) {
            r = archive_read_next_header(a, &entry);
            if (r == ARCHIVE_EOF) {
                break;
            }
            if (r < ARCHIVE_OK) {
                log_error << "Read error, filename: " << zipFile << ", directory: " << directory << ", error: " << archive_error_string(a);
            }
            if (r < ARCHIVE_WARN) {
                log_error << archive_error_string(a);
                return;
            }
            archive_entry_set_pathname(entry, (directory + "/" + archive_entry_pathname(entry)).c_str());
            r = archive_write_header(ext, entry);
            if (r < ARCHIVE_OK) {
                log_error << archive_error_string(ext);
            } else if (archive_entry_size(entry) > 0) {
                r = CopyData(a, ext);
                if (r < ARCHIVE_OK) {
                    log_error << archive_error_string(ext);
                }
                if (r < ARCHIVE_WARN) {
                    return;
                }
            }
            r = archive_write_finish_entry(ext);
            if (r < ARCHIVE_OK) {
                log_error << archive_error_string(ext);
            }
            if (r < ARCHIVE_WARN) {
                return;
            }
        }
        archive_read_close(a);
        archive_read_free(a);
        archive_write_close(ext);
        archive_write_free(ext);
    }

    void TarUtils::TarDirectory(const std::string &tarFile, const std::string &directory) {

        log_trace << "Create gzipped tarfile, tarFile: " << tarFile << ", directory: " << directory;
        archive *a = archive_write_new();
        archive_write_add_filter_gzip(a);
        archive_write_set_format_gnutar(a);
        archive_write_open_filename(a, tarFile.c_str());

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

        std::string entryName = fileName;
        StringUtils::Replace(entryName, directory, "");
        log_trace << "Removed directory, entryName: " << entryName << ", filename: " << fileName << ", directory: " << directory;

        struct stat st {};
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
            archive_entry_set_filetype(entry, S_IFREG);
            archive_entry_set_perm(entry, 0644);
            archive_entry_set_uid(entry, 0);
            archive_entry_set_gid(entry, 0);
            log_trace << "Is regular file";
        }
        archive_write_header(archive, entry);
        log_trace << "Wrote header";

#ifdef WIN32
        if (!isDir && !isLink) {
            int fd;
            if (fileName.length() > MAX_PATH) {
                fileName = PrepareFilename(fileName);
            }
            if (const int rc = _sopen_s(&fd, fileName.c_str(), _O_RDONLY | _O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE)) {
                log_error << "Could not open file for reading, filename: " << fileName << ", error: " << strerror(rc);
            } else if (fd >= 0) {
                unsigned char buff[8192];
                long len = _read(fd, buff, sizeof(buff));
                while (len > 0) {
                    archive_write_data(archive, buff, len);
                    len = _read(fd, buff, sizeof(buff));
                    log_trace << "File written to archive, name: " << entryName;
                }
                _close(fd);
            } else {
                log_error << "Cannot open file: " << fileName;
            }
        }
#else
        const int fd = open(fileName.c_str(), O_RDONLY);
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
#endif
        log_trace << "Finished writing file: " << fileName;
        archive_entry_free(entry);
    }

    std::string TarUtils::Readsymlink(const std::string &path) {
        size_t len;
#ifndef WIN32
        if (char buf[1024]; (len = readlink(path.c_str(), buf, sizeof(buf) - 1)) != -1) {
            buf[len] = '\0';
            return {buf};
        }
#endif
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

#ifdef _WIN32
    std::string TarUtils::PrepareFilename(std::string &filename) {
        StringUtils::Replace(filename, "/", "\\");
        return "\\\\?\\" + filename;
    }

#endif
}// namespace AwsMock::Core
