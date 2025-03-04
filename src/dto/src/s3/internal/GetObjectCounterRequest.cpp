//
// Created by vogje01 on 31/05/2023.
//

#include <awsmock/dto/s3/internal/GetObjectCounterRequest.h>

namespace AwsMock::Dto::S3 {

    void GetObjectCounterRequest::FromJson(const std::string &body) {

        try {

            value document = bsoncxx::from_json(body);
            region = Core::Bson::BsonUtils::GetStringValue(document, "region");
            oid = Core::Bson::BsonUtils::GetStringValue(document, "id");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string GetObjectCounterRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "oid", oid);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string GetObjectCounterRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const GetObjectCounterRequest &r) {
        os << "GetObjectCounterRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::S3
