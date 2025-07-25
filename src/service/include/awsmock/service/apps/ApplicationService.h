//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_APPLICATION_SERVICE_H
#define AWSMOCK_SERVICE_APPLICATION_SERVICE_H

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/dto/apps/internal/CreateApplicationRequest.h>
#include <awsmock/dto/apps/internal/CreateApplicationResponse.h>
#include <awsmock/dto/apps/internal/DeleteApplicationRequest.h>
#include <awsmock/dto/apps/internal/GetApplicationRequest.h>
#include <awsmock/dto/apps/internal/GetApplicationResponse.h>
#include <awsmock/dto/apps/internal/ListApplicationCountersRequest.h>
#include <awsmock/dto/apps/internal/ListApplicationCountersResponse.h>
#include <awsmock/dto/apps/internal/RebuildApplicationRequest.h>
#include <awsmock/dto/apps/internal/RestartApplicationRequest.h>
#include <awsmock/dto/apps/internal/StartApplicationRequest.h>
#include <awsmock/dto/apps/internal/StopApplicationRequest.h>
#include <awsmock/dto/apps/internal/UpdateApplicationRequest.h>
#include <awsmock/dto/apps/internal/UploadApplicationCodeRequest.h>
#include <awsmock/dto/apps/mapper/Mapper.h>
#include <awsmock/repository/ApplicationDatabase.h>
#include <awsmock/service/monitoring/MetricDefinition.h>
#include <awsmock/service/monitoring/MetricServiceTimer.h>

namespace AwsMock::Service {

    using std::chrono::system_clock;

    /**
     * @brief Application service
     *
     * @par
     * Handles all application requests finally.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ApplicationService {

      public:

        /**
         * @brief Constructor
         */
        explicit ApplicationService();

        /**
         * @brief Create a new application
         *
         * @param request create application request
         * @return ListApplicationCountersResponse DTO
         * @see Dto::Apps::CreateApplicationRequest
         * @see Dto::Apps::ListApplicationCountersResponse
         */
        [[nodiscard]] Dto::Apps::ListApplicationCountersResponse CreateApplication(const Dto::Apps::CreateApplicationRequest &request) const;

        /**
         * @brief Get an application
         *
         * @param request get application request
         * @return GetApplicationResponse DTO
         * @see Dto::Apps::GetApplicationRequest
         * @see Dto::Apps::GetApplicationResponse
         */
        [[nodiscard]] Dto::Apps::GetApplicationResponse GetApplication(const Dto::Apps::GetApplicationRequest &request) const;

        /**
         * @brief Start an application
         *
         * @param request start application request
         * @see Dto::Apps::StartApplicationRequest
         */
        //void StartApplication(const Dto::Apps::StopApplicationRequest &request) const;

        /**
         * @brief Update an application
         *
         * @param request update application request
         * @return GetApplicationResponse DTO
         * @see Dto::Apps::UpdateApplicationRequest
         * @see Dto::Apps::GetApplicationResponse
         */
        [[nodiscard]] Dto::Apps::GetApplicationResponse UpdateApplication(const Dto::Apps::UpdateApplicationRequest &request) const;

        /**
         * @brief Upload application code
         *
         * @param request upload application code request
         * @see Dto::Apps::UploadApplicationCode
         */
        void UploadApplicationCode(const Dto::Apps::UploadApplicationCodeRequest &request) const;

        /**
         * @brief List all applications
         *
         * @param request list application request
         * @return ListApplicationCountersResponse DTO
         * @see Dto::Apps::ListApplicationCountersRequest
         * @see Dto::Apps::ListApplicationCountersResponse
         */
        [[nodiscard]] Dto::Apps::ListApplicationCountersResponse ListApplications(const Dto::Apps::ListApplicationCountersRequest &request) const;


        /**
         * @brief List all application names
         *
         * @return list of strings
         */
        [[nodiscard]] std::vector<std::string> ListApplicationNames() const;

        /**
         * @brief Starts an application
         *
         * @param request start application request
         * @return ListApplicationCountersResponse DTO
         * @see Dto::Apps::StartApplicationRequest
         * @see Dto::Apps::ListApplicationCountersResponse
         */
        [[nodiscard]] Dto::Apps::ListApplicationCountersResponse StartApplication(const Dto::Apps::StartApplicationRequest &request) const;

        /**
         * @brief Stops an application
         *
         * @param request stop application request
         * @return ListApplicationCountersResponse DTO
         * @see Dto::Apps::StopApplicationRequest
         * @see Dto::Apps::ListApplicationCountersResponse
         */
        [[nodiscard]] Dto::Apps::ListApplicationCountersResponse StopApplication(const Dto::Apps::StopApplicationRequest &request) const;

        /**
         * @brief Restarts an application
         *
         * @par
         * This method will stop the container, delete the container and will rebuild the container from the corresponding image.
         *
         * @param request restart application request
         * @return ListApplicationCountersResponse DTO
         * @see Dto::Apps::RestartApplicationRequest
         * @see Dto::Apps::ListApplicationCountersResponse
         */
        [[nodiscard]] Dto::Apps::ListApplicationCountersResponse RestartApplication(const Dto::Apps::RestartApplicationRequest &request) const;

        /**
         * @brief Rebuilds an application
         *
         * @par
         * If a container is currently running, it will be stopped and removed. Also, the image will be removed first. Afterwards, the image is recreated and
         * a new container is started with the new image.
         *
         * @param request rebuild application request
         * @return ListApplicationCountersResponse DTO
         * @see Dto::Apps::RebuildApplicationCodeRequest
         * @see Dto::Apps::ListApplicationCountersResponse
         */
        [[nodiscard]] Dto::Apps::ListApplicationCountersResponse RebuildApplication(const Dto::Apps::RebuildApplicationCodeRequest &request) const;

        /**
         * @brief Deletes an application
         *
         * @param request delete application request
         * @return ListApplicationCountersResponse DTO
         * @see Dto::Apps::DeleteApplicationRequest
         * @see Dto::Apps::ListApplicationCountersResponse
         */
        [[nodiscard]] Dto::Apps::ListApplicationCountersResponse DeleteApplication(const Dto::Apps::DeleteApplicationRequest &request) const;

      private:

        /**
         * @brief Saves the Base64 file
         *
         * @param applicationCode base64 encoded application code
         * @param application application entity
         * @param version application version
         */
        static std::string WriteBase64File(const std::string &applicationCode, Database::Entity::Apps::Application &application, const std::string &version);

        /**
         * @brief Stop any container, removes container and deletes the image
         *
         * @param application application entity
         */
        static void DeleteImage(const Database::Entity::Apps::Application &application);

        /**
         * Database connection
         */
        Database::ApplicationDatabase &_database;

        /**
         * AWS account userPoolId
         */
        std::string _accountId;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_APPLICATION_SERVICE_H
