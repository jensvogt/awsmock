
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

                    Dto::Cognito::CreateUserPoolRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;

                    log_debug << "Got create user pool request, json: " << cognitoRequest.ToString();

                    Dto::Cognito::CreateUserPoolResponse serviceResponse = _cognitoService.CreateUserPool(cognitoRequest);
                    log_info << "User pool created, userPoolId: " << serviceResponse.userPoolId;

                    return SendOkResponse(request, serviceResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::CREATE_USER_POOL_DOMAIN: {
                    Dto::Cognito::CreateUserPoolDomainRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;

                    log_debug << "Got create user pool domain request, json: " << cognitoRequest.ToString();

                    Dto::Cognito::CreateUserPoolDomainResponse serviceResponse = _cognitoService.CreateUserPoolDomain(cognitoRequest);
                    log_info << "User pool domain created, domain: " << serviceResponse.cloudFrontDomain;

                    return SendOkResponse(request, serviceResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::UPDATE_USER_POOL_DOMAIN: {
                    Dto::Cognito::UpdateUserPoolDomainRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;

                    log_debug << "Got update user pool domain request, json: " << cognitoRequest.ToString();

                    Dto::Cognito::UpdateUserPoolDomainResponse cognitoResponse = _cognitoService.UpdateUserPoolDomain(cognitoRequest);
                    log_info << "User pool domain updated, domain: " << cognitoResponse.cloudFrontDomain;

                    return SendOkResponse(request, cognitoResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::CREATE_USER_POOL_CLIENT: {
                    Dto::Cognito::CreateUserPoolClientRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;

                    log_debug << "Got create user pool client request, json: " << cognitoRequest.ToString();

                    Dto::Cognito::CreateUserPoolClientResponse serviceResponse = _cognitoService.CreateUserPoolClient(cognitoRequest);
                    log_info << "User pool client created, userPoolId: " << cognitoRequest.userPoolId << " clientName: " << cognitoRequest.clientName;

                    return SendOkResponse(request, serviceResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::LIST_USER_POOLS: {

                    Dto::Cognito::ListUserPoolRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;

                    log_debug << "Got list user pool request, json: " << cognitoRequest.ToString();

                    Dto::Cognito::ListUserPoolResponse serviceResponse = _cognitoService.ListUserPools(cognitoRequest);
                    log_info << "User pools listed, count: " << serviceResponse.userPools.size();

                    return SendOkResponse(request, serviceResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::LIST_USER_POOL_COUNTERS: {
                    Dto::Cognito::ListUserPoolCountersRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;

                    log_debug << "Got list user pool counters request, json: " << cognitoRequest.ToString();

                    Dto::Cognito::ListUserPoolCountersResponse serviceResponse = _cognitoService.ListUserPoolCounters(cognitoRequest);
                    log_info << "User pools counters listed, count: " << serviceResponse.userPools.size();

                    return SendOkResponse(request, serviceResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::UPDATE_USER_POOL: {
                    Dto::Cognito::UpdateUserPoolRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;

                    log_debug << "Got update user pool request, json: " << cognitoRequest.ToString();

                    _cognitoService.UpdateUserPool(cognitoRequest);
                    log_info << "User pool updated, userPoolId: " << cognitoRequest.userPoolId;

                    return SendOkResponse(request);
                }

                case Dto::Common::CognitoCommandType::LIST_USER_POOL_CLIENTS: {
                    Dto::Cognito::ListUserPoolClientsRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;

                    log_debug << "Got list user pool clients request, json: " << cognitoRequest.ToString();

                    Dto::Cognito::ListUserPoolClientsResponse serviceResponse = _cognitoService.ListUserPoolClients(cognitoRequest);
                    log_info << "User pool clients listed, userPoolId: " << serviceResponse.userPoolsClients.size();

                    return SendOkResponse(request, serviceResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::DESCRIBE_USER_POOL_CLIENTS: {
                    Dto::Cognito::DescribeUserPoolClientRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;

                    log_debug << "Got describe user pool client request, json: " << cognitoRequest.ToString();

                    Dto::Cognito::DescribeUserPoolClientResponse cognitoResponse = _cognitoService.DescribeUserPoolClient(cognitoRequest);
                    log_info << "Describe user pool client, userPoolId: " << cognitoResponse.userPoolClient.userPoolId << " clientId: " << cognitoResponse.userPoolClient.clientId;

                    return SendOkResponse(request, cognitoResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::UPDATE_USER_POOL_CLIENT: {
                    Dto::Cognito::UpdateUserPoolClientRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;

                    log_debug << "Got update user pool client request, json: " << cognitoRequest.ToString();

                    _cognitoService.UpdateUserPoolClient(cognitoRequest);
                    log_info << "User pool client updated, userPoolId: " << cognitoRequest.userPoolId << " clientId: " << cognitoRequest.clientId;

                    return SendOkResponse(request);
                }

                case Dto::Common::CognitoCommandType::DELETE_USER_POOL_CLIENT: {
                    Dto::Cognito::DeleteUserPoolClientRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;

                    log_debug << "Got delete user pool client request, json: " << cognitoRequest.ToString();

                    _cognitoService.DeleteUserPoolClient(cognitoRequest);
                    log_info << "User pool client deleted, userPoolId: " << cognitoRequest.userPoolId << " clientId: " << cognitoRequest.clientId;

                    return SendOkResponse(request);
                }

                case Dto::Common::CognitoCommandType::DESCRIBE_USER_POOL: {
                    Dto::Cognito::DescribeUserPoolRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;

                    log_debug << "Describe user pool request, json: " << cognitoRequest.ToString();

                    Dto::Cognito::DescribeUserPoolResponse serviceResponse = _cognitoService.DescribeUserPool(cognitoRequest);
                    log_info << "Describe user pool, userPoolId: " << cognitoRequest.userPoolId;

                    return SendOkResponse(request, serviceResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::DELETE_USER_POOL: {
                    Dto::Cognito::DeleteUserPoolRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;
                    log_debug << "Got delete user pool request, json: " << cognitoRequest.ToString();

                    _cognitoService.DeleteUserPool(cognitoRequest);
                    log_info << "User pool deleted, userPoolId: " << cognitoRequest.userPoolId;

                    return SendOkResponse(request);
                }

                case Dto::Common::CognitoCommandType::LIST_USERS: {
                    Dto::Cognito::ListUsersRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;
                    log_debug << "Got list users request: " << cognitoRequest.ToString();

                    Dto::Cognito::ListUsersResponse cognitoResponse = _cognitoService.ListUsers(cognitoRequest);
                    log_info << "Users listed, userPoolId: " << cognitoRequest.userPoolId << " count: " << cognitoResponse.users.size();

                    return SendOkResponse(request, cognitoResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::LIST_USER_COUNTERS: {
                    Dto::Cognito::ListUserCountersRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;
                    log_debug << "Got list user counters request: " << cognitoRequest.ToString();

                    Dto::Cognito::ListUserCountersResponse cognitoResponse = _cognitoService.ListUserCounters(cognitoRequest);
                    log_info << "User counters listed, userPoolId: " << cognitoRequest.userPoolId << " count: " << cognitoResponse.users.size();

                    return SendOkResponse(request, cognitoResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::CREATE_GROUP: {
                    Dto::Cognito::CreateGroupRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;

                    log_debug << "Got create group request, json: " << cognitoRequest.ToString();

                    Dto::Cognito::CreateGroupResponse serviceResponse = _cognitoService.CreateGroup(cognitoRequest);
                    log_info << "Group created, group: " << serviceResponse.group;

                    return SendOkResponse(request, serviceResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::LIST_GROUPS: {
                    Dto::Cognito::ListGroupsRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;

                    log_debug << "Got list groups request, json: " << cognitoRequest.ToString();

                    Dto::Cognito::ListGroupsResponse serviceResponse = _cognitoService.ListGroups(cognitoRequest);
                    log_info << "Groups listed, userPoolId: " << cognitoRequest.userPoolId << " count: " << serviceResponse.groups.size();

                    return SendOkResponse(request, serviceResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::LIST_USERS_IN_GROUP: {
                    Dto::Cognito::ListUsersInGroupRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;

                    log_debug << "List users in group request, json: " << cognitoRequest.ToString();

                    Dto::Cognito::ListUsersInGroupResponse serviceResponse = _cognitoService.ListUsersInGroup(cognitoRequest);
                    log_info << "Users in group listed, userPoolId: " << cognitoRequest.userPoolId;

                    return SendOkResponse(request, serviceResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::ADMIN_REMOVE_USER_FROM_GROUP: {
                    Dto::Cognito::AdminRemoveUserFromGroupRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;

                    log_debug << "Remove user from group request, json: " << cognitoRequest.ToString();

                    _cognitoService.AdminRemoveUserFromGroup(cognitoRequest);
                    log_info << "Remove user from group, userPoolId: " << cognitoRequest.userPoolId;

                    return SendOkResponse(request);
                }

                case Dto::Common::CognitoCommandType::DELETE_GROUP: {
                    Dto::Cognito::DeleteGroupRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;

                    log_debug << "Got delete group request, json: " << cognitoRequest.ToString();

                    _cognitoService.DeleteGroup(cognitoRequest);
                    log_info << "Group deleted, userPoolId: " << cognitoRequest.userPoolId << " group: " << cognitoRequest.groupName;

                    return SendOkResponse(request);
                }

                case Dto::Common::CognitoCommandType::SIGN_UP: {
                    Dto::Cognito::SignUpRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;

                    log_debug << "Got sign up request, json: " << cognitoRequest.ToString();

                    Dto::Cognito::SignUpResponse cognitoResponse = _cognitoService.SignUp(cognitoRequest);
                    log_info << "Sign up user, user: " << cognitoRequest.user << " clientId: " << cognitoRequest.clientId;

                    return SendOkResponse(request, cognitoResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::ADMIN_CONFIRM_SIGN_UP: {
                    Dto::Cognito::AdminConfirmUserRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;

                    log_debug << "Got confirm sign up request, json: " << cognitoRequest.ToString();

                    _cognitoService.ConfirmUser(cognitoRequest);
                    log_info << "User confirmed, userPoolId: " << cognitoRequest.userPoolId << " userName: " << cognitoRequest.userName;

                    return SendOkResponse(request);
                }

                case Dto::Common::CognitoCommandType::INITIATE_AUTH: {
                    Dto::Cognito::InitiateAuthRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;

                    log_debug << "Got initiate auth request, json: " << cognitoRequest.ToString();

                    Dto::Cognito::InitiateAuthResponse cognitoResponse = _cognitoService.InitiateAuth(cognitoRequest);
                    log_info << "User authorization initiated, userName: " << cognitoRequest.GetUserId();
                    std::string tmp = cognitoResponse.ToJson();
                    return SendOkResponse(request, cognitoResponse.ToJson(), headers);
                }

                case Dto::Common::CognitoCommandType::RESPOND_TO_AUTH_CHALLENGE: {
                    Dto::Cognito::RespondToAuthChallengeRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;

                    log_debug << "Got response to auth challenge request, json: " << cognitoRequest.ToString();

                    Dto::Cognito::RespondToAuthChallengeResponse cognitoResponse = _cognitoService.RespondToAuthChallenge(cognitoRequest);
                    log_info << "Respond to auth challenge, clientId: " << cognitoRequest.clientId << " json: " << cognitoResponse.ToJson();

                    return SendOkResponse(request, cognitoResponse.ToJson(), headers);
                }

                case Dto::Common::CognitoCommandType::GLOBAL_SIGN_OUT: {
                    Dto::Cognito::GlobalSignOutRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;

                    log_debug << "Got global sign out request, json: " << cognitoRequest.ToString();

                    _cognitoService.GlobalSignOut(cognitoRequest);
                    log_info << "Global sign out, accessToken: " << cognitoRequest.accessToken;

                    return SendOkResponse(request, {}, headers);
                }

                case Dto::Common::CognitoCommandType::ADMIN_CREATE_USER: {
                    Dto::Cognito::AdminCreateUserRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;
                    log_debug << "Got admin create user request, json: " << cognitoRequest.ToString();

                    Dto::Cognito::AdminCreateUserResponse cognitoResponse = _cognitoService.AdminCreateUser(cognitoRequest);
                    log_info << "User created, userPoolId: " << cognitoRequest.userPoolId;

                    return SendOkResponse(request, cognitoResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::ADMIN_GET_USER: {
                    Dto::Cognito::AdminGetUserRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;
                    log_debug << "Got admin get user request, json: " << cognitoRequest.ToString();

                    Dto::Cognito::AdminGetUserResponse cognitoResponse = _cognitoService.AdminGetUser(cognitoRequest);
                    log_info << "User returned, userPoolId: " << cognitoRequest.userPoolId << " username: " << cognitoRequest.user;

                    return SendOkResponse(request, cognitoResponse.ToJson());
                }

                case Dto::Common::CognitoCommandType::ADMIN_ENABLE_USER: {
                    Dto::Cognito::AdminEnableUserRequest cognitoRequest;
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;
                    log_debug << "Got admin enable user request, json: " << cognitoRequest.ToString();

                    _cognitoService.AdminEnableUser(cognitoRequest);
                    log_info << "User enabled, userPoolId: " << cognitoRequest.userPoolId << " user: " << cognitoRequest.user;

                    return SendOkResponse(request);
                }

                case Dto::Common::CognitoCommandType::ADMIN_DISABLE_USER: {
                    Dto::Cognito::AdminDisableUserRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;
                    log_debug << "Got admin disable user request, json: " << cognitoRequest.ToString();

                    _cognitoService.AdminDisableUser(cognitoRequest);
                    log_info << "User disabled, userPoolId: " << cognitoRequest.userPoolId << " user: " << cognitoRequest.user;

                    return SendOkResponse(request);
                }

                case Dto::Common::CognitoCommandType::ADMIN_DELETE_USER: {
                    Dto::Cognito::AdminDeleteUserRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;
                    log_debug << "Got admin delete user request, json: " << cognitoRequest.ToString();

                    _cognitoService.AdminDeleteUser(cognitoRequest);
                    log_info << "User deleted, userPoolId: " << cognitoRequest.userPoolId << " user: " << cognitoRequest.user;

                    return SendOkResponse(request);
                }

                case Dto::Common::CognitoCommandType::ADMIN_ADD_USER_TO_GROUP: {
                    Dto::Cognito::AdminAddUserToGroupRequest cognitoRequest{};
                    cognitoRequest.FromJson(clientCommand.payload);
                    cognitoRequest.region = clientCommand.region;
                    cognitoRequest.requestId = clientCommand.requestId;
                    cognitoRequest.user = clientCommand.user;

                    log_debug << "Add user to group request, json: " << cognitoRequest.ToString();

                    _cognitoService.AdminAddUserToGroup(cognitoRequest);
                    log_info << "Add user to group, userPoolId: " << cognitoRequest.userPoolId;

                    return SendOkResponse(request);
                }

                default:
                    return SendBadRequestError(request, "Unknown action");
            }
        } catch (Core::JsonException &exc) {
            log_error << exc.message();
            return SendInternalServerError(request, exc.message());
        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
            return SendInternalServerError(request, exc.message());
        } catch (Core::BadRequestException &exc) {
            log_error << exc.what();
            return SendInternalServerError(request, exc.what());
        } catch (Core::NotFoundException &exc) {
            log_error << exc.what();
            return SendInternalServerError(request, exc.what());
        }
    }

}// namespace AwsMock::Service
