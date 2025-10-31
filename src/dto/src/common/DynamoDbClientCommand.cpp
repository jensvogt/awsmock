//
// Created by vogje01 on 11/26/23.
//

#include <awsmock/dto/common/DynamoDbClientCommand.h>

namespace AwsMock::Dto::Common {

    void DynamoDbClientCommand::FromRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {

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

        switch (method) {
            case http::verb::post:
                command = DynamoDbCommandTypeFromString(GetClientCommandFromHeader(request));
                break;
            case http::verb::get:
            case http::verb::put:
            case http::verb::delete_:
            case http::verb::head:
                break;
            default: ;
        }
    }

    std::string DynamoDbClientCommand::GetClientCommandFromHeader(const http::request<http::dynamic_body> &request) {

        if (Core::HttpUtils::HasHeader(request, "X-Amz-Target")) {
            const std::string headerValue = Core::HttpUtils::GetHeaderValue(request, "X-Amz-Target");
            const std::string action = Core::StringUtils::Split(headerValue, ".")[1];
            return Core::StringUtils::ToSnakeCase(action);
        }
        if (Core::HttpUtils::HasHeader(request, "x-awsmock-target") && Core::HttpUtils::GetHeaderValue(request, "x-awsmock-target") == "dynamodb") {
            return Core::StringUtils::ToSnakeCase(Core::HttpUtils::GetHeaderValue(request, "x-awsmock-action"));
        }
        return "unknown";
    }

    std::string DynamoDbClientCommand::ToJson() const {
        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "region", region);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "method", boost::lexical_cast<std::string>(method));
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "command", DynamoDbCommandTypeToString(command));
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "user", user);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "url", url);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "contentType", contentType);
            Core::Bson::BsonUtils::SetLongValue(rootDocument, "contentLength", contentLength);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "payload", payload);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "requestId", requestId);

            if (!headers.empty()) {
                array jsonArray;
                for (const auto &[fst, snd]: headers) {
                    document jsonHeaderObject;
                    jsonHeaderObject.append(kvp(fst, snd));
                    jsonArray.append(jsonHeaderObject);
                }
            }
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string DynamoDbClientCommand::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const DynamoDbClientCommand &r) {
        os << "DynamoDbClientCommand=" << r.ToJson();
        return os;
    }
}// namespace AwsMock::Dto::Common
