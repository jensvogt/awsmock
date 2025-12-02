//
// Created by vogje01 on 11/26/23.
//

#include <awsmock/dto/common/SNSClientCommand.h>

namespace AwsMock::Dto::Common {

    void SNSClientCommand::FromRequest(const http::request<http::dynamic_body> &request, const std::string &awsRegion, const std::string &awsUser) {

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
        this->headers = Core::HttpUtils::GetHeaders(request);
        this->requestId = Core::HttpUtils::GetHeaderValue(request, "amz-sdk-invocation-id", Core::AwsUtils::CreateRequestId());

        if (userAgent.clientCommand.empty()) {

            this->command = SNSCommandTypeFromString(GetCommandFromHeader(request));

        } else {

            this->command = SNSCommandTypeFromString(userAgent.clientCommand);
        }
        log_trace << "Client command: " << ToJson();
        log_trace << "Client payload: " << this->payload;
    }

    std::string SNSClientCommand::GetCommandFromHeader(const http::request<http::dynamic_body> &request) const {

        std::string cmd;
        const std::string cType = request["Content-Type"];
        if (Core::HttpUtils::HasHeader(request, "x-awsmock-target")) {

            cmd = Core::HttpUtils::GetHeaderValue(request, "x-awsmock-action");

        } else if (Core::StringUtils::ContainsIgnoreCase(cType, "application/x-www-form-urlencoded")) {

            cmd = Core::HttpUtils::GetStringParameter(payload, "Action");

        } else if (Core::StringUtils::ContainsIgnoreCase(cType, "application/x-amz-json-1.0")) {

            cmd = Core::StringUtils::Split(request["X-Amz-Target"], ".")[1];
        }
        return Core::StringUtils::ToSnakeCase(cmd);
    }

    std::string SNSClientCommand::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "method", boost::lexical_cast<std::string>(method));
            Core::Bson::BsonUtils::SetStringValue(document, "command", SNSCommandTypeToString(command));
            Core::Bson::BsonUtils::SetStringValue(document, "user", user);
            Core::Bson::BsonUtils::SetStringValue(document, "contentType", contentType);
            Core::Bson::BsonUtils::SetLongValue(document, "contentLength", contentLength);
            Core::Bson::BsonUtils::SetStringValue(document, "requestId", requestId);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string SNSClientCommand::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const SNSClientCommand &r) {
        os << "SNSClientCommand=" << r.ToJson();
        return os;
    }
}// namespace AwsMock::Dto::Common
