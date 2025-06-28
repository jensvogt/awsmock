//
// Created by vogje01 on 5/10/24.
//

#include <awsmock/dto/cognito/mapper/Mapper.h>

namespace AwsMock::Dto::Cognito {

    ListUserPoolResponse Mapper::map(const ListUserPoolRequest &request, const std::vector<Database::Entity::Cognito::UserPool> &userPoolList, const long total) {

        ListUserPoolResponse response;
        response.requestId = request.requestId;
        response.region = request.region;
        response.user = request.user;
        response.total = total;
        for (const auto &userPool: userPoolList) {
            response.userPools.emplace_back(map(userPool));
        }
        return response;
    }

    Database::Entity::Cognito::UserPoolDomain Mapper::map(const CreateUserPoolDomainRequest &request) {

        Database::Entity::Cognito::UserPoolDomain userPoolDomain;
        userPoolDomain.domain = request.domain;
        return userPoolDomain;
    }

    Database::Entity::Cognito::UserPoolClient Mapper::map(const CreateUserPoolClientRequest &request) {

        Database::Entity::Cognito::UserPoolClient userPoolClient;
        userPoolClient.userPoolId = request.userPoolId;
        userPoolClient.clientId = Core::StringUtils::GenerateRandomString(26);
        userPoolClient.clientName = request.clientName;
        userPoolClient.accessTokenValidity = GetValidityInSeconds(request.accessTokenValidity, request.tokenValidityUnits.accessToken);
        userPoolClient.idTokenValidity = GetValidityInSeconds(request.idTokenValidity, request.tokenValidityUnits.idToken);
        userPoolClient.refreshTokenValidity = GetValidityInSeconds(request.refreshTokenValidity, request.tokenValidityUnits.refreshToken);
        userPoolClient.generateSecret = request.generateSecret;

        return userPoolClient;
    }

    Database::Entity::Cognito::Group Mapper::map(const CreateGroupRequest &request) {
        Database::Entity::Cognito::Group group;
        group.region = request.region;
        group.userPoolId = request.userPoolId;
        group.groupName = request.groupName;
        group.description = request.description;
        group.precedence = request.precedence;
        return group;
    }

    CreateGroupResponse Mapper::map(const CreateGroupRequest &request, const Database::Entity::Cognito::Group &group) {
        CreateGroupResponse response;
        response.group = map(group);
        response.requestId = request.requestId;
        response.region = request.region;
        response.user = request.user;
        return response;
    }

    ListGroupsResponse Mapper::map(const ListGroupsRequest &request, const std::vector<Database::Entity::Cognito::Group> &groupList) {

        ListGroupsResponse response;
        response.requestId = request.requestId;
        response.region = request.region;
        response.user = request.user;

        for (const auto &group: groupList) {
            response.groups.emplace_back(map(group));
        }
        return response;
    }

    ListUsersInGroupResponse Mapper::map(const ListUsersInGroupRequest &request, const std::vector<Database::Entity::Cognito::User> &userList) {

        ListUsersInGroupResponse response;
        response.requestId = request.requestId;
        response.region = request.region;
        response.user = request.user;

        for (const auto &user: userList) {
            response.users.emplace_back(map(user));
        }
        return response;
    }

    DescribeUserPoolResponse Mapper::map(const DescribeUserPoolRequest &request, const Database::Entity::Cognito::UserPool &userPool) {
        DescribeUserPoolResponse response;
        response.requestId = request.requestId;
        response.region = request.region;
        response.user = request.user;
        response.userPool = map(userPool);
        return response;
    }

    UserPool Mapper::map(const Database::Entity::Cognito::UserPool &userPoolEntity) {
        UserPool userPoolDto;
        userPoolDto.user = userPoolEntity.userPoolId;
        userPoolDto.region = userPoolEntity.region;
        userPoolDto.name = userPoolEntity.name;
        userPoolDto.created = userPoolEntity.created;
        userPoolDto.modified = userPoolEntity.modified;
        return userPoolDto;
    }

    std::vector<UserPool> Mapper::map(const std::vector<Database::Entity::Cognito::UserPool> &userPoolEntities) {
        std::vector<UserPool> userPoolDtos;
        for (const auto &userPoolEntity: userPoolEntities) {
            userPoolDtos.emplace_back(map(userPoolEntity));
        }
        return userPoolDtos;
    }

    std::vector<UserPoolCounter> Mapper::mapCounter(const std::vector<Database::Entity::Cognito::UserPool> &userPoolEntities) {
        std::vector<UserPoolCounter> userPoolCounterDtos;
        for (const auto &userPoolEntity: userPoolEntities) {
            UserPoolCounter userPoolCounterDto;
            userPoolCounterDto.id = userPoolEntity.userPoolId,
            userPoolCounterDto.region = userPoolEntity.region,
            userPoolCounterDto.name = userPoolEntity.name,
            userPoolCounterDto.userPoolId = userPoolEntity.userPoolId,
            userPoolCounterDto.arn = userPoolEntity.arn,
            userPoolCounterDto.domain = userPoolEntity.domain.domain,
            userPoolCounterDto.created = userPoolEntity.created,
            userPoolCounterDto.modified = userPoolEntity.modified;
            userPoolCounterDtos.emplace_back(userPoolCounterDto);
        }
        return userPoolCounterDtos;
    }

