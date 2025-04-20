//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_DTO_S3_EVENT_NOTIFICATION_H
#define AWSMOCK_DTO_S3_EVENT_NOTIFICATION_H

// C++ standard includes
#include <chrono>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/s3/model/UserIdentity.h>

namespace AwsMock::Dto::S3 {

    using std::chrono::system_clock;

    /**
     * {
     *   "Records":[
     *       {
     *        "eventVersion":"2.2",
     *        "eventSource":"aws:s3",
     *        "awsRegion":"us-west-2",
     *        "eventTime":"The time, in ISO-8601 format, for example, 1970-01-01T00:00:00.000Z, when Amazon S3 finished processing the request",
     *        "eventName":"event-type",
     *        "userIdentity":{
     *           "principalId":"Amazon-customer-ID-of-the-user-who-caused-the-event"
     *        },
     *        "requestParameters":{
     *           "sourceIPAddress":"ip-address-where-request-came-from"
     *        },
     *        "responseElements":{
     *           "x-amz-request-userPoolId":"Amazon S3 generated request ID",
     *           "x-amz-userPoolId-2":"Amazon S3 host that processed the request"
     *        },
     *        "s3":{
     *           "s3SchemaVersion":"1.0",
     *           "configurationId":"ID found in the bucket notification configuration",
     *           "bucket":{
     *              "name":"bucket-name",
     *              "ownerIdentity":{
     *                 "principalId":"Amazon-customer-ID-of-the-bucket-owner"
     *              },
     *              "arn":"bucket-ARN"
     *           },
     *           "object":{
     *               "key":"object-key",
     *              "size":"object-size in bytes",
     *              "eTag":"object eTag",
     *              "versionId":"object version if bucket is versioning-enabled, otherwise null",
     *              "sequencer": "a string representation of a hexadecimal value used to determine event sequence, only used with PUTs and DELETEs"
     *           }
     *        },
     *        "glacierEventData": {
     *           "restoreEventData": {
     *              "lifecycleRestorationExpiryTime": "The time, in ISO-8601 format, for example, 1970-01-01T00:00:00.000Z, of Restore Expiry",
     *              "lifecycleRestoreStorageClass": "Source storage class for restore"
     *           }
     *        }
     *     }
     *   ]
     * }
     */
    struct RequestParameter {

        /**
         * Request parameters
         */
        std::string requestParameters;

        /**
         * Source IP address
         */
        std::string sourceIPAddress;

        /**
         * @brief Convert to JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const {

            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "requestParameters", requestParameters);
                Core::Bson::BsonUtils::SetStringValue(document, "sourceIPAddress", sourceIPAddress);
                return Core::Bson::BsonUtils::ToJsonString(document);

            } catch (bsoncxx::exception &e) {
                log_error << e.what();
                throw Core::JsonException(e.what());
            }
        }

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const {
            std::stringstream ss;
            ss << *this;
            return ss.str();
        }

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const RequestParameter &r) {
            os << "RequestParameter=" << r.ToString();
            return os;
        }
    };

    struct ResponseElements {

        /**
         * Request ID
         */
        std::string xAmzRequestId;

        /**
         * ID2
         */
        std::string xAmzId2;

        /**
         * @brief Convert to JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const {

            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "xAmzRequestId", xAmzRequestId);
                Core::Bson::BsonUtils::SetStringValue(document, "xAmzId2", xAmzId2);
                return Core::Bson::BsonUtils::ToJsonString(document);

            } catch (bsoncxx::exception &e) {
                log_error << e.what();
                throw Core::JsonException(e.what());
            }
        }

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const {
            std::stringstream ss;
            ss << *this;
            return ss.str();
        }

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const ResponseElements &r) {
            os << "ResponseElements=" << r.ToJson();
            return os;
        }
    };

    struct OwnerIdentity {

        /**
         * Principal ID
         */
        std::string principalId;

