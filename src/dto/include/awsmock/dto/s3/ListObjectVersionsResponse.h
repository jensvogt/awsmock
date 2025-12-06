//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_S3_LIST_OBJECT_VERSIONS_RESPONSE_H
#define AWSMOCK_DTO_S3_LIST_OBJECT_VERSIONS_RESPONSE_H

// C++ Standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/s3/model/ObjectVersion.h>
#include <awsmock/dto/s3/model/Owner.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief List S3 object version response
     *
     * Example:
     * @code{.xml}
     * <ListVersionsResult>
     *   <IsTruncated>boolean</IsTruncated>
     *   <KeyMarker>string</KeyMarker>
     *   <VersionIdMarker>string</VersionIdMarker>
     *   <NextKeyMarker>string</NextKeyMarker>
     *   <NextVersionIdMarker>string</NextVersionIdMarker>
     *   <Version>
     *     <ChecksumAlgorithm>string</ChecksumAlgorithm>
     *     ...
     *     <ETag>string</ETag>
     *     <IsLatest>boolean</IsLatest>
     *     <Key>string</Key>
     *     <LastModified>timestamp</LastModified>
     *     <Owner>
     *       <DisplayName>string</DisplayName>
     *       <ID>string</ID>
     *      </Owner>
     *     <RestoreStatus>
     *       <IsRestoreInProgress>boolean</IsRestoreInProgress>
     *       <RestoreExpiryDate>timestamp</RestoreExpiryDate>
     *     </RestoreStatus>
     *     <Size>long</Size>
     *     <StorageClass>string</StorageClass>
     *     <VersionId>string</VersionId>
     *   </Version>
     *   ...
     *   <DeleteMarker>
     *     <IsLatest>boolean</IsLatest>
     *     <Key>string</Key>
     *     <LastModified>timestamp</LastModified>
     *     <Owner>
     *       <DisplayName>string</DisplayName>
     *       <ID>string</ID>
     *     </Owner>
     *     <VersionId>string</VersionId>
     *   </DeleteMarker>
     *   ...
     *   <Name>string</Name>
     *   <Prefix>string</Prefix>
     *   <Delimiter>string</Delimiter>
     *   <MaxKeys>integer</MaxKeys>
     *   <CommonPrefixes>
     *     <Prefix>string</Prefix>
     *   </CommonPrefixes>
     *   ...
     *   <EncodingType>string</EncodingType>
     * </ListVersionsResult>
     * @endcode
     */
    struct DeleteMarker final : Common::BaseCounter<DeleteMarker> {

        /**
         * Is latest
         */
        bool isLatest;

        /**
         * Key
         */
        std::string key;

        /**
         * Owner
         */
        Owner owner;

        /**
         * Version ID
         */
        std::string versionId;

        /**
         * Last modified
         */
        system_clock::time_point lastModified;

      private:

        friend DeleteMarker tag_invoke(boost::json::value_to_tag<DeleteMarker>, boost::json::value const &v) {
            DeleteMarker r;
            r.key = Core::Json::GetStringValue(v, "Key");
            r.owner = boost::json::value_to<Owner>(v.at("Owner"));
            r.isLatest = Core::Json::GetBoolValue(v, "IsLatest");
            r.versionId = Core::Json::GetStringValue(v, "VersionId");
            r.lastModified = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "LastModified"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteMarker const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"Key", obj.key},
                    {"Owner", boost::json::value_from(obj.owner)},
                    {"IsLatest", obj.isLatest},
                    {"VersionId", obj.versionId},
                    {"LastModified", Core::DateTimeUtils::ToISO8601(obj.lastModified)},
            };
        }
    };

    struct ListObjectVersionsResponse final : Common::BaseCounter<ListObjectVersionsResponse> {

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
         * Encoding type
         */
        std::string encodingType;

        /**
         * Maximal keys
         */
        long maxKeys{};

        /**
         * Truncation flag
         */
        bool isTruncated = false;

        /**
         * Key marker
         */
        std::string keyMarker;

        /**
         * Version ID marker
         */
        std::string versionIdMarker;

        /**
         * Next key marker
         */
        std::string nextKeyMarker;

        /**
         * Next version ID marker
         */
        std::string nextVersionIdMarker;

        /**
         * Common prefixes
         */
        std::vector<std::string> commonPrefixes;

        /**
         * Versions
         */
        std::vector<ObjectVersion> versions;

        /**
         * Delete markers
         */
        std::vector<DeleteMarker> deleteMarkers;

        /**
         * Convert to XML representation
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const {

            try {

                boost::property_tree::ptree root;
                boost::property_tree::ptree listVersionResult;

                listVersionResult.add("Region", region);
                listVersionResult.add("Name", name);
                listVersionResult.add("Prefix", prefix);
                listVersionResult.add("Delimiter", delimiter);
                listVersionResult.add("EncodingType", encodingType);
                listVersionResult.add("MaxKeys", maxKeys);
                listVersionResult.add("IsTruncated", isTruncated);
                listVersionResult.add("KeyMarker", keyMarker);
                listVersionResult.add("VersionIdMarker", versionIdMarker);
                listVersionResult.add("NextKeyMarker", nextKeyMarker);
                listVersionResult.add("NextVersionIdMarker", nextVersionIdMarker);

                // Prefixes
                if (!commonPrefixes.empty()) {
                    boost::property_tree::ptree prefixArray;
                    for (const auto &p: commonPrefixes) {
                        prefixArray.push_back(boost::property_tree::basic_ptree<std::string, std::string>::value_type(std::make_pair("", p)));
                    }
                    listVersionResult.add_child("CommonPrefixes", prefixArray);
                }

                // Versions
                if (!versions.empty()) {
                    for (const auto &v: versions) {
                        boost::property_tree::ptree jsonVersionObject;
                        jsonVersionObject.add("Key", v.key);
                        jsonVersionObject.add("ETag", v.eTag);
                        jsonVersionObject.add("VersionId", v.versionId);
                        jsonVersionObject.add("StorageClass", v.storageClass);
                        jsonVersionObject.add("IsLatest", v.isLatest);
                        jsonVersionObject.add("Size", v.size);
                        jsonVersionObject.add("LastModified", Core::DateTimeUtils::ToISO8601(v.lastModified));
                        listVersionResult.add_child("Version", jsonVersionObject);
                    }
                }

                // Delete markers
                if (!deleteMarkers.empty()) {
                    for (const auto &m: deleteMarkers) {
                        boost::property_tree::ptree jsonDeleteObject;
                        jsonDeleteObject.add("Key", m.key);
                        jsonDeleteObject.add("IsLatest", m.isLatest);
                        jsonDeleteObject.add("VersionId", m.versionId);
                        jsonDeleteObject.add("LastModified", Core::DateTimeUtils::ToISO8601(m.lastModified));
                        listVersionResult.add_child("DeleteMarker", jsonDeleteObject);
                    }
                }
                root.add_child("ListVersionResult", listVersionResult);

                std::string tmp = Core::XmlUtils::ToXmlString(root);
                return Core::XmlUtils::ToXmlString(root);

            } catch (std::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend ListObjectVersionsResponse tag_invoke(boost::json::value_to_tag<ListObjectVersionsResponse>, boost::json::value const &v) {
            ListObjectVersionsResponse r;
            r.name = Core::Json::GetStringValue(v, "name");
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.delimiter = Core::Json::GetStringValue(v, "delimiter");
            r.encodingType = Core::Json::GetStringValue(v, "encodingType");
            r.maxKeys = Core::Json::GetLongValue(v, "maxKeys");
            r.isTruncated = Core::Json::GetBoolValue(v, "isTruncated");
            r.keyMarker = Core::Json::GetStringValue(v, "keyMarker");
            r.versionIdMarker = Core::Json::GetStringValue(v, "versionIdMarker");
            r.nextKeyMarker = Core::Json::GetStringValue(v, "nextKeyMarker");
            r.nextVersionIdMarker = Core::Json::GetStringValue(v, "nextVersionIdMarker");
            if (Core::Json::AttributeExists(v, "commonPrefixes")) {
                r.commonPrefixes = boost::json::value_to<std::vector<std::string>>(v.at("commonPrefixes"));
            }
            if (Core::Json::AttributeExists(v, "versions")) {
                r.versions = boost::json::value_to<std::vector<ObjectVersion>>(v.at("versions"));
            }
            if (Core::Json::AttributeExists(v, "deleteMarkers")) {
                r.deleteMarkers = boost::json::value_to<std::vector<DeleteMarker>>(v.at("deleteMarkers"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListObjectVersionsResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"name", obj.name},
                    {"prefix", obj.prefix},
                    {"delimiter", obj.delimiter},
                    {"encodingType", obj.encodingType},
                    {"maxKeys", obj.maxKeys},
                    {"isTruncated", obj.isTruncated},
                    {"keyMarker", obj.keyMarker},
                    {"versionIdMarker", obj.versionIdMarker},
                    {"nextKeyMarker", obj.nextKeyMarker},
                    {"nextVersionIdMarker", obj.nextVersionIdMarker},
                    {"commonPrefixes", boost::json::value_from(obj.commonPrefixes)},
                    {"versions", boost::json::value_from(obj.versions)},
                    {"deleteMarkers", boost::json::value_from(obj.deleteMarkers)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_LIST_OBJECT_VERSIONS_RESPONSE_H
