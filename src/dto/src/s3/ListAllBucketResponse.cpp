//
// Created by vogje01 on 30/05/2023.
//

// AwsMock includes
#include <awsmock/dto/s3/ListAllBucketResponse.h>

namespace AwsMock::Dto::S3 {

    std::string ListAllBucketResponse::ToXml() const {

        boost::property_tree::ptree root;
        root.add("ListAllMyBucketsResult.Total", total);

        boost::property_tree::ptree bucketsTree;
        for (auto &it: bucketList) {
            boost::property_tree::ptree bucketTree;
            bucketTree.add("BucketRegion", it.region);
            bucketTree.add("Name", it.name);
            bucketTree.add("CreationDate", Core::DateTimeUtils::ToISO8601(it.created));
            bucketsTree.push_back(std::make_pair("Bucket", bucketTree));
        }
        root.add_child("ListAllMyBucketsResult.Buckets", bucketsTree);
        return Core::XmlUtils::ToXmlString(root);
    }

    std::string ListAllBucketResponse::ToJson() const {

        document rootDocument;

        // Get metadata
        if (bucketList.empty()) {
            array jsonBucketArray;
            for (const auto &bucket: bucketList) {
                jsonBucketArray.append(bucket.ToDocument());
            }
            rootDocument.append(kvp("buckets", jsonBucketArray));
        }
        Core::Bson::BsonUtils::SetLongValue(rootDocument, "total", total);
        return Core::Bson::BsonUtils::ToJsonString(rootDocument);
    }

    std::string ListAllBucketResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListAllBucketResponse &r) {
        os << "ListAllBucketResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::S3
