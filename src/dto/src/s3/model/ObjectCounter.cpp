//
// Created by vogje01 on 12/18/23.
//

#include <awsmock/dto/s3/model/ObjectCounter.h>

namespace AwsMock::Dto::S3 {

    view_or_value<view, value> ObjectCounter::ToDocument() const {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "oid", oid);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "region", region);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "bucketName", bucketName);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "key", key);
            Core::Bson::BsonUtils::SetLongValue(rootDocument, "size", size);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "contentType", contentType);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "internalName", internalName);
            Core::Bson::BsonUtils::SetDateValue(rootDocument, "created", created);
            Core::Bson::BsonUtils::SetDateValue(rootDocument, "modified", modified);

            // Metadata
            if (!metadata.empty()) {
                array jsonMetadataArray;
                for (const auto &[fst, snd]: metadata) {
                    document jsonMetadataObject;
                    jsonMetadataObject.append(kvp("key", fst));
                    jsonMetadataObject.append(kvp("value", snd));
                    jsonMetadataArray.append(jsonMetadataObject);
                }
                rootDocument.append(kvp("metadata", jsonMetadataArray));
            }
            return rootDocument.extract();

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ObjectCounter::ToJson() const {

        try {

            return Core::Bson::BsonUtils::ToJsonString(ToDocument());

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ObjectCounter::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ObjectCounter &c) {
        os << "ObjectCounter=" << c.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::S3
