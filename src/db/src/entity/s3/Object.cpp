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
                kvp("versionId", versionId),
                kvp("created", bsoncxx::types::b_date(created)),
                kvp("modified", bsoncxx::types::b_date(modified)));

        return objectDoc;
    }

    void Object::FromDocument(const view_or_value<view, value> &mResult) {
        oid = Core::Bson::BsonUtils::GetOidValue(mResult.view()["_id"]);
        region = Core::Bson::BsonUtils::GetStringValue(mResult.view()["region"]);
        bucket = Core::Bson::BsonUtils::GetStringValue(mResult.view()["bucket"]);
        bucketArn = Core::Bson::BsonUtils::GetStringValue(mResult.view()["bucketArn"]);
        key = Core::Bson::BsonUtils::GetStringValue(mResult.view()["key"]);
        owner = Core::Bson::BsonUtils::GetStringValue(mResult.view()["owner"]);
        size = Core::Bson::BsonUtils::GetLongValue(mResult.view()["size"]);
        md5sum = Core::Bson::BsonUtils::GetStringValue(mResult.view()["md5sum"]);
        sha1sum = Core::Bson::BsonUtils::GetStringValue(mResult.view()["sha1sum"]);
        sha256sum = Core::Bson::BsonUtils::GetStringValue(mResult.view()["sha256sum"]);
        contentType = Core::Bson::BsonUtils::GetStringValue(mResult.view()["contentType"]);
        internalName = Core::Bson::BsonUtils::GetStringValue(mResult.view()["internalName"]);
        versionId = Core::Bson::BsonUtils::GetStringValue(mResult.view()["versionId"]);
        created = Core::Bson::BsonUtils::GetDateValue(mResult.view()["created"]);
        modified = Core::Bson::BsonUtils::GetDateValue(mResult.view()["modified"]);

        // Get metadata
        if (mResult.view().find("metadata") != mResult.view().end()) {
            for (const view metadataView = mResult.view()["metadata"].get_document().value; const bsoncxx::document::element &metadataElement: metadataView) {
                std::string metadataKey = bsoncxx::string::to_string(metadataElement.key());
                std::string metadataValue = bsoncxx::string::to_string(metadataView[metadataKey].get_string().value);
                metadata.emplace(metadataKey, metadataValue);
            }
        }
    }

}// namespace AwsMock::Database::Entity::S3