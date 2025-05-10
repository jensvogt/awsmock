//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_LAMBDA_SERVICE_H
#define AWSMOCK_SERVICE_LAMBDA_SERVICE_H

// C++ standard includes
#include <string>

// Boost includes
#include <boost/thread/thread.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/SystemUtils.h>
#include <awsmock/core/TarUtils.h>
#include <awsmock/core/exception/BadRequestException.h>
#include <awsmock/core/exception/NotFoundException.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/dto/lambda/AccountSettingsResponse.h>
#include <awsmock/dto/lambda/CreateEventSourceMappingsRequest.h>
#include <awsmock/dto/lambda/CreateEventSourceMappingsResponse.h>
#include <awsmock/dto/lambda/CreateFunctionRequest.h>
#include <awsmock/dto/lambda/CreateFunctionResponse.h>
#include <awsmock/dto/lambda/CreateTagRequest.h>
#include <awsmock/dto/lambda/DeleteFunctionRequest.h>
#include <awsmock/dto/lambda/DeleteTagsRequest.h>
#include <awsmock/dto/lambda/GetFunctionResponse.h>
#include <awsmock/dto/lambda/ListEventSourceMappingsRequest.h>
#include <awsmock/dto/lambda/ListEventSourceMappingsResponse.h>
#include <awsmock/dto/lambda/ListFunctionResponse.h>
#include <awsmock/dto/lambda/ListTagsResponse.h>
#include <awsmock/dto/lambda/intern/AddFunctionEnvironmentRequest.h>
#include <awsmock/dto/lambda/intern/AddFunctionTagRequest.h>
#include <awsmock/dto/lambda/intern/DeleteFunctionEnvironmentRequest.h>
#include <awsmock/dto/lambda/intern/DeleteFunctionTagRequest.h>
#include <awsmock/dto/lambda/intern/DeleteImageRequest.h>
#include <awsmock/dto/lambda/intern/GetFunctionCountersRequest.h>
#include <awsmock/dto/lambda/intern/GetFunctionCountersResponse.h>
#include <awsmock/dto/lambda/intern/ListFunctionCountersRequest.h>
#include <awsmock/dto/lambda/intern/ListFunctionCountersResponse.h>
#include <awsmock/dto/lambda/intern/ListLambdaEnvironmentCountersRequest.h>
#include <awsmock/dto/lambda/intern/ListLambdaEnvironmentCountersResponse.h>
#include <awsmock/dto/lambda/intern/ListLambdaTagCountersRequest.h>
#include <awsmock/dto/lambda/intern/ListLambdaTagCountersResponse.h>
#include <awsmock/dto/lambda/intern/ResetFunctionCountersRequest.h>
#include <awsmock/dto/lambda/intern/StartFunctionRequest.h>
#include <awsmock/dto/lambda/intern/StopFunctionRequest.h>
#include <awsmock/dto/lambda/intern/UpdateFunctionEnvironmentRequest.h>
#include <awsmock/dto/lambda/intern/UpdateFunctionTagRequest.h>
#include <awsmock/dto/lambda/intern/UploadFunctionCodeRequest.h>
#include <awsmock/dto/lambda/mapper/Mapper.h>
#include <awsmock/dto/lambda/model/Function.h>
#include <awsmock/dto/s3/model/EventNotification.h>
#include <awsmock/dto/sqs/model/EventNotification.h>
#include <awsmock/entity/sqs/Message.h>
#include <awsmock/repository/LambdaDatabase.h>
#include <awsmock/service/container/ContainerService.h>
#include <awsmock/service/lambda/LambdaCreator.h>
#include <awsmock/service/lambda/LambdaExecutor.h>
#include <awsmock/service/monitoring/MetricService.h>

// Maximal output length for a synchronous invocation call
#define MAX_OUTPUT_LENGTH (4 * 1024)

namespace AwsMock::Service {

    using std::chrono::system_clock;

