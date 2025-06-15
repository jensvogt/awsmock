
#include <awsmock/dto/s3/ListBucketResponse.h>

namespace AwsMock::Dto::S3 {

    ListBucketResponse::ListBucketResponse(const std::string &bucket, const std::vector<Database::Entity::S3::Object> &objectList) {

        maxKeys = 1000;
        name = bucket;

        if (objectList.empty()) {
            return;
        }

        for (auto &it: objectList) {
            Owner owner;
            owner.displayName = it.owner;
            owner.id = it.owner;

            Content content;
            content.key = it.key;
            content.etag = it.md5sum;
            content.size = it.size;
            content.owner = owner;
            content.storageClass = "STANDARD";
            content.modified = system_clock::now();
            contents.push_back(content);
        }
    }

    std::string ListBucketResponse::ToJson() const {

        try {
            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "name", name);
            Core::Bson::BsonUtils::SetBoolValue(rootDocument, "isTruncated", isTruncated);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "prefix", prefix);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "delimiter", delimiter);
            Core::Bson::BsonUtils::SetIntValue(rootDocument, "maxKeys", maxKeys);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "encodingType", encodingType);
            Core::Bson::BsonUtils::SetIntValue(rootDocument, "keyCount", keyCount);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "continuationToken", continuationToken);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "nextContinuationToken", nextContinuationToken);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "startAfter", startAfter);
            Core::Bson::BsonUtils::SetLongValue(rootDocument, "total", total);

            // Contents
            if (!contents.empty()) {

                array jsonArray;
                for (auto &it: contents) {
                    document element;
                    Core::Bson::BsonUtils::SetStringValue(rootDocument, "key", it.key);
                    Core::Bson::BsonUtils::SetDateValue(rootDocument, "lastModified", it.modified);
                    Core::Bson::BsonUtils::SetStringValue(rootDocument, "etag", it.etag);
                    Core::Bson::BsonUtils::SetStringValue(rootDocument, "owner", it.owner.id);
                    Core::Bson::BsonUtils::SetStringValue(rootDocument, "displayName", it.owner.displayName);
                    Core::Bson::BsonUtils::SetStringValue(rootDocument, "id", it.owner.id);
                    Core::Bson::BsonUtils::SetLongValue(rootDocument, "size", it.size);
                    Core::Bson::BsonUtils::SetStringValue(rootDocument, "storageClass", it.storageClass);
                    jsonArray.append(element);
                }
                rootDocument.append(kvp("content", jsonArray));
            }

            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListBucketResponse::ToXml() {

        boost::property_tree::ptree root;
        root.add("ListBucketResult.IsTruncated", isTruncated);
        root.add("ListBucketResult.Name", name);
        root.add("ListBucketResult.Prefix", prefix);
        root.add("ListBucketResult.Delimiter", delimiter);
        root.add("ListBucketResult.MaxKeys", maxKeys);
        root.add("ListBucketResult.EncodingType", encodingType);
        root.add("ListBucketResult.KeyCount", keyCount);
        root.add("ListBucketResult.ContinuationToken", continuationToken);
        root.add("ListBucketResult.NextContinuationToken", nextContinuationToken);
        root.add("ListBucketResult.StartAfter", startAfter);

        // Contents
        if (!contents.empty()) {
            for (auto &it: contents) {
                boost::property_tree::ptree xmlContent;
                xmlContent.add("Key", it.key);
                xmlContent.add("LastModified", Core::DateTimeUtils::ToISO8601(it.modified));
                xmlContent.add("ETag", it.etag);
                xmlContent.add("Size", it.size);
                xmlContent.add("StorageClass", it.storageClass);
                xmlContent.add("Owner.DisplayName", it.owner.displayName);
                xmlContent.add("Owner.ID", it.owner.id);
                root.add_child("ListBucketResult.Contents", xmlContent);
            }
        }
        return Core::XmlUtils::ToXmlString(root);
    }

    [[nodiscard]] std::string ListBucketResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListBucketResponse &r) {
        os << "ListBucketResult=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::S3