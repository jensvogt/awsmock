//
// Created by vogje01 on 1/5/25.
//

#ifndef AWSMOCK_SERVICE_TEST_BASE_H
#define AWSMOCK_SERVICE_TEST_BASE_H

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <awsmock/core/config/Configuration.h>

#define AWS_CMD "/usr/local/bin/aws"
#define TEST_IMAGE_NAME std::string("jensvogt/awsmock-test")
#define TEST_CONTAINER_VERSION std::string("latest")
#define TEST_CONTAINER_NAME std::string("awsmock-test")

namespace AwsMock::Core {

    class TestBase {

      public:

        /**
         * @brief Returns the current region.
         *
         * @return current region
         */
        std::string GetRegion() { return _region; }

        /**
         * @brief Returns the current endpoint.
         *
         * @return current endpoint
         */
        std::string GetEndpoint() { return _endpoint; }

        static bool InitUnitTests();

        static void InitializeShm();

      private:

        boost::thread _thread;
        std::string _endpoint, _region;
        Configuration &_configuration = Configuration::instance();
    };
}// namespace AwsMock::Core
#endif// AWSMOCK_SERVICE_TEST_BASE_H