//
// Created by vogje01 on 02/06/2023.
//
// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/repository/RepositoryFactory.h>
#include <awsmock/repository/ssm/ISSMRepository.h>
#include <boost/test/utils/runtime/modifier.hpp>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

#define TEST_ACCOUNT_ID "000000000000"
#define TEST_REGION "eu-central-1"
#define TEST_NAME "test-parameter-name"
#define TEST_DESCRIPTION "description"
#define TEST_TIER "tier"

namespace Awsmock::Database {

    Entity::SSM::Parameter CreateDefaultParameter(const std::string &region, const std::string &name, const std::string &value) {
        Entity::SSM::Parameter parameter;
        parameter.region = region;
        parameter.parameterName = name;
        parameter.parameterValue = value;
        parameter.description = TEST_DESCRIPTION;
        parameter.tier = TEST_TIER;
        return parameter;
    }

    struct SsmDbFixture {
        SsmDbFixture() {
            RepositoryFactory::instance().initialize(BackendType::MONGODB, "test");
        }
        ~SsmDbFixture() {
            const long messageCount = RepositoryFactory::instance().ssmRepository()->deleteAllParameters();
            log_debug << "Parameters deleted " << messageCount;
        }
    };

    BOOST_FIXTURE_TEST_SUITE(SsmDbTests, SsmDbFixture)

    BOOST_AUTO_TEST_CASE(createParameterTest) {

        // arrange
        const std::shared_ptr<ISSMRepository> ssmDatabase = RepositoryFactory::instance().ssmRepository();
        Entity::SSM::Parameter parameter = CreateDefaultParameter(TEST_REGION, "parameter-name", "parameter-value");

        // act
        const Entity::SSM::Parameter result = ssmDatabase->createParameter(parameter);

        // assert
        BOOST_CHECK_EQUAL(result.oid.empty(), false);
        BOOST_CHECK_EQUAL(result.parameterName, "parameter-name");
        BOOST_CHECK_EQUAL(result.parameterValue, "parameter-value");
        BOOST_CHECK_EQUAL(1, ssmDatabase->countParameters({}, {}));
    }

    BOOST_AUTO_TEST_CASE(ExistsParameterTest) {

        // arrange
        const std::shared_ptr<ISSMRepository> ssmDatabase = RepositoryFactory::instance().ssmRepository();
        Entity::SSM::Parameter parameter = CreateDefaultParameter(TEST_REGION, "parameter-name", "parameter-value");
        parameter = ssmDatabase->createParameter(parameter);

        // act
        const bool result1 = ssmDatabase->parameterExists(parameter.parameterName);
        const bool result2 = ssmDatabase->parameterExists("blabla");

        // assert
        BOOST_CHECK_EQUAL(true, result1);
        BOOST_CHECK_EQUAL(false, result2);
    }

    BOOST_AUTO_TEST_CASE(getParameterByNameMTest) {

        // arrange
        const std::shared_ptr<ISSMRepository> ssmDatabase = RepositoryFactory::instance().ssmRepository();
        Entity::SSM::Parameter parameter = CreateDefaultParameter(TEST_REGION, "parameter-name", "parameter-value");
        parameter = ssmDatabase->createParameter(parameter);

        // act
        const Entity::SSM::Parameter result = ssmDatabase->getParameterByName(parameter.parameterName);

        // assert
        BOOST_CHECK_EQUAL(result.parameterName, "parameter-name");
    }

    BOOST_AUTO_TEST_CASE(getParameterByIdTest) {

        // arrange
        const std::shared_ptr<ISSMRepository> ssmDatabase = RepositoryFactory::instance().ssmRepository();
        Entity::SSM::Parameter parameter = CreateDefaultParameter(TEST_REGION, "parameter-name", "parameter-value");
        parameter = ssmDatabase->createParameter(parameter);

        // act
        const Entity::SSM::Parameter result = ssmDatabase->getParameterById(parameter.oid);

        // assert
        BOOST_CHECK_EQUAL(result.parameterName, "parameter-name");
    }

    BOOST_AUTO_TEST_CASE(UpdateParameterTest) {

        // arrange
        const std::shared_ptr<ISSMRepository> ssmDatabase = RepositoryFactory::instance().ssmRepository();
        Entity::SSM::Parameter parameter = CreateDefaultParameter(TEST_REGION, "parameter-name", "parameter-value");
        parameter = ssmDatabase->createParameter(parameter);
        parameter.description = "new description";

        // act
        const Entity::SSM::Parameter result = ssmDatabase->updateParameter(parameter);

        // assert
        BOOST_CHECK_EQUAL(result.description, "new description");
    }

    BOOST_AUTO_TEST_CASE(DeleteParameterTest) {

        // arrange
        const std::shared_ptr<ISSMRepository> ssmDatabase = RepositoryFactory::instance().ssmRepository();
        Entity::SSM::Parameter parameter = CreateDefaultParameter(TEST_REGION, "parameter-name", "parameter-value");
        parameter = ssmDatabase->createParameter(parameter);

        // act
        const long result = ssmDatabase->deleteParameter(parameter);

        // assert
        BOOST_CHECK_EQUAL(1, result);
    }

    BOOST_AUTO_TEST_CASE(DeleteAllParameterTest) {

        // arrange
        const std::shared_ptr<ISSMRepository> ssmDatabase = RepositoryFactory::instance().ssmRepository();
        Entity::SSM::Parameter parameter1 = CreateDefaultParameter(TEST_REGION, "parameter-name", "parameter-value");
        parameter1 = ssmDatabase->createParameter(parameter1);
        BOOST_CHECK_EQUAL(false, parameter1.oid.empty());
        Entity::SSM::Parameter parameter2 = CreateDefaultParameter(TEST_REGION, "parameter-name", "parameter-value");
        parameter2 = ssmDatabase->createParameter(parameter2);
        BOOST_CHECK_EQUAL(false, parameter2.oid.empty());

        // act
        const long result = ssmDatabase->deleteAllParameters();

        // assert
        BOOST_CHECK_EQUAL(2, result);
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace Awsmock::Database
