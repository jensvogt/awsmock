//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/core/DirUtils.h>

namespace AwsMock::Core {

    std::string DirUtils::RelativePath(const std::string &dir) {
        return dir.substr(1);
    }

    std::string DirUtils::GetTempDir() {
        std::string uuid = StringUtils::CreateRandomUuid();
        std::ranges::transform(uuid, uuid.begin(), tolower);
        boost::replace_all(uuid, "-", "");
        return uuid;
    }

    std::string DirUtils::CreateTempDir(const std::string &parent) {
        std::string localParent = parent;
        if (localParent.empty()) {
            localParent = boost::filesystem::temp_directory_path().string();
        }
        auto tempDir = localParent + FileUtils::separator() + boost::filesystem::unique_path().string();
        MakeDirectory(tempDir, true);
        return tempDir;
    }

    bool DirUtils::DirectoryExists(const std::string &dirName) {
        return boost::filesystem::exists(dirName) && boost::filesystem::is_directory(dirName);
    }

    void DirUtils::EnsureDirectory(const std::string &dirName) {
        if (!DirectoryExists(dirName)) {
            MakeDirectory(dirName, true);
        }
    }

    long DirUtils::DirectoryCountFiles(const std::string &dirName, const bool recursive) {
        if (recursive) {
            return std::count_if(
                    boost::filesystem::recursive_directory_iterator(dirName),
                    boost::filesystem::recursive_directory_iterator(),
                    static_cast<bool (*)(const boost::filesystem::path &)>(boost::filesystem::is_regular_file));
        }
        return std::count_if(
                boost::filesystem::directory_iterator(dirName),
                boost::filesystem::directory_iterator(),
                static_cast<bool (*)(const boost::filesystem::path &)>(boost::filesystem::is_regular_file));
    }

    bool DirUtils::DirectoryEmpty(const std::string &dirName) {
        return DirectoryCountFiles(dirName) == 0;
    }

    bool DirUtils::IsDirectory(const std::string &dirName) {
        return boost::filesystem::exists(dirName) && boost::filesystem::is_directory(dirName);
    }

    void DirUtils::MakeDirectory(const std::string &dirName, const bool recursive) {
        if (recursive) {
            boost::filesystem::create_directories(dirName);
        } else {
            boost::filesystem::create_directory(dirName);
        }
    }

    std::vector<std::string> DirUtils::ListFiles(const std::string &dirName, const bool recursive) {
        std::vector<std::string> fileNames;
        if (recursive) {
            for (auto &entry: boost::make_iterator_range(boost::filesystem::recursive_directory_iterator(dirName), {})) {
                if (is_regular_file(entry)) {
                    fileNames.emplace_back(entry.path().string().c_str());
                }
            }
        } else {
            for (auto &entry: boost::make_iterator_range(boost::filesystem::directory_iterator(dirName), {})) {
                if (is_regular_file(entry)) {
                    fileNames.emplace_back(entry.path().string().c_str());
                }
            }
        }
        return fileNames;
    }

    std::vector<std::string> DirUtils::ListFilesByPrefix(const std::string &dirName, const std::string &prefix) {

        std::vector<std::string> fileNames;
        for (auto &entry: boost::make_iterator_range(boost::filesystem::directory_iterator(dirName), {})) {
            if (is_regular_file(entry) && StringUtils::StartsWith(entry.path().string(), dirName + FileUtils::separator() + prefix)) {
                fileNames.emplace_back(entry.path().string());
            }
        }

        // start at position 6, comparing 6 characters
        if (!fileNames.empty()) {
            std::ranges::sort(fileNames, SubstringCompare("-."));
        }
        return fileNames;
    }

    std::vector<std::string> DirUtils::ListFilesByExtension(const std::string &dirName, const std::string &extension, const bool sorting) {

        std::vector<std::string> fileNames;
        for (auto &entry: boost::make_iterator_range(boost::filesystem::directory_iterator(dirName), {})) {
            if (is_regular_file(entry) && StringUtils::EndsWith(entry.path().string(), extension)) {
                fileNames.emplace_back(entry.path().string());
            }
        }
        if (sorting) {
            std::ranges::sort(fileNames);
        }
        return fileNames;
    }

    std::vector<std::string> DirUtils::ListFilesByPattern(const std::string &dirName, const std::string &pattern, const bool recursive, const bool sorting) {

        const std::regex regex(pattern);
        std::vector<std::string> fileNames;

        if (recursive) {
            for (auto &entry: boost::make_iterator_range(boost::filesystem::recursive_directory_iterator(dirName), {})) {
                if (is_regular_file(entry) && std::regex_match(entry.path().string().c_str(), regex)) {
                    fileNames.emplace_back(entry.path().string().c_str());
                }
            }
        } else {
            for (auto &entry: boost::make_iterator_range(boost::filesystem::directory_iterator(dirName), {})) {
                if (is_regular_file(entry) && std::regex_match(entry.path().string().c_str(), regex)) {
                    fileNames.emplace_back(entry.path().string().c_str());
                }
            }
        }
        if (sorting) {
            std::ranges::sort(fileNames);
        }
        return fileNames;
    }

    void DirUtils::DeleteDirectory(const std::string &dirName) {

        if (DirectoryExists(dirName)) {
            boost::filesystem::remove_all(dirName);
        }
    }

    void DirUtils::DeleteFilesInDirectory(const std::string &dirName) {

        if (DirectoryExists(dirName)) {
            for (const std::vector<std::string> files = ListFiles(dirName); auto &it: files) {
                boost::filesystem::remove_all(it);
            }
        }
    }
}// namespace AwsMock::Core