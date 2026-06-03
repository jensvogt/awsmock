//
// Created by vogje01 on 07/09/2023.
//

#pragma once

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/entity/module/ModuleState.h>
#include <awsmock/repository/module/ModuleMongoRepository.h>

namespace Awsmock::Database {

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

}//namespace Awsmock::Database
