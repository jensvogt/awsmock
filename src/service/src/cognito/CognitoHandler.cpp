
#include <awsmock/service/cognito/CognitoHandler.h>

namespace AwsMock::Service {

    const std::map<std::string, std::string> CognitoHandler::headers = CreateHeaderMap();

    http::response<http::dynamic_body> CognitoHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_debug << "Cognito POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::CognitoClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {

            switch (clientCommand.command) {
                case Dto::Common::CognitoCommandType::CREATE_USER_POOL: {

                    Dto::Cognito::CreateUserPoolRequest cognitoRequest = Dto::Cognito::CreateUserPoolRequest::FromJson(clientCommand);
                    Dto::Cognito::CreateUserPoolResponse serviceResponse = _cognitoService.CreateUserPool(cognitoRequest);
                    log_info << "User pool created, userPoolId: " << serviceResponse.userPool.userPoolId;
                    return SendResponse(request, http::status::ok, serviceResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::CREATE_USER_POOL_DOMAIN: {

                    Dto::Cognito::CreateUserPoolDomainRequest cognitoRequest = Dto::Cognito::CreateUserPoolDomainRequest::FromJson(clientCommand);
                    Dto::Cognito::CreateUserPoolDomainResponse cognitoResponse = _cognitoService.CreateUserPoolDomain(cognitoRequest);
                    log_info << "User pool domain created, domain: " << cognitoResponse.cloudFrontDomain;
                    return SendResponse(request, http::status::ok, cognitoResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::UPDATE_USER_POOL_DOMAIN: {

                    Dto::Cognito::UpdateUserPoolDomainRequest cognitoRequest = Dto::Cognito::UpdateUserPoolDomainRequest::FromJson(clientCommand);
                    Dto::Cognito::UpdateUserPoolDomainResponse cognitoResponse = _cognitoService.UpdateUserPoolDomain(cognitoRequest);
                    log_info << "User pool domain updated, domain: " << cognitoResponse.cloudFrontDomain;
                    return SendResponse(request, http::status::ok, cognitoResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::CREATE_USER_POOL_CLIENT: {

                    Dto::Cognito::CreateUserPoolClientRequest cognitoRequest = Dto::Cognito::CreateUserPoolClientRequest::FromJson(clientCommand);
                    Dto::Cognito::CreateUserPoolClientResponse cognitoResponse = _cognitoService.CreateUserPoolClient(cognitoRequest);
                    log_info << "User pool client created, userPoolId: " << cognitoRequest.userPoolId << " clientName: " << cognitoRequest.clientName;
                    return SendResponse(request, http::status::ok, cognitoResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::LIST_USER_POOLS: {

                    Dto::Cognito::ListUserPoolRequest cognitoRequest = Dto::Cognito::ListUserPoolRequest::FromJson(clientCommand);
                    Dto::Cognito::ListUserPoolResponse cognitoResponse = _cognitoService.ListUserPools(cognitoRequest);
                    log_debug << "User pools listed, response: " << cognitoResponse.ToJson();
                    return SendResponse(request, http::status::ok, cognitoResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::LIST_USER_POOL_COUNTERS: {

                    Dto::Cognito::ListUserPoolCountersRequest cognitoRequest = Dto::Cognito::ListUserPoolCountersRequest::FromJson(clientCommand);
                    Dto::Cognito::ListUserPoolCountersResponse cognitoResponse = _cognitoService.ListUserPoolCounters(cognitoRequest);
                    log_debug << "User pools counters listed, response: " << cognitoResponse.ToJson();
                    return SendResponse(request, http::status::ok, cognitoResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::UPDATE_USER_POOL: {

                    Dto::Cognito::UpdateUserPoolRequest cognitoRequest = Dto::Cognito::UpdateUserPoolRequest::FromJson(clientCommand);
                    _cognitoService.UpdateUserPool(cognitoRequest);
                    log_info << "User pool updated, userPoolId: " << cognitoRequest.userPoolId;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::CognitoCommandType::LIST_USER_POOL_CLIENTS: {

                    Dto::Cognito::ListUserPoolClientsRequest cognitoRequest = Dto::Cognito::ListUserPoolClientsRequest::FromJson(clientCommand);
                    Dto::Cognito::ListUserPoolClientsResponse cognitoResponse = _cognitoService.ListUserPoolClients(cognitoRequest);
                    log_info << "User pool clients listed, userPoolId: " << cognitoResponse.userPoolsClients.size();
                    return SendResponse(request, http::status::ok, cognitoResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::DESCRIBE_USER_POOL_CLIENTS: {

                    Dto::Cognito::DescribeUserPoolClientRequest cognitoRequest = Dto::Cognito::DescribeUserPoolClientRequest::FromJson(clientCommand);
                    Dto::Cognito::DescribeUserPoolClientResponse cognitoResponse = _cognitoService.DescribeUserPoolClient(cognitoRequest);
                    log_info << "Describe user pool client, userPoolId: " << cognitoResponse.userPoolClient.userPoolId << " clientId: " << cognitoResponse.userPoolClient.clientId;
                    return SendResponse(request, http::status::ok, cognitoResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::UPDATE_USER_POOL_CLIENT: {

                    Dto::Cognito::UpdateUserPoolClientRequest cognitoRequest = Dto::Cognito::UpdateUserPoolClientRequest::FromJson(clientCommand);
                    _cognitoService.UpdateUserPoolClient(cognitoRequest);
                    log_info << "User pool client updated, userPoolId: " << cognitoRequest.userPoolId << " clientId: " << cognitoRequest.clientId;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::CognitoCommandType::DELETE_USER_POOL_CLIENT: {

                    Dto::Cognito::DeleteUserPoolClientRequest cognitoRequest = Dto::Cognito::DeleteUserPoolClientRequest::FromJson(clientCommand);
                    _cognitoService.DeleteUserPoolClient(cognitoRequest);
                    log_info << "User pool client deleted, userPoolId: " << cognitoRequest.userPoolId << " clientId: " << cognitoRequest.clientId;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::CognitoCommandType::DESCRIBE_USER_POOL: {

                    Dto::Cognito::DescribeUserPoolRequest cognitoRequest = Dto::Cognito::DescribeUserPoolRequest::FromJson(clientCommand);
                    Dto::Cognito::DescribeUserPoolResponse cognitoResponse = _cognitoService.DescribeUserPool(cognitoRequest);
                    log_info << "Describe user pool, userPoolId: " << cognitoRequest.userPoolId;
                    return SendResponse(request, http::status::ok, cognitoResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::DELETE_USER_POOL: {

                    Dto::Cognito::DeleteUserPoolRequest cognitoRequest = Dto::Cognito::DeleteUserPoolRequest::FromJson(clientCommand);
                    _cognitoService.DeleteUserPool(cognitoRequest);
                    log_info << "User pool deleted, userPoolId: " << cognitoRequest.userPoolId;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::CognitoCommandType::LIST_USERS: {

                    Dto::Cognito::ListUsersRequest cognitoRequest = Dto::Cognito::ListUsersRequest::FromJson(clientCommand);
                    Dto::Cognito::ListUsersResponse cognitoResponse = _cognitoService.ListUsers(cognitoRequest);
                    log_info << "Users listed, userPoolId: " << cognitoRequest.userPoolId << " count: " << cognitoResponse.users.size();
                    return SendResponse(request, http::status::ok, cognitoResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::LIST_USER_COUNTERS: {

                    Dto::Cognito::ListUserCountersRequest cognitoRequest = Dto::Cognito::ListUserCountersRequest::FromJson(clientCommand);
                    Dto::Cognito::ListUserCountersResponse cognitoResponse = _cognitoService.ListUserCounters(cognitoRequest);
                    log_info << "User counters listed, userPoolId: " << cognitoRequest.userPoolId << " count: " << cognitoResponse.users.size();
                    return SendResponse(request, http::status::ok, cognitoResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::CREATE_GROUP: {

                    Dto::Cognito::CreateGroupRequest cognitoRequest = Dto::Cognito::CreateGroupRequest::FromJson(clientCommand);
                    Dto::Cognito::CreateGroupResponse cognitoResponse = _cognitoService.CreateGroup(cognitoRequest);
                    log_info << "Group created, group: " << cognitoResponse.group;
                    return SendResponse(request, http::status::ok, cognitoResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::LIST_GROUPS: {

                    Dto::Cognito::ListGroupsRequest cognitoRequest = Dto::Cognito::ListGroupsRequest::FromJson(clientCommand);
                    Dto::Cognito::ListGroupsResponse cognitoResponse = _cognitoService.ListGroups(cognitoRequest);
                    log_info << "Groups listed, userPoolId: " << cognitoRequest.userPoolId << " count: " << cognitoResponse.groups.size();
                    return SendResponse(request, http::status::ok, cognitoResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::LIST_USERS_IN_GROUP: {

                    Dto::Cognito::ListUsersInGroupRequest cognitoRequest = Dto::Cognito::ListUsersInGroupRequest::FromJson(clientCommand);
                    Dto::Cognito::ListUsersInGroupResponse cognitoResponse = _cognitoService.ListUsersInGroup(cognitoRequest);
                    log_info << "Users in group listed, userPoolId: " << cognitoRequest.userPoolId;
                    return SendResponse(request, http::status::ok, cognitoResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::ADMIN_REMOVE_USER_FROM_GROUP: {

                    Dto::Cognito::AdminRemoveUserFromGroupRequest cognitoRequest = Dto::Cognito::AdminRemoveUserFromGroupRequest::FromJson(clientCommand);
                    _cognitoService.AdminRemoveUserFromGroup(cognitoRequest);
                    log_info << "Remove user from group, userPoolId: " << cognitoRequest.userPoolId;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::CognitoCommandType::DELETE_GROUP: {

                    Dto::Cognito::DeleteGroupRequest cognitoRequest = Dto::Cognito::DeleteGroupRequest::FromJson(clientCommand);
                    _cognitoService.DeleteGroup(cognitoRequest);
                    log_info << "Group deleted, userPoolId: " << cognitoRequest.userPoolId << " group: " << cognitoRequest.groupName;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::CognitoCommandType::SIGN_UP: {

                    Dto::Cognito::SignUpRequest cognitoRequest = Dto::Cognito::SignUpRequest::FromJson(clientCommand);
                    Dto::Cognito::SignUpResponse cognitoResponse = _cognitoService.SignUp(cognitoRequest);
                    log_info << "Sign up user, user: " << cognitoRequest.user << " clientId: " << cognitoRequest.clientId;
                    return SendResponse(request, http::status::ok, cognitoResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::ADMIN_CONFIRM_SIGN_UP: {

                    Dto::Cognito::AdminConfirmUserRequest cognitoRequest = Dto::Cognito::AdminConfirmUserRequest::FromJson(clientCommand);
                    _cognitoService.ConfirmUser(cognitoRequest);
                    log_info << "User confirmed, userPoolId: " << cognitoRequest.userPoolId << " userName: " << cognitoRequest.userName;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::CognitoCommandType::INITIATE_AUTH: {

                    Dto::Cognito::InitiateAuthRequest cognitoRequest = Dto::Cognito::InitiateAuthRequest::FromJson(clientCommand);
                    Dto::Cognito::InitiateAuthResponse cognitoResponse = _cognitoService.InitiateAuth(cognitoRequest);
                    log_info << "User authorization initiated, userName: " << cognitoRequest.GetUserId();
                    return SendResponse(request, http::status::ok, cognitoResponse.ToJson(), headers);
                }

                case Dto::Common::CognitoCommandType::RESPOND_TO_AUTH_CHALLENGE: {

                    Dto::Cognito::RespondToAuthChallengeRequest cognitoRequest = Dto::Cognito::RespondToAuthChallengeRequest::FromJson(clientCommand);
                    Dto::Cognito::RespondToAuthChallengeResponse cognitoResponse = _cognitoService.RespondToAuthChallenge(cognitoRequest);
                    log_info << "Respond to auth challenge, clientId: " << cognitoRequest.clientId << " json: " << cognitoResponse.ToJson();
                    return SendResponse(request, http::status::ok, cognitoResponse.ToJson(), headers);
                }

                case Dto::Common::CognitoCommandType::GLOBAL_SIGN_OUT: {

                    Dto::Cognito::GlobalSignOutRequest cognitoRequest = Dto::Cognito::GlobalSignOutRequest::FromJson(clientCommand);
                    CognitoService::GlobalSignOut(cognitoRequest);
                    log_info << "Global sign out, accessToken: " << cognitoRequest.accessToken;
                    return SendResponse(request, http::status::ok, {}, headers);
                }

                case Dto::Common::CognitoCommandType::ADMIN_CREATE_USER: {

                    Dto::Cognito::AdminCreateUserRequest cognitoRequest = Dto::Cognito::AdminCreateUserRequest::FromJson(clientCommand);
                    Dto::Cognito::AdminCreateUserResponse cognitoResponse = _cognitoService.AdminCreateUser(cognitoRequest);
                    log_info << "User created, userPoolId: " << cognitoRequest.userPoolId;
                    return SendResponse(request, http::status::ok, cognitoResponse.ToJson(), headers);
                }

                case Dto::Common::CognitoCommandType::ADMIN_GET_USER: {

                    Dto::Cognito::AdminGetUserRequest cognitoRequest = Dto::Cognito::AdminGetUserRequest::FromJson(clientCommand);
                    Dto::Cognito::AdminGetUserResponse cognitoResponse = _cognitoService.AdminGetUser(cognitoRequest);
                    log_info << "User returned, userPoolId: " << cognitoRequest.userPoolId << " username: " << cognitoRequest.user;
                    return SendResponse(request, http::status::ok, cognitoResponse.ToJson(), headers);
                }

                case Dto::Common::CognitoCommandType::ADMIN_ENABLE_USER: {

                    Dto::Cognito::AdminEnableUserRequest cognitoRequest = Dto::Cognito::AdminEnableUserRequest::FromJson(clientCommand);
                    _cognitoService.AdminEnableUser(cognitoRequest);
                    log_info << "User enabled, userPoolId: " << cognitoRequest.userPoolId << " user: " << cognitoRequest.user;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::CognitoCommandType::ADMIN_DISABLE_USER: {

                    Dto::Cognito::AdminDisableUserRequest cognitoRequest = Dto::Cognito::AdminDisableUserRequest::FromJson(clientCommand);
                    _cognitoService.AdminDisableUser(cognitoRequest);
                    log_info << "User disabled, userPoolId: " << cognitoRequest.userPoolId << " user: " << cognitoRequest.user;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::CognitoCommandType::ADMIN_DELETE_USER: {

                    Dto::Cognito::AdminDeleteUserRequest cognitoRequest = Dto::Cognito::AdminDeleteUserRequest::FromJson(clientCommand);
                    _cognitoService.AdminDeleteUser(cognitoRequest);
                    log_info << "User deleted, userPoolId: " << cognitoRequest.userPoolId << " user: " << cognitoRequest.user;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::CognitoCommandType::ADMIN_ADD_USER_TO_GROUP: {

                    Dto::Cognito::AdminAddUserToGroupRequest cognitoRequest = Dto::Cognito::AdminAddUserToGroupRequest::FromJson(clientCommand);
                    _cognitoService.AdminAddUserToGroup(cognitoRequest);
                    log_info << "Add user to group, userPoolId: " << cognitoRequest.userPoolId;
                    return SendResponse(request, http::status::ok);
                }

                default:
                    return SendResponse(request, http::status::bad_request, "Unknown action");
            }
        } catch (Core::JsonException &exc) {
            log_error << exc.message();
            return SendResponse(request, http::status::internal_server_error, exc.message());
        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
            return SendResponse(request, http::status::internal_server_error, exc.message());
        } catch (Core::BadRequestException &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.message());
        } catch (Core::NotFoundException &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.message());
        }
    }

}// namespace AwsMock::Service
