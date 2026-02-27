//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWSMOCK_CORE_SYSTEM_UTILS_TEST_H
#define AWSMOCK_CORE_SYSTEM_UTILS_TEST_H

// Local includes
#include <awsmock/core/SystemUtils.h>

namespace AwsMock::Core {

    BOOST_AUTO_TEST_CASE(GetHostNameTest) {

        // arrange

        // act
        const std::string hostName = SystemUtils::GetHostName();

        // assert
        BOOST_CHECK_EQUAL(hostName.empty(), false);
    }

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_SYSTEM_UTILS_TEST_H