        /**
         * @brief Convert to JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const {
            return Core::Bson::BsonUtils::ToJsonString(ToDocument());
        }

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "principalId", principalId);
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Converts a JSON representation to s DTO.
         *
         * @param document JSON object.
         */
        void FromDocument(const view_or_value<view, value> &document) {

            try {

                principalId = Core::Bson::BsonUtils::GetStringValue(document, "principalId");

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const {
            std::stringstream ss;
            ss << *this;
            return ss.str();
        }

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const OwnerIdentity &o) {
            os << "OwnerIdentity=" << o.ToJson();
            return os;
        }
    };

    struct Object {

        /**
         * S3 object key
         */
        std::string key;

        /**
         * Object file size
         */
        long size = 0;

        /**
         * Etag
         */
        std::string etag;

        /**
         * Object version
         */
        std::string versionId;

        /**
         * S3 sequencer
         */
        std::string sequencer;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {
                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "key", key);
                Core::Bson::BsonUtils::SetLongValue(document, "size", size);
                Core::Bson::BsonUtils::SetStringValue(document, "etag", etag);
                Core::Bson::BsonUtils::SetStringValue(document, "versionId", versionId);
                Core::Bson::BsonUtils::SetStringValue(document, "sequencer", sequencer);
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Converts a JSON representation to a DTO.
         *
         * @param document JSON object
         */
        void FromDocument(const view_or_value<view, value> &document) {

            try {

                key = Core::Bson::BsonUtils::GetStringValue(document, "key");
                size = Core::Bson::BsonUtils::GetLongValue(document, "size");
                etag = Core::Bson::BsonUtils::GetStringValue(document, "etag");
                versionId = Core::Bson::BsonUtils::GetStringValue(document, "versionId");
                sequencer = Core::Bson::BsonUtils::GetStringValue(document, "sequencer");
                key = Core::Bson::BsonUtils::GetStringValue(document, "key");

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as JSON string
         */
        [[nodiscard]] std::string ToJson() const {
            return Core::Bson::BsonUtils::ToJsonString(ToDocument());
        }

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const {
            std::stringstream ss;
            ss << *this;
            return ss.str();
        }

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const Object &o) {
            os << "Object=" << o.ToJson();
            return os;
        }
    };

    struct NotificationBucket {

        /**
         * Bucket name
         */
        std::string name;

        /**
         * Bucket ARN
         */
        std::string arn;

        /**
         * Owner identity
         */
        OwnerIdentity ownerIdentity;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {
                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "name", name);
                Core::Bson::BsonUtils::SetStringValue(document, "arn", arn);
                Core::Bson::BsonUtils::SetDocumentValue(document, "ownerIdentity", ownerIdentity.ToDocument());
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }


        /**
         * @brief Converts a JSON representation to a DTO.
         *
         * @param document JSON object
         */
        void FromDocument(const view_or_value<view, value> &document) {

            try {

                name = Core::Bson::BsonUtils::GetStringValue(document, "name");
                arn = Core::Bson::BsonUtils::GetStringValue(document, "arn");
                if (document.view().find("ownerIdentity") != document.view().end()) {
                    ownerIdentity.FromDocument(document.view()["ownerIdentity"].get_document().value);
                }

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }
    };

    struct S3 {

        /**
         * Schema version
         */
        std::string s3SchemaVersion = "1.0";

        /**
         * S3 event configuration ID
         */
        std::string configurationId;

        /**
         * S3 bucket
         */
        NotificationBucket bucket;

        /**
         * S3 object
         */
        Object object;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToJson() const {
            return Core::Bson::BsonUtils::ToJsonString(ToDocument());
        }

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {

                // Fix dates
                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "s3SchemaVersion", s3SchemaVersion);
                Core::Bson::BsonUtils::SetStringValue(document, "configurationId", configurationId);
                document.append(kvp("bucket", bucket.ToDocument()));
                document.append(kvp("object", object.ToDocument()));
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Converts a JSON representation to s DTO.
         *
         * @param document JSON object.
         */
        void FromDocument(const view_or_value<view, value> &document) {

            try {

                s3SchemaVersion = Core::Bson::BsonUtils::GetStringValue(document, "s3SchemaVersion");
                configurationId = Core::Bson::BsonUtils::GetStringValue(document, "configurationId");

                if (document.view().find("bucket") != document.view().end()) {
                    bucket.FromDocument(document.view()["bucket"].get_document().value);
                }
                if (document.view().find("object") != document.view().end()) {
                    object.FromDocument(document.view()["object"].get_document().value);
                }

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const {
            std::stringstream ss;
            ss << *this;
            return ss.str();
        }

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const S3 &s) {
            os << "S3=" << s.ToJson();
            return os;
        }
    };

