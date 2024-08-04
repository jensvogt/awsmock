//
// Created by vogje01 on 5/10/24.
//

#include <awsmock/dto/cognito/mapper/Mapper.h>

namespace AwsMock::Dto::Cognito {

    Dto::Cognito::ListUserPoolResponse Mapper::map(const ListUserPoolRequest &request, const std::vector<Database::Entity::Cognito::UserPool> &userPoolList) {

        Dto::Cognito::ListUserPoolResponse response;

        for (const auto &userPool: userPoolList) {
            response.userPools.emplace_back(Mapper::map(userPool));
        }
        return response;
    }

    Database::Entity::Cognito::UserPoolDomain Mapper::map(const Dto::Cognito::CreateUserPoolDomainRequest &request) {

        Database::Entity::Cognito::UserPoolDomain userPoolDomain;
        userPoolDomain.domain = request.domain;
        return userPoolDomain;
    }

    Database::Entity::Cognito::UserPoolClient Mapper::map(const Dto::Cognito::CreateUserPoolClientRequest &request) {

        Database::Entity::Cognito::UserPoolClient userPoolClient;
        userPoolClient.userPoolId = request.userPoolId;
        userPoolClient.clientId = Core::StringUtils::GenerateRandomString(26);
        userPoolClient.clientName = request.clientName;
        if (request.generateSecret) {
            userPoolClient.clientSecret = Core::StringUtils::GenerateRandomString(52);
        }
        userPoolClient.generateSecret = request.generateSecret;
        return userPoolClient;
    }

    Database::Entity::Cognito::Group Mapper::map(const Dto::Cognito::CreateGroupRequest &request) {
        Database::Entity::Cognito::Group group = {
                .region = request.region,
                .userPoolId = request.userPoolId,
                .groupName = request.groupName,
                .description = request.description,
                .precedence = request.precedence};
        return group;
    }

    Dto::Cognito::CreateGroupResponse Mapper::map(const CreateGroupRequest &request, const Database::Entity::Cognito::Group &group) {
        Dto::Cognito::CreateGroupResponse response = {{.requestId = request.requestId, .region = group.region, .user = request.user}, Mapper::map(group)};
        return response;
    }

    Dto::Cognito::ListGroupsResponse Mapper::map(const ListGroupsRequest &request, const std::vector<Database::Entity::Cognito::Group> &groupList) {

        Dto::Cognito::ListGroupsResponse response{{.requestId = request.requestId, .region = request.region, .user = request.user}};

        for (const auto &group: groupList) {
            response.groups.emplace_back(Mapper::map(group));
        }
        return response;
    }

    Dto::Cognito::ListUsersInGroupResponse Mapper::map(const ListUsersInGroupRequest &request, const std::vector<Database::Entity::Cognito::User> &userList) {

        Dto::Cognito::ListUsersInGroupResponse response{{.requestId = request.requestId, .region = request.region, .user = request.user}};

        for (const auto &user: userList) {
            response.users.emplace_back(Mapper::map(user));
        }
        return response;
    }

    Dto::Cognito::DescribeUserPoolResponse Mapper::map(const Dto::Cognito::DescribeUserPoolRequest &request, const Database::Entity::Cognito::UserPool &userPool) {
        DescribeUserPoolResponse response = {{request.requestId, request.region, request.user}};
        response.userPool = Mapper::map(userPool);
        return response;
    }

    Dto::Cognito::UserPool Mapper::map(const Database::Entity::Cognito::UserPool &userPoolEntity) {
        UserPool userPoolDto = {
                .id = userPoolEntity.userPoolId,
                .region = userPoolEntity.region,
                .name = userPoolEntity.name,
                .userPoolId = userPoolEntity.userPoolId,
                .arn = userPoolEntity.arn,
                .domain = userPoolEntity.domain.domain,
                .created = userPoolEntity.created,
                .modified = userPoolEntity.modified};
        return userPoolDto;
    }

    Dto::Cognito::User Mapper::map(const Database::Entity::Cognito::User &userEntity) {
        User userDto = {
                .region = userEntity.region,
                .userPoolId = userEntity.userPoolId,
                .userName = userEntity.userName,
                .enabled = userEntity.enabled,
                .created = userEntity.created,
                .modified = userEntity.modified,
        };

        for (const auto &group: userEntity.groups) {
            userDto.groups.emplace_back(Mapper::map(group));
        }

        return userDto;
    }

    Dto::Cognito::Group Mapper::map(const Database::Entity::Cognito::Group &groupEntity) {
        Group groupDto = {
                .region = groupEntity.region,
                .groupName = groupEntity.groupName,
                .userPoolId = groupEntity.userPoolId,
                .precedence = groupEntity.precedence,
                .created = groupEntity.created,
                .modified = groupEntity.modified};
        return groupDto;
    }

}// namespace AwsMock::Dto::Cognito