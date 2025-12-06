//
// Created by vogje01 on 5/10/24.
//

#include <awsmock/dto/ssm/mapper/Mapper.h>

namespace AwsMock::Dto::SSM {

    Parameter Mapper::map(const Database::Entity::SSM::Parameter &parameterEntity) {

        Parameter responseDto;
        responseDto.region = parameterEntity.region;
        responseDto.name = parameterEntity.parameterName;
        responseDto.parameterValue = parameterEntity.parameterValue;
        responseDto.description = parameterEntity.description;
        responseDto.arn = parameterEntity.arn;
        responseDto.tier = parameterEntity.tier;
        responseDto.type = ParameterTypeFromString(parameterEntity.type);
        responseDto.kmsKeyArn = parameterEntity.kmsKeyArn;
        responseDto.created = parameterEntity.created;
        responseDto.modified = parameterEntity.modified;
        if (!parameterEntity.tags.empty()) {
            for (const auto &[fst, snd]: parameterEntity.tags) {
                responseDto.tags[fst] = snd;
            }
        }
        return responseDto;
    }

    std::vector<Parameter> Mapper::map(const std::vector<Database::Entity::SSM::Parameter> &parameterEntities) {

        std::vector<Parameter> parameters;
        for (const auto &parameterEntity: parameterEntities) {
            parameters.emplace_back(map(parameterEntity));
        }
        return parameters;
    }

    GetParameterResponse Mapper::map(const GetParameterRequest &request, const Database::Entity::SSM::Parameter &parameterEntity) {

        GetParameterResponse response;
        response.region = request.region;
        response.parameter = map(parameterEntity);
        return response;
    }

    GetParameterCounterResponse Mapper::map(const GetParameterCounterRequest &request, const Database::Entity::SSM::Parameter &parameterEntity) {

        GetParameterCounterResponse response;
        response.region = request.region;
        response.parameter = map(parameterEntity);
        return response;
    }

    DescribeParametersResponse Mapper::map(const DescribeParametersRequest &request, const std::vector<Database::Entity::SSM::Parameter> &parameterEntities) {
        DescribeParametersResponse response;
        response.region = request.region;
        for (const auto &parameterEntity: parameterEntities) {
            Parameter parameter = map(parameterEntity);
            response.parameters.emplace_back(parameter);
        }
        return response;
    }

    ListParameterCountersResponse Mapper::map(const ListParameterCountersRequest &request, const std::vector<Database::Entity::SSM::Parameter> &parameterEntities) {
        ListParameterCountersResponse response;
        response.region = request.region;
        for (const auto &parameterEntity: parameterEntities) {
            Parameter parameter = map(parameterEntity);
            response.parameterCounters.emplace_back(parameter);
        }
        return response;
    }

}// namespace AwsMock::Dto::SSM