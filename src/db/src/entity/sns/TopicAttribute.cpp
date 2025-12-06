//
// Created by vogje01 on 12/10/23.
//

#include <awsmock/entity/sns/TopicAttribute.h>

namespace AwsMock::Database::Entity::SNS {

    view_or_value<view, value> TopicAttribute::ToDocument() const {

        view_or_value<view, value> topicAttributeDoc = make_document(
                kvp("deliveryPolicy", deliveryPolicy),
                kvp("displayName", displayName),
                kvp("fifoTopic", fifoTopic),
                kvp("policy", policy),
                kvp("signatureVersion", signatureVersion),
                kvp("tracingConfig", tracingConfig),
                kvp("kmsMasterKeyId", kmsMasterKeyId),
                kvp("archivePolicy", archivePolicy),
                kvp("beginningArchiveTime", bsoncxx::types::b_date(beginningArchiveTime)),
                kvp("contentBasedDeduplication", contentBasedDeduplication),
                kvp("availableMessages", bsoncxx::types::b_int64(availableMessages)));

        return topicAttributeDoc;
    }

    void TopicAttribute::FromDocument(const std::optional<view> &mResult) {

        try {

            deliveryPolicy = Core::Bson::BsonUtils::GetStringValue(mResult, "deliveryPolicy");
            displayName = Core::Bson::BsonUtils::GetStringValue(mResult, "displayName");
            fifoTopic = Core::Bson::BsonUtils::GetBoolValue(mResult, "fifoTopic");
            policy = Core::Bson::BsonUtils::GetStringValue(mResult, "policy");
            signatureVersion = Core::Bson::BsonUtils::GetStringValue(mResult, "signatureVersion");
            tracingConfig = Core::Bson::BsonUtils::GetStringValue(mResult, "tracingConfig");
            kmsMasterKeyId = Core::Bson::BsonUtils::GetStringValue(mResult, "kmsMasterKeyId");
            archivePolicy = Core::Bson::BsonUtils::GetStringValue(mResult, "archivePolicy");
            beginningArchiveTime = Core::Bson::BsonUtils::GetDateValue(mResult, "beginningArchiveTime");
            contentBasedDeduplication = Core::Bson::BsonUtils::GetBoolValue(mResult, "contentBasedDeduplication");
            availableMessages = Core::Bson::BsonUtils::GetLongValue(mResult, "availableMessages");

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string TopicAttribute::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const TopicAttribute &t) {
        os << "TopicAttribute=" << to_json(t.ToDocument());
        return os;
    }

}// namespace AwsMock::Database::Entity::SNS
