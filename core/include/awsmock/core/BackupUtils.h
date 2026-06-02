//
// Created by vogje01 on 6/10/25.
//

#pragma once

// C++ includes
#include <ranges>
#include <string>

// Awsmock includes
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/config/Configuration.h>

namespace Awsmock::Core {

    using std::chrono::system_clock;

    /**
     * @brief Backup utilities
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class BackupUtils {

      public:

        /**
         * Constructor
         */
        BackupUtils() = default;

        /**
         * Destruktor
         */
        ~BackupUtils() = default;

        /**
         * @brief Get the current backup file
         *
         * @param module module name
         * @return full path of the backup file
         */
        static std::string GetBackupFilename(const std::string &module);

        /**
         * @brief Returns a list of backup files for a module.
         *
         * @param module module name
         * @param retention number of backup files to keep
         * @return list of backup files
         */
        static std::vector<std::string> GetBackupFiles(const std::string &module, int retention);

      private:

        /**
         * Returns the current date time as string
         */
        static std::string GetTimestamp();
    };

}// namespace Awsmock::Core
