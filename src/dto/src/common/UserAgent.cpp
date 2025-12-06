//
// Created by vogje01 on 11/26/23.
//

#include <awsmock/dto/common/UserAgent.h>

namespace AwsMock::Dto::Common {

    void UserAgent::FromRequest(const http::request<http::dynamic_body> &request) {

        if (!Core::HttpUtils::HasHeader(request, "User-Agent")) {

            // C++ does not send a user agent
            type = UserAgentType::AWS_SDK_CPP;
            contentType = "xml";
            return;
        }

        // We have a user agent, so use i
        std::string userAgentHeader = request["User-Agent"];

        if (const std::vector<std::string> parts = Core::StringUtils::Split(request["User-Agent"], " "); parts.empty() || parts.size() >= 4) {
            type = UserAgentTypeFromString(parts[0]);
            if (type == UserAgentType::AWS_CLI) {

                clientApplication = parts[0];
                clientLanguage = parts[1];
                clientOs = parts[2];
                clientExecutableType = parts[3];
                clientPrompt = parts[4];
                const std::string command = Core::StringUtils::Split(parts[parts.size() - 1], "#")[1];
                clientModule = Core::StringUtils::Split(command, ".")[0];
                clientCommand = Core::StringUtils::Split(command, ".")[1];
            }
        }

        if (Core::HttpUtils::HasHeader(request, "Content-Type")) {
            contentType = Core::StringUtils::Contains(request["Content-Type"], "json") ? "json" : "xml";
        }
        log_trace << "UserAgent: " << ToString();
    }

    std::string UserAgent::ToJson() const {

        try {
            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "clientApplication", clientApplication);
            Core::Bson::BsonUtils::SetStringValue(document, "clientLanguage", clientLanguage);
            Core::Bson::BsonUtils::SetStringValue(document, "clientExecutableType", clientExecutableType);
            Core::Bson::BsonUtils::SetStringValue(document, "clientPrompt", clientPrompt);
            Core::Bson::BsonUtils::SetStringValue(document, "clientModule", clientModule);
            Core::Bson::BsonUtils::SetStringValue(document, "clientCommand", clientCommand);
            Core::Bson::BsonUtils::SetStringValue(document, "contentType", contentType);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string UserAgent::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const UserAgent &r) {
        os << "UserAgent=" << r.ToJson();
        return os;
    }
}// namespace AwsMock::Dto::Common
