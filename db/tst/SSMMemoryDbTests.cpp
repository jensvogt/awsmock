//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWSMOCK_REPOSITORY_SSM_MEMORYDB_TESTS_H
#define AWSMOCK_REPOSITORY_SSM_MEMORYDB_TESTS_H

// AwsMock includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/SSMDatabase.h>

#define MODULE "test-module"

namespace AwsMock::Database {

    struct SSMMemoryDbTest {

        SSMMemoryDbTest() {
            _region = _configuration.GetValue<std::string>("awsmock.region");
        }

        ~SSMMemoryDbTest() {
            const long deleted = _ssmDatabase.DeleteAllParameters();
            log_debug << "Parameter cleaned, count:" << deleted;
        }

        std::string _region;
        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration(false);
        SSMDatabase &_ssmDatabase = SSMDatabase::instance();
    };

    BOOST_FIXTURE_TEST_CASE(ParameterCreateMTest, SSMMemoryDbTest) {

        // arrange
        Entity::SSM::Parameter parameter = {.region = _region, .parameterName = "parameter-name", .parameterValue = "parameter-value", .description = "description", .tier = "tier", .version = 1};

        // act
        const Entity::SSM::Parameter result = _ssmDatabase.CreateParameter(parameter);

        // assert
        BOOST_CHECK_EQUAL(result.oid.empty(), false);
        BOOST_CHECK_EQUAL(result.parameterName, "parameter-name");
        BOOST_CHECK_EQUAL(1, _ssmDatabase.CountParameters());
    }

    BOOST_FIXTURE_TEST_CASE(ParameterExistsMTest, SSMMemoryDbTest) {

        // arrange
        Entity::SSM::Parameter parameter = {.region = _region, .parameterName = "parameter-name", .parameterValue = "parameter-value", .description = "description", .tier = "tier", .version = 1};
        Entity::SSM::Parameter createResult = _ssmDatabase.CreateParameter(parameter);

        // act
        const bool result1 = _ssmDatabase.ParameterExists(parameter.parameterName);
        const bool result2 = _ssmDatabase.ParameterExists("blabla");

        // assert
        BOOST_CHECK_EQUAL(result1, true);
        BOOST_CHECK_EQUAL(result2, false);
    }

    BOOST_FIXTURE_TEST_CASE(ParameterGetByNameMTest, SSMMemoryDbTest) {

        // arrange
        Entity::SSM::Parameter parameter = {.region = _region, .parameterName = "parameter-name", .parameterValue = "parameter-value", .description = "description", .tier = "tier", .version = 1};
        Entity::SSM::Parameter createResult = _ssmDatabase.CreateParameter(parameter);

        // act
        const Entity::SSM::Parameter result = _ssmDatabase.GetParameterByName(parameter.parameterName);

        // assert
        BOOST_CHECK_EQUAL(result.parameterName, "parameter-name");
    }

    BOOST_FIXTURE_TEST_CASE(ParameterGetByIdMTest, SSMMemoryDbTest) {

        // arrange
        Entity::SSM::Parameter parameter = {.region = _region, .parameterName = "parameter-name", .parameterValue = "parameter-value", .description = "description", .tier = "tier", .version = 1};
        const Entity::SSM::Parameter createResult = _ssmDatabase.CreateParameter(parameter);

        // act
        const Entity::SSM::Parameter result = _ssmDatabase.GetParameterById(createResult.oid);

        // assert
        BOOST_CHECK_EQUAL(result.parameterName, "parameter-name");
    }

    BOOST_FIXTURE_TEST_CASE(ParameterUpdateMTest, SSMMemoryDbTest) {

        // arrange
        Entity::SSM::Parameter parameter = {.region = _region, .parameterName = "parameter-name", .parameterValue = "parameter-value", .description = "description", .tier = "tier", .version = 1};
        parameter = _ssmDatabase.CreateParameter(parameter);
        parameter.description = "new description";

        // act
        const Entity::SSM::Parameter result = _ssmDatabase.UpdateParameter(parameter);

        // assert
        BOOST_CHECK_EQUAL(result.description, "new description");
    }

    BOOST_FIXTURE_TEST_CASE(ParameterDeleteMTest, SSMMemoryDbTest) {

        // arrange
        Entity::SSM::Parameter parameter = {.region = _region, .parameterName = "parameter-name", .parameterValue = "parameter-value", .description = "description", .tier = "tier", .version = 1};
        parameter = _ssmDatabase.CreateParameter(parameter);

        // act
        _ssmDatabase.DeleteParameter(parameter);
        const long count = _ssmDatabase.CountParameters();

        // assert
        BOOST_CHECK_EQUAL(0, count);
    }

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_SSM_MEMORYDB_TESTS_H