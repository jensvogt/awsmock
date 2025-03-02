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
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/dto/ssm/DeleteParameterRequest.h>
#include <awsmock/dto/ssm/DescribeParametersRequest.h>
#include <awsmock/dto/ssm/DescribeParametersResponse.h>
#include <awsmock/dto/ssm/GetParameterRequest.h>
#include <awsmock/dto/ssm/GetParameterResponse.h>
#include <awsmock/dto/ssm/PutParameterRequest.h>
#include <awsmock/dto/ssm/PutParameterResponse.h>
#include <awsmock/dto/ssm/mapper/Mapper.h>
#include <awsmock/repository/SSMDatabase.h>
#include <awsmock/service/monitoring/MetricDefinition.h>
#include <awsmock/service/monitoring/MetricService.h>
#include <awsmock/service/monitoring/MetricServiceTimer.h>

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
         * @see Dto::KMS::PutParameterRequest
         * @see Dto::KMS::PutParameterResponse
         */
        Dto::SSM::PutParameterResponse PutParameter(const Dto::SSM::PutParameterRequest &request) const;

        /**
         * @brief Returns a new parameter
         *
         * @param request get parameter request
         * @return GetParameterResponse
         * @see Dto::KMS::GetParameterRequest
         * @see Dto::KMS::GetParameterResponse
         */
        Dto::SSM::GetParameterResponse GetParameter(const Dto::SSM::GetParameterRequest &request) const;

        /**
         * @brief Describe the parameters
         *
         * @param request describe parameters request
         * @return describe parameters response
         * @see Dto::KMS::DescribeParametersRequest
         * @see Dto::KMS::DescribeParametersResponse
         */
        Dto::SSM::DescribeParametersResponse DescribeParameters(const Dto::SSM::DescribeParametersRequest &request) const;

        /**
         * @brief Deletes a parameter
         *
         * @param request delete parameter request
         * @see Dto::KMS::DeleteParameterRequest
         */
        void DeleteParameter(const Dto::SSM::DeleteParameterRequest &request) const;

      private:

        /**
         * Account ID
         */
        std::string _accountId;

        /**
         * Database connection
         */
        Database::SSMDatabase &_ssmDatabase;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_KMS_SERVICE_H