    /**
     * @brief Lambda service module. Handles all lambda related requests:
     *
     * <ul>
     * <li>Create a lambda function.</li>
     * <li>Delete a lambda function.</li>
     * <li>List lambda function.</li>
     * <li>Create lambda function tags.</li>
     * <li>List lambda function tags.</li>
     * <li>Delete lambda function tags.</li>
     * <li>Invoke a lambda function with AWS S3 notification payload.</li>
     * <li>Invoke a lambda function with AWS SQS notification payload.</li>
     * </ul>
     *
     * @par
     * As the AWS lambda runtime environment (RIE) cannot handle several concurrent requests, the lambda function is run in a customized docker
     * container (@see AwsMock::Worker::LambdaExecutor). Each invocation first checks the database for an idle instance of the lambda function.
     * If no idle instance is found, the lambda creator will create a new instance of the docker container and start it with a random name.
     *
     * @par
     * The execution commands are sent via HTTP to the docker image. RIE is using port 8080 for the REST invocation requests. This port is mapped to the docker host on a randomly chosen port,
     * between 32768 and 65536.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class LambdaService {

      public:

        /**
         * @brief Constructor
         */
        explicit LambdaService() : _lambdaDatabase(Database::LambdaDatabase::instance()), _s3Database(Database::S3Database::instance()) {};

        /**
         * @brief Create lambda function
         *
         * @param request create lambda request
         * @return CreateFunctionResponse
         */
        Dto::Lambda::CreateFunctionResponse CreateFunction(Dto::Lambda::CreateFunctionRequest &request) const;

        /**
         * @brief List lambda functions
         *
         * @param region AWS region name
         * @return Dto::Lambda::CreateFunctionResponse
         */
        [[nodiscard]] Dto::Lambda::ListFunctionResponse ListFunctions(const std::string &region) const;

        /**
         * @brief List lambda function counters
         *
         * @param request list lambda function counters request
         * @return ListFunctionCountersResponse
         * @see Dto::Lambda::ListFunctionCountersRequest
         * @see Dto::Lambda::ListFunctionCountersResponse
         */
        [[nodiscard]] Dto::Lambda::ListFunctionCountersResponse ListFunctionCounters(const Dto::Lambda::ListFunctionCountersRequest &request) const;

        /**
         * @brief List lambda tags counters
         *
         * @param request list lambda tags counters request
         * @return ListFunctionCountersResponse
         * @see Dto::Lambda::ListLambdaTagCountersRequest
         * @see Dto::Lambda::ListFunctionCountersResponse
         */
        [[nodiscard]] Dto::Lambda::ListLambdaTagCountersResponse ListLambdaTagCounters(const Dto::Lambda::ListLambdaTagCountersRequest &request) const;

        /**
         * @brief List lambda environment counters
         *
         * @param request list lambda environment counters request
         * @return ListLambdaEnvironmentCountersResponse
         * @see ListLambdaEnvironmentCountersRequest
         * @see ListLambdaEnvironmentCountersResponse
         */
        [[nodiscard]] Dto::Lambda::ListLambdaEnvironmentCountersResponse ListLambdaEnvironmentCounters(const Dto::Lambda::ListLambdaEnvironmentCountersRequest &request) const;

        /**
         * @brief Add a lambda environment variable
         *
         * @param request add lambda environment variable request
         * @see Dto::Lambda::AddFunctionTagRequest
         */
        void AddLambdaEnvironment(const Dto::Lambda::AddFunctionEnvironmentRequest &request) const;

        /**
         * @brief Update a lambda environment variable
         *
         * @param request update lambda environment variablerequest
         * @see Dto::Lambda::UpdateFunctionEnvironmentRequest
         */
        void UpdateLambdaEnvironment(const Dto::Lambda::UpdateFunctionEnvironmentRequest &request) const;

        /**
         * @brief Delete a lambda environment variable
         *
         * @param request delete lambda environment variable request
         * @see Dto::Lambda::DeleteFunctionEnvironmentRequest
         */
        void DeleteLambdaEnvironment(const Dto::Lambda::DeleteFunctionEnvironmentRequest &request) const;

        /**
         * @brief Add a lambda tags
         *
         * @param request add lambda tags request
         * @see Dto::Lambda::AddFunctionTagRequest
         */
        void AddLambdaTag(const Dto::Lambda::AddFunctionTagRequest &request) const;

        /**
         * @brief Update a lambda tags
         *
         * @param request update lambda tags request
         * @see Dto::Lambda::UpdateFunctionTagRequest
         */
        void UpdateLambdaTag(const Dto::Lambda::UpdateFunctionTagRequest &request) const;

