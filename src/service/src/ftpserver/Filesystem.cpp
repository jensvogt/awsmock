#include <awsmock/ftpserver/Filesystem.h>

#include <array>
#include <awsmock/core/LogStream.h>
#include <cstdint>
#include <cstring>
#include <ctime>
#ifndef WIN32
#include <dirent.h>
#endif
#include <boost/filesystem/directory.hpp>
#include <boost/filesystem/file_status.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <iomanip>
#include <iostream>
#include <list>
#include <mutex>
#include <regex>
#include <sstream>
#include <sys/stat.h>

////////////////////////////////////////////////////////////////////////////////
/// Filesystem
////////////////////////////////////////////////////////////////////////////////

namespace AwsMock::FtpServer {

    FileStatus::FileStatus(const std::string &path) : path_(path), file_status_{} {
        const int error_code = stat(path.c_str(), &file_status_);
        if (error_code) {
            //log_error << "Cannot stat, file: " << path_ << " error: " << error_code;
        }
        is_ok_ = (error_code == 0);
    }

    bool FileStatus::isOk() const {
        return is_ok_;
    }

    FileType FileStatus::type() const {
        if (!is_ok_)
            return FileType::Unknown;

        switch (file_status_.st_mode & S_IFMT) {
            case S_IFREG:
                return FileType::RegularFile;
            case S_IFDIR:
                return FileType::Dir;
            case S_IFCHR:
                return FileType::CharacterDevice;
#ifndef _WIN32
            case S_IFBLK:
                return FileType::BlockDevice;
            case S_IFIFO:
                return FileType::Fifo;
            case S_IFLNK:
                return FileType::SymbolicLink;
            case S_IFSOCK:
                return FileType::Socket;
#endif
            default:
                return FileType::Unknown;
        }
    }

    int64_t FileStatus::fileSize() const {
        if (!is_ok_)
            return 0;

        return file_status_.st_size;
    }
    bool FileStatus::permissionRootRead() const { return 0 != (file_status_.st_mode & boost::filesystem::owner_read); }
    bool FileStatus::permissionRootWrite() const { return 0 != (file_status_.st_mode & boost::filesystem::owner_write); }
    bool FileStatus::permissionRootExecute() const { return 0 != (file_status_.st_mode & boost::filesystem::owner_exe); }
    bool FileStatus::permissionGroupRead() const { return 0 != (file_status_.st_mode & boost::filesystem::group_read); }
    bool FileStatus::permissionGroupWrite() const { return 0 != (file_status_.st_mode & boost::filesystem::group_write); }
    bool FileStatus::permissionGroupExecute() const { return 0 != (file_status_.st_mode & boost::filesystem::group_exe); }
    bool FileStatus::permissionOwnerRead() const { return 0 != (file_status_.st_mode & boost::filesystem::owner_read); }
    bool FileStatus::permissionOwnerWrite() const { return 0 != (file_status_.st_mode & boost::filesystem::others_write); }
    bool FileStatus::permissionOwnerExecute() const { return 0 != (file_status_.st_mode & boost::filesystem::others_exe); }

    std::string FileStatus::permissionString() const {
        std::string permission_string(9, '-');

        if (!is_ok_)
            return permission_string;
#ifdef WIN32
        // TODO: Check  Linux/macOS
        // Root
        permission_string[0] = ((file_status_.st_mode & boost::filesystem::owner_read) != 0) ? 'r' : '-';
        permission_string[1] = ((file_status_.st_mode & boost::filesystem::owner_write) != 0) ? 'w' : '-';
        permission_string[2] = ((file_status_.st_mode & boost::filesystem::owner_exe) != 0) ? 'x' : '-';
        // Group
        permission_string[3] = ((file_status_.st_mode & boost::filesystem::group_read) != 0) ? 'r' : '-';
        permission_string[4] = ((file_status_.st_mode & boost::filesystem::group_write) != 0) ? 'w' : '-';
        permission_string[5] = ((file_status_.st_mode & boost::filesystem::group_exe) != 0) ? 'x' : '-';
        // Owner
        permission_string[6] = ((file_status_.st_mode & boost::filesystem::owner_read) != 0) ? 'r' : '-';
        permission_string[7] = ((file_status_.st_mode & boost::filesystem::others_write) != 0) ? 'w' : '-';
        permission_string[8] = ((file_status_.st_mode & boost::filesystem::others_exe) != 0) ? 'x' : '-';
#else
        // Root
        permission_string[0] = ((file_status_.st_mode & boost::filesystem::owner_read) != 0) ? 'r' : '-';
        permission_string[1] = ((file_status_.st_mode & boost::filesystem::owner_write) != 0) ? 'w' : '-';
        permission_string[2] = ((file_status_.st_mode & boost::filesystem::owner_exe) != 0) ? 'x' : '-';
        // Group
        permission_string[3] = ((file_status_.st_mode & boost::filesystem::group_read) != 0) ? 'r' : '-';
        permission_string[4] = ((file_status_.st_mode & boost::filesystem::group_write) != 0) ? 'w' : '-';
        permission_string[5] = ((file_status_.st_mode & boost::filesystem::group_exe) != 0) ? 'x' : '-';
        // Owner
        permission_string[6] = ((file_status_.st_mode & boost::filesystem::owner_read) != 0) ? 'r' : '-';
        permission_string[7] = ((file_status_.st_mode & boost::filesystem::others_write) != 0) ? 'w' : '-';
        permission_string[8] = ((file_status_.st_mode & boost::filesystem::others_exe) != 0) ? 'x' : '-';
#endif
        return permission_string;
    }

