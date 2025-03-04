//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/dto/s3/PutBucketEncryptionRequest.h>

namespace AwsMock::Dto::S3 {

    void PutBucketEncryptionRequest::FromXml(const std::string &xmlString) {

        try {

            boost::property_tree::ptree pt;
            read_xml(xmlString, pt);

            for (const auto &configuratioun: pt.get_child("ServerSideEncryptionConfiguration")) {
            }
            /*            Poco::XML::DOMParser parser;
            Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parseString(xmlString);

            Poco::XML::Node *rootNode = pDoc->getNodeByPath("/ServerSideEncryptionConfiguration");
            if (rootNode) {

                Poco::XML::Node *ruleNode = rootNode->firstChild();
                Poco::XML::Node *sseEncryptionConfiguration = ruleNode->firstChild();

                if (sseEncryptionConfiguration) {
                    for (int i = 0; i < sseEncryptionconfiguration.childNodes()->length(); i++) {
                        Poco::XML::Node *childNode = sseEncryptionconfiguration.childNodes()->item(i);
                        if (childNode->nodeName() == "SSEAlgorithm") {
                            sseAlgorithm = childNode->lastChild()->innerText();
                        } else if (childNode->nodeName() == "KMSKeyID" || childNode->nodeName() == "KMSMasterKeyID") {
                            kmsKeyId = childNode->lastChild()->innerText();
                        }
                    }
                }
            }*/
        } catch (std::exception &e) {
            log_error << e.what();
            throw Core::JsonException(e.what());
        }
    }

    std::string PutBucketEncryptionRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "bucket", bucket);
            Core::Bson::BsonUtils::SetStringValue(document, "sseAlgorithm", sseAlgorithm);
            Core::Bson::BsonUtils::SetStringValue(document, "kmsKeyId", kmsKeyId);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (std::exception &e) {
            log_error << e.what();
            throw Core::JsonException(e.what());
        }
    }

    std::string PutBucketEncryptionRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const PutBucketEncryptionRequest &r) {
        os << "PutBucketEncryptionRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::S3
