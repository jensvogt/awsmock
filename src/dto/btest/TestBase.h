//
// Created by vogje01 on 1/5/25.
//

#ifndef AWSMOCK_DTO_TEST_BASE_H
#define AWSMOCK_DTO_TEST_BASE_H

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <awsmock/core/config/Configuration.h>

namespace AwsMock::Dto {

    class TestBase {

      public:

        static bool InitUnitTests();

        static void InitializeShm();

      private:

        std::string _endpoint, _region;
        Core::Configuration &_configuration = Core::Configuration::instance();
    };

}// namespace AwsMock::Dto

#endif// AWSMOCK_DTO_TEST_BASE_H