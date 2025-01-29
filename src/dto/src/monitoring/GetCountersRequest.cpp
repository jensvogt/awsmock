//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/monitoring/GetCountersRequest.h>

namespace AwsMock::Dto::Monitoring {

    GetCountersRequest GetCountersRequest::FromJson(const std::string &body) {

        try {
            const value documentValue = bsoncxx::from_json(body);
            GetCountersRequest request;
            request.region = Core::Bson::BsonUtils::GetStringValue(documentValue, "region");
            request.name = Core::Bson::BsonUtils::GetStringValue(documentValue, "name");
            request.labelName = Core::Bson::BsonUtils::GetStringValue(documentValue, "labelName");
            request.labelValue = Core::Bson::BsonUtils::GetStringValue(documentValue, "labelValue");
            request.step = Core::Bson::BsonUtils::GetIntValue(documentValue, "step");
            request.start = Core::DateTimeUtils::FromUnixTimestamp(Core::Bson::BsonUtils::GetLongValue(documentValue, "start"));
            request.end = Core::DateTimeUtils::FromUnixTimestamp(Core::Bson::BsonUtils::GetLongValue(documentValue, "end"));

            return request;

        } catch (Core::JsonException &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string GetCountersRequest::ToJson() const {

        try {
            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "name", name);
            Core::Bson::BsonUtils::SetStringValue(document, "labelName", labelName);
            Core::Bson::BsonUtils::SetStringValue(document, "labelValue", labelValue);
            Core::Bson::BsonUtils::SetIntValue(document, "step", step);
            Core::Bson::BsonUtils::SetDateValue(document, "start", start);
            Core::Bson::BsonUtils::SetDateValue(document, "end", end);

            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (Core::JsonException &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string GetCountersRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const GetCountersRequest &r) {
        os << "GetCountersRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Monitoring
