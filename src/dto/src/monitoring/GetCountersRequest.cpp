//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/monitoring/GetCountersRequest.h>

namespace AwsMock::Dto::Monitoring {

    GetCountersRequest GetCountersRequest::FromJson(const std::string &jsonString) {

        GetCountersRequest request;

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(jsonString);
        const auto &rootObject = result.extract<Poco::JSON::Object::Ptr>();

        try {
            if (!rootObject->get("region").isEmpty()) {
                request.region = rootObject->get("region").convert<std::string>();
            }
            if (!rootObject->get("name").isEmpty()) {
                request.name = rootObject->get("name").convert<std::string>();
            }
            if (!rootObject->get("labelName").isEmpty()) {
                request.name = rootObject->get("labelName").convert<std::string>();
            }
            if (!rootObject->get("labelValue").isEmpty()) {
                request.name = rootObject->get("labelValue").convert<std::string>();
            }

            request.start = Core::DateTimeUtils::FromISO8601(rootObject->get("start").convert<std::string>());
            request.end = Core::DateTimeUtils::FromISO8601(rootObject->get("end").convert<std::string>());

            return request;
        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    std::string GetCountersRequest::ToJson() const {

        try {
            Poco::JSON::Object rootJson;
            rootJson.set("region", region);
            rootJson.set("name", name);
            rootJson.set("labelName", labelName);
            rootJson.set("labelValue", labelValue);

            return Core::JsonUtils::ToJsonString(rootJson);

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    std::string GetCountersRequest::ToString() const {
        std::stringstream ss;
        ss << (*this);
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const GetCountersRequest &r) {
        os << "GetCountersRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Monitoring