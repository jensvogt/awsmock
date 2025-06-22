
#include <awsmock/service/kms/KMSHandler.h>

namespace AwsMock::Service {

    boost::beast::http::response<http::dynamic_body> KMSHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_trace << "KMS POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::KMSClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {
            switch (clientCommand.command) {

                case Dto::Common::KMSCommandType::CREATE_KEY: {

                    Dto::KMS::CreateKeyRequest kmsRequest = Dto::KMS::CreateKeyRequest::FromJson(clientCommand);
                    Dto::KMS::CreateKeyResponse kmsResponse = _kmsService.CreateKey(kmsRequest);
                    log_info << "Key created, keyId: " << kmsResponse.key.keyId;

                    return SendOkResponse(request, kmsResponse.ToJson());
                }

                case Dto::Common::KMSCommandType::SCHEDULE_KEY_DELETION: {

                    Dto::KMS::ScheduleKeyDeletionRequest kmsRequest = Dto::KMS::ScheduleKeyDeletionRequest::FromJson(clientCommand);
                    Dto::KMS::ScheduledKeyDeletionResponse kmsResponse = _kmsService.ScheduleKeyDeletion(kmsRequest);
                    log_info << "Key deletion scheduled, keyId: " << kmsResponse.keyId;

                    return SendOkResponse(request, kmsResponse.ToJson());
                }

                case Dto::Common::KMSCommandType::LIST_KEYS: {

                    Dto::KMS::ListKeysRequest kmsRequest = Dto::KMS::ListKeysRequest::FromJson(clientCommand);
                    Dto::KMS::ListKeysResponse kmsResponse = _kmsService.ListKeys(kmsRequest);
                    log_info << "List keys received, count: " << kmsResponse.keys.size();

                    return SendOkResponse(request, kmsResponse.ToJson());
                }

                case Dto::Common::KMSCommandType::LIST_KEY_COUNTERS: {

                    Dto::KMS::ListKeyCountersRequest kmsRequest = Dto::KMS::ListKeyCountersRequest::FromJson(clientCommand);
                    Dto::KMS::ListKeyCountersResponse kmsResponse = _kmsService.ListKeyCounters(kmsRequest);
                    log_info << "List key counters received, count: " << kmsResponse.keyCounters.size();

                    return SendOkResponse(request, kmsResponse.ToJson());
                }

                case Dto::Common::KMSCommandType::DESCRIBE_KEY: {

                    Dto::KMS::DescribeKeyRequest kmsRequest = Dto::KMS::DescribeKeyRequest::FromJson(clientCommand);
                    Dto::KMS::DescribeKeyResponse kmsResponse = _kmsService.DescribeKey(kmsRequest);
                    log_info << "Describe key received, count: " << kmsResponse.key.keyId;

                    return SendOkResponse(request, kmsResponse.ToJson());
                }

                case Dto::Common::KMSCommandType::ENCRYPT: {

                    Dto::KMS::EncryptRequest kmsRequest = Dto::KMS::EncryptRequest::FromJson(clientCommand);
                    Dto::KMS::EncryptResponse kmsResponse = _kmsService.Encrypt(kmsRequest);

                    log_info << "Encrypt received, size: " << kmsResponse.ciphertext.length();
                    return SendOkResponse(request, kmsResponse.ToJson());
                }

                case Dto::Common::KMSCommandType::DECRYPT: {

                    Dto::KMS::DecryptRequest kmsRequest = Dto::KMS::DecryptRequest::FromJson(clientCommand);
                    Dto::KMS::DecryptResponse kmsResponse = _kmsService.Decrypt(kmsRequest);

                    log_info << "Decrypt received, size: " << kmsResponse.plaintext.length();
                    return SendOkResponse(request, kmsResponse.ToJson());
                }

                default:
                case Dto::Common::KMSCommandType::UNKNOWN: {
                    log_error << "Unknown method";
                    return SendBadRequestError(request, "Unknown method");
                }
            }
        } catch (std::exception &exc) {
            log_error << exc.what();
            return SendInternalServerError(request, exc.what());
        } catch (...) {
            log_error << "Invalid request";
            return SendInternalServerError(request, "Invalid request");
        }
    }
}// namespace AwsMock::Service
