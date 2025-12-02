//
// Created by vogje01 on 11/26/23.
//

#include <awsmock/dto/common/KMSClientCommand.h>

namespace AwsMock::Dto::Common {

    void KMSClientCommand::FromRequest(const http::request<http::dynamic_body> &request, const std::string &awsRegion, const std::string &awsUser) {

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

            this->command = KMSCommandTypeFromString(GetCommandFromHeader(request));

        } else {

            this->command = KMSCommandTypeFromString(userAgent.clientCommand);
        }
    }

    std::string KMSClientCommand::GetCommandFromHeader(const http::request<http::dynamic_body> &request) {

        std::string cmd;
        if (Core::HttpUtils::HasHeader(request, "x-awsmock-target")) {

            cmd = Core::HttpUtils::GetHeaderValue(request, "x-awsmock-action");

        } else if (Core::HttpUtils::HasHeader(request, "X-Amz-Target")) {

            cmd = Core::StringUtils::Split(request["X-Amz-Target"], ".")[1];
        }
        return Core::StringUtils::ToSnakeCase(cmd);
    }

    std::string KMSClientCommand::ToJson() const {

        try {
            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "method", boost::lexical_cast<std::string>(method));
            Core::Bson::BsonUtils::SetStringValue(document, "command", KMSCommandTypeToString(command));
            Core::Bson::BsonUtils::SetStringValue(document, "contentType", contentType);
            Core::Bson::BsonUtils::SetStringValue(document, "user", user);
            Core::Bson::BsonUtils::SetStringValue(document, "payload", payload.length() > 256 ? Core::StringUtils::SubString(payload, 0, 256) : payload);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string KMSClientCommand::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const KMSClientCommand &r) {
        os << "KMSClientCommand=" << r.ToJson();
        return os;
    }
}// namespace AwsMock::Dto::Common
