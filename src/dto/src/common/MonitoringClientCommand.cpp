//
// Created by vogje01 on 11/26/23.
//

#include <awsmock/dto/common/MonitoringClientCommand.h>

namespace AwsMock::Dto::Common {

    void MonitoringClientCommand::FromRequest(const http::request<http::dynamic_body> &request, const std::string &awsRegion, const std::string &awsUser) {

        // Basic values
        this->method = request.method();
        this->region = awsRegion;
        this->user = awsUser;
        this->url = request.target();
        this->contentType = Core::HttpUtils::GetContentType(request);
        this->contentLength = Core::HttpUtils::GetContentLength(request);
        this->payload = Core::HttpUtils::GetBodyAsString(request);
        this->headers = Core::HttpUtils::GetHeaders(request);
        this->requestId = Core::HttpUtils::GetHeaderValue(request, "RequestId", Core::AwsUtils::CreateRequestId());

        if (Core::HttpUtils::HasHeader(request, "x-awsmock-target")) {

            std::string target = Core::HttpUtils::GetHeaderValue(request, "x-awsmock-target");
            const std::string action = Core::HttpUtils::GetHeaderValue(request, "x-awsmock-action");
            command = MonitoringCommandTypeFromString(action);
        }
    }

    std::string MonitoringClientCommand::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "method", std::string(to_string(method)));
            Core::Bson::BsonUtils::SetStringValue(document, "command", MonitoringCommandTypeToString(command));
            Core::Bson::BsonUtils::SetStringValue(document, "contentType", contentType);
            Core::Bson::BsonUtils::SetIntValue(document, "contentLength", contentLength);
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string MonitoringClientCommand::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const MonitoringClientCommand &r) {
        os << "MonitoringClientCommand=" << r.ToJson();
        return os;
    }
}// namespace AwsMock::Dto::Common