        /**
         * @brief Delete a lambda tags
         *
         * @param request delete lambda tags request
         * @see Dto::Lambda::DeleteFunctionTagRequest
         */
        void DeleteLambdaTag(const Dto::Lambda::DeleteFunctionTagRequest &request) const;

        /**
         * @brief Invoke SQS function.
         *
         * If the logType is set and is equal to 'Tail', the function will be invoked synchronously and the output will be appended to the response.
         *
         * @param region AWS region
         * @param functionName lambda function name
         * @param payload SQS message
         * @param receiptHandle receipt handle of the message which triggered the lambda
         */
        void InvokeLambdaFunction(const std::string &region, const std::string &functionName, const std::string &payload, const std::string &receiptHandle = {}) const;

        /**
         * @brief Create a new tag for a lambda function.
         *
         * @param request lambda create tag request
         */
        void CreateTag(const Dto::Lambda::CreateTagRequest &request) const;

        /**
         * @brief Returns a list of tags for an ARN.
         *
         * @param arn lambda function ARN
         * @return ListTagsResponse
         * @see Lambda::ListTagsResponse
         */
        [[nodiscard]] Dto::Lambda::ListTagsResponse ListTags(const std::string &arn) const;

        /**
         * @brief Gets a single lambda function
         *
         * @param region AWS region
         * @param name function name
         * @return GetFunctionResponse
         * @throws Core::ServiceException
         * @see Dto::Lambda::GetFunctionResponse
         */
        [[nodiscard]] Dto::Lambda::GetFunctionResponse GetFunction(const std::string &region, const std::string &name) const;

        /**
         * @brief Upload new function code
         *
         * @param request upload function code request
         * @throws Core::ServiceException
         * @see Dto::Lambda::UploadFunctionCodeRequest
         */
        void UploadFunctionCode(const Dto::Lambda::UploadFunctionCodeRequest &request) const;

        /**
         * @brief Gets a single lambda function counters
         *
         * @param request get function counters request
         * @return GetFunctionCountersResponse
         * @throws Core::ServiceException
         * @see Dto::Lambda::GetFunctionCountersRequest
         * @see Dto::Lambda::GetFunctionCountersResponse
         */
        Dto::Lambda::GetFunctionCountersResponse GetFunctionCounters(const Dto::Lambda::GetFunctionCountersRequest &request) const;

        /**
         * @brief Reset function counters
         *
         * @param request reset function counters request
         * @throws Core::ServiceException
         * @see Dto::Lambda::ResetFunctionCountersRequest
         */
        void ResetFunctionCounters(const Dto::Lambda::ResetFunctionCountersRequest &request) const;

        /**
         * @brief Returns the account settings
         *
         * @return AccountSettingsResponse
         * @throws Core::ServiceException
         * @see Dto::Lambda::AccountSettingsResponse
         */
        Dto::Lambda::AccountSettingsResponse GetAccountSettings() const;

        /**
         * @brief Creates an event source mapping.
         *
         * @par
         * The event source mapping is created in the SQS/SNS entities and executes whenever a SQS/SNS message is created.
         *
         * @param request create event source mappings request
         * @return create event source mappings response
         * @throws Core::ServiceException
         * @see Dto::Lambda::CreateEventSourceMappingsRequest
         * @see Dto::Lambda::CreateEventSourceMappingsResponse
         */
        Dto::Lambda::CreateEventSourceMappingsResponse CreateEventSourceMappings(const Dto::Lambda::CreateEventSourceMappingsRequest &request) const;

        /**
         * @brief List am event source mappings
         *
         * @param request list event source mappings request
         * @return list event source mappings response
         * @throws Core::ServiceException
         * @see Dto::Lambda::ListEventSourceMappingsRequest
         * @see Dto::Lambda::ListEventSourceMappingsResponse
         */
        Dto::Lambda::ListEventSourceMappingsResponse ListEventSourceMappings(const Dto::Lambda::ListEventSourceMappingsRequest &request) const;

        /**
         * @brief Starts the lambda function by starting a docker container
         *
         * @param request start lambda function request
         * @throws Core::ServiceException
         * @see Dto::Lambda::StartFunctionRequest
         */
        void StartFunction(const Dto::Lambda::StartFunctionRequest &request) const;

        /**
         * @brief Stops the lambda function by stopping all running docker containers
         *
         * @param request stop lambda function request
         * @throws Core::ServiceException
         * @see Dto::Lambda::StopFunctionRequest
         */
        void StopFunction(const Dto::Lambda::StopFunctionRequest &request) const;

