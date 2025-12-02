//
// Created by vogje01 on 11/26/23.
//

#include <awsmock/dto/common/SSMClientCommand.h>

namespace AwsMock::Dto::Common {

    void SSMClientCommand::FromRequest(const http::request<http::dynamic_body> &request, const std::string &awsRegion, const std::string &awsUser) {

        UserAgent userAgent;
        userAgent.FromRequest(request);

        // Basic values
        this->region = awsRegion;
        this->user = awsUser;
        this->method = request.method();
        this->contentType = Core::HttpUtils::GetContentType(request);
        this->contentLength = Core::HttpUtils::GetContentLength(request);
        this->payload = Core::HttpUtils::GetBodyAsString(request);
        this->url = request.target();
        this->requestId = Core::HttpUtils::GetHeaderValue(request, "RequestId", Core::AwsUtils::CreateRequestId());

        if (userAgent.clientCommand.empty()) {

            this->command = SSMCommandTypeFromString(GetCommandFromHeader(request));

        } else {

            this->command = SSMCommandTypeFromString(userAgent.clientCommand);
        }
    }

    std::string SSMClientCommand::GetCommandFromHeader(const http::request<http::dynamic_body> &request) {

        std::string cmd;
        if (Core::HttpUtils::HasHeader(request, "x-awsmock-target")) {

            // awsmock command from UI
            cmd = Core::HttpUtils::GetHeaderValue(request, "x-awsmock-action");

        } else {
            const std::string headerValue = Core::HttpUtils::GetHeaderValue(request, "X-Amz-Target");
            cmd = Core::StringUtils::Split(headerValue, ".")[1];
        }
        return Core::StringUtils::ToSnakeCase(cmd);
    }

    std::string SSMClientCommand::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "method", boost::lexical_cast<std::string>(method));
            Core::Bson::BsonUtils::SetStringValue(document, "command", SSMCommandTypeToString(command));
            Core::Bson::BsonUtils::SetStringValue(document, "user", user);
            Core::Bson::BsonUtils::SetStringValue(document, "contentType", contentType);
            Core::Bson::BsonUtils::SetStringValue(document, "payload", payload);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string SSMClientCommand::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const SSMClientCommand &r) {
        os << "SSMClientCommand=" << r.ToJson();
        return os;
    }
}// namespace AwsMock::Dto::Common