    User Mapper::map(const Database::Entity::Cognito::User &userEntity) {
        User userDto;
        userDto.region = userEntity.region;
        userDto.userPoolId = userEntity.userPoolId;
        userDto.userName = userEntity.userName;
        userDto.enabled = userEntity.enabled;
        userDto.created = userEntity.created;
        userDto.modified = userEntity.modified;
        for (const auto &group: userEntity.groups) {
            userDto.groups.emplace_back(map(group));
        }

        return userDto;
    }

    std::vector<UserCounter> Mapper::map(const std::vector<Database::Entity::Cognito::User> &userEntities) {
        std::vector<UserCounter> userDtos;
        for (const auto &userEntity: userEntities) {
            UserCounter userDto;
            userDto.region = userEntity.region;
            userDto.userPoolId = userEntity.userPoolId;
            userDto.userName = userEntity.userName;
            userDto.enabled = userEntity.enabled;
            userDto.userStatus = userEntity.userStatus;
            userDto.password = userEntity.password;
            userDto.created = userEntity.created;
            userDto.modified = userEntity.modified;

            for (const auto &group: userEntity.groups) {
                userDto.groups.emplace_back(map(group));
            }
            userDtos.emplace_back(userDto);
        }
        return userDtos;
    }

    Group Mapper::map(const Database::Entity::Cognito::Group &groupEntity) {
        Group groupDto;
        groupDto.region = groupEntity.region;
        groupDto.groupName = groupEntity.groupName;
        groupDto.userPoolId = groupEntity.userPoolId;
        groupDto.precedence = groupEntity.precedence;
        groupDto.created = groupEntity.created;
        groupDto.modified = groupEntity.modified;
        return groupDto;
    }

    UserPoolClient Mapper::map(const Database::Entity::Cognito::UserPoolClient &clientEntity) {
        TokenValidityUnits tokenValidityUnits;
        tokenValidityUnits.accessToken = GetMaxTokenUnits(clientEntity.accessTokenValidity);
        tokenValidityUnits.refreshToken = GetMaxTokenUnits(clientEntity.refreshTokenValidity);
        tokenValidityUnits.idToken = GetMaxTokenUnits(clientEntity.idTokenValidity);

        UserPoolClient userPoolClient;
        userPoolClient.userPoolId = clientEntity.userPoolId;
        userPoolClient.clientId = clientEntity.clientId;
        userPoolClient.clientName = clientEntity.clientName;
        userPoolClient.clientSecret = clientEntity.clientSecret;
        userPoolClient.accessTokenValidity = GetMaxValidityToken(clientEntity.accessTokenValidity, tokenValidityUnits.accessToken);
        userPoolClient.idTokenValidity = GetMaxValidityToken(clientEntity.idTokenValidity, tokenValidityUnits.idToken);
        userPoolClient.refreshTokenValidity = GetMaxValidityToken(clientEntity.refreshTokenValidity, tokenValidityUnits.refreshToken);
        userPoolClient.created = clientEntity.created;
        userPoolClient.modified = clientEntity.modified;
        return userPoolClient;
    }

    ListUserPoolClientsResponse Mapper::map(const ListUserPoolClientsRequest &request, const std::vector<Database::Entity::Cognito::UserPoolClient> &userPoolClients) {

        ListUserPoolClientsResponse response;

        for (const auto &userPoolClient: userPoolClients) {
            response.userPoolsClients.emplace_back(map(userPoolClient));
        }
        return response;
    }

    long Mapper::GetValidityInSeconds(const long validity, const ValidityUnits validityUnits) {
        switch (validityUnits) {
            case seconds:
                return validity;
            case minutes:
                return validity * 60;
            case hours:
                return validity * 3600;
            case days:
                return validity * 3600 * 24;
            default:
                return validity * 3600 * 8;
        }
    }

    ValidityUnits Mapper::GetMaxTokenUnits(const long validity) {
        if (validity > 3600 * 24) {
            return days;
        }
        if (validity > 3600) {
            return hours;
        }
        if (validity > 60) {
            return minutes;
        }
        return seconds;
    }

    long Mapper::GetMaxValidityToken(const long validity, const ValidityUnits validityUnits) {
        switch (validityUnits) {
            case seconds:
                return validity;
            case minutes:
                return validity / 60;
            case hours:
                return validity / 3600;
            case days:
                return validity / (3600 * 24);
            default:
                return validity;
        }
    }

}// namespace AwsMock::Dto::Cognito