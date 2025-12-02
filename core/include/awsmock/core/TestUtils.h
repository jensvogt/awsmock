//
// Created by vogje01 on 07/09/2023.
//

#ifndef AWSMOCK_CORE_TEST_UTILS_H
#define AWSMOCK_CORE_TEST_UTILS_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/SystemUtils.h>
#include <awsmock/core/config/Configuration.h>

#define TMP_CONFIGURATION_FILE "/tmp/awsmock.json"

namespace AwsMock::Core {

    /**
     * @brief Test utilities
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class TestUtils {
      public:

        /**
         * @brief Creates a test configuration file.
         *
         * @param withDatabase set to true when the configuration should be for a database
         */
        static void CreateTestConfigurationFile(bool withDatabase = true);

        /**
         * @brief Returns the name of the generated test configuration properties file.
         *
         * @return name of the generated test configuration file
         */
        static std::string GetTestConfigurationFilename();

        /**
         * @brief Returns the name of the generated test configuration properties file.
         *
         * @param withDatabase run with MongoDB database
         * @return name of the generated test configuration file
         */
        static Configuration &GetTestConfiguration(bool withDatabase = true);

        /**
         * @brief Send a CLI command
         *
         * @param command command name
         * @param args vector of string arguments command name
         * @return exec result
         */
        static std::string SendCliCommand(const std::string &command, const std::vector<std::string> &args);
    };
}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_TEST_UTILS_H
