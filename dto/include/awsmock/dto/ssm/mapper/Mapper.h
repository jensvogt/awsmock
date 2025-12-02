//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_SSM_MAPPER_H
#define AWSMOCK_DTO_SSM_MAPPER_H

// AwsMock includes
#include <awsmock/dto/ssm/DescribeParametersRequest.h>
#include <awsmock/dto/ssm/DescribeParametersResponse.h>
#include <awsmock/dto/ssm/GetParameterRequest.h>
#include <awsmock/dto/ssm/GetParameterResponse.h>
#include <awsmock/dto/ssm/internal/GetParameterCounterRequest.h>
#include <awsmock/dto/ssm/internal/GetParameterCounterResponse.h>
#include <awsmock/dto/ssm/internal/ListParameterCountersRequest.h>
#include <awsmock/dto/ssm/internal/ListParameterCountersResponse.h>
#include <awsmock/dto/ssm/model/ParameterType.h>
#include <awsmock/entity/ssm/Parameter.h>

namespace AwsMock::Dto::SSM {

    /**
     * @brief Maps an entity to the corresponding DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class Mapper {

      public:

        /**
         * @brief Maps a single parameter
         *
         * @param parameterEntity parameter entity
         * @return parameter DTO
         */
        static Parameter map(const Database::Entity::SSM::Parameter &parameterEntity);

        /**
         * @brief Maps a list of parameters
         *
         * @param parameterEntities parameter entity list
         * @return parameter DTO
         */
        static std::vector<Parameter> map(const std::vector<Database::Entity::SSM::Parameter> &parameterEntities);

        /**
         * @brief Maps an SSM parameter entity to a parameter response
         *
         * @par
         * Some values will be pulled over from the request.
         *
         * @param parameter SSM parameter
         * @param request request struct
         * @return GetParameterResponse
         * @see GetParameterResponse
         */
        static GetParameterResponse map(const GetParameterRequest &request, const Database::Entity::SSM::Parameter &parameter);

        /**
         * @brief Maps an SSM parameter entity to a parameter response
         *
         * @par
         * Some values will be pulled over from the request.
         *
         * @param parameter SSM parameter
         * @param request request struct
         * @return GetParameterResponse
         * @see GetParameterResponse
         */
        static GetParameterCounterResponse map(const GetParameterCounterRequest &request, const Database::Entity::SSM::Parameter &parameter);

        /**
         * @brief Maps a list of SSM parameter entities to a parameter describe response
         *
         * Some values will be pulled over from the request.
         *
         * @param request request struct
         * @param parameterEntities SSM parameter entity list
         * @return ListObjectVersionsResponse
         * @see ListObjectVersionsResponse
         */
        static DescribeParametersResponse map(const DescribeParametersRequest &request, const std::vector<Database::Entity::SSM::Parameter> &parameterEntities);

        /**
         * @brief Maps a list of SSM parameter entities to a parameter list response
         *
         * Some values will be pulled over from the request.
         *
         * @param request request struct
         * @param parameterEntities SSM parameter entity list
         * @return ListParameterCountersResponse
         * @see ListParameterCountersResponse
         */
        static ListParameterCountersResponse map(const ListParameterCountersRequest &request, const std::vector<Database::Entity::SSM::Parameter> &parameterEntities);
    };

}// namespace AwsMock::Dto::SSM

#endif// AWSMOCK_DTO_S3_MAPPER_H
