//
// Created by vogje01 on 30/05/2025.
//

// Boost includes
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/repository/RepositoryFactory.h>
#include <awsmock/service/ssm/SSMService.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

#define TEST_REGION "eu-central-1"
#define TEST_PARAMETER_NAME "/test/param"
#define TEST_PARAMETER_VALUE "test-value"
#define TEST_PARAMETER_DESCRIPTION "test-parameter-description"

namespace Awsmock::Database {

    Dto::SSM::PutParameterResponse PutDefaultParameter(const Service::SSMService &ssmService, const std::string &name = TEST_PARAMETER_NAME) {
        Dto::SSM::PutParameterRequest request;
        request.region = TEST_REGION;
        request.name = name;
        request.parameterValue = TEST_PARAMETER_VALUE;
        request.description = TEST_PARAMETER_DESCRIPTION;
        request.type = Dto::SSM::ParameterType::string;
        return ssmService.PutParameter(request);
    }

    struct SSMServiceFixture {
        SSMServiceFixture() = default;
        ~SSMServiceFixture() {
            try {
                const long count = RepositoryFactory::instance().ssmRepository()->deleteAllParameters();
                log_debug << "SSM parameters deleted, count: " << count;
            } catch (const std::exception &exc) {
                log_error << "SSM fixture cleanup failed: " << exc.what();
            }
        }
    };

    BOOST_FIXTURE_TEST_SUITE(SSMServiceTests, SSMServiceFixture)

    BOOST_AUTO_TEST_CASE(PutParameterTest) {

        // arrange
        const Service::SSMService ssmService;
        Dto::SSM::PutParameterRequest request;
        request.region = TEST_REGION;
        request.name = TEST_PARAMETER_NAME;
        request.parameterValue = TEST_PARAMETER_VALUE;
        request.description = TEST_PARAMETER_DESCRIPTION;
        request.type = Dto::SSM::ParameterType::string;

        // act
        const Dto::SSM::PutParameterResponse response = ssmService.PutParameter(request);

        // assert
        BOOST_CHECK_EQUAL(1, response.version);
    }

    BOOST_AUTO_TEST_CASE(PutParameterExistsTest) {

        // arrange
        const Service::SSMService ssmService;
        PutDefaultParameter(ssmService);

        Dto::SSM::PutParameterRequest request;
        request.region = TEST_REGION;
        request.name = TEST_PARAMETER_NAME;
        request.parameterValue = TEST_PARAMETER_VALUE;
        request.type = Dto::SSM::ParameterType::string;

        // act / assert
        BOOST_CHECK_THROW(
                [&] { return ssmService.PutParameter(request); }(),
                Core::ServiceException);
    }

    BOOST_AUTO_TEST_CASE(GetParameterTest) {

        // arrange
        const Service::SSMService ssmService;
        PutDefaultParameter(ssmService);

        Dto::SSM::GetParameterRequest getRequest;
        getRequest.region = TEST_REGION;
        getRequest.name = TEST_PARAMETER_NAME;

        // act
        const Dto::SSM::GetParameterResponse response = ssmService.GetParameter(getRequest);

        // assert
        BOOST_CHECK_EQUAL(TEST_PARAMETER_NAME, response.parameter.name);
        BOOST_CHECK_EQUAL(TEST_PARAMETER_VALUE, response.parameter.parameterValue);
        BOOST_CHECK_EQUAL(TEST_PARAMETER_DESCRIPTION, response.parameter.description);
        BOOST_CHECK_EQUAL(Dto::SSM::ParameterType::string, response.parameter.type);
        BOOST_CHECK_EQUAL(false, response.parameter.arn.empty());
    }

    BOOST_AUTO_TEST_CASE(GetParameterNotFoundTest) {

        // arrange
        const Service::SSMService ssmService;

        Dto::SSM::GetParameterRequest getRequest;
        getRequest.region = TEST_REGION;
        getRequest.name = "/nonexistent/param";

        // act / assert
        BOOST_CHECK_THROW(
                [&] { return ssmService.GetParameter(getRequest); }(),
                Core::ServiceException);
    }

    BOOST_AUTO_TEST_CASE(DescribeParametersTest) {

        // arrange
        const Service::SSMService ssmService;
        PutDefaultParameter(ssmService, "/test/param1");
        PutDefaultParameter(ssmService, "/test/param2");

        Dto::SSM::DescribeParametersRequest describeRequest;
        describeRequest.region = TEST_REGION;

        // act
        const Dto::SSM::DescribeParametersResponse response = ssmService.DescribeParameters(describeRequest);

        // assert
        BOOST_CHECK_EQUAL(false, response.parameters.empty());
        BOOST_CHECK_EQUAL(2, response.parameters.size());
    }

