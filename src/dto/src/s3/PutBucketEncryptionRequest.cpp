//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/dto/s3/PutBucketEncryptionRequest.h>

namespace AwsMock::Dto::S3 {

    void PutBucketEncryptionRequest::FromXml(const std::string &xmlString) {

        try {

            boost::property_tree::ptree pt;
            Core::XmlUtils::ReadXml(xmlString, &pt);

            sseAlgorithm = pt.get<std::string>("ServerSideEncryptionConfiguration.Rule.ApplyServerSideEncryptionByDefault.SSEAlgorithm");
            kmsKeyId = pt.get<std::string>("ServerSideEncryptionConfiguration.Rule.ApplyServerSideEncryptionByDefault.KMSMasterKeyID");

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
