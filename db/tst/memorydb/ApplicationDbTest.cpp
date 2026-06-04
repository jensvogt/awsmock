//
// Created by vogje01 on 02/06/2023.
//
// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/repository/RepositoryFactory.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

#define TEST_REGION "eu-central-1"
#define TEST_APPLICATION_NAME "test-application"
#define TEST_RUNTIME "java21"
#define TEST_TYPE "web"
#define TEST_VERSION "1.0.0"

namespace Awsmock::Database {

    Entity::Apps::Application CreateDefaultApplication(const std::string &region, const std::string &name) {
        Entity::Apps::Application application;
        application.region = region;
        application.name = name;
        application.runtime = TEST_RUNTIME;
        application.type = TEST_TYPE;
        application.version = TEST_VERSION;
        application.privatePort = 8080;
        application.publicPort = 9090;
        application.enabled = true;
        return application;
    }

    struct ApplicationMemoryDbFixture {
        ApplicationMemoryDbFixture() {
            RepositoryFactory::instance().initialize(BackendType::MEMORY);
        }
        ~ApplicationMemoryDbFixture() {
            const long count = RepositoryFactory::instance().applicationRepository()->DeleteAllApplications();
            log_debug << "Applications deleted, count: " << count;
        }
    };

    BOOST_FIXTURE_TEST_SUITE(ApplicationMemoryDbTests, ApplicationMemoryDbFixture)

    BOOST_AUTO_TEST_CASE(ApplicationCreate) {

        // arrange
        const std::shared_ptr<IApplicationRepository> applicationRepository = RepositoryFactory::instance().applicationRepository();
        Entity::Apps::Application application = CreateDefaultApplication(TEST_REGION, TEST_APPLICATION_NAME);

        // act
        const Entity::Apps::Application result = applicationRepository->CreateApplication(application);

        // assert
        BOOST_CHECK_EQUAL(result.name, TEST_APPLICATION_NAME);
        BOOST_CHECK_EQUAL(result.runtime, TEST_RUNTIME);
        BOOST_CHECK_EQUAL(result.type, TEST_TYPE);
        BOOST_CHECK_EQUAL(result.version, TEST_VERSION);
        BOOST_CHECK_EQUAL(result.privatePort, 8080);
        BOOST_CHECK_EQUAL(result.publicPort, 9090);
        BOOST_CHECK_EQUAL(result.enabled, true);
        BOOST_CHECK(!result.oid.empty());
    }

    BOOST_AUTO_TEST_CASE(ApplicationCount) {

        // arrange
        const std::shared_ptr<IApplicationRepository> applicationRepository = RepositoryFactory::instance().applicationRepository();
        Entity::Apps::Application application = CreateDefaultApplication(TEST_REGION, TEST_APPLICATION_NAME);
        application = applicationRepository->CreateApplication(application);
        BOOST_CHECK_EQUAL(application.name, TEST_APPLICATION_NAME);

        // act
        const long result = applicationRepository->CountApplications(TEST_REGION, {});

        // assert
        BOOST_CHECK_EQUAL(1, result);
    }

