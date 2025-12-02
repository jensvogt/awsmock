
#include <awsmock/service/transfer/TransferHandler.h>

namespace AwsMock::Service {

    http::response<http::dynamic_body> TransferHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_trace << "Transfer server POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        try {
            std::string body = Core::HttpUtils::GetBodyAsString(request);
            std::string target = GetTarget(request);

            if (target == "TransferService.CreateServer") {

                Dto::Transfer::CreateServerRequest transferRequest = Dto::Transfer::CreateServerRequest::FromJson(body);
                transferRequest.region = region;
                Dto::Transfer::CreateServerResponse transferResponse = _transferService.CreateTransferServer(transferRequest);
                return SendOkResponse(request, transferResponse.ToJson());
            }

            if (target == "TransferService.CreateUser") {

                Dto::Transfer::CreateUserRequest transferRequest;
                transferRequest.FromJson(body);
                transferRequest.region = region;
                Dto::Transfer::CreateUserResponse transferResponse = _transferService.CreateUser(transferRequest);
                return SendOkResponse(request, transferResponse.ToJson());
            }

            if (target == "TransferService.CreateProtocol") {

                Dto::Transfer::CreateProtocolRequest transferRequest = Dto::Transfer::CreateProtocolRequest::FromJson(body);
                transferRequest.region = region;
                _transferService.CreateProtocol(transferRequest);
                return SendOkResponse(request);
            }

            if (target == "TransferService.ListServers") {

                Dto::Transfer::ListServerRequest transferRequest = Dto::Transfer::ListServerRequest::FromJson(body);
                transferRequest.region = region;
                Dto::Transfer::ListServerResponse transferResponse = _transferService.ListServers(transferRequest);
                log_info << "List servers, region: " << transferRequest.region << ", json: " << transferResponse.ToJson();
                return SendOkResponse(request, transferResponse.ToJson());
            }

            if (target == "TransferService.ListServerCounters") {

                Dto::Transfer::ListServerCountersRequest transferRequest = Dto::Transfer::ListServerCountersRequest::FromJson(body);
                transferRequest.region = region;

                Dto::Transfer::ListServerCountersResponse transferResponse = _transferService.ListServerCounters(transferRequest);
                return SendOkResponse(request, transferResponse.ToJson());
            }

            if (target == "TransferService.ListUserCounters") {

                Dto::Transfer::ListUserCountersRequest transferRequest = Dto::Transfer::ListUserCountersRequest::FromJson(body);
                transferRequest.region = region;

                Dto::Transfer::ListUserCountersResponse transferResponse = _transferService.ListUserCounters(transferRequest);
                return SendOkResponse(request, transferResponse.ToJson());
            }

            if (target == "TransferService.ListProtocolCounters") {

                Dto::Transfer::ListProtocolCountersRequest transferRequest = Dto::Transfer::ListProtocolCountersRequest::FromJson(body);
                transferRequest.region = region;

                Dto::Transfer::ListProtocolCountersResponse transferResponse = _transferService.ListProtocolCounters(transferRequest);
                return SendOkResponse(request, transferResponse.ToJson());
            }

            if (target == "TransferService.ListTagCounters") {

                Dto::Transfer::ListTagCountersRequest transferRequest = Dto::Transfer::ListTagCountersRequest::FromJson(body);
                transferRequest.region = region;

                Dto::Transfer::ListTagCountersResponse transferResponse = _transferService.ListTagCounters(transferRequest);
                std::string tmp = transferResponse.ToJson();
                return SendOkResponse(request, transferResponse.ToJson());
            }

            if (target == "TransferService.ListUsers") {

                std::string serverId;
                Dto::Transfer::ListUsersRequest transferRequest;
                transferRequest.FromJson(body);
                transferRequest.region = region;

                Dto::Transfer::ListUsersResponse transferResponse = _transferService.ListUsers(transferRequest);
                return SendOkResponse(request, transferResponse.ToJson());
            }

            if (target == "TransferService.StartServer") {

                Dto::Transfer::StartServerRequest transferRequest;
                transferRequest.FromJson(body);
                transferRequest.region = region;
                _transferService.StartServer(transferRequest);
                return SendOkResponse(request);
            }

            if (target == "TransferService.StopServer") {

                Dto::Transfer::StopServerRequest transferRequest;
                transferRequest.FromJson(body);
                transferRequest.region = region;

                _transferService.StopServer(transferRequest);
                return SendOkResponse(request);
            }

            if (target == "TransferService.DeleteServer") {

                Dto::Transfer::DeleteServerRequest transferRequest;
                transferRequest.FromJson(body);
                transferRequest.region = region;

                _transferService.DeleteServer(transferRequest);
                return SendOkResponse(request);
            }

            if (target == "TransferService.GetServerDetails") {

                Dto::Transfer::GetServerDetailsRequest transferRequest;
                transferRequest.FromJson(body);
                transferRequest.region = region;

                Dto::Transfer::GetServerDetailsResponse transferResponse = _transferService.GetServerDetails(transferRequest);
                log_debug << "Get transfer details, region: " << transferRequest.region << " serverId: " << transferRequest.serverId;
                log_trace << transferResponse.ToJson();
                return SendOkResponse(request, transferResponse.ToJson());
            }

            if (target == "TransferService.DeleteUser") {

                Dto::Transfer::DeleteUserRequest transferRequest;
                transferRequest.FromJson(body);
                transferRequest.region = region;

                _transferService.DeleteUser(transferRequest);
                log_info << "Delete user, region: " << transferRequest.region << " serverId: " << transferRequest.serverId << " userName: " << transferRequest.userName;

                return SendOkResponse(request);
            }

            if (target == "TransferService.DeleteProtocol") {

                Dto::Transfer::DeleteProtocolRequest transferRequest;
                transferRequest.FromJson(body);
                transferRequest.region = region;

                _transferService.DeleteProtocol(transferRequest);
                log_info << "Delete protocol, region: " << transferRequest.region << " serverId: " << transferRequest.serverId << " protocol: " << Dto::Transfer::ProtocolTypeToString(transferRequest.protocol);

                return SendOkResponse(request);
            }

            log_error << "Unknown method";
            return SendBadRequestError(request, "Unknown method");

        } catch (std::exception &exc) {
            return SendInternalServerError(request, exc.what());
        }
    }

    std::string TransferHandler::GetTarget(const http::request<http::dynamic_body> &request) {
        return Core::HttpUtils::GetHeaderValue(request, "X-Amz-Target");
    }

}// namespace AwsMock::Service
