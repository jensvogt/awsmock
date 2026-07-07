//
// Created by vogje01 on 7/7/26.
//

// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/repository/RepositoryFactory.h>
#include <awsmock/service/apigateway/ApiGatewayService.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

#define TEST_REGION "eu-central-1"
#define TEST_REST_API_NAME "test-rest-api"
#define TEST_RESOURCE_PATH "items"
#define TEST_HTTP_METHOD "GET"
#define TEST_INTEGRATION_TYPE "AWS_PROXY"
#define TEST_INTEGRATION_URI "arn:aws:apigateway:eu-central-1:lambda:path/2015-03-31/functions/arn:aws:lambda:eu-central-1:000000000000:function:test-fn/invocations"
#define TEST_INTEGRATION_HTTP_METHOD "POST"

namespace Awsmock::Database {

    Dto::ApiGateway::CreateRestApiResponse CreateDefaultRestApi(const Service::ApiGatewayService &svc) {
        Dto::ApiGateway::CreateRestApiRequest request;
        request.region = TEST_REGION;
        request.name = TEST_REST_API_NAME;
        return svc.createRestApi(request);
    }

    Dto::ApiGateway::CreateResourceResponse CreateDefaultResource(const Service::ApiGatewayService &svc, const std::string &restApiId, const std::string &parentId) {
        Dto::ApiGateway::CreateResourceRequest request;
        request.region = TEST_REGION;
        request.restApiId = restApiId;
        request.parentId = parentId;
        request.pathPart = TEST_RESOURCE_PATH;
        return svc.createResource(request);
    }

    struct ApiGatewayServiceFixture {
        ApiGatewayServiceFixture() {
            RepositoryFactory::instance().initialize(BackendType::MONGODB, "test");
        }
        ~ApiGatewayServiceFixture() {
            try {
                const auto apis = Database::RepositoryFactory::instance().apigatewayRepository()->listRestApis();
                for (const auto &api: apis) {
                    Database::RepositoryFactory::instance().apigatewayRepository()->deleteRestApi(api.id);
                }
                Database::RepositoryFactory::instance().apigatewayRepository()->deleteAllKeys();
            } catch (const std::exception &exc) {
                log_error << "ApiGateway fixture cleanup failed: " << exc.what();
            }
        }
        boost::asio::io_context _ioc;
    };

    BOOST_FIXTURE_TEST_SUITE(ApiGatewayServiceTests, ApiGatewayServiceFixture)

    BOOST_AUTO_TEST_CASE(CreateRestApiTest) {

        // arrange
        Service::ApiGatewayService svc{_ioc};

        // act
        const auto response = CreateDefaultRestApi(svc);

        // assert
        BOOST_CHECK_EQUAL(false, response.id.empty());
        BOOST_CHECK_EQUAL(TEST_REST_API_NAME, response.name);
        BOOST_CHECK_EQUAL(false, response.rootResourceId.empty());
    }

    BOOST_AUTO_TEST_CASE(GetRestApisTest) {

        // arrange
        Service::ApiGatewayService svc{_ioc};
        Dto::ApiGateway::CreateRestApiRequest req1;
        req1.region = TEST_REGION;
        req1.name = "test-api-1";
        std::ignore = svc.createRestApi(req1);
        Dto::ApiGateway::CreateRestApiRequest req2;
        req2.region = TEST_REGION;
        req2.name = "test-api-2";
        std::ignore = svc.createRestApi(req2);

        Dto::ApiGateway::GetRestApisRequest listRequest;
        listRequest.region = TEST_REGION;

        // act
        const auto response = svc.getRestApis(listRequest);

        // assert
        BOOST_CHECK_EQUAL(2, response.restApis.size());
    }

