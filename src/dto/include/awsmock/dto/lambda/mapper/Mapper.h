//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_LAMBDA_MAPPER_H
#define AWSMOCK_DTO_LAMBDA_MAPPER_H

// AwsMock includes
#include <awsmock/dto/lambda/CreateEventSourceMappingsRequest.h>
#include <awsmock/dto/lambda/CreateFunctionRequest.h>
#include <awsmock/dto/lambda/CreateFunctionResponse.h>
#include <awsmock/dto/lambda/ListEventSourceMappingsResponse.h>
#include <awsmock/dto/lambda/internal/GetLambdaResultCounterResponse.h>
#include <awsmock/dto/lambda/internal/ListFunctionCountersRequest.h>
#include <awsmock/dto/lambda/internal/ListFunctionCountersResponse.h>
#include <awsmock/dto/lambda/internal/ListLambdaEventSourceCountersResponse.h>
#include <awsmock/dto/lambda/internal/ListLambdaResultCountersResponse.h>
#include <awsmock/dto/lambda/model/Function.h>
#include <awsmock/dto/lambda/model/FunctionCounter.h>
#include <awsmock/dto/lambda/model/LambdaResult.h>
#include <awsmock/entity/lambda/Lambda.h>
#include <awsmock/entity/lambda/LambdaResult.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Maps an entity to the corresponding DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class Mapper {

      public:

        /**
         * @brief Maps a lambda function entity to a lambda function DTO
         *
         * @param lambdaEntity lambda function entity
         * @return lambda function DTO
         */
        static Function mapFunction(const Database::Entity::Lambda::Lambda &lambdaEntity);

        /**
         * @brief Maps a lambda entity to a create function response
         *
         * Some values will be pulled over from the request.
         *
         * @param lambdaEntity lambda entity
         * @param request lambda create function request struct
         * @return CreateFunctionResponse
         * @see CreateFunctionResponse
         */
        static CreateFunctionResponse map(const CreateFunctionRequest &request, const Database::Entity::Lambda::Lambda &lambdaEntity);

        /**
         * @brief Maps a lambda entity to a create function request.
         *
         * Some values will be pulled over from the request. This is needed, as the server starts all lambdas during startup and needs to convert the lambda entities
         * stored in the database to a CreateLambdaRequest, which ís sent to the lambda service.
         *
         * @param lambdaEntity lambda entity
         * @return CreateFunctionRequest
         * @see CreateFunctionRequest
         */
        static CreateFunctionRequest map(const Database::Entity::Lambda::Lambda &lambdaEntity);

        /**
         * @brief Maps a lambda create request to a lambda entity
         *
         * @param request create lambda request
         * @return lambda entity
         * @see CreateFunctionRequest
         */
        static Database::Entity::Lambda::Lambda map(const CreateFunctionRequest &request);

        /**
         * @brief Maps a lambda create eventsource mappings request request to a lambda event source mapping
         *
         * @param request create lambda request
         * @return lambda event source mapping
         * @see CreateEventSourceMappingsRequest
         * @see EventSourceMapping
         */
        static Database::Entity::Lambda::EventSourceMapping map(const CreateEventSourceMappingsRequest &request);

        /**
         * @brief Maps a list of lambda event source mappings to a lambda event source mapping response.
         *
         * @param eventSourceMappings list of event source mapping entities
         * @return lambda event source mapping list response
         * @see ListEventSourceMappingsRequest
         * @see EventSourceMapping
         */
        static ListEventSourceMappingsResponse map(const std::vector<Database::Entity::Lambda::EventSourceMapping> &eventSourceMappings);

        /**
         * @brief Maps a list of lambda entities to list lambda counters response
         *
         * @param lambdaEntities lambda entities
         * @return ListFunctionCountersResponse
         * @see ListFunctionCountersRequest
         * @see ListFunctionCountersResponse
         */
        static ListFunctionCountersResponse map(const std::vector<Database::Entity::Lambda::Lambda> &lambdaEntities);

        /**
         * @brief Maps a list of lambda result entities to a list lambda result counters response
         *
         * @param lambdaResultEntities lambda result entities
         * @return lambda result counters response
         */
        static ListLambdaResultCountersResponse map(const std::vector<Database::Entity::Lambda::LambdaResult> &lambdaResultEntities);

        /**
         * @brief Maps a lambda result entities to a lambda result counters response
         *
         * @param resultEntity lambda result entities
         * @return lambda result counters response
         */
        static GetLambdaResultCounterResponse map(const Database::Entity::Lambda::LambdaResult &resultEntity);

        /**
         * @brief Maps a lambda result entities to a lambda result counters response
         *
         * @param resultEntity lambda result entities
         * @return lambda result counters response
         */
        static LambdaResultCounter mapCounter(const Database::Entity::Lambda::LambdaResult &resultEntity);

        /**
         * @brief Maps a lambda event source entity to an event source DTO
         *
         * @param functionArn ARN of the lambda function
         * @param eventSourceMappings event source mapping entity
         * @return event source mapping DTO
         */
        static EventSourceMapping map(const std::string &functionArn, const Database::Entity::Lambda::EventSourceMapping &eventSourceMappings);

        /**
         * @brief Maps a list of lambda event source entity to a list of event source DTO
         *
         * @param functionArn ARN of the lambda function
         * @param eventSourceMappingEntities list of event source mapping entities
         * @return list of event source mapping DTOs
         */
        static std::vector<EventSourceMapping> mapCounters(const std::string &functionArn, const std::vector<Database::Entity::Lambda::EventSourceMapping> &eventSourceMappingEntities);

        /**
         * @brief Maps a lambda result
         *
         * @param resultEntity result entity
         * @return lambda result DTO
         */
        static LambdaResult mapResult(const Database::Entity::Lambda::LambdaResult &resultEntity);
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_MAPPER_H
