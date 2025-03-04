//
// Created by vogje01 on 7/6/24.
//

#include <awsmock/dto/sqs/model/EventRecord.h>

namespace AwsMock::Dto::SQS {
    std::string Record::ToJson() const {
        try {
            return Core::Bson::BsonUtils::ToJsonString(ToDocument());
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    view_or_value<view, value> Record::ToDocument() const {
        try {
            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "awsRegion", region);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "messageId", messageId);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "receiptHandle", receiptHandle);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "body", body);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "md5OfBody", md5Sum);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "eventSource", eventSource);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "eventSourceARN", eventSourceArn);

            if (!messageAttributes.empty()) {
                document jsonMessageAttributeObject;
                for (const auto &[fst, snd]: messageAttributes) {
                    jsonMessageAttributeObject.append(kvp(fst, snd.ToDocument()));
                }
                rootDocument.append(kvp("messageAttributes", jsonMessageAttributeObject));
            }

            if (!attributes.empty()) {
                document jsonAttributeObject;
                jsonAttributeObject.append(kvp("ApproximateReceiveCount", "1"));
                jsonAttributeObject.append(kvp("ApproximateFirstReceiveTimestamp", bsoncxx::types::b_int64(Core::DateTimeUtils::UnixTimestampMs(system_clock::now()))));
                jsonAttributeObject.append(kvp("SenderId", Core::AwsUtils::CreateSQSSenderId()));
                jsonAttributeObject.append(kvp("SentTimestamp", bsoncxx::types::b_int64(Core::DateTimeUtils::UnixTimestampMs(system_clock::now()))));
                rootDocument.append(kvp("attributes", jsonAttributeObject));
            }
            return rootDocument.extract();
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void Record::FromDocument(const view_or_value<view, value> &document) {
        try {
            region = Core::Bson::BsonUtils::GetStringValue(document, "awsRegion");
            messageId = Core::Bson::BsonUtils::GetStringValue(document, "messageId");
            receiptHandle = Core::Bson::BsonUtils::GetStringValue(document, "receiptHandle");
            body = Core::Bson::BsonUtils::GetStringValue(document, "body");
            md5Sum = Core::Bson::BsonUtils::GetStringValue(document, "md5OfBody");
            eventSource = Core::Bson::BsonUtils::GetStringValue(document, "eventSource");
            eventSourceArn = Core::Bson::BsonUtils::GetStringValue(document, "eventSourceArn");
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    [[nodiscard]] std::string Record::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Record &r) {
        os << "Record=" + r.ToJson();
        return os;
    }
};// namespace AwsMock::Dto::SQS
