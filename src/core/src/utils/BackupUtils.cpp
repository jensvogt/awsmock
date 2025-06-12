//
// Created by vogje01 on 6/10/25.
//


#include <awsmock/core/BackupUtils.h>

namespace AwsMock::Core {

    std::string BackupUtils::GetBackupFilename(const std::string &module) {
        const auto backupDir = Configuration::instance().GetValue<std::string>("awsmock.backup-dir");
        return backupDir + "/" + module + "-" + GetTimestamp() + ".json";
    }

    std::string BackupUtils::GetTimestamp() {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        return std::format("{:%Y%m%d%H%M}", now);
    }

}// namespace AwsMock::Core