//
// Created by vogje01 on 13/10/2023.
//

#include <awsmock/dto/s3/PutBucketVersioningRequest.h>

namespace AwsMock::Dto::S3 {

    void PutBucketVersioningRequest::FromXml(const std::string &xmlString) {

        // TODO:
        /*
        try {
            Poco::XML::DOMParser parser;
            Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parseString(xmlString);

            Poco::XML::Node *node = pDoc->getNodeByPath("/VersioningConfiguration/Status");
            if (node) {
                status = node->innerText();
            } else {
                std::cerr << "Exception: Wrong versioning state" << std::endl;
            }

        } catch (Poco::Exception &exc) {
            std::cerr << "Exception: " << exc.message() << std::endl;
        }*/
    }

    std::string PutBucketVersioningRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const PutBucketVersioningRequest &r) {
        os << "PutBucketVersioningRequest={region='" << r.region << "', user='" << r.user << "', bucket='" << r.bucket << "', state='" << r.status << "'}";
        return os;
    }

}// namespace AwsMock::Dto::S3