    BOOST_AUTO_TEST_CASE(ListParameterCountersTest) {

        // arrange
        const Service::SSMService ssmService;
        PutDefaultParameter(ssmService, "/test/param1");
        PutDefaultParameter(ssmService, "/test/param2");

        Dto::SSM::ListParameterCountersRequest listRequest;
        listRequest.region = TEST_REGION;
        listRequest.pageSize = 10;
        listRequest.pageIndex = 0;

        // act
        const Dto::SSM::ListParameterCountersResponse response = ssmService.ListParameterCounters(listRequest);

        // assert
        BOOST_CHECK_EQUAL(false, response.parameterCounters.empty());
        BOOST_CHECK_EQUAL(2, response.parameterCounters.size());
        BOOST_CHECK_EQUAL(2, response.total);
    }

    BOOST_AUTO_TEST_CASE(UpdateParameterTest) {

        // arrange
        const Service::SSMService ssmService;
        PutDefaultParameter(ssmService);

        Dto::SSM::UpdateParameterCounterRequest updateRequest;
        updateRequest.region = TEST_REGION;
        updateRequest.name = TEST_PARAMETER_NAME;
        updateRequest.value = "updated-value";
        updateRequest.description = "updated-description";
        updateRequest.type = Dto::SSM::ParameterType::string;
        updateRequest.pageSize = 10;
        updateRequest.pageIndex = 0;

        // act
        Dto::SSM::ListParameterCountersResponse updateResponse = ssmService.UpdateParameter(updateRequest);
        BOOST_CHECK_EQUAL(1, updateResponse.parameterCounters.size());

        // assert
        Dto::SSM::GetParameterRequest getRequest;
        getRequest.region = TEST_REGION;
        getRequest.name = TEST_PARAMETER_NAME;
        const Dto::SSM::GetParameterResponse getResponse = ssmService.GetParameter(getRequest);
        BOOST_CHECK_EQUAL("updated-value", getResponse.parameter.parameterValue);
        BOOST_CHECK_EQUAL("updated-description", getResponse.parameter.description);
    }

    BOOST_AUTO_TEST_CASE(UpdateParameterNotFoundTest) {

        // arrange
        const Service::SSMService ssmService;

        Dto::SSM::UpdateParameterCounterRequest updateRequest;
        updateRequest.region = TEST_REGION;
        updateRequest.name = "/nonexistent/param";
        updateRequest.value = "updated-value";
        updateRequest.type = Dto::SSM::ParameterType::string;

        // act / assert
        BOOST_CHECK_THROW(
                [&] { return ssmService.UpdateParameter(updateRequest); }(),
                Core::ServiceException);
    }

    BOOST_AUTO_TEST_CASE(DeleteParameterTest) {

        // arrange
        const Service::SSMService ssmService;
        PutDefaultParameter(ssmService);

        Dto::SSM::DeleteParameterRequest deleteRequest;
        deleteRequest.region = TEST_REGION;
        deleteRequest.name = TEST_PARAMETER_NAME;

        // act
        BOOST_CHECK_NO_THROW(ssmService.DeleteParameter(deleteRequest));

        // assert
        Dto::SSM::DescribeParametersRequest describeRequest;
        describeRequest.region = TEST_REGION;
        const Dto::SSM::DescribeParametersResponse listResponse = ssmService.DescribeParameters(describeRequest);
        BOOST_CHECK_EQUAL(true, listResponse.parameters.empty());
    }

    BOOST_AUTO_TEST_CASE(DeleteParameterNotFoundTest) {

        // arrange
        const Service::SSMService ssmService;

        Dto::SSM::DeleteParameterRequest deleteRequest;
        deleteRequest.region = TEST_REGION;
        deleteRequest.name = "/nonexistent/param";

        // act / assert
        BOOST_CHECK_THROW(
                [&] { return ssmService.DeleteParameter(deleteRequest); }(),
                Core::ServiceException);
    }

    BOOST_AUTO_TEST_CASE(DeleteParameterCounterTest) {

        // arrange
        const Service::SSMService ssmService;
        PutDefaultParameter(ssmService, "/test/param1");
        PutDefaultParameter(ssmService, "/test/param2");

        Dto::SSM::DeleteParameterCounterRequest deleteRequest;
        deleteRequest.region = TEST_REGION;
        deleteRequest.name = "/test/param1";
        deleteRequest.pageSize = 10;
        deleteRequest.pageIndex = 0;

        // act
        const Dto::SSM::ListParameterCountersResponse response = ssmService.DeleteParameterCounter(deleteRequest);

        // assert
        BOOST_CHECK_EQUAL(1, response.parameterCounters.size());
        BOOST_CHECK_EQUAL(1, response.total);
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace Awsmock::Database
