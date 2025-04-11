//
// Created by vogje01 on 03/10/2023.
//

#include <awsmock/dto/sqs/ReceiveMessageResponse.h>
#include <awsmock/dto/sqs/mapper/Mapper.h>

namespace AwsMock::Dto::SQS {

    std::string ReceiveMessageResponse::ToJson() {

        try {

            document rootDocument;

            if (!messageList.empty()) {

                array messageArray;
                for (const auto &message: messageList) {

                    document messageDocument;
                    Core::Bson::BsonUtils::SetStringValue(messageDocument, "Body", message.body);
                    Core::Bson::BsonUtils::SetStringValue(messageDocument, "ReceiptHandle", message.receiptHandle);
                    Core::Bson::BsonUtils::SetStringValue(messageDocument, "MD5OfBody", message.md5Body);
                    Core::Bson::BsonUtils::SetStringValue(messageDocument, "MessageId", message.messageId);

                    // Message attributes, first converted to DTO, then to document
                    document messageAttributesDocument;
                    MessageAttributeList messageAttributeListDto = Mapper::map(message.messageAttributes);
                    for (const auto &[fst, snd]: messageAttributeListDto) {
                        document messageAttributeDocument;
                        Core::Bson::BsonUtils::SetStringValue(messageAttributeDocument, "StringValue", snd.stringValue);
                        Core::Bson::BsonUtils::SetLongValue(messageAttributeDocument, "NumberValue", snd.numberValue);
                        Core::Bson::BsonUtils::SetStringValue(messageAttributeDocument, "DataType", MessageAttributeDataTypeToString(snd.type));
                        messageAttributesDocument.append(kvp(fst, messageAttributeDocument));
                    }
                    messageDocument.append(kvp("MessageAttributes", messageAttributesDocument.extract()));

                    // Attributes
                    document attributeDocument;
                    for (const auto &[fst, snd]: message.attributes) {
                        attributeDocument.append(kvp(fst, snd));
                    }
                    messageDocument.append(kvp("Attributes", attributeDocument));

                    // MD5 of message attributes
                    Core::Bson::BsonUtils::SetStringValue(messageDocument, "MD5OfMessageAttributes", MessageAttribute::GetMd5Attributes(messageAttributeListDto));
                    messageArray.append(messageDocument);
                }

                // Add message array
                rootDocument.append(kvp("Messages", messageArray));
            }
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ReceiveMessageResponse::ToXml() const {

        boost::property_tree::ptree rootTree;
        const boost::property_tree::ptree messagesTree;
        for (auto &it: messageList) {

            // Message
            boost::property_tree::ptree messageTree;
            messageTree.put("MessageId", it.messageId);
            messageTree.put("ReceiptHandle", it.receiptHandle);
            messageTree.put("Body", it.body);
            messageTree.put("MD5OfBody", it.md5Body);
            messageTree.put("MD5OfMessageAttributes", it.md5MessageAttributes);

            // Attribute
            boost::property_tree::ptree attributesTree;
            for (const auto &[fst, snd]: it.attributes) {
                boost::property_tree::ptree attributeTree;
                attributeTree.put("Name", fst);
                attributeTree.put("Value", snd);
                attributesTree.push_back(std::make_pair("", attributeTree));
            }
            messageTree.push_back(std::make_pair("Attributes", attributesTree));

            // Message attributes
            boost::property_tree::ptree messageAttributesTree;
            for (const auto &at: it.messageAttributes) {
                boost::property_tree::ptree attributeTree;
                attributeTree.put("DataType", at.attributeType);
                attributeTree.put("StringValue", at.attributeValue);
                messageAttributesTree.push_back(std::make_pair("", attributeTree));
            }
            messageTree.push_back(std::make_pair("MessageAttributes", messageAttributesTree));
        }
        rootTree.add_child("ReceiveMessageResponse.ReceiveMessageResult.Messages", messagesTree);

        // Metadata
        rootTree.put("ResponseMetadata.RequestId", requestId);

        return Core::XmlUtils::ToXmlString(rootTree);
    }

    std::string ReceiveMessageResponse::ToString() {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, ReceiveMessageResponse &r) {
        os << "ReceiveMessageResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::SQS
