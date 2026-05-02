//
// Created by vogje01 on 6/10/25.
//

#include <awsmock/core/BackupUtils.h>

namespace AwsMock::Core {
    std::string BackupUtils::GetBackupFilename(const std::string &module) {
        const auto backupDir = Configuration::instance().GetValue<std::string>("awsmock.backup-dir");
        DirUtils::EnsureDirectory(backupDir);
        return backupDir + "/" + module + "-" + GetTimestamp() + ".json";
    }

    std::string BackupUtils::GetTimestamp() {
        const std::time_t now = std::time(nullptr);
        const tm *temp = localtime(&now);
        char buffer[15];
        strftime(buffer, sizeof(buffer), "%Y%m%d%H%M", temp);
        return {buffer};
    }

    std::vector<std::string> BackupUtils::GetBackupFiles(const std::string &module, const int retention) {

        const auto backupDir = Configuration::instance().GetValue<std::string>("awsmock.backup-dir");
        if (std::vector<std::filesystem::path> fileList = DirUtils::ListFilesByPrefix(backupDir, module + "-"); fileList.size() > retention) {
            return {fileList.begin(), fileList.end() - retention};
        }
        return {};
    }

}// namespace AwsMock::Core