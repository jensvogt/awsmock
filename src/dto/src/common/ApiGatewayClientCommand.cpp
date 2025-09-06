//
// Created by vogje01 on 11/26/23.
//

#include "awsmock/dto/common/UserAgent.h"


#include <awsmock/dto/common/ApiGatewayClientCommand.h>

namespace AwsMock::Dto::Common {

    void ApiGatewayClientCommand::FromRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {

        // From AwsMock UI
        if (Core::HttpUtils::HasHeader(request, "x-awsmock-target") && Core::HttpUtils::GetHeaderValue(request, "x-awsmock-target") == "apigateway") {

            this->command = ApiGatewayCommandTypeFromString(Core::HttpUtils::GetHeaderValue(request, "x-awsmock-action"));

        } else {

            UserAgent userAgent;
            userAgent.FromRequest(request);
            this->command = ApiGatewayCommandTypeFromString(Core::StringUtils::ToSnakeCase(userAgent.clientCommand));
        }

        // Basic values
        this->region = region;
        this->user = user;
        this->method = request.method();
        this->url = request.target();
        this->contentType = Core::HttpUtils::GetContentType(request);
        this->contentLength = Core::HttpUtils::GetContentLength(request);
        this->payload = Core::HttpUtils::GetBodyAsString(request);
        this->headers = Core::HttpUtils::GetHeaders(request);
        this->requestId = Core::HttpUtils::GetHeaderValue(request, "RequestId", Core::AwsUtils::CreateRequestId());
    }

    std::string ApiGatewayClientCommand::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "method", boost::lexical_cast<std::string>(method));
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "command", ApiGatewayCommandTypeToString(command));
            Core::Bson::BsonUtils::SetStringValue(document, "url", url);
            Core::Bson::BsonUtils::SetStringValue(document, "contentType", contentType);
            Core::Bson::BsonUtils::SetLongValue(document, "contentLength", contentLength);
            Core::Bson::BsonUtils::SetStringValue(document, "payload", payload);
            Core::Bson::BsonUtils::SetStringValue(document, "requestId", requestId);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ApiGatewayClientCommand::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ApiGatewayClientCommand &r) {
        os << "ApiGatewayClientCommand=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Common
