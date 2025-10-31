//
// Created by vogje01 on 11/26/23.
//

#include <awsmock/dto/common/SQSClientCommand.h>

namespace AwsMock::Dto::Common {

    void SQSClientCommand::FromRequest(const http::request<http::dynamic_body> &request, const std::string &awsRegion, const std::string &awsUser) {

        UserAgent userAgent;
        userAgent.FromRequest(request);

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

        if (userAgent.clientCommand.empty()) {

            this->command = SqsCommandTypeFromString(GetCommandFromHeader(request));

        } else {

            this->command = SqsCommandTypeFromString(userAgent.clientCommand);
        }
    }

    std::string SQSClientCommand::GetCommandFromHeader(const http::request<http::dynamic_body> &request) const {

        std::string cmd;
        const std::string cType = request["Content-Type"];
        if (Core::HttpUtils::HasHeader(request, "x-awsmock-target")) {

            // awsmock command from UI
            cmd = Core::HttpUtils::GetHeaderValue(request, "x-awsmock-action");

        } else if (Core::StringUtils::ContainsIgnoreCase(cType, "application/x-www-form-urlencoded")) {

            // AWS CLI
            cmd = Core::HttpUtils::GetStringParameter(payload, "Action");

        } else if (Core::StringUtils::ContainsIgnoreCase(cType, "application/x-amz-json-1.0")) {

            // AWS Java SDK
            const std::string headerValue = request["X-Amz-Target"];
            cmd = Core::StringUtils::Split(headerValue, ".")[1];
        }
        return Core::StringUtils::ToSnakeCase(cmd);
    }

    std::string SQSClientCommand::ToJson() const {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "region", region);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "user", user);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "url", url);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "method", boost::lexical_cast<std::string>(method));
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "contentType", contentType);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "payload", payload);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "command", SqsCommandTypeToString(command));

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

    std::string SQSClientCommand::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const SQSClientCommand &r) {
        os << "SQSClientCommand=" << r.ToJson();
        return os;
    }
}// namespace AwsMock::Dto::Common