    BOOST_AUTO_TEST_CASE(CreateResourceTest) {

        // arrange
        Service::ApiGatewayService svc{_ioc};
        const auto api = CreateDefaultRestApi(svc);
        BOOST_CHECK_EQUAL(false, api.id.empty());

        // act
        const auto resource = CreateDefaultResource(svc, api.id, api.rootResourceId);

        // assert
        BOOST_CHECK_EQUAL(false, resource.id.empty());
        BOOST_CHECK_EQUAL(TEST_RESOURCE_PATH, resource.pathPart);
        BOOST_CHECK_EQUAL(api.rootResourceId, resource.parentId);
    }

    BOOST_AUTO_TEST_CASE(GetResourcesTest) {

        // arrange
        Service::ApiGatewayService svc{_ioc};
        const auto api = CreateDefaultRestApi(svc);
        CreateDefaultResource(svc, api.id, api.rootResourceId);

        Dto::ApiGateway::GetResourcesRequest request;
        request.region = TEST_REGION;
        request.restApiId = api.id;

        // act
        const auto response = svc.getResources(request);

        // assert — only the resource we created (no auto root resource in map)
        BOOST_CHECK_EQUAL(false, response.resources.empty());
        BOOST_CHECK_EQUAL(1, response.resources.size());
    }

    BOOST_AUTO_TEST_CASE(PutMethodTest) {

        // arrange
        const Service::ApiGatewayService svc{_ioc};
        const auto api = CreateDefaultRestApi(svc);
        const auto resource = CreateDefaultResource(svc, api.id, api.rootResourceId);

        // act
        BOOST_CHECK_NO_THROW(svc.putMethod(api.id, resource.id, TEST_HTTP_METHOD, false));

        // assert — verify via getResources that the method is recorded
        Dto::ApiGateway::GetResourcesRequest getRequest;
        getRequest.region = TEST_REGION;
        getRequest.restApiId = api.id;
        const auto getResponse = svc.getResources(getRequest);
        const auto it = std::ranges::find_if(getResponse.resources, [&](const auto &r) { return r.id == resource.id; });
        BOOST_REQUIRE(it != getResponse.resources.end());
        BOOST_CHECK_EQUAL(true, it->resourceMethods.contains(TEST_HTTP_METHOD));
    }

    BOOST_AUTO_TEST_CASE(PutIntegrationTest) {

        // arrange
        const Service::ApiGatewayService svc{_ioc};
        const auto api = CreateDefaultRestApi(svc);
        const auto resource = CreateDefaultResource(svc, api.id, api.rootResourceId);
        svc.putMethod(api.id, resource.id, TEST_HTTP_METHOD, false);

        // act
        BOOST_CHECK_NO_THROW(svc.putIntegration(api.id, resource.id, TEST_HTTP_METHOD,
                                                TEST_INTEGRATION_TYPE, TEST_INTEGRATION_URI, TEST_INTEGRATION_HTTP_METHOD));

        // assert
        const auto method = svc.getIntegration(api.id, resource.id, TEST_HTTP_METHOD);
        BOOST_CHECK_EQUAL(TEST_INTEGRATION_TYPE, method.integrationType);
        BOOST_CHECK_EQUAL(TEST_INTEGRATION_URI, method.integrationUri);
        BOOST_CHECK_EQUAL(TEST_INTEGRATION_HTTP_METHOD, method.integrationHttpMethod);
    }

    BOOST_AUTO_TEST_CASE(GetIntegrationTest) {

        // arrange
        Service::ApiGatewayService svc{_ioc};
        const auto api = CreateDefaultRestApi(svc);
        const auto resource = CreateDefaultResource(svc, api.id, api.rootResourceId);
        svc.putMethod(api.id, resource.id, TEST_HTTP_METHOD, false);
        svc.putIntegration(api.id, resource.id, TEST_HTTP_METHOD,
                           TEST_INTEGRATION_TYPE, TEST_INTEGRATION_URI, TEST_INTEGRATION_HTTP_METHOD);

        // act
        const auto method = svc.getIntegration(api.id, resource.id, TEST_HTTP_METHOD);

        // assert
        BOOST_CHECK_EQUAL(TEST_HTTP_METHOD, method.httpMethod);
        BOOST_CHECK_EQUAL(TEST_INTEGRATION_TYPE, method.integrationType);
        BOOST_CHECK_EQUAL(TEST_INTEGRATION_URI, method.integrationUri);
        BOOST_CHECK_EQUAL(TEST_INTEGRATION_HTTP_METHOD, method.integrationHttpMethod);
    }

