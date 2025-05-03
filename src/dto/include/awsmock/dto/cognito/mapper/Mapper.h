//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_COGNITO_MAPPER_H
#define AWSMOCK_DTO_COGNITO_MAPPER_H

// AwsMock includes
#include <awsmock/core/StringUtils.h>
#include <awsmock/dto/cognito/CreateGroupRequest.h>
#include <awsmock/dto/cognito/CreateGroupResponse.h>
#include <awsmock/dto/cognito/CreateUserPoolClientRequest.h>
#include <awsmock/dto/cognito/CreateUserPoolDomainRequest.h>
#include <awsmock/dto/cognito/DescribeUserPoolRequest.h>
#include <awsmock/dto/cognito/DescribeUserPoolResponse.h>
#include <awsmock/dto/cognito/ListGroupsRequest.h>
#include <awsmock/dto/cognito/ListGroupsResponse.h>
#include <awsmock/dto/cognito/ListUserPoolClientsRequest.h>
#include <awsmock/dto/cognito/ListUserPoolClientsResponse.h>
#include <awsmock/dto/cognito/ListUserPoolRequest.h>
#include <awsmock/dto/cognito/ListUserPoolResponse.h>
#include <awsmock/dto/cognito/ListUsersInGroupRequest.h>
#include <awsmock/dto/cognito/ListUsersInGroupResponse.h>
#include <awsmock/dto/cognito/model/User.h>
#include <awsmock/dto/cognito/model/UserCounter.h>
#include <awsmock/dto/cognito/model/UserPool.h>
#include <awsmock/dto/cognito/model/UserPoolCounter.h>
#include <awsmock/entity/cognito/Group.h>
#include <awsmock/entity/cognito/User.h>
#include <awsmock/entity/cognito/UserPool.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Maps an entity to the corresponding DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class Mapper {

      public:

        /**
         * @brief Maps a user pool entity to a user pool DTO
         *
         * @param userPoolEntity user pool entity
         * @return UserPool DTO
         */
        static UserPool map(const Database::Entity::Cognito::UserPool &userPoolEntity);

        /**
         * @brief Maps a list of user pool entities to a list of user pool DTOs
         *
         * @param userPoolEntities list of user pool entities
         * @return vector of UserPool DTOs
         */
        static std::vector<UserPool> map(const std::vector<Database::Entity::Cognito::UserPool> &userPoolEntities);

        /**
         * @brief Maps a list of user pool entities to a list of user pool DTOs
         *
         * @param userPoolEntities list of user pool entities
         * @return vector of UserPool counter DTOs
         */
        static std::vector<UserPoolCounter> mapCounter(const std::vector<Database::Entity::Cognito::UserPool> &userPoolEntities);

        /**
         * @brief Maps a user entity to a user DTO
         *
         * @param userEntity user entity
         * @return User DTO
         */
        static User map(const Database::Entity::Cognito::User &userEntity);

        /**
         * @brief Map a list of entities to DTOs
         *
         * @param userEntities user entities
         * @return user DTOs
         */
        static std::vector<UserCounter> map(const std::vector<Database::Entity::Cognito::User> &userEntities);

        /**
         * @brief Maps a group entity to a group DTO
         *
         * @param groupEntity group entity
         * @return Group DTO
         */
        static Group map(const Database::Entity::Cognito::Group &groupEntity);

        /**
         * @brief Maps a list of cognito user pools a list user pool response
         *
         * Some values will be pulled over from the request.
         *
         * @param request original request
         * @param userPoolList cognito user pool entity list
         * @param total total number of user pools
         * @return ListUserPoolResponse
         * @see ListUserPoolResponse
         */
        static ListUserPoolResponse map(const ListUserPoolRequest &request, const std::vector<Database::Entity::Cognito::UserPool> &userPoolList, long total);

        /**
         * @brief Maps a list of cognito user pool clients to a list user pool clients response
         *
         * Some values will be pulled over from the request.
         *
         * @param request original request
         * @param userPoolClient cognito user pool client entity
         * @return ListUserPoolClientsResponse
         * @see ListUserPoolClientsResponse
         */
        static ListUserPoolClientsResponse map(const ListUserPoolClientsRequest &request, const std::vector<Database::Entity::Cognito::UserPoolClient> &userPoolClient);

        /**
         * @brief Maps a user pool domain DTO to an entity
         *
         * @param request CreateUserPoolDomain request struct
         * @return UserPoolDomain
         * @see CreateUserPoolDomainRequest
         */
        static Database::Entity::Cognito::UserPoolDomain map(const CreateUserPoolDomainRequest &request);

        /**
         * @brief Maps a user pool client DTO to an entity
         *
         * @param request CreateUserPoolClientRequest request struct
         * @return UserPoolClient
         * @see CreateUserPoolClientRequest
         */
        static Database::Entity::Cognito::UserPoolClient map(const CreateUserPoolClientRequest &request);

        /**
         * @brief Maps a user pool client entity  to an DTO
         *
         * @param clientEntity user pool client entity request struct
         * @return UserPoolClient
         * @see UserPoolClient
         */
        static UserPoolClient map(const Database::Entity::Cognito::UserPoolClient &clientEntity);

        /**
         * @brief Maps a create group DTO request to an entity
         *
         * @param request CreateGroupRequest struct
         * @return Group entity
         * @see CreateGroupRequest
         */
        static Database::Entity::Cognito::Group map(const CreateGroupRequest &request);

        /**
         * @brief Maps a group entity to a CreateGroupResponse
         *
         * @param request original request
         * @param group group entity
         * @return CreateGroupResponse
         * @see CreateGroupResponse
         */
        static CreateGroupResponse map(const CreateGroupRequest &request, const Database::Entity::Cognito::Group &group);

        /**
         * @brief Maps a list of cognito groups to a list group response
         *
         * Some values will be pulled over from the request.
         *
         * @param request original request
         * @param groupList cognito user group entity list
         * @return ListGroupResponse
         * @see ListGroupResponse
         */
        static ListGroupsResponse map(const ListGroupsRequest &request, const std::vector<Database::Entity::Cognito::Group> &groupList);

        /**
         * @brief Maps a list of cognito users in a group to a list user in group response
         *
         * Some values will be pulled over from the request.
         *
         * @param request original request
         * @param userList cognito user entity list
         * @return ListUsersInGroupResponse
         * @see ListUsersInGroupResponse
         */
        static ListUsersInGroupResponse map(const ListUsersInGroupRequest &request, const std::vector<Database::Entity::Cognito::User> &userList);

        /**
         * @brief Maps a user pool to a describe user pool response
         *
         * Some values will be pulled over from the request.
         *
         * @param request original request
         * @param userPool cognito user pool entity
         * @return DescribeUserPoolResponse
         * @see DescribeUserPoolResponse
         */
        static DescribeUserPoolResponse map(const DescribeUserPoolRequest &request, const Database::Entity::Cognito::UserPool &userPool);

        /**
         * @brief Converts validity with validity units to seconds
         *
         * @param validity validity value
         * @param validityUnits validity unit
         * @return validity in seconds
         */
        static long GetValidityInSeconds(long validity, ValidityUnits validityUnits);

        /**
         * @brief Returns the maximum validity units
         *
         * @param validity
         * @return validity units
         */
        static ValidityUnits GetMaxTokenUnits(long validity);

        /**
         * @brief Returns the scaled validity
         *
         * @param validity validity value
         * @param validityUnits validity units
         * @return validity in hours, minutes, days, seconds
         */
        static long GetMaxValidityToken(long validity, ValidityUnits validityUnits);
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_MAPPER_H
