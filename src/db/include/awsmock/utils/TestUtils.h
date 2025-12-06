//
// Created by vogje01 on 07/09/2023.
//

#ifndef AWSMOCK_DB_TEST_UTILS_H
#define AWSMOCK_DB_TEST_UTILS_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/entity/module/ModuleState.h>
#include <awsmock/repository/ModuleDatabase.h>

namespace AwsMock::Database {

    class TestUtils {

      public:

        /**
         * Creates all services in the module database
         */
        static void CreateServices();

        /**
        * Service names
        */
        static std::vector<std::string> _modules;
    };

}//namespace AwsMock::Database

#endif// AWSMOCK_DB_TEST_UTILS_H
