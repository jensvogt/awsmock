//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_LAMBDA_SERVICE_H
#define AWSMOCK_SERVICE_LAMBDA_SERVICE_H

// C++ standard includes
#include <string>

// Boost includes
#include <boost/signals2/signal.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/PagingUtils.h>
#include <awsmock/core/Semaphore.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/SystemUtils.h>
#include <awsmock/core/TarUtils.h>
#include <awsmock/core/exception/BadRequestException.h>
#include <awsmock/core/exception/NotFoundException.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/mapper/Mapper.h>
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
#include <awsmock/dto/lambda/internal/AddEnvironmentRequest.h>
#include <awsmock/dto/lambda/internal/AddEventSourceRequest.h>
#include <awsmock/dto/lambda/internal/AddTagRequest.h>
#include <awsmock/dto/lambda/internal/DeleteEnvironmentRequest.h>
#include <awsmock/dto/lambda/internal/DeleteEventSourceRequest.h>
#include <awsmock/dto/lambda/internal/DeleteImageRequest.h>
#include <awsmock/dto/lambda/internal/DeleteLambdaResultCounterRequest.h>
#include <awsmock/dto/lambda/internal/DeleteLambdaResultCountersRequest.h>
#include <awsmock/dto/lambda/internal/DeleteTagRequest.h>
#include <awsmock/dto/lambda/internal/DisableAllLambdasRequest.h>
#include <awsmock/dto/lambda/internal/DisableLambdaRequest.h>
#include <awsmock/dto/lambda/internal/EnableAllLambdasRequest.h>
#include <awsmock/dto/lambda/internal/EnableLambdaRequest.h>
#include <awsmock/dto/lambda/internal/GetFunctionCountersRequest.h>
#include <awsmock/dto/lambda/internal/GetFunctionCountersResponse.h>
#include <awsmock/dto/lambda/internal/GetLambdaResultCounterRequest.h>
#include <awsmock/dto/lambda/internal/GetLambdaResultCounterResponse.h>
#include <awsmock/dto/lambda/internal/ListFunctionCountersRequest.h>
#include <awsmock/dto/lambda/internal/ListFunctionCountersResponse.h>
#include <awsmock/dto/lambda/internal/ListLambdaArnsResponse.h>
#include <awsmock/dto/lambda/internal/ListLambdaEnvironmentCountersRequest.h>
#include <awsmock/dto/lambda/internal/ListLambdaEnvironmentCountersResponse.h>
#include <awsmock/dto/lambda/internal/ListLambdaEventSourceCountersRequest.h>
#include <awsmock/dto/lambda/internal/ListLambdaEventSourceCountersResponse.h>
#include <awsmock/dto/lambda/internal/ListLambdaInstanceCountersRequest.h>
#include <awsmock/dto/lambda/internal/ListLambdaInstanceCountersResponse.h>
#include <awsmock/dto/lambda/internal/ListLambdaResultCountersRequest.h>
#include <awsmock/dto/lambda/internal/ListLambdaResultCountersResponse.h>
#include <awsmock/dto/lambda/internal/ListLambdaTagCountersRequest.h>
#include <awsmock/dto/lambda/internal/ListLambdaTagCountersResponse.h>
#include <awsmock/dto/lambda/internal/ResetFunctionCountersRequest.h>
#include <awsmock/dto/lambda/internal/StartLambdaRequest.h>
#include <awsmock/dto/lambda/internal/StopLambdaInstanceRequest.h>
#include <awsmock/dto/lambda/internal/StopLambdaRequest.h>
#include <awsmock/dto/lambda/internal/UpdateFunctionEnvironmentRequest.h>
#include <awsmock/dto/lambda/internal/UpdateFunctionTagRequest.h>
#include <awsmock/dto/lambda/internal/UpdateLambdaRequest.h>
#include <awsmock/dto/lambda/internal/UploadFunctionCodeRequest.h>
#include <awsmock/dto/lambda/mapper/Mapper.h>
#include <awsmock/dto/lambda/model/Function.h>
#include <awsmock/dto/lambda/model/InvocationType.h>
#include <awsmock/dto/s3/PutBucketNotificationConfigurationRequest.h>
#include <awsmock/dto/s3/model/EventNotification.h>
#include <awsmock/dto/sqs/model/EventNotification.h>
#include <awsmock/entity/sqs/Message.h>
#include <awsmock/repository/LambdaDatabase.h>
#include <awsmock/repository/SNSDatabase.h>
#include <awsmock/service/container/ContainerService.h>
#include <awsmock/service/lambda/LambdaCreator.h>
#include <awsmock/service/lambda/LambdaExecutor.h>
#include <awsmock/service/monitoring/MetricService.h>

