//
// Created by vogje01 on 18/05/2023.
//

#include <awsmock/core/TarUtils.h>

namespace AwsMock::Core {

    void TarUtils::Unzip(const std::string &zipFile, const std::string &directory) {
        archive_entry *entry;
        int r;

        /* Select which attributes we want to restore. */
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
            log_error << "Could not open ZIP file, path: " << zipFile;
            return;
        }
        for (;;) {
            r = archive_read_next_header(a, &entry);
            if (r == ARCHIVE_EOF)
                break;
            if (r < ARCHIVE_OK)
                fprintf(stderr, "%s\n", archive_error_string(a));
            if (r < ARCHIVE_WARN)
                exit(1);
            archive_entry_set_pathname(entry, (directory + "/" + archive_entry_pathname(entry)).c_str());
            r = archive_write_header(ext, entry);
            if (r < ARCHIVE_OK)
                fprintf(stderr, "%s\n", archive_error_string(ext));
            else if (archive_entry_size(entry) > 0) {
                r = CopyData(a, ext);
                if (r < ARCHIVE_OK)
                    fprintf(stderr, "%s\n", archive_error_string(ext));
                if (r < ARCHIVE_WARN)
                    exit(1);
            }
            r = archive_write_finish_entry(ext);
            if (r < ARCHIVE_OK)
                fprintf(stderr, "%s\n", archive_error_string(ext));
            if (r < ARCHIVE_WARN)
                exit(1);
        }
        archive_read_close(a);
        archive_read_free(a);
        archive_write_close(ext);
        archive_write_free(ext);
    }

    void TarUtils::TarDirectory(const std::string &tarFile, const std::string &directory) {

        archive *a = archive_write_new();
        archive_write_add_filter_gzip(a);
        archive_write_set_format_gnutar(a);
        archive_write_open_filename(a, tarFile.c_str());

        boost::filesystem::recursive_directory_iterator dir(directory);
        const boost::filesystem::recursive_directory_iterator end;
        int count = 0;
        while (dir != end) {
            if (dir->path() != tarFile) {
                WriteFile(a, dir->path().string(), directory, dir->is_directory(), dir->is_symlink());
                count++;
            }
            ++dir;
        }
        archive_write_close(a);
        archive_write_free(a);
    }

    void TarUtils::WriteFile(archive *archive, const std::string &fileName, const std::string &removeDir, const bool isDir, const bool isLink) {

        struct stat st{};
        char buff[8192];

        std::string entryName = fileName;
        StringUtils::Replace(entryName, removeDir, "");
        log_trace << "Removed directory, name: " << entryName;

        stat(fileName.c_str(), &st);
        archive_entry *entry = archive_entry_new();// Note 2
        archive_entry_set_pathname(entry, entryName.c_str());
        archive_entry_set_size(entry, st.st_size);
        std::string link;
        if (isDir) {
            archive_entry_set_filetype(entry, S_IFDIR);
            archive_entry_set_perm(entry, 0755);
            archive_entry_set_uid(entry, 0);
            archive_entry_set_gid(entry, 0);
        } else if (isLink) {
            link = Readsymlink(fileName);
            archive_entry_set_filetype(entry, AE_IFLNK);
            archive_entry_set_symlink(entry, link.c_str());
            archive_entry_set_perm(entry, 0644);
            archive_entry_set_uid(entry, 0);
            archive_entry_set_gid(entry, 0);
        } else {
            archive_entry_set_filetype(entry, S_IFREG);
            archive_entry_set_perm(entry, 0644);
            archive_entry_set_uid(entry, 0);
            archive_entry_set_gid(entry, 0);
        }
        archive_write_header(archive, entry);
        const int fd = open(fileName.c_str(), O_RDONLY);
        long len = read(fd, buff, sizeof(buff));
        while (len > 0) {
            archive_write_data(archive, buff, len);
            len = read(fd, buff, sizeof(buff));
            log_trace << "File written to archive, name: " << entryName;
        }
        close(fd);
        archive_entry_free(entry);
    }

    std::string TarUtils::Readsymlink(const std::string &path) {
        size_t len;
#ifdef WIN32
        // TODO: Fix windows porting issues
#else
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
            r = archive_write_data_block(aw, buff, size, offset);
            if (r < ARCHIVE_OK) {
                fprintf(stderr, "%s\n", archive_error_string(aw));
                return (r);
            }
        }
    }
}// namespace AwsMock::Core
