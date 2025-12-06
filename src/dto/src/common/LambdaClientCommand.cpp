//
// Created by vogje01 on 11/26/23.
//

#include <awsmock/dto/common/LambdaClientCommand.h>

namespace AwsMock::Dto::Common {

    void LambdaClientCommand::FromRequest(const http::request<http::dynamic_body> &request, const std::string &awsRegion, const std::string &user) {

        // Basic values
        this->method = request.method();
        this->region = awsRegion;
        this->user = user;
        this->url = request.target();
        this->contentType = Core::HttpUtils::GetContentType(request);
        this->contentLength = Core::HttpUtils::GetContentLength(request);
        this->payload = Core::HttpUtils::GetBodyAsString(request);
        this->headers = Core::HttpUtils::GetHeaders(request);
        this->requestId = Core::HttpUtils::GetHeaderValue(request, "RequestId", Core::AwsUtils::CreateRequestId());

        if (Core::HttpUtils::HasHeader(request, "x-awsmock-target") && Core::HttpUtils::GetHeaderValue(request, "x-awsmock-target") == "lambda") {

            const std::string cmd = Core::StringUtils::ToSnakeCase(Core::HttpUtils::GetHeaderValue(request, "x-awsmock-action"));
            this->command = LambdaCommandTypeFromString(cmd);

        } else {

            UserAgent userAgent;
            userAgent.FromRequest(request);
            if (userAgent.clientCommand.empty()) {

                this->command = LambdaCommandTypeFromString(GetCommandFromHeader(request));

            } else {

                this->command = LambdaCommandTypeFromString(userAgent.clientCommand);
            }
        }
    }

    std::string LambdaClientCommand::GetCommandFromHeader(const http::request<http::dynamic_body> &request) const {

        std::string cmd;
        if (const std::string cType = request["Content-Type"]; Core::StringUtils::ContainsIgnoreCase(cType, "application/x-www-form-urlencoded")) {

            cmd = Core::HttpUtils::GetStringParameter(payload, "Action");

        } else if (Core::StringUtils::ContainsIgnoreCase(cType, "application/x-amz-json-1.0")) {

            const std::string headerValue = request["X-Amz-Target"];
            cmd = Core::StringUtils::Split(headerValue, ".")[1];

        } else if (Core::HttpUtils::HasHeader(request, "x-awsmock-target") && Core::HttpUtils::GetHeaderValue(request, "x-awsmock-target") == "lambda") {

            cmd = Core::HttpUtils::GetHeaderValue(request, "x-awsmock-action");
        }
        return Core::StringUtils::ToSnakeCase(cmd);
    }

    std::string LambdaClientCommand::ToJson() const {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "method", boost::lexical_cast<std::string>(method));
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "region", region);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "user", user);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "url", url);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "contentType", contentType);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "payload", payload);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "command", LambdaCommandTypeToString(command));

            // Headers
            if (!headers.empty()) {
                array jsonHeaders;
                for (const auto &[fst, snd]: headers) {
                    document jsonHeader;
                    jsonHeader.append(kvp(fst, snd));
                    jsonHeaders.append(jsonHeader);
                }
                rootDocument.append(kvp("headers", jsonHeaders));
            }
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string LambdaClientCommand::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const LambdaClientCommand &r) {
        os << "LambdaClientCommand=" << r.ToJson();
        return os;
    }
}// namespace AwsMock::Dto::Common