// Maximal output length for a synchronous invocation call
#define MAX_OUTPUT_LENGTH (4 * 1024)

namespace AwsMock::Service {

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
         *
         * @param ioc boost asio IO context
         */
        explicit LambdaService(boost::asio::io_context &ioc) : _lambdaDatabase(Database::LambdaDatabase::instance()), _s3Database(Database::S3Database::instance()), _sqsDatabase(Database::SQSDatabase::instance()), _snsDatabase(Database::SNSDatabase::instance()), _ioc(ioc) {}

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
         * @brief List lambda instances counters
         *
         * @param request list lambda instances counters request
         * @return ListFunctionCountersResponse
         * @see Dto::Lambda::ListLambdaInstanceCountersRequest
         * @see Dto::Lambda::ListFunctionCountersResponse
         */
        [[nodiscard]] Dto::Lambda::ListLambdaInstanceCountersResponse ListLambdaInstanceCounters(const Dto::Lambda::ListLambdaInstanceCountersRequest &request) const;

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
         * @brief List lambda event source counters
         *
         * @param request list lambda event source counters request
         * @return ListLambdaEventSourceCountersResponse
         * @see ListLambdaEventSourceCountersRequest
         * @see ListLambdaEventSourceCountersResponse
         */
        [[nodiscard]] Dto::Lambda::ListLambdaEventSourceCountersResponse ListLambdaEventSourceCounters(const Dto::Lambda::ListLambdaEventSourceCountersRequest &request) const;

        /**
         * @brief Update a lambda function
         *
         * @param request update lambda request
         * @see Dto::Lambda::UpdateLambdaRequest
         */
        void UpdateLambda(const Dto::Lambda::UpdateLambdaRequest &request) const;

        /**
         * @brief Add a lambda environment variable
         *
         * @param request add lambda environment variable request
         * @see Dto::Lambda::AddFunctionTagRequest
         */
        void AddLambdaEnvironment(const Dto::Lambda::AddEnvironmentRequest &request) const;

        /**
         * @brief Update a lambda environment variable
         *
         * @param request update lambda environment variable request
         * @see Dto::Lambda::UpdateFunctionEnvironmentRequest
         */
        void UpdateLambdaEnvironment(const Dto::Lambda::UpdateFunctionEnvironmentRequest &request) const;

        /**
         * @brief Delete a lambda environment variable
         *
         * @param request delete lambda environment variable request
         * @see Dto::Lambda::DeleteFunctionEnvironmentRequest
         */
        void DeleteLambdaEnvironment(const Dto::Lambda::DeleteEnvironmentRequest &request) const;

        /**
         * @brief Add a lambda event source
         *
         * @param request add lambda vent source request
         * @see Dto::Lambda::AddEventSourceRequest
         */
        void AddEventSource(const Dto::Lambda::AddEventSourceRequest &request) const;

        /**
         * @brief Delete a lambda event source
         *
         * @param request delete lambda event source request
         * @see Dto::Lambda::DeleteEventSourceRequest
         */
        void DeleteEventSource(const Dto::Lambda::DeleteEventSourceRequest &request) const;

        /**
         * @brief Add a lambda tags
         *
         * @param request add lambda tags request
         * @see Dto::Lambda::AddFunctionTagRequest
         */
        void AddLambdaTag(const Dto::Lambda::AddTagRequest &request) const;

        /**
         * @brief Update a lambda tags
         *
         * @param request update lambda tags request
         * @see Dto::Lambda::UpdateFunctionTagRequest
         */
        void UpdateLambdaTag(const Dto::Lambda::UpdateFunctionTagRequest &request) const;

        /**
         * @brief Enable an lambda
         *
         * @param request enable lambda request
         * @see Dto::Apps::EnableLambdaRequest
         */
        void EnableLambda(const Dto::Lambda::EnableLambdaRequest &request) const;