    std::string FileStatus::ownerString() const// NOLINT(readability-convert-member-functions-to-static) Reason: I want being able to extend the stub code here and return an actual owner
    {
        return "fineFTP";
    }

    std::string FileStatus::groupString() const// NOLINT(readability-convert-member-functions-to-static) Reason: I want being able to extend the stub code here and return an actual group
    {
        return "fineFTP";
    }

    std::string FileStatus::timeString() const {
        if (!is_ok_)
            return "Jan  1 1970";

        // The FTP Time format can be:
        //
        //     MMM DD hh:mm
        //   OR
        //     MMM DD  YYYY
        //   OR
        //     MMM DD YYYY
        //
        // This means, that we can only return the time for files with the same
        // year as the current year.
        //
        // https://files.stairways.com/other/ftp-list-specs-info.txt

        const auto now = std::chrono::system_clock::now();
        const time_t now_time_t = std::chrono::system_clock::to_time_t(now);
        //struct tm* now_timeinfo = localtime(&now_time_t);
        //int current_year = now_timeinfo->tm_year;

        //struct tm* file_timeinfo = localtime(&file_status_.st_ctime);
        //int file_year = file_timeinfo->tm_year;


        std::tm now_timeinfo{};
        std::tm file_timeinfo{};

#if defined(__unix__)
        localtime_r(&now_time_t, &now_timeinfo);
        localtime_r(&file_status_.st_ctime, &file_timeinfo);
#else
        static std::mutex mtx;
        {
            std::lock_guard lock(mtx);

            now_timeinfo = *std::localtime(&now_time_t);
            file_timeinfo = *std::localtime(&file_status_.st_ctime);
        }
#endif

        const int current_year = now_timeinfo.tm_year;
        const int file_year = file_timeinfo.tm_year;


        // Hardcoded english month names, because returning a localized string by strftime here may break certain FTP clients
        static const std::array<std::string, 12> month_names =
                {
                        "Jan",
                        "Feb",
                        "Mar",
                        "Apr",
                        "May",
                        "Jun",
                        "Jul",
                        "Aug",
                        "Sep",
                        "Oct",
                        "Nov",
                        "Dec"};

        std::stringstream date;

        if (file_year == current_year) {
            // We are allowed to return the time!
            date << std::setw(3) << file_timeinfo.tm_mday << " "
                 << std::setw(2) << file_timeinfo.tm_hour << ":"
                 << std::setw(2) << std::setfill('0') << file_timeinfo.tm_min;
        } else {
            // We must not return the time, only the date :(
            static constexpr auto tm_year_base_year = 1900;
            date << std::setw(3) << file_timeinfo.tm_mday
                 << "  " << (file_timeinfo.tm_year + tm_year_base_year);
        }

        return month_names.at(file_timeinfo.tm_mon) + date.str();
    }

    bool FileStatus::canOpenDir() const {
        if (!is_ok_)
            return false;

        if (type() != FileType::Dir)
            return false;

        bool can_open_dir(false);
#ifdef WIN32
        const boost::filesystem::path p(path_);
        return is_directory(boost::filesystem::directory_entry(p));
#else
        DIR *dp = opendir(path_.c_str());
        if (dp != nullptr) {
            can_open_dir = true;
            closedir(dp);
        }
        return can_open_dir;
#endif
    }

