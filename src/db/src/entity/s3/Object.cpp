//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/s3/Object.h>

namespace AwsMock::Database::Entity::S3 {

    view_or_value<view, value> Object::ToDocument() const {

        auto metadataDoc = document{};
        for (const auto &[fst, snd]: metadata) {
            metadataDoc.append(kvp(fst, snd));
        }

        view_or_value<view, value> objectDoc = make_document(
                kvp("region", region),
                kvp("bucket", bucket),
                kvp("bucketArn", bucketArn),
                kvp("key", key),
                kvp("owner", owner),
                kvp("size", static_cast<bsoncxx::types::b_int64>(size)),
                kvp("md5sum", md5sum),
                kvp("sha1sum", sha1sum),
                kvp("sha256sum", sha256sum),
                kvp("contentType", contentType),
                kvp("metadata", metadataDoc),
                kvp("internalName", internalName),
                kvp("localName", localName),
                kvp("versionId", versionId),
                kvp("created", bsoncxx::types::b_date(created)),
                kvp("modified", bsoncxx::types::b_date(modified)));

        return objectDoc;
    }

    void Object::FromDocument(const std::optional<view> &mResult) {
        oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
        region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
        bucket = Core::Bson::BsonUtils::GetStringValue(mResult, "bucket");
        bucketArn = Core::Bson::BsonUtils::GetStringValue(mResult, "bucketArn");
        key = Core::Bson::BsonUtils::GetStringValue(mResult, "key");
        owner = Core::Bson::BsonUtils::GetStringValue(mResult, "owner");
        size = Core::Bson::BsonUtils::GetLongValue(mResult, "size");
        md5sum = Core::Bson::BsonUtils::GetStringValue(mResult, "md5sum");
        sha1sum = Core::Bson::BsonUtils::GetStringValue(mResult, "sha1sum");
        sha256sum = Core::Bson::BsonUtils::GetStringValue(mResult, "sha256sum");
        contentType = Core::Bson::BsonUtils::GetStringValue(mResult, "contentType");
        internalName = Core::Bson::BsonUtils::GetStringValue(mResult, "internalName");
        localName = Core::Bson::BsonUtils::GetStringValue(mResult, "localName");
        versionId = Core::Bson::BsonUtils::GetStringValue(mResult, "versionId");
        created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
        modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

        // Get metadata
        if (mResult.value().find("metadata") != mResult.value().end()) {
            for (const view metadataView = mResult.value()["metadata"].get_document().value; const bsoncxx::document::element &metadataElement: metadataView) {
                std::string metadataKey = bsoncxx::string::to_string(metadataElement.key());
                std::string metadataValue = bsoncxx::string::to_string(metadataView[metadataKey].get_string().value);
                metadata.emplace(metadataKey, metadataValue);
            }
        }
    }

}// namespace AwsMock::Database::Entity::S3