    struct Record {

        /**
         * Record version
         */
        std::string eventVersion = "2.2";

        /**
         * Event source
         */
        std::string eventSource = "aws:s3";

        /**
         * Region
         */
        std::string region;

        /**
         * Event time
         */
        system_clock::time_point eventTime = system_clock::now();

        /**
         * Event name
         */
        std::string eventName;

        /**
         * Event user
         */
        UserIdentity userIdentity;

        /**
         * Event request parameter
         */
        RequestParameter requestParameter;

        /**
         * Response element
         */
        ResponseElements responseElements;

        /**
         * S3 event parameter
         */
        S3 s3;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {
                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "eventVersion", eventVersion);
                Core::Bson::BsonUtils::SetStringValue(document, "eventSource", eventSource);
                Core::Bson::BsonUtils::SetStringValue(document, "awsRegion", region);
                Core::Bson::BsonUtils::SetStringValue(document, "eventTime", Core::DateTimeUtils::ToISO8601(eventTime));
                Core::Bson::BsonUtils::SetStringValue(document, "eventName", eventName);

                document.append(kvp("userIdentity", userIdentity.ToDocument()));
                document.append(kvp("s3", s3.ToDocument()));

                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Converts a JSON representation to s DTO.
         *
         * @param document JSON object.
         */
        void FromDocument(const view_or_value<view, value> &document) {

            try {
                eventVersion = Core::Bson::BsonUtils::GetStringValue(document, "eventVersion");
                eventSource = Core::Bson::BsonUtils::GetStringValue(document, "eventSource");
                region = Core::Bson::BsonUtils::GetStringValue(document, "region");
                eventTime = Core::Bson::BsonUtils::GetDateValue(document, "eventTime");
                eventName = Core::Bson::BsonUtils::GetStringValue(document, "eventName");

                // S3
                if (document.view().find("s3") != document.view().end()) {
                    s3.FromDocument(document.view()["s3"].get_document().value);
                }

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Converts the DTO to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const {
            return Core::Bson::BsonUtils::ToJsonString(ToDocument());
        }

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const {
            std::stringstream ss;
            ss << *this;
            return ss.str();
        }

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const Record &r) {
            os << "Record=" << r.ToJson();
            return os;
        }
    };

    /**
     * @brief AWS S3 event notification DTO.
     *
     * @par
     * Issued in case the S3 bucket has an event notification attached.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct EventNotification {

        /**
         * S3 event record
         */
        std::vector<Record> records;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToJson() const {

            try {

                document document;
                if (!records.empty()) {
                    array recordsJsonArray;
                    for (const auto &record: records) {
                        recordsJsonArray.append(record.ToDocument());
                    }
                    document.append(kvp("Records", recordsJsonArray));
                }

                return Core::Bson::BsonUtils::ToJsonString(document);

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Converts a JSON representation to s DTO.
         *
         * @param jsonString JSON string.
         */
        void FromJson(const std::string &jsonString) {

            try {

                // Record parsing
                if (const value document = bsoncxx::from_json(jsonString); document.view().find("Records") != document.view().end()) {
                    for (const bsoncxx::array::view recordArray = document.view()["Records"].get_array().value; const auto &it: recordArray) {
                        Record record;
                        record.FromDocument(it.get_document().value);
                        records.push_back(record);
                    }
                }

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const {
            std::stringstream ss;
            ss << *this;
            return ss.str();
        }

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const EventNotification &e) {
            os << "EventNotification=" << e.ToJson();
            return os;
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_EVENT_NOTIFICATION_H