        /**
         * @brief Enable all lambdas
         *
         * @param request enable sll lambdas request
         * @see Dto::Apps::EnableAllLambdaRequest
         */
        void EnableAllLambdas(const Dto::Lambda::EnableAllLambdasRequest &request) const;

        /**
         * @brief Disable an lambda
         *
         * @param request disable lambda request
         * @see Dto::Apps::DisableLambdaRequest
         */
        void DisableLambda(const Dto::Lambda::DisableLambdaRequest &request) const;

        /**
         * @brief Disable all lambdas
         *
         * @param request disable sll lambdas request
         * @see Dto::Apps::DisableAllLambdaRequest
         */
        void DisableAllLambdas(const Dto::Lambda::DisableAllLambdasRequest &request) const;

        /**
         * @brief Delete a lambda tags
         *
         * @param request delete lambda tags request
         * @see Dto::Lambda::DeleteFunctionTagRequest
         */
        void DeleteLambdaTag(const Dto::Lambda::DeleteTagRequest &request) const;

        /**
         * @brief Invoke SQS function.
         *
         * If the logType is set and is equal to 'Tail', the function will be invoked synchronously and the output will be appended to the response.
         *
         * @param region AWS region
         * @param functionName lambda function name
         * @param payload SQS message
         * @param invocationType invocation type synchronous/asynchronous
         * @return lambda result in case of synchronous invocation, otherwise empty struct
         */
        [[nodiscard]] Dto::Lambda::LambdaResult InvokeLambdaFunction(const std::string &region, const std::string &functionName, std::string &payload, const Dto::Lambda::LambdaInvocationType &invocationType) const;

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
        [[nodiscard]] Dto::Lambda::GetFunctionCountersResponse GetFunctionCounters(const Dto::Lambda::GetFunctionCountersRequest &request) const;

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
        [[nodiscard]] Dto::Lambda::AccountSettingsResponse GetAccountSettings() const;

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
        [[nodiscard]] Dto::Lambda::CreateEventSourceMappingsResponse CreateEventSourceMappings(const Dto::Lambda::CreateEventSourceMappingsRequest &request) const;

        /**
         * @brief List am event source mappings
         *
         * @param request list event source mappings request
         * @return list event source mappings response
         * @throws Core::ServiceException
         * @see Dto::Lambda::ListEventSourceMappingsRequest
         * @see Dto::Lambda::ListEventSourceMappingsResponse
         */
        [[nodiscard]] Dto::Lambda::ListEventSourceMappingsResponse ListEventSourceMappings(const Dto::Lambda::ListEventSourceMappingsRequest &request) const;

        /**
         * @brief Returns a list of all available functions ARNs
         *
         * @return ListLambdaArnsResponse
         * @see ListLambdaArnsResponse
         */
        [[nodiscard]] Dto::Lambda::ListLambdaArnsResponse ListLambdaArns() const;

        /**
         * @brief Returns a lambda result counter
         *
         * @param request get result counter request
         * @return GetLambdaResultCounterResponse
         * @throws Core::ServiceException
         * @see Dto::Lambda::GetLambdaResultCounterRequest
         * @see Dto::Lambda::GetLambdaResultCounterResponse
         */
        [[nodiscard]] Dto::Lambda::GetLambdaResultCounterResponse GetLambdaResultCounter(const Dto::Lambda::GetLambdaResultCounterRequest &request) const;

        /**
         * @brief Returns a list of all available lambda result counters
         *
         * @param request list lambda result counters request
         * @return ListLambdaResultCountersResponse
         * @throws Core::ServiceException
         * @see Dto::Lambda::ListLambdaResultCountersRequest
         * @see Dto::Lambda::ListLambdaResultCountersResponse
         */
        [[nodiscard]] Dto::Lambda::ListLambdaResultCountersResponse ListLambdaResultCounters(const Dto::Lambda::ListLambdaResultCountersRequest &request) const;

        /**
         * @brief Deletes a lambda result counter
         *
         * @param request delete lambda result counter request
         * @return number of results deleted
         * @throws Core::ServiceException
         * @see Dto::Lambda::DeleteLambdaResultCounterRequest
         */
        [[nodiscard]] long DeleteLambdaResultCounter(const Dto::Lambda::DeleteLambdaResultCounterRequest &request) const;

