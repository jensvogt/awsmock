//
// Created by vogje01 on 19/07/2026.
//

#include <awsmock/dto/common/STSClientCommand.h>

namespace Awsmock::Dto::Common {

    void STSClientCommand::FromRequest(const http::request<http::dynamic_body> &request, const std::string &awsRegion, const std::string &awsUser) {

        UserAgent userAgent;
        userAgent.FromRequest(request);

        this->region = awsRegion;
        this->user = awsUser;
        this->method = request.method();
        this->contentType = Core::HttpUtils::GetContentType(request);
        this->contentLength = Core::HttpUtils::GetContentLength(request);
        this->payload = Core::HttpUtils::GetBodyAsString(request);
        this->url = request.target();
        this->requestId = Core::HttpUtils::GetHeaderValue(request, "RequestId", Core::AwsUtils::CreateRequestId());

        // STS uses Action= in a form-urlencoded POST body
        const std::string action = Core::HttpUtils::GetStringParameterFromBody(this->payload, "Action");
        this->command = STSCommandTypeFromString(Core::StringUtils::ToSnakeCase(action));
    }

    std::string STSClientCommand::ToJson() const {

        try {
            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "method", boost::lexical_cast<std::string>(method));
            Core::Bson::BsonUtils::SetStringValue(document, "command", STSCommandTypeToString(command));
            Core::Bson::BsonUtils::SetStringValue(document, "user", user);
            Core::Bson::BsonUtils::SetStringValue(document, "contentType", contentType);
            Core::Bson::BsonUtils::SetStringValue(document, "payload", payload);
            return Core::Bson::BsonUtils::ToJsonString(document);
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string STSClientCommand::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const STSClientCommand &r) {
        os << "STSClientCommand=" << r.ToJson();
        return os;
    }
}// namespace Awsmock::Dto::Common
