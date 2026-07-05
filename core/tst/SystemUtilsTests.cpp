//
// Created by vogje01 on 02/06/2023.
//

#pragma once

// Boost includes
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test_log.hpp>
#include <boost/test/unit_test_suite.hpp>

// Local includes
#include <awsmock/core/SystemUtils.h>

namespace Awsmock::Core {

    BOOST_AUTO_TEST_CASE(GetHostNameTest) {

        // arrange

        // act
        const std::string hostName = SystemUtils::GetHostName();

        // assert
        BOOST_CHECK_EQUAL(hostName.empty(), false);
    }

}// namespace Awsmock::Core
