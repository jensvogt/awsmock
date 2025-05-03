//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/sqs/SendMessageResponse.h>

namespace AwsMock::Dto::SQS {
    /*
    std::string SendMessageResponse::ToJson() const {

        try {
            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "MessageId", messageId);
            Core::Bson::BsonUtils::SetStringValue(document, "MD5OfMessageBody", md5Body);
            Core::Bson::BsonUtils::SetStringValue(document, "MD5OfMessageAttributes", md5MessageAttributes);
            Core::Bson::BsonUtils::SetStringValue(document, "MD5OfMessageSystemAttributes", md5SystemAttributes);
            Core::Bson::BsonUtils::SetStringValue(document, "SequenceNumber", sequenceNumber);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void SendMessageResponse::FromJson(const std::string &jsonString) {

        try {

            const value documentValue = bsoncxx::from_json(jsonString);
            messageId = Core::Bson::BsonUtils::GetStringValue(documentValue, "MessageId");
            sequenceNumber = Core::Bson::BsonUtils::GetStringValue(documentValue, "SequenceNumber");
            md5Body = Core::Bson::BsonUtils::GetStringValue(documentValue, "MD5OfMessageBody");
            md5MessageAttributes = Core::Bson::BsonUtils::GetStringValue(documentValue, "MD5OfMessageAttributes");
            md5SystemAttributes = Core::Bson::BsonUtils::GetStringValue(documentValue, "MD5OfMessageSystemAttributes");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void SendMessageResponse::FromXml(const std::string &xmlString) {

        try {

            boost::property_tree::ptree pt;
            read_xml(xmlString, pt);
            messageId = pt.get<std::string>("MessageId");
            md5Body = pt.get<std::string>("MD5OfMessageBody");
            md5MessageAttributes = pt.get<std::string>("MD5OfMessageAttributes");
            md5SystemAttributes = pt.get<std::string>("MD5OfMessageSystemAttributes");
            sequenceNumber = pt.get<std::string>("SequenceNumber");

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string SendMessageResponse::ToXml() const {

        try {

            boost::property_tree::ptree pt;
            pt.put("SendMessageResponse.SendMessageResult.MessageId", messageId);
            pt.put("SendMessageResponse.SendMessageResult.MD5OfMessageBody", md5Body);
            pt.put("SendMessageResponse.SendMessageResult.MD5OfMessageAttributes", md5MessageAttributes);
            pt.put("SendMessageResponse.SendMessageResult.MD5OfMessageSystemAttributes", md5SystemAttributes);
            pt.put("SendMessageResponse.SendMessageResult.SequenceNumber", sequenceNumber);
            pt.put("SendMessageResponse.ResponseMetadata.RequestId", sequenceNumber);
            pt.put("SendMessageResponse.ResponseMetadata.requestId", sequenceNumber);
            return Core::XmlUtils::ToXmlString(pt);

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string SendMessageResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const SendMessageResponse &r) {
        os << "SendMessageResponse={" + r.ToJson();
        return os;
    }*/

}// namespace AwsMock::Dto::SQS