    std::map<std::string, FileStatus> dirContent(const std::string &path) {
        std::map<std::string, FileStatus> content;
        log_debug << "Get directory content, path: " << path;
#ifdef WIN32
        // TODO: Check Linux/macOS
        if (const boost::filesystem::path p(path); is_directory(boost::filesystem::directory_entry(p))) {
            for (auto &entry: boost::make_iterator_range(boost::filesystem::directory_iterator(p), {})) {
                std::string tmp = entry.path().filename().string();
                content.emplace(std::string(entry.path().filename().string()), FileStatus(entry.path().string()));
            }
        }
#else
        DIR *dp = opendir(path.c_str());
        dirent *dirp = nullptr;
        if (dp == nullptr) {
            log_error << "Error opening directory: " << strerror(errno) << ", returning empty dir";
            return content;
        }

        while ((dirp = readdir(dp)) != nullptr) {
            content.emplace(std::string(dirp->d_name), FileStatus(path + "/" + std::string(dirp->d_name)));
            log_debug << "Adding file, path: " << path << "/" << std::string(dirp->d_name);
        }
        closedir(dp);
        log_debug << "Found directory content, path: " << path << " count: " << content.size();
#endif
        return content;
    }

    std::string cleanPath(const std::string &path, const bool windows_path, const char output_separator) {
        if (path.empty()) {
            return ".";
        }

        // Find the root for absolute paths
        std::string absolute_root;

        if (windows_path) {
            /* On Windows, a root folder can be:
       *    C:\
       *    //Host
       *    \\Host
       */

            const std::regex win_local_drive(R"(^[a-zA-Z]\:)");       // Local drive
            const std::regex win_network_drive(R"(^[/\\]{2}[^/\\]+)");// Network path starting with two slashes or backslashes followed by a hostname

            if (std::regex_search(path, win_local_drive)) {
                // Windows local drive, consisting of drive-letter and colon
                absolute_root = path.substr(0, 2);
            } else if (std::regex_search(path, win_network_drive)) {
                // Window network drive, consisting of \\ and hostname
                const size_t sep_pos = path.find_first_of("/\\", 2);
                absolute_root = path.substr(0, sep_pos);// If no seperator was found, this will return the entire string
            }
        } else {
            // On Unix there is only one root and it is '/'
            if (path[0] == '/') {
                absolute_root = '/';
            }
        }

        // Split the path
        std::list<std::string> components;

        if (path.size() >= (absolute_root.size() + 1)) {
            size_t start = 0;
            size_t end = 0;

            if (absolute_root.empty())
                start = 0;
            else
                start = absolute_root.size();

            do {
                if (windows_path)
                    end = path.find_first_of("/\\", start);
                else
                    end = path.find_first_of('/', start);

                std::string this_component;
                if (end == std::string::npos)
                    this_component = path.substr(start);
                else
                    this_component = path.substr(start, end - start);

                // The components-stack that will increase and shrink depending on the folders and .. elements in the splitted path
                if (this_component.empty() || (this_component == ".")) {
                } else if (this_component == "..") {
                    if (!absolute_root.empty()) {
                        if (!components.empty()) {
                            // Move one folder up if we are not already at the root
                            components.pop_back();
                        }
                    } else {
                        if (!components.empty() && (components.back() != "..")) {
                            // Move one folder up by removing it. We must not remove ".." elements that we were not able to resolve previously.
                            components.pop_back();
                        } else {
                            components.emplace_back("..");
                        }
                    }
                } else {
                    components.push_back(this_component);
                }

                if (end == std::string::npos)
                    break;
                else
                    start = end + 1;

            } while (start < path.size());

            // Join the components again
            if (components.empty() && absolute_root.empty()) {
                return ".";
            }
        }

        std::stringstream path_ss;
        path_ss << absolute_root;

        if (windows_path && !absolute_root.empty()) {
            path_ss << output_separator;// The windows drive must be followed by a separator. When referencing a network drive.
        }

        auto comp_it = components.begin();
        while (comp_it != components.end()) {
            if (comp_it != components.begin())
                path_ss << output_separator;

            path_ss << *comp_it;

            comp_it++;
        }

        return path_ss.str();
    }

    std::string cleanPathNative(const std::string &path) {
        constexpr bool windows_path = false;
        constexpr char separator = '/';
        return cleanPath(path, windows_path, separator);
    }
}// namespace AwsMock::FtpServer