        /**
         * @brief Deletes all lambda result counter
         *
         * @param request delete lambda result counters request
         * @return number of results deleted
         * @throws Core::ServiceException
         * @see Dto::Lambda::DeleteLambdaResultCountersRequest
         */
        [[nodiscard]] long DeleteLambdaResultCounters(const Dto::Lambda::DeleteLambdaResultCountersRequest &request) const;

        /**
         * @brief Starts the lambda function by starting a docker container
         *
         * @param request start lambda function request
         * @throws Core::ServiceException
         * @see Dto::Lambda::StartFunctionRequest
         */
        void StartLambda(const Dto::Lambda::StartLambdaRequest &request) const;

        /**
         * @brief Starts all lambda functions
         *
         * @throws Core::ServiceException
         */
        void StartAllLambdas() const;

        /**
         * @brief Stops the lambda function by stopping all running docker containers
         *
         * @param request stop lambda function request
         * @throws Core::ServiceException
         * @see Dto::Lambda::StopFunctionRequest
         */
        void StopLambda(const Dto::Lambda::StopLambdaRequest &request) const;

        /**
         * @brief Stops all lambda functions
         *
         * @throws Core::ServiceException
         */
        void StopAllLambdas() const;

        /**
         * @brief Stops the lambda instance
         *
         * @param request stop lambda instance request
         * @throws Core::ServiceException
         * @see Dto::Lambda::StopFunctionInstanceRequest
         */
        void StopLambdaInstance(const Dto::Lambda::StopLambdaInstanceRequest &request) const;

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
        void DeleteTags(const Dto::Lambda::DeleteTagsRequest &request) const;

        /**
         * @brief Signal for a lambda update code
         *
         * @tparam name of the lambda
         */
        boost::signals2::signal<void(std::string)> sigLambdaCodeUpdated;

      private:

        /**
         * @brief Tries to find an idle lambda function instance
         *
         * @par
         * Tries to find an idle instance. If no instance is found and the current total number of lambda instances is below the
         * concurrency limit, a new instance will be created, otherwise it will wait for an idle instance in a loop with 1 sec.
         * Period time.
         *
         * @param lambda lambda entity to check
         * @param instance
         * @return lambda instance
         */
        void FindIdleInstance(Database::Entity::Lambda::Lambda &lambda, Database::Entity::Lambda::Instance &instance) const;

        /**
         * @brief Stops all running instances and deleted any existing containers and images.
         *
         * @param lambda lambda entity to cleanup
         */
        static void CleanupDocker(Database::Entity::Lambda::Lambda &lambda);

        /**
         * @brief Waits in a  loop for an idle lambda instance
         *
         * @par
         * This is a blocking call, the calling function will be blocked until an idle instance is available. The maximal waiting
         * time is limited by the total timeout period of the lambda (i.e. 900sec.)
         *
         * @param lambda lambda entity to check
         * @return idle instance
         * @see Database::Entity::Lambda::Lambda
         */
        Database::Entity::Lambda::Instance WaitForIdleInstance(Database::Entity::Lambda::Lambda &lambda) const;

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
        [[nodiscard]] std::string GetLambdaCodeFromS3(const Database::Entity::Lambda::Lambda &lambda) const;

        /**
         * @brief Depending on the type, create a SQS notification configuration, SNS notification or S3 notification configuration
         *
         * @param request add event notification request
         */
        void CreateResourceNotification(const Dto::Lambda::AddEventSourceRequest &request) const;

        /**
         * @brief Writes the base64 ZIP file coming from the frontend to the local lambda data dir.
         *
         * @param base64File name of the file
         * @param content base64 encoded content
         */
        static void WriteBase64File(const std::string &base64File, const std::string &content);

        /**
         * Lambda database connection
         */
        Database::LambdaDatabase &_lambdaDatabase;

        /**
         * S3 database connection
         */
        Database::S3Database &_s3Database;

        /**
         * SQS database connection
         */
        Database::SQSDatabase &_sqsDatabase;

        /**
         * SQS database connection
         */
        Database::SNSDatabase &_snsDatabase;

        /**
         * Boost IO context
         */
        boost::asio::io_context &_ioc;

        /**
         * Lambda executor
         */
        LambdaExecutor lambdaExecutor;

        /**
         * Function mutexes
         */
        static std::map<std::string, std::shared_ptr<boost::mutex>> _instanceMutex;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_LAMBDA_SERVICE_H
