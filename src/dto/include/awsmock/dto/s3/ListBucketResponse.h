//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_S3_LIST_BUCKET_RESPONSE_H
#define AWSMOCK_DTO_S3_LIST_BUCKET_RESPONSE_H

// C++ Standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/s3/model/Content.h>
#include <awsmock/dto/s3/model/Owner.h>
#include <awsmock/entity/s3/Object.h>

namespace AwsMock::Dto::S3 {

    struct ListBucketResponse final : Common::BaseCounter<ListBucketResponse> {

        /**
         * Name
         */
        std::string name;

        /**
         * Prefix
         */
        std::string prefix;

        /**
         * Delimiter
         */
        std::string delimiter;

        /**
         * Truncation flag
         */
        bool isTruncated = false;

        /**
         * Maximal keys
         */
        long maxKeys = 1000;

        /**
         * Encoding type
         */
        std::string encodingType;

        /**
         * Key count
         */
        long keyCount = 0;

        /**
         * Continuation token
         */
        std::string continuationToken;

        /**
         * Next continuation token
         */
        std::string nextContinuationToken;

        /**
         * Start after
         */
        std::string startAfter;

        /**
         * Total number of buckets
         */
        long total = 0;

        /**
         * Contents
         */
        std::vector<Content> contents;

        /**
         * @brief Convert to XML representation
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const {

            boost::property_tree::ptree root;
            root.add("ListBucketResult.IsTruncated", isTruncated);
            root.add("ListBucketResult.Name", name);
            root.add("ListBucketResult.Prefix", prefix);
            root.add("ListBucketResult.Delimiter", delimiter);
            root.add("ListBucketResult.MaxKeys", maxKeys);
            root.add("ListBucketResult.EncodingType", encodingType);
            root.add("ListBucketResult.KeyCount", keyCount);
            root.add("ListBucketResult.ContinuationToken", continuationToken);
            root.add("ListBucketResult.NextContinuationToken", nextContinuationToken);
            root.add("ListBucketResult.StartAfter", startAfter);

            // Contents
            if (!contents.empty()) {
                for (auto &it: contents) {
                    boost::property_tree::ptree xmlContent;
                    xmlContent.add("Key", it.key);
                    xmlContent.add("LastModified", Core::DateTimeUtils::ToISO8601(it.modified));
                    xmlContent.add("ETag", it.etag);
                    xmlContent.add("Size", it.size);
                    xmlContent.add("StorageClass", it.storageClass);
                    xmlContent.add("Owner.DisplayName", it.owner.displayName);
                    xmlContent.add("Owner.ID", it.owner.id);
                    root.add_child("ListBucketResult.Contents", xmlContent);
                }
            }
            return Core::XmlUtils::ToXmlString(root);
        }

      private:

        friend ListBucketResponse tag_invoke(boost::json::value_to_tag<ListBucketResponse>, boost::json::value const &v) {
            ListBucketResponse r;
            r.name = Core::Json::GetStringValue(v, "name");
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.delimiter = Core::Json::GetStringValue(v, "delimiter");
            r.isTruncated = Core::Json::GetBoolValue(v, "isTruncated");
            r.maxKeys = Core::Json::GetLongValue(v, "maxKeys");
            r.encodingType = Core::Json::GetStringValue(v, "encodingType");
            r.keyCount = Core::Json::GetLongValue(v, "keyCount");
            r.continuationToken = Core::Json::GetStringValue(v, "continuationToken");
            r.nextContinuationToken = Core::Json::GetStringValue(v, "nextContinuationToken");
            r.startAfter = Core::Json::GetStringValue(v, "startAfter");
            r.total = Core::Json::GetLongValue(v, "total");
            if (Core::Json::AttributeExists(v, "contents")) {
                r.contents = boost::json::value_to<std::vector<Content>>(v.at("contents"));
            }
            return r;
        }
        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListBucketResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"name", obj.name},
                    {"prefix", obj.prefix},
                    {"delimiter", obj.delimiter},
                    {"isTruncated", obj.isTruncated},
                    {"maxKeys", obj.maxKeys},
                    {"encodingType", obj.encodingType},
                    {"keyCount", obj.keyCount},
                    {"continuationToken", obj.continuationToken},
                    {"nextContinuationToken", obj.nextContinuationToken},
                    {"startAfter", obj.startAfter},
                    {"total", obj.total},
                    {"contents", boost::json::value_from(obj.contents)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_LIST_BUCKET_RESPONSE_H
