//
// Created by vogje01 on 02/06/2023.
//
// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/repository/SSMDatabase.h>
#include <boost/test/utils/runtime/modifier.hpp>

#define TEST_ACCOUNT_ID "000000000000"
#define TEST_REGION "eu-central-1"
#define TEST_NAME "test-parameter-name"
#define TEST_DESCRIPTION "description"
#define TEST_TIER "tier"

namespace AwsMock::Database {

    Entity::SSM::Parameter CreateDefaultParameter(const std::string &region, const std::string &name, const std::string &value) {
        Entity::SSM::Parameter parameter;
        parameter.region = region;
        parameter.parameterName = name;
        parameter.parameterValue = value;
        parameter.description = TEST_DESCRIPTION;
        parameter.tier = TEST_TIER;
        return parameter;
    }

    struct SsmMemoryDbFixture {
        SsmMemoryDbFixture() = default;
        ~SsmMemoryDbFixture() {
            const long messageCount = SSMDatabase::instance().DeleteAllParameters();
            log_debug << "Parameters deleted " << messageCount;
        }
    };

    BOOST_FIXTURE_TEST_SUITE(SsmMemoryDbTests, SsmMemoryDbFixture)

    BOOST_AUTO_TEST_CASE(CreateParameterTest) {

        // arrange
        const SSMDatabase &ssmDatabase = SSMDatabase::instance();
        Entity::SSM::Parameter parameter = CreateDefaultParameter(TEST_REGION, "parameter-name", "parameter-value");

        // act
        const Entity::SSM::Parameter result = ssmDatabase.CreateParameter(parameter);

        // assert
        BOOST_CHECK_EQUAL(result.oid.empty(), false);
        BOOST_CHECK_EQUAL(result.parameterName, "parameter-name");
        BOOST_CHECK_EQUAL(result.parameterValue, "parameter-value");
        BOOST_CHECK_EQUAL(1, ssmDatabase.CountParameters());
    }

    BOOST_AUTO_TEST_CASE(ExistsParameterTest) {

        // arrange
        const SSMDatabase &ssmDatabase = SSMDatabase::instance();
        Entity::SSM::Parameter parameter = CreateDefaultParameter(TEST_REGION, "parameter-name", "parameter-value");
        parameter = ssmDatabase.CreateParameter(parameter);

        // act
        const bool result1 = ssmDatabase.ParameterExists(parameter.parameterName);
        const bool result2 = ssmDatabase.ParameterExists("blabla");

        // assert
        BOOST_CHECK_EQUAL(true, result1);
        BOOST_CHECK_EQUAL(false, result2);
    }

    BOOST_AUTO_TEST_CASE(GetParameterByNameMTest) {

        // arrange
        const SSMDatabase &ssmDatabase = SSMDatabase::instance();
        Entity::SSM::Parameter parameter = CreateDefaultParameter(TEST_REGION, "parameter-name", "parameter-value");
        parameter = ssmDatabase.CreateParameter(parameter);

        // act
        const Entity::SSM::Parameter result = ssmDatabase.GetParameterByName(parameter.parameterName);

        // assert
        BOOST_CHECK_EQUAL(result.parameterName, "parameter-name");
    }

    BOOST_AUTO_TEST_CASE(GetParameterByIdTest) {

        // arrange
        const SSMDatabase &ssmDatabase = SSMDatabase::instance();
        Entity::SSM::Parameter parameter = CreateDefaultParameter(TEST_REGION, "parameter-name", "parameter-value");
        parameter = ssmDatabase.CreateParameter(parameter);

        // act
        const Entity::SSM::Parameter result = ssmDatabase.GetParameterById(parameter.oid);

        // assert
        BOOST_CHECK_EQUAL(result.parameterName, "parameter-name");
    }

    BOOST_AUTO_TEST_CASE(UpdateParameterTest) {

        // arrange
        const SSMDatabase &ssmDatabase = SSMDatabase::instance();
        Entity::SSM::Parameter parameter = CreateDefaultParameter(TEST_REGION, "parameter-name", "parameter-value");
        parameter = ssmDatabase.CreateParameter(parameter);
        parameter.description = "new description";

        // act
        const Entity::SSM::Parameter result = ssmDatabase.UpdateParameter(parameter);

        // assert
        BOOST_CHECK_EQUAL(result.description, "new description");
    }

    BOOST_AUTO_TEST_CASE(DeleteParameterTest) {

        // arrange
        const SSMDatabase &ssmDatabase = SSMDatabase::instance();
        Entity::SSM::Parameter parameter = CreateDefaultParameter(TEST_REGION, "parameter-name", "parameter-value");
        parameter = ssmDatabase.CreateParameter(parameter);

        // act
        const long result = ssmDatabase.DeleteParameter(parameter);

        // assert
        BOOST_CHECK_EQUAL(1, result);
    }

    BOOST_AUTO_TEST_CASE(DeleteAllParameterTest) {

        // arrange
        const SSMDatabase &ssmDatabase = SSMDatabase::instance();
        Entity::SSM::Parameter parameter1 = CreateDefaultParameter(TEST_REGION, "parameter-name", "parameter-value");
        parameter1 = ssmDatabase.CreateParameter(parameter1);
        BOOST_CHECK_EQUAL(false, parameter1.oid.empty());
        Entity::SSM::Parameter parameter2 = CreateDefaultParameter(TEST_REGION, "parameter-name", "parameter-value");
        parameter2 = ssmDatabase.CreateParameter(parameter2);
        BOOST_CHECK_EQUAL(false, parameter2.oid.empty());

        // act
        const long result = ssmDatabase.DeleteAllParameters();

        // assert
        BOOST_CHECK_EQUAL(2, result);
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace AwsMock::Database
