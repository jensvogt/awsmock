//
// Created by vogje01 on 7/6/24.
//

#include <awsmock/dto/sqs/model/MessageSuccessful.h>

namespace AwsMock::Dto::SQS {

    std::string MessageSuccessful::ToJson() const {

        try {

            return Core::Bson::BsonUtils::ToJsonString(ToDocument());

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    view_or_value<view, value> MessageSuccessful::ToDocument() const {

        try {
            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Id", id);
            Core::Bson::BsonUtils::SetStringValue(document, "MessageId", messageId);
            Core::Bson::BsonUtils::SetStringValue(document, "SequenceNumber", sequenceNumber);
            Core::Bson::BsonUtils::SetStringValue(document, "MD5OfMessageBody", md5Body);
            Core::Bson::BsonUtils::SetStringValue(document, "MD5OfMessageAttributes", md5MessageAttributes);
            Core::Bson::BsonUtils::SetStringValue(document, "MD5OfMessageSystemAttributes", md5SystemAttributes);
            return document.extract();

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void MessageSuccessful::FromDocument(const view_or_value<view, value> &document) {

        try {

            id = Core::Bson::BsonUtils::GetStringValue(document, "Id");
            messageId = Core::Bson::BsonUtils::GetStringValue(document, "MessageId");
            sequenceNumber = Core::Bson::BsonUtils::GetStringValue(document, "SequenceNumber");
            md5Body = Core::Bson::BsonUtils::GetStringValue(document, "MD5OfMessageBody");
            md5MessageAttributes = Core::Bson::BsonUtils::GetStringValue(document, "MD5OfMessageAttributes");
            md5SystemAttributes = Core::Bson::BsonUtils::GetStringValue(document, "MD5OfMessageSystemAttributes");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    [[nodiscard]] std::string MessageSuccessful::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const MessageSuccessful &r) {
        os << "MessageSuccessful=" + r.ToJson();
        return os;
    }
};// namespace AwsMock::Dto::SQS
