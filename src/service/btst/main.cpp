//
// Created by vogje01 on 01/09/2022.
//

// C++ includes

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#define BOOST_TEST_MODULE ServiceTests

// Boost includes
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/thread.hpp>

// AwsMock includes
#include <awsmock/core/SharedMemoryUtils.h>
#include <awsmock/core/TestUtils.h>
#include <awsmock/dto/docker/CreateContainerResponse.h>
#include <awsmock/service/container/ContainerService.h>
#include <awsmock/service/monitoring/MetricService.h>
#include <awsmock/utils/ConnectionPool.h>
#include <awsmock/utils/TestUtils.h>

#include "KMSServiceCliTests.cpp"
#include "KMSServiceJavaTests.cpp"
#include "KMSServiceTests.cpp"
#include "SQSServiceCliTests.cpp"
#include "SQSServiceJavaTests.cpp"
#include "SecretsManagerServiceTests.cpp"

// entry point:
int main(const int argc, char *argv[]) {

    return boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}