    BOOST_AUTO_TEST_CASE(GetIntegrationNotFoundTest) {

        // arrange
        Service::ApiGatewayService svc{_ioc};
        const auto api = CreateDefaultRestApi(svc);
        const auto resource = CreateDefaultResource(svc, api.id, api.rootResourceId);
        svc.putMethod(api.id, resource.id, TEST_HTTP_METHOD, false);

        // act / assert — no integration set, should throw NotFoundException
        bool threw = false;
        try { std::ignore = svc.getIntegration(api.id, resource.id, "DELETE"); } catch (const Core::NotFoundException &) { threw = true; }
        BOOST_CHECK_EQUAL(true, threw);
    }

    BOOST_AUTO_TEST_CASE(DeleteIntegrationTest) {

        // arrange
        const Service::ApiGatewayService svc{_ioc};
        const auto api = CreateDefaultRestApi(svc);
        const auto resource = CreateDefaultResource(svc, api.id, api.rootResourceId);
        svc.putMethod(api.id, resource.id, TEST_HTTP_METHOD, false);
        svc.putIntegration(api.id, resource.id, TEST_HTTP_METHOD,
                           TEST_INTEGRATION_TYPE, TEST_INTEGRATION_URI, TEST_INTEGRATION_HTTP_METHOD);

        // act
        BOOST_CHECK_NO_THROW(svc.deleteIntegration(api.id, resource.id, TEST_HTTP_METHOD));

        // assert — integration fields are cleared
        const auto method = svc.getIntegration(api.id, resource.id, TEST_HTTP_METHOD);
        BOOST_CHECK_EQUAL(true, method.integrationType.empty());
        BOOST_CHECK_EQUAL(true, method.integrationUri.empty());
        BOOST_CHECK_EQUAL(true, method.integrationHttpMethod.empty());
    }

    BOOST_AUTO_TEST_CASE(DeleteResourceTest) {

        // arrange
        Service::ApiGatewayService svc{_ioc};
        const auto api = CreateDefaultRestApi(svc);
        const auto resource = CreateDefaultResource(svc, api.id, api.rootResourceId);

        Dto::ApiGateway::DeleteResourceRequest deleteRequest;
        deleteRequest.restApiId = api.id;
        deleteRequest.resourceId = resource.id;

        // act
        BOOST_CHECK_NO_THROW(svc.deleteResource(deleteRequest));

        // assert — resources map is now empty
        Dto::ApiGateway::GetResourcesRequest getRequest;
        getRequest.region = TEST_REGION;
        getRequest.restApiId = api.id;
        const auto getResponse = svc.getResources(getRequest);
        BOOST_CHECK_EQUAL(0, getResponse.resources.size());
    }

    BOOST_AUTO_TEST_CASE(DeleteRestApiTest) {

        // arrange
        Service::ApiGatewayService svc{_ioc};
        const auto api = CreateDefaultRestApi(svc);
        BOOST_CHECK_EQUAL(false, api.id.empty());

        Dto::ApiGateway::DeleteRestApiRequest deleteRequest;
        deleteRequest.restApiId = api.id;

        // act
        BOOST_CHECK_NO_THROW(svc.deleteRestApi(deleteRequest));

        // assert
        Dto::ApiGateway::GetRestApisRequest listRequest;
        listRequest.region = TEST_REGION;
        const auto listResponse = svc.getRestApis(listRequest);
        BOOST_CHECK_EQUAL(0, listResponse.restApis.size());
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace Awsmock::Database
