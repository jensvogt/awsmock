
#include <awsmock/service/secretsmanager/SecretsManagerHandler.h>

namespace AwsMock::Service {

    http::response<http::dynamic_body> SecretsManagerHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_debug << "SecretsManager POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::SecretsManagerClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {

            switch (clientCommand.command) {

                case Dto::Common::SecretsManagerCommandType::CREATE_SECRET: {

                    Dto::SecretsManager::CreateSecretRequest secretsManagerRequest = Dto::SecretsManager::CreateSecretRequest::FromJson(clientCommand);
                    Dto::SecretsManager::CreateSecretResponse secretsManagerResponse = _secretsManagerService.CreateSecret(secretsManagerRequest);
                    log_info << "Secret created, secretId: " << secretsManagerResponse.name;
                    return SendOkResponse(request, secretsManagerResponse.ToJson());
                }

                case Dto::Common::SecretsManagerCommandType::DELETE_SECRET: {

                    Dto::SecretsManager::DeleteSecretRequest secretsManagerRequest = Dto::SecretsManager::DeleteSecretRequest::FromJson(clientCommand);
                    Dto::SecretsManager::DeleteSecretResponse secretsManagerResponse = _secretsManagerService.DeleteSecret(secretsManagerRequest);
                    log_info << "Secret deleted, secretId: " << secretsManagerResponse.name;
                    return SendOkResponse(request, secretsManagerResponse.ToJson());
                }

                case Dto::Common::SecretsManagerCommandType::DESCRIBE_SECRET: {

                    Dto::SecretsManager::DescribeSecretRequest secretsManagerRequest = Dto::SecretsManager::DescribeSecretRequest::FromJson(clientCommand);
                    Dto::SecretsManager::DescribeSecretResponse secretsManagerResponse = _secretsManagerService.DescribeSecret(secretsManagerRequest);
                    log_info << "Secret described, response: " << secretsManagerResponse;
                    return SendOkResponse(request, secretsManagerResponse.ToJson());
                }

                case Dto::Common::SecretsManagerCommandType::GET_SECRET_VALUE: {

                    Dto::SecretsManager::GetSecretValueRequest secretsManagerRequest = Dto::SecretsManager::GetSecretValueRequest::FromJson(clientCommand);
                    Dto::SecretsManager::GetSecretValueResponse secretsManagerResponse = _secretsManagerService.GetSecretValue(secretsManagerRequest);
                    log_info << "Secret get value, secretId: " << secretsManagerResponse.name;
                    return SendOkResponse(request, secretsManagerResponse.ToJson());
                }

                case Dto::Common::SecretsManagerCommandType::UPDATE_SECRET: {

                    Dto::SecretsManager::UpdateSecretRequest secretsManagerRequest = Dto::SecretsManager::UpdateSecretRequest::FromJson(clientCommand);
                    Dto::SecretsManager::UpdateSecretResponse secretsManagerResponse = _secretsManagerService.UpdateSecret(secretsManagerRequest);
                    log_info << "Secret updated, secretId: " << secretsManagerResponse.name;
                    return SendOkResponse(request, secretsManagerResponse.ToJson());
                }

                case Dto::Common::SecretsManagerCommandType::ROTATE_SECRET: {

                    Dto::SecretsManager::RotateSecretRequest secretsManagerRequest = Dto::SecretsManager::RotateSecretRequest::FromJson(clientCommand);
                    Dto::SecretsManager::RotateSecretResponse secretsManagerResponse = _secretsManagerService.RotateSecret(secretsManagerRequest);
                    log_info << "Secret rotated, secretId: " << secretsManagerResponse.arn;
                    return SendOkResponse(request, secretsManagerResponse.ToJson());
                }

                case Dto::Common::SecretsManagerCommandType::LIST_SECRETS: {

                    Dto::SecretsManager::ListSecretsRequest secretsManagerRequest = Dto::SecretsManager::ListSecretsRequest::FromJson(clientCommand);
                    Dto::SecretsManager::ListSecretsResponse secretsManagerResponse = _secretsManagerService.ListSecrets(secretsManagerRequest);
                    log_info << "Secrets listed, region: " << secretsManagerResponse.region;
                    return SendOkResponse(request, secretsManagerResponse.ToJson());
                }

                case Dto::Common::SecretsManagerCommandType::LIST_SECRET_COUNTERS: {

                    Dto::SecretsManager::ListSecretCountersRequest secretsManagerRequest = Dto::SecretsManager::ListSecretCountersRequest::FromJson(clientCommand);
                    Dto::SecretsManager::ListSecretCountersResponse secretsManagerResponse = _secretsManagerService.ListSecretCounters(secretsManagerRequest);
                    log_info << "Secrets counters listed, region: " << secretsManagerResponse.region;
                    return SendOkResponse(request, secretsManagerResponse.ToJson());
                }

                case Dto::Common::SecretsManagerCommandType::GET_SECRET_DETAILS: {

                    Dto::SecretsManager::GetSecretDetailsRequest secretsManagerRequest = Dto::SecretsManager::GetSecretDetailsRequest::FromJson(clientCommand);
                    Dto::SecretsManager::GetSecretDetailsResponse secretsManagerResponse = _secretsManagerService.GetSecretDetails(secretsManagerRequest);
                    log_info << "Get secret details, secretId: " << secretsManagerResponse.secretId;
                    return SendOkResponse(request, secretsManagerResponse.ToJson());
                }

                case Dto::Common::SecretsManagerCommandType::LIST_SECRET_VERSIONS: {

                    Dto::SecretsManager::ListSecretVersionCountersRequest secretsManagerRequest = Dto::SecretsManager::ListSecretVersionCountersRequest::FromJson(clientCommand);
                    Dto::SecretsManager::ListSecretVersionCountersResponse secretsManagerResponse = _secretsManagerService.ListSecretVersionCounters(secretsManagerRequest);
                    log_info << "Get secret versions, secretId: " << secretsManagerRequest.secretId;
                    return SendOkResponse(request, secretsManagerResponse.ToJson());
                }

                default:
                    log_error << "Unknown method";
                    SendBadRequestError(request, "Unknown method");
            }

        } catch (Core::JsonException &exc) {
            log_error << "JsonException: " << exc.message();
            return SendInternalServerError(request, exc.message());
        } catch (Core::DatabaseException &exc) {
            log_error << "DatabaseException: " << exc.message();
            return SendInternalServerError(request, exc.message());
        } catch (Core::ServiceException &exc) {
            log_error << "ServiceException: " << exc.message();
            return SendInternalServerError(request, exc.message());
        }
        log_error << "Unknown method";
        return SendBadRequestError(request, "Unknown method");
    }
}// namespace AwsMock::Service
