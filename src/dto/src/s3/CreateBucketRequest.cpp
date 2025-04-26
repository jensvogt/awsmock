//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/s3/CreateBucketRequest.h>

namespace AwsMock::Dto::S3 {

    std::string CreateBucketRequest::ToJson() const {

        try {

            log_info << "region: " << region << ", owner: " << owner << ", name: " << name;
            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "name", name);
            Core::Bson::BsonUtils::SetStringValue(document, "owner", owner);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void CreateBucketRequest::FromXml(const std::string &xmlString) {

        try {

            boost::property_tree::ptree pt;
            read_xml(xmlString, pt);
            region = pt.get<std::string>("CreateBucketConfiguration.LocationConstraint");

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string CreateBucketRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const CreateBucketRequest &r) {
        os << "CreateBucketRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::S3
