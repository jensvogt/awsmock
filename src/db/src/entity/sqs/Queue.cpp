//
// Created by vogje01 on 01/06/2023.
//

#include <awsmock/entity/sqs/Queue.h>

namespace AwsMock::Database::Entity::SQS {

    view_or_value<view, value> Queue::ToDocument() const {

        auto tagsDoc = document{};
        if (!tags.empty()) {
            for (const auto &[fst, snd]: tags) {
                tagsDoc.append(kvp(fst, snd));
            }
        }

        document rootDocument;
        Core::Bson::BsonUtils::SetStringValue(rootDocument, "region", region);
        Core::Bson::BsonUtils::SetStringValue(rootDocument, "owner", owner);
        Core::Bson::BsonUtils::SetStringValue(rootDocument, "name", name);
        Core::Bson::BsonUtils::SetStringValue(rootDocument, "queueUrl", queueUrl);
        Core::Bson::BsonUtils::SetStringValue(rootDocument, "queueArn", queueArn);
        Core::Bson::BsonUtils::SetDocumentValue(rootDocument, "attributes", attributes.ToDocument());
        Core::Bson::BsonUtils::SetDocumentValue(rootDocument, "tags", tagsDoc);
        Core::Bson::BsonUtils::SetLongValue(rootDocument, "size", size);
        Core::Bson::BsonUtils::SetBoolValue(rootDocument, "isDlq", isDlq);
        Core::Bson::BsonUtils::SetStringValue(rootDocument, "mainQueue", mainQueue);
        Core::Bson::BsonUtils::SetDateValue(rootDocument, "created", created);
        Core::Bson::BsonUtils::SetDateValue(rootDocument, "modified", modified);

        return rootDocument.extract();
    }

    Queue Queue::FromDocument(const std::optional<view> &mResult) {

        try {
            oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
            region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
            name = Core::Bson::BsonUtils::GetStringValue(mResult, "name");
            owner = Core::Bson::BsonUtils::GetStringValue(mResult, "owner");
            queueUrl = Core::Bson::BsonUtils::GetStringValue(mResult, "queueUrl");
            queueArn = Core::Bson::BsonUtils::GetStringValue(mResult, "queueArn");
            attributes.FromDocument(mResult.value()["attributes"].get_document().value);
            size = Core::Bson::BsonUtils::GetLongValue(mResult, "size");
            isDlq = Core::Bson::BsonUtils::GetBoolValue(mResult, "isDlq");
            mainQueue = Core::Bson::BsonUtils::GetStringValue(mResult, "mainQueue");
            created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
            modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

            // Get tags
            if (mResult.value().find("tags") != mResult.value().end()) {
                for (const view tagsView = mResult.value()["tags"].get_document().value; const bsoncxx::document::element &tagElement: tagsView) {
                    std::string key = bsoncxx::string::to_string(tagElement.key());
                    std::string value = bsoncxx::string::to_string(tagsView[key].get_string().value);
                    tags.emplace(key, value);
                }
            }

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
        return *this;
    }

}// namespace AwsMock::Database::Entity::SQS