        /**
         * @brief Delete lambda function
         *
         * This method will also delete the corresponding container and images.
         *
         * @param request delete lambda request
         * @throws Core::ServiceException
         */
        void DeleteFunction(const Dto::Lambda::DeleteFunctionRequest &request) const;

        /**
         * @brief Delete lambda function docker image
         *
         * This method will also stop and remove all docker container
         *
         * @param request delete image request
         * @see Dto::Lambda::DeleteImageRequest
         * @throws Core::ServiceException
         */
        void DeleteImage(const Dto::Lambda::DeleteImageRequest &request) const;

        /**
         * @brief Delete lambda function tags
         *
         * @param request delete tags request
         * @throws Core::ServiceException
         * @see Dto::Lambda::DeleteTagsRequest
         */
        void DeleteTags(Dto::Lambda::DeleteTagsRequest &request) const;

      private:

        /**
         * @brief Invoke the lambda function synchronously.
         *
         * The output will be returned to the calling method.
         *
         * @param host lambda docker container host
         * @param port lambda docker container port
         * @param payload payload for the function
         * @param oid lambda OID
         * @param instanceId instance ID
         * @return output from lambda invocation call
         */
        static std::string InvokeLambdaSynchronously(const std::string &host, int port, const std::string &payload, const std::string &oid, const std::string &instanceId);

        /**
         * @brief Tries to find an idle instance
         *
         * @param lambda lambda entity to check
         * @return containerId of the idle instance
         */
        static std::string FindIdleInstance(Database::Entity::Lambda::Lambda &lambda);

        /**
         * @brief Stops all running instances and deleted any existing containers and images.
         *
         * @param lambda lambda entity to cleanup
         */
        static void CleanupDocker(Database::Entity::Lambda::Lambda &lambda);

        /**
         * @brief Returns the host name, to where we send lambda invocation notifications
         *
         * @par
         * Depending on whether the lambda function is invoked from a dockerized AwsMock manager or a manager running on the
         * host machine, the hostname to which we need to send the invocation notification differs. For a host manager we need
         * to use 'localhost', for a dockerized manager we need to use the container name.
         *
         * @param instance lambda instance to check
         * @return containerId of the idle instance
         * @see Database::Entity::Lambda::Instance
         */
        static std::string GetHostname(Database::Entity::Lambda::Instance &instance);

        /**
         * @brief Returns the lambda port, to where we send lambda invocation notifications
         *
         * @par
         * Depending on whether the lambda function is invoked from a dockerized AwsMock manager or a manager running on the
         * host machine, the port to which we need to send the invocation notification differs. For a host manager we need
         * to use the container exposed port, for a dockerized manager we need to use the container internal port, i.e. 8080.
         *
         * @param instance lambda instance to check
         * @return containerId of the idle instance
         * @see Database::Entity::Lambda::Instance
         */
        static int GetContainerPort(const Database::Entity::Lambda::Instance &instance);

        /**
         * @brief Waits in a  loop for an idle lambda instance
         *
         * @par
         * This is a blocking call, the calling function will be blocked until an idle instance is available. The maximal waiting
         * time is limited by the total timeout period of the lambda (i.e. 900sec.)
         *
         * @param lambda lambda entity to check
         * @see Database::Entity::Lambda::Lambda
         */
        static void WaitForIdleInstance(Database::Entity::Lambda::Lambda &lambda);

        /**
         * @brief Returns the full path to the base64 encoded lambda function code.
         * @param lambda lambda entity
         * @return full path to base64 function code
         * @see Database::Entity::Lambda::Lambda
         */
        static std::string GetLambdaCodePath(const Database::Entity::Lambda::Lambda &lambda);

        /**
         * @brief Get the lambda code from a S3 bucket and key
         *
         * @param lambda lambda entity
         * @return lambda code size
         */
        std::string GetLambdaCodeFromS3(const Database::Entity::Lambda::Lambda &lambda) const;

        /**
         * Lambda database connection
         */
        Database::LambdaDatabase &_lambdaDatabase;

        /**
         * Lambda database connection
         */
        Database::S3Database &_s3Database;

        /**
         * Mutex
         */
        static boost::mutex _mutex;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_LAMBDA_SERVICE_H
