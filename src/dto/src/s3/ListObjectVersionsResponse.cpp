//
// Created by vogje01 on 30/05/2023.
//

// AwsMock includes
#include <awsmock/dto/s3/ListObjectVersionsResponse.h>

namespace AwsMock::Dto::S3 {

    Poco::JSON::Object DeleteMarker::ToJsonObject() const {

        try {
            Poco::JSON::Object rootJson;
            rootJson.set("Key", key);
            rootJson.set("IsLatest", isLatest);
            rootJson.set("LastModified", Poco::DateTimeFormatter::format(lastModified, Poco::DateTimeFormat::ISO8601_FORMAT));
            rootJson.set("Owner", owner.ToJsonObject());
            rootJson.set("VersionId", versionId);
            return rootJson;

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    Poco::XML::AutoPtr<Poco::XML::Element> DeleteMarker::ToXmlElement(Poco::XML::AutoPtr<Poco::XML::Document> pDoc) const {

        Poco::XML::AutoPtr<Poco::XML::Element> pRoot = pDoc->createElement("DeleteMarker");
        Core::XmlUtils::CreateTextNode(pDoc, pRoot, "Key", key);
        Core::XmlUtils::CreateTextNode(pDoc, pRoot, "IsLatest", isLatest);
        Core::XmlUtils::CreateTextNode(pDoc, pRoot, "LastModified", lastModified);
        Core::XmlUtils::CreateTextNode(pDoc, pRoot, "VersionId", versionId);
        pRoot->appendChild(owner.ToXmlElement(pDoc));
        return pRoot;
    }


    std::string ListObjectVersionsResponse::ToJson() const {

        try {
            Poco::JSON::Object rootJson;
            rootJson.set("Region", region);
            rootJson.set("Name", name);
            rootJson.set("Prefix", prefix);
            rootJson.set("Delimiter", delimiter);
            rootJson.set("EncodingType", encodingType);
            rootJson.set("MaxKeys", maxKeys);
            rootJson.set("IsTruncated", isTruncated);
            rootJson.set("KeyMarker", keyMarker);
            rootJson.set("VersionIdMarker", versionIdMarker);
            rootJson.set("NextKeyMarker", keyMarker);
            rootJson.set("NextVersionIdMarker", versionIdMarker);

            // Prefixes
            Poco::JSON::Array jsonPrefixArray;
            for (const auto &p: commonPrefixes) {
                Poco::JSON::Object prefixObject;
                prefixObject.set("prefix", p);
                jsonPrefixArray.add(prefixObject);
            }
            rootJson.set("CommonPrefixes", jsonPrefixArray);

            // Versions
            Poco::JSON::Array jsonVersionArray;
            for (const auto &v: versions) {
                jsonVersionArray.add(v.ToJsonObject());
            }
            rootJson.set("Versions", jsonVersionArray);

            // Delete markers
            Poco::JSON::Array jsonDeleteMarkerArray;
            for (const auto &m: deleteMarkers) {
                jsonDeleteMarkerArray.add(m.ToJsonObject());
            }
            rootJson.set("DeleteMarkers", jsonDeleteMarkerArray);

            std::ostringstream os;
            rootJson.stringify(os);
            return os.str();

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

    std::string ListObjectVersionsResponse::ToXml() const {

        Poco::XML::AutoPtr<Poco::XML::Document> pDoc = new Poco::XML::Document;
        Poco::XML::AutoPtr<Poco::XML::Element> pRoot = pDoc->createElement("ListVersionsResult");

        Core::XmlUtils::CreateTextNode(pDoc, pRoot, "Region", region);
        Core::XmlUtils::CreateTextNode(pDoc, pRoot, "Name", name);
        Core::XmlUtils::CreateTextNode(pDoc, pRoot, "Prefix", prefix);
        Core::XmlUtils::CreateTextNode(pDoc, pRoot, "Delimiter", delimiter);
        Core::XmlUtils::CreateTextNode(pDoc, pRoot, "EncodingType", encodingType);
        Core::XmlUtils::CreateTextNode(pDoc, pRoot, "MaxKeys", maxKeys);
        Core::XmlUtils::CreateTextNode(pDoc, pRoot, "IsTruncated", isTruncated);
        Core::XmlUtils::CreateTextNode(pDoc, pRoot, "KeyMarker", keyMarker);
        Core::XmlUtils::CreateTextNode(pDoc, pRoot, "VersionIdMarker", versionIdMarker);
        Core::XmlUtils::CreateTextNode(pDoc, pRoot, "NextKeyMarker", nextKeyMarker);
        Core::XmlUtils::CreateTextNode(pDoc, pRoot, "NextVersionIdMarker", nextVersionIdMarker);
        pDoc->appendChild(pRoot);

        // Prefixes
        Core::XmlUtils::CreateTextArray(pDoc, pRoot, "CommonPrefixes", "Prefix", commonPrefixes);

        // Versions
        for (const auto &it: versions) {
            Poco::XML::AutoPtr<Poco::XML::Element> pVersion = it.ToXmlElement(pDoc);
            pRoot->appendChild(pVersion);
        }

        // Delete marker
        for (const auto &it: deleteMarkers) {
            Poco::XML::AutoPtr<Poco::XML::Element> pDeleteMarker = it.ToXmlElement(pDoc);
            pRoot->appendChild(pDeleteMarker);
        }

        return Core::XmlUtils::ToXmlString(pDoc);
    }

    std::string ListObjectVersionsResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListObjectVersionsResponse &r) {
        os << "ListObjectVersionsResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::S3
