//
// Created by vogje01 on 12/18/23.
//

#include <awsmock/dto/s3/model/BucketCounter.h>

namespace AwsMock::Dto::S3 {

    view_or_value<view, value> BucketCounter::ToDocument() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "bucketName", bucketName);
            Core::Bson::BsonUtils::SetLongValue(document, "keys", keys);
            Core::Bson::BsonUtils::SetLongValue(document, "size", size);
            Core::Bson::BsonUtils::SetStringValue(document, "owner", owner);
            Core::Bson::BsonUtils::SetDateValue(document, "created", created);
            Core::Bson::BsonUtils::SetDateValue(document, "modified", modified);
            return document.extract();

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string BucketCounter::ToJson() const {
        return Core::Bson::BsonUtils::ToJsonString(ToDocument());
    }

    std::string BucketCounter::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const BucketCounter &c) {
        os << "BucketCounter=" << c.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::S3
