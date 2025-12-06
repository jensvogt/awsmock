//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_SSM_SERVICE_H
#define AWSMOCK_SERVICE_SSM_SERVICE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/monitoring/MonitoringTimer.h>
#include <awsmock/dto/common/mapper/Mapper.h>
#include <awsmock/dto/ssm/DeleteParameterRequest.h>
#include <awsmock/dto/ssm/DescribeParametersRequest.h>
#include <awsmock/dto/ssm/DescribeParametersResponse.h>
#include <awsmock/dto/ssm/GetParameterRequest.h>
#include <awsmock/dto/ssm/GetParameterResponse.h>
#include <awsmock/dto/ssm/PutParameterRequest.h>
#include <awsmock/dto/ssm/PutParameterResponse.h>
#include <awsmock/dto/ssm/internal/CreateParameterCounterRequest.h>
#include <awsmock/dto/ssm/internal/DeleteParameterCounterRequest.h>
#include <awsmock/dto/ssm/internal/GetParameterCounterRequest.h>
#include <awsmock/dto/ssm/internal/GetParameterCounterResponse.h>
#include <awsmock/dto/ssm/internal/ListParameterCountersRequest.h>
#include <awsmock/dto/ssm/internal/ListParameterCountersResponse.h>
#include <awsmock/dto/ssm/internal/UpdateParameterCounterRequest.h>
#include <awsmock/dto/ssm/mapper/Mapper.h>
#include <awsmock/repository/SSMDatabase.h>
#include <awsmock/service/kms/KMSService.h>
#include <awsmock/service/monitoring/MetricService.h>

#define DEFAULT_SSM_ACCOUNT_ID "000000000000"

namespace AwsMock::Service {

    using std::chrono::system_clock;

    /**
     * @brief SSM service
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SSMService {

      public:

        /**
         * @brief Constructor
         */
        explicit SSMService();

        /**
         * @brief Creates a new parameter
         *
         * @param request put parameter request
         * @return PutParameterResponse
         * @see Dto::SSM::PutParameterRequest
         * @see Dto::SSM::PutParameterResponse
         */
        [[nodiscard]] Dto::SSM::PutParameterResponse PutParameter(const Dto::SSM::PutParameterRequest &request) const;

        /**
         * @brief Returns a parameter
         *
         * @param request get parameter request
         * @return GetParameterResponse
         * @see Dto::SSM::GetParameterRequest
         * @see Dto::SSM::GetParameterResponse
         */
        [[nodiscard]] Dto::SSM::GetParameterResponse GetParameter(const Dto::SSM::GetParameterRequest &request) const;

        /**
         * @brief Returns a parameter counter
         *
         * @param request get parameter request
         * @return GetParameterResponse
         * @see Dto::SSM::GetParameterRequest
         * @see Dto::SSM::GetParameterResponse
         */
        [[nodiscard]] Dto::SSM::GetParameterCounterResponse GetParameterCounter(const Dto::SSM::GetParameterCounterRequest &request) const;

        /**
         * @brief Describe the parameters
         *
         * @param request describe parameters request
         * @return describe parameters response
         * @see Dto::SSM::DescribeParametersRequest
         * @see Dto::SSM::DescribeParametersResponse
         */
        [[nodiscard]] Dto::SSM::DescribeParametersResponse DescribeParameters(const Dto::SSM::DescribeParametersRequest &request) const;

        /**
         * @brief List all parameter counters
         *
         * @param request list parameter counters request
         * @return list parameter counters response
         * @see Dto::SSM::ListParameterCountersRequest
         * @see Dto::SSM::ListParameterCountersResponse
         */
        [[nodiscard]] Dto::SSM::ListParameterCountersResponse ListParameterCounters(const Dto::SSM::ListParameterCountersRequest &request) const;

        /**
         * @brief Create a new parameter
         *
         * @param request create parameter request
         * @return list parameter counters response
         * @see Dto::SSM::CreateParameterCounterRequest
         * @see Dto::SSM::ListParameterCountersResponse
         */
        [[nodiscard]] Dto::SSM::ListParameterCountersResponse CreateParameter(const Dto::SSM::CreateParameterCounterRequest &request) const;

        /**
         * @brief Update a parameter
         *
         * @param request update parameter request
         * @return list parameter counters response
         * @see Dto::SSM::CreateParameterCounterRequest
         * @see Dto::SSM::ListParameterCountersResponse
         */
        [[nodiscard]] Dto::SSM::ListParameterCountersResponse UpdateParameter(const Dto::SSM::UpdateParameterCounterRequest &request) const;

        /**
         * @brief Deletes a parameter
         *
         * @param request delete parameter request
         * @see Dto::SSM::DeleteParameterRequest
         */
        void DeleteParameter(const Dto::SSM::DeleteParameterRequest &request) const;

        /**
         * @brief Deletes a parameter from the frontend
         *
         * @param request delete parameter request
         * @see Dto::SSM::DeleteParameterRequest
         */
        [[nodiscard]] Dto::SSM::ListParameterCountersResponse DeleteParameterCounter(const Dto::SSM::DeleteParameterCounterRequest &request) const;

      private:

        /**
         * Account ID
         */
        std::string _accountId;

        /**
         * Database connection
         */
        Database::SSMDatabase &_ssmDatabase;

        /**
         * KMS service
         */
        KMSService _kmsService;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_KMS_SERVICE_H
