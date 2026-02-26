
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
                    log_info << "Key created, keyId: " << kmsResponse.keyMetadata.keyId;

                    return SendResponse(request, http::status::ok, kmsResponse.ToJson());
                }

                case Dto::Common::KMSCommandType::SCHEDULE_KEY_DELETION: {

                    Dto::KMS::ScheduleKeyDeletionRequest kmsRequest = Dto::KMS::ScheduleKeyDeletionRequest::FromJson(clientCommand);
                    Dto::KMS::ScheduledKeyDeletionResponse kmsResponse = _kmsService.ScheduleKeyDeletion(kmsRequest);
                    log_info << "Key deletion scheduled, keyId: " << kmsResponse.keyId;

                    return SendResponse(request, http::status::ok, kmsResponse.ToJson());
                }

                case Dto::Common::KMSCommandType::LIST_KEYS: {

                    Dto::KMS::ListKeysRequest kmsRequest = Dto::KMS::ListKeysRequest::FromJson(clientCommand);
                    Dto::KMS::ListKeysResponse kmsResponse = _kmsService.ListKeys(kmsRequest);
                    log_info << "List keys received, count: " << kmsResponse.keys.size();

                    return SendResponse(request, http::status::ok, kmsResponse.ToJson());
                }

                case Dto::Common::KMSCommandType::LIST_KEY_COUNTERS: {

                    Dto::KMS::ListKeyCountersRequest kmsRequest = Dto::KMS::ListKeyCountersRequest::FromJson(clientCommand);
                    Dto::KMS::ListKeyCountersResponse kmsResponse = _kmsService.ListKeyCounters(kmsRequest);
                    log_info << "List key counters received, count: " << kmsResponse.keyCounters.size();

                    return SendResponse(request, http::status::ok, kmsResponse.ToJson());
                }

                case Dto::Common::KMSCommandType::LIST_KEY_ARNS: {

                    Dto::KMS::ListKeyArnsResponse kmsResponse = _kmsService.ListKeyArns();
                    log_info << "List key ARNs received, count: " << kmsResponse.keyArns.size();

                    return SendResponse(request, http::status::ok, kmsResponse.ToJson());
                }

                case Dto::Common::KMSCommandType::DESCRIBE_KEY: {

                    Dto::KMS::DescribeKeyRequest kmsRequest = Dto::KMS::DescribeKeyRequest::FromJson(clientCommand);
                    Dto::KMS::DescribeKeyResponse kmsResponse = _kmsService.DescribeKey(kmsRequest);
                    log_info << "Describe key received, count: " << kmsResponse.keyMetadata.keyId;
                    log_info << "Describe key received, count: " << kmsResponse.keyMetadata;

                    return SendResponse(request, http::status::ok, kmsResponse.ToJson());
                }

                case Dto::Common::KMSCommandType::GET_KEY_COUNTER: {

                    Dto::KMS::GetKeyCounterRequest kmsRequest = Dto::KMS::GetKeyCounterRequest::FromJson(clientCommand);
                    Dto::KMS::GetKeyCounterResponse kmsResponse = _kmsService.GetKeyCounter(kmsRequest);
                    log_info << "Get key received, id: " << kmsRequest.keyId;

                    return SendResponse(request, http::status::ok, kmsResponse.ToJson());
                }

                case Dto::Common::KMSCommandType::UPDATE_KEY_COUNTER: {

                    Dto::KMS::UpdateKeyCounterRequest kmsRequest = Dto::KMS::UpdateKeyCounterRequest::FromJson(clientCommand);
                    _kmsService.UpdateKeyCounter(kmsRequest);
                    log_info << "Update key, id: " << kmsRequest.keyCounter.keyId;

                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::KMSCommandType::ENCRYPT: {

                    Dto::KMS::EncryptRequest kmsRequest = Dto::KMS::EncryptRequest::FromJson(clientCommand);
                    Dto::KMS::EncryptResponse kmsResponse = _kmsService.Encrypt(kmsRequest);

                    log_info << "Encrypt received, size: " << kmsResponse.ciphertext.length();
                    return SendResponse(request, http::status::ok, kmsResponse.ToJson());
                }

                case Dto::Common::KMSCommandType::DECRYPT: {
                    Dto::KMS::DecryptRequest kmsRequest = Dto::KMS::DecryptRequest::FromJson(clientCommand);
                    Dto::KMS::DecryptResponse kmsResponse = _kmsService.Decrypt(kmsRequest);
                    log_info << "Decrypt received, size: " << kmsResponse.plaintext.length();
                    return SendResponse(request, http::status::ok, kmsResponse.ToJson());
                }

                case Dto::Common::KMSCommandType::DELETE_KEY: {
                    Dto::KMS::DeleteKeyRequest kmsRequest = Dto::KMS::DeleteKeyRequest::FromJson(clientCommand);
                    _kmsService.DeleteKey(kmsRequest);
                    log_info << "Key deleted, size: " << kmsRequest.keyId;
                    return SendResponse(request, http::status::ok);
                }

                default:
                case Dto::Common::KMSCommandType::UNKNOWN: {
                    log_error << "Unknown method";
                    return SendResponse(request, http::status::bad_request, "Unknown method");
                }
            }
        } catch (std::exception &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.what());
        } catch (...) {
            log_error << "Invalid request";
            return SendResponse(request, http::status::internal_server_error, "Invalid request");
        }
    }
}// namespace AwsMock::Service
