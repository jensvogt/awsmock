
#include <awsmock/dto/kms/CreateAliasRequest.h>
#include <awsmock/dto/kms/DeleteAliasRequest.h>
#include <awsmock/dto/kms/GenerateDataKeyRequest.h>
#include <awsmock/dto/kms/GenerateDataKeyResponse.h>
#include <awsmock/dto/kms/GetKeyPolicyRequest.h>
#include <awsmock/dto/kms/GetKeyPolicyResponse.h>
#include <awsmock/dto/kms/ListAliasesRequest.h>
#include <awsmock/dto/kms/ListAliasesResponse.h>
#include <awsmock/dto/kms/ListResourceTagsRequest.h>
#include <awsmock/dto/kms/ListResourceTagsResponse.h>
#include <awsmock/dto/kms/PutKeyPolicyRequest.h>
#include <awsmock/dto/kms/TagResourceRequest.h>
#include <awsmock/dto/kms/UntagResourceRequest.h>
#include <awsmock/dto/kms/UpdateAliasRequest.h>
#include <awsmock/service/kms/KMSHandler.h>

namespace Awsmock::Service {

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

                case Dto::Common::KMSCommandType::GENERATE_DATA_KEY: {
                    Dto::KMS::GenerateDataKeyRequest kmsRequest = Dto::KMS::GenerateDataKeyRequest::FromJson(clientCommand);
                    Dto::KMS::GenerateDataKeyResponse kmsResponse = _kmsService.GenerateDataKey(kmsRequest);
                    log_info << "GenerateDataKey received, keyId: " << kmsRequest.keyId;
                    return SendResponse(request, http::status::ok, kmsResponse.ToJson());
                }

                case Dto::Common::KMSCommandType::DELETE_KEY: {
                    Dto::KMS::DeleteKeyRequest kmsRequest = Dto::KMS::DeleteKeyRequest::FromJson(clientCommand);
                    _kmsService.DeleteKey(kmsRequest);
                    log_info << "Key deleted, size: " << kmsRequest.keyId;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::KMSCommandType::GET_KEY_POLICY: {
                    Dto::KMS::GetKeyPolicyRequest kmsRequest = Dto::KMS::GetKeyPolicyRequest::FromJson(clientCommand);
                    Dto::KMS::GetKeyPolicyResponse kmsResponse = _kmsService.GetKeyPolicy(kmsRequest);
                    log_info << "GetKeyPolicy, keyId: " << kmsRequest.keyId;
                    return SendResponse(request, http::status::ok, kmsResponse.ToJson());
                }

                case Dto::Common::KMSCommandType::PUT_KEY_POLICY: {
                    Dto::KMS::PutKeyPolicyRequest kmsRequest = Dto::KMS::PutKeyPolicyRequest::FromJson(clientCommand);
                    _kmsService.PutKeyPolicy(kmsRequest);
                    log_info << "PutKeyPolicy, keyId: " << kmsRequest.keyId;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::KMSCommandType::LIST_RESOURCE_TAGS: {
                    Dto::KMS::ListResourceTagsRequest kmsRequest = Dto::KMS::ListResourceTagsRequest::FromJson(clientCommand);
                    Dto::KMS::ListResourceTagsResponse kmsResponse = _kmsService.ListResourceTags(kmsRequest);
                    log_info << "ListResourceTags, keyId: " << kmsRequest.keyId;
                    return SendResponse(request, http::status::ok, kmsResponse.ToJson());
                }

                case Dto::Common::KMSCommandType::TAG_RESOURCE: {
                    Dto::KMS::TagResourceRequest kmsRequest = Dto::KMS::TagResourceRequest::FromJson(clientCommand);
                    _kmsService.TagResource(kmsRequest);
                    log_info << "TagResource, keyId: " << kmsRequest.keyId;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::KMSCommandType::UNTAG_RESOURCE: {
                    Dto::KMS::UntagResourceRequest kmsRequest = Dto::KMS::UntagResourceRequest::FromJson(clientCommand);
                    _kmsService.UntagResource(kmsRequest);
                    log_info << "UntagResource, keyId: " << kmsRequest.keyId;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::KMSCommandType::GET_KEY_ROTATION_STATUS: {
                    log_info << "GetKeyRotationStatus received";
                    return SendResponse(request, http::status::ok, R"({"KeyRotationEnabled":false})");
                }

                case Dto::Common::KMSCommandType::ENABLE_KEY_ROTATION: {
                    log_info << "EnableKeyRotation received";
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::KMSCommandType::DISABLE_KEY_ROTATION: {
                    log_info << "DisableKeyRotation received";
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::KMSCommandType::CREATE_ALIAS: {
                    Dto::KMS::CreateAliasRequest kmsRequest = Dto::KMS::CreateAliasRequest::FromJson(clientCommand);
                    _kmsService.CreateAlias(kmsRequest);
                    log_info << "CreateAlias, aliasName: " << kmsRequest.aliasName;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::KMSCommandType::DELETE_ALIAS: {
                    Dto::KMS::DeleteAliasRequest kmsRequest = Dto::KMS::DeleteAliasRequest::FromJson(clientCommand);
                    _kmsService.DeleteAlias(kmsRequest);
                    log_info << "DeleteAlias, aliasName: " << kmsRequest.aliasName;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::KMSCommandType::UPDATE_ALIAS: {
                    Dto::KMS::UpdateAliasRequest kmsRequest = Dto::KMS::UpdateAliasRequest::FromJson(clientCommand);
                    _kmsService.UpdateAlias(kmsRequest);
                    log_info << "UpdateAlias, aliasName: " << kmsRequest.aliasName;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::KMSCommandType::LIST_ALIASES: {
                    Core::HttpUtils::DumpRequest(request);
                    Dto::KMS::ListAliasesRequest kmsRequest = Dto::KMS::ListAliasesRequest::FromJson(clientCommand);
                    Dto::KMS::ListAliasesResponse kmsResponse = _kmsService.ListAliases(kmsRequest);
                    log_info << "ListAliases, count: " << kmsResponse.aliases.size();
                    return SendResponse(request, http::status::ok, kmsResponse.ToJson());
                }

                default:
                case Dto::Common::KMSCommandType::UNKNOWN: {
                    log_error << "Unknown method";
                    Core::HttpUtils::DumpRequest(request);
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
}// namespace Awsmock::Service
