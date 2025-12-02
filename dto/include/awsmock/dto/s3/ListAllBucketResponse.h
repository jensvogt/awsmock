//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_LIST_ALL_BUCKET_RESPONSE_H
#define AWSMOCK_DTO_S3_LIST_ALL_BUCKET_RESPONSE_H

// C++ standard includes
#include <string>
#include <type_traits>
#include <vector>

// Boost includes
#include <boost/describe.hpp>
#include <boost/json.hpp>
#include <boost/mp11.hpp>
#include <boost/version.hpp>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/dto/common/BaseDto.h>
#include <awsmock/dto/s3/model/Bucket.h>
#include <awsmock/entity/s3/Bucket.h>
#include <boost/serialization/base_object.hpp>

namespace AwsMock::Dto::S3 {

    struct ListAllBucketResponse final : Common::BaseCounter<ListAllBucketResponse> {

        /**
         * List of buckets
         */
        std::vector<Bucket> bucketList;

        /**
         * Total number of buckets
         */
        long total = 0;

        /**
         * @brief Convert to XML representation
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const {

            boost::property_tree::ptree root;
            root.add("ListAllMyBucketsResult.Total", total);

            boost::property_tree::ptree bucketsTree;
            for (auto &it: bucketList) {
                boost::property_tree::ptree bucketTree;
                bucketTree.add("BucketRegion", it.region);
                bucketTree.add("Name", it.bucketName);
                bucketTree.add("CreationDate", Core::DateTimeUtils::ToISO8601(it.created));
                bucketsTree.push_back(std::make_pair("Bucket", bucketTree));
            }
            root.add_child("ListAllMyBucketsResult.Buckets", bucketsTree);
            return Core::XmlUtils::ToXmlString(root);
        }

      private:

        friend ListAllBucketResponse tag_invoke(boost::json::value_to_tag<ListAllBucketResponse>, boost::json::value const &v) {
            ListAllBucketResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            r.bucketList = boost::json::value_to<std::vector<Bucket>>(v.at("buckets"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListAllBucketResponse const &obj) {
            jv = {
                    {"total", obj.total},
                    {"bucketList", boost::json::value_from(obj.bucketList)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_LIST_ALL_BUCKET_RESPONSE_H
