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
        Dto::Apps::ListApplicationCountersResponse CreateApplication(const Dto::Apps::CreateApplicationRequest &request) const;

        /**
         * @brief Get an application
         *
         * @param request get application request
         * @return GetApplicationResponse DTO
         * @see Dto::Apps::GetApplicationRequest
         * @see Dto::Apps::GetApplicationResponse
         */
        Dto::Apps::GetApplicationResponse GetApplication(const Dto::Apps::GetApplicationRequest &request) const;

        /**
         * @brief List all applications
         *
         * @param request list application request
         * @return ListApplicationCountersResponse DTO
         * @see Dto::Apps::ListApplicationCountersRequest
         * @see Dto::Apps::ListApplicationCountersResponse
         */
        Dto::Apps::ListApplicationCountersResponse ListApplications(const Dto::Apps::ListApplicationCountersRequest &request) const;

        /**
         * @brief Deletes an application
         *
         * @param request delete application request
         * @return ListApplicationCountersResponse DTO
         * @see Dto::Apps::DeleteApplicationRequest
         * @see Dto::Apps::ListApplicationCountersResponse
         */
        Dto::Apps::ListApplicationCountersResponse DeleteApplication(const Dto::Apps::DeleteApplicationRequest &request) const;

      private:

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