    BOOST_AUTO_TEST_CASE(ApplicationExists) {

        // arrange
        const std::shared_ptr<IApplicationRepository> applicationRepository = RepositoryFactory::instance().applicationRepository();
        Entity::Apps::Application application = CreateDefaultApplication(TEST_REGION, TEST_APPLICATION_NAME);
        application = applicationRepository->CreateApplication(application);
        BOOST_CHECK_EQUAL(application.name, TEST_APPLICATION_NAME);

        // act
        const bool result = applicationRepository->ApplicationExists(TEST_REGION, TEST_APPLICATION_NAME);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_AUTO_TEST_CASE(ApplicationGet) {

        // arrange
        const std::shared_ptr<IApplicationRepository> applicationRepository = RepositoryFactory::instance().applicationRepository();
        Entity::Apps::Application application = CreateDefaultApplication(TEST_REGION, TEST_APPLICATION_NAME);
        application = applicationRepository->CreateApplication(application);
        BOOST_CHECK_EQUAL(application.name, TEST_APPLICATION_NAME);

        // act
        const Entity::Apps::Application result = applicationRepository->GetApplication(TEST_REGION, TEST_APPLICATION_NAME);

        // assert
        BOOST_CHECK_EQUAL(result.name, TEST_APPLICATION_NAME);
        BOOST_CHECK_EQUAL(result.region, TEST_REGION);
        BOOST_CHECK_EQUAL(result.runtime, TEST_RUNTIME);
    }

    BOOST_AUTO_TEST_CASE(ApplicationUpdate) {

        // arrange
        const std::shared_ptr<IApplicationRepository> applicationRepository = RepositoryFactory::instance().applicationRepository();
        Entity::Apps::Application application = CreateDefaultApplication(TEST_REGION, TEST_APPLICATION_NAME);
        application = applicationRepository->CreateApplication(application);
        BOOST_CHECK_EQUAL(application.name, TEST_APPLICATION_NAME);

        // act
        application.description = "updated description";
        const Entity::Apps::Application result = applicationRepository->UpdateApplication(application);

        // assert
        BOOST_CHECK_EQUAL(result.description, "updated description");
    }

    BOOST_AUTO_TEST_CASE(ApplicationSetEnabled) {

        // arrange
        const std::shared_ptr<IApplicationRepository> applicationRepository = RepositoryFactory::instance().applicationRepository();
        Entity::Apps::Application application = CreateDefaultApplication(TEST_REGION, TEST_APPLICATION_NAME);
        application.enabled = true;
        application = applicationRepository->CreateApplication(application);
        BOOST_CHECK_EQUAL(application.name, TEST_APPLICATION_NAME);

        // act
        applicationRepository->SetEnabled(TEST_REGION, TEST_APPLICATION_NAME, false);
        const Entity::Apps::Application result = applicationRepository->GetApplication(TEST_REGION, TEST_APPLICATION_NAME);

        // assert
        BOOST_CHECK_EQUAL(result.enabled, false);
    }

    BOOST_AUTO_TEST_CASE(ApplicationList) {

        // arrange
        const std::shared_ptr<IApplicationRepository> applicationRepository = RepositoryFactory::instance().applicationRepository();
        Entity::Apps::Application application = CreateDefaultApplication(TEST_REGION, TEST_APPLICATION_NAME);
        application = applicationRepository->CreateApplication(application);
        BOOST_CHECK_EQUAL(application.name, TEST_APPLICATION_NAME);

        // act
        const std::vector<Entity::Apps::Application> result = applicationRepository->ListApplications(TEST_REGION, {}, 10, 0, {});

        // assert
        BOOST_CHECK_EQUAL(1, result.size());
        BOOST_CHECK_EQUAL(result[0].name, TEST_APPLICATION_NAME);
        BOOST_CHECK_EQUAL(result[0].runtime, TEST_RUNTIME);
        BOOST_CHECK_EQUAL(result[0].type, TEST_TYPE);
    }

    BOOST_AUTO_TEST_CASE(ApplicationListPrefix) {

        // arrange
        const std::shared_ptr<IApplicationRepository> applicationRepository = RepositoryFactory::instance().applicationRepository();
        Entity::Apps::Application app1 = CreateDefaultApplication(TEST_REGION, "test-app-1");
        Entity::Apps::Application app2 = CreateDefaultApplication(TEST_REGION, "other-app");
        app1 = applicationRepository->CreateApplication(app1);
        app2 = applicationRepository->CreateApplication(app2);

        // act
        const std::vector<Entity::Apps::Application> result = applicationRepository->ListApplications(TEST_REGION, "test", 10, 0, {});

        // assert
        BOOST_CHECK_EQUAL(1, result.size());
        BOOST_CHECK_EQUAL(result[0].name, "test-app-1");
        BOOST_CHECK_EQUAL(false, app1.oid.empty());
        BOOST_CHECK_EQUAL(false, app2.oid.empty());
    }

    BOOST_AUTO_TEST_CASE(ApplicationDelete) {

        // arrange
        const std::shared_ptr<IApplicationRepository> applicationRepository = RepositoryFactory::instance().applicationRepository();
        Entity::Apps::Application application = CreateDefaultApplication(TEST_REGION, TEST_APPLICATION_NAME);
        application = applicationRepository->CreateApplication(application);
        BOOST_CHECK_EQUAL(application.name, TEST_APPLICATION_NAME);

        // act
        const long deleted = applicationRepository->DeleteApplication(TEST_REGION, TEST_APPLICATION_NAME);
        const bool result = applicationRepository->ApplicationExists(TEST_REGION, TEST_APPLICATION_NAME);

        // assert
        BOOST_CHECK_EQUAL(result, false);
        BOOST_CHECK_EQUAL(1, deleted);
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace Awsmock::Database
