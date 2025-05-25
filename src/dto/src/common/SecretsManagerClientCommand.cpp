//
// Created by vogje01 on 11/26/23.
//

#include <awsmock/dto/common/SecretsManagerClientCommand.h>

namespace AwsMock::Dto::Common {

    void SecretsManagerClientCommand::FromRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {

        UserAgent userAgent;
        userAgent.FromRequest(request);

        // Basic values
        this->region = region;
        this->user = user;
        this->method = request.method();
        this->headers = Core::HttpUtils::GetHeaders(request);
        this->contentType = Core::HttpUtils::GetContentType(request);
        this->contentLength = Core::HttpUtils::GetContentLength(request);
        this->payload = Core::HttpUtils::GetBodyAsString(request);
        this->url = request.target();
        this->requestId = Core::HttpUtils::GetHeaderValue(request, "RequestId", Core::AwsUtils::CreateRequestId());

        // Command
        const std::string action = Core::HttpUtils::GetHeaderValue(request, "X-Amz-Target");

        switch (method) {
            case http::verb::get:
            case http::verb::put:
            case http::verb::delete_:
                break;
            case http::verb::post:
                if (Core::StringUtils::ContainsIgnoreCase(action, "CreateSecret")) {
                    command = SecretsManagerCommandType::CREATE_SECRET;
                } else if (Core::StringUtils::ContainsIgnoreCase(action, "DeleteSecret")) {
                    command = SecretsManagerCommandType::DELETE_SECRET;
                } else if (Core::StringUtils::ContainsIgnoreCase(action, "DescribeSecret")) {
                    command = SecretsManagerCommandType::DESCRIBE_SECRET;
                } else if (Core::StringUtils::ContainsIgnoreCase(action, "GetSecretValue")) {
                    command = SecretsManagerCommandType::GET_SECRET_VALUE;
                } else if (Core::StringUtils::ContainsIgnoreCase(action, "UpdateSecret")) {
                    command = SecretsManagerCommandType::UPDATE_SECRET;
                } else if (Core::StringUtils::ContainsIgnoreCase(action, "RotateSecret")) {
                    command = SecretsManagerCommandType::ROTATE_SECRET;
                } else if (Core::StringUtils::ContainsIgnoreCase(action, "ListSecrets")) {
                    command = SecretsManagerCommandType::LIST_SECRETS;
                }
                break;
            default:;
        }
    }

    std::string SecretsManagerClientCommand::ToJson() const {

        try {
            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "method", boost::lexical_cast<std::string>(method));
            Core::Bson::BsonUtils::SetStringValue(document, "command", SecretsManagerCommandTypeToString(command));
            Core::Bson::BsonUtils::SetStringValue(document, "user", user);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string SecretsManagerClientCommand::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const SecretsManagerClientCommand &r) {
        os << "SecretsManagerClientCommand=" << r.ToJson();
        return os;
    }
}// namespace AwsMock::Dto::Common
