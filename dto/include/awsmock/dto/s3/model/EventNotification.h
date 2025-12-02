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
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/s3/model/Owner.h>
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
    struct RequestParameter final : Common::BaseCounter<RequestParameter> {

        /**
         * Request parameters
         */
        std::string requestParameters;

        /**
         * Source IP address
         */
        std::string sourceIPAddress;

      private:

        friend RequestParameter tag_invoke(boost::json::value_to_tag<RequestParameter>, boost::json::value const &v) {
            RequestParameter r;
            r.requestParameters = Core::Json::GetStringValue(v, "requestParameters");
            r.sourceIPAddress = Core::Json::GetStringValue(v, "sourceIPAddress");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, RequestParameter const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"requestParameters", obj.requestParameters},
                    {"sourceIPAddress", obj.sourceIPAddress},
            };
        }
    };

    struct ResponseElements final : Common::BaseCounter<ResponseElements> {

        /**
         * Request ID
         */
        std::string xAmzRequestId;

        /**
         * ID2
         */
        std::string xAmzId2;

      private:

        friend ResponseElements tag_invoke(boost::json::value_to_tag<ResponseElements>, boost::json::value const &v) {
            ResponseElements r;
            r.xAmzRequestId = Core::Json::GetStringValue(v, "xAmzRequestId");
            r.xAmzId2 = Core::Json::GetStringValue(v, "xAmzId2");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ResponseElements const &obj) {
            jv = {
                    {"xAmzRequestId", obj.xAmzRequestId},
                    {"xAmzId2", obj.xAmzId2},
            };
        }
    };

    struct Object final : Common::BaseCounter<Object> {

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

      private:

        friend Object tag_invoke(boost::json::value_to_tag<Object>, boost::json::value const &v) {
            Object r;
            r.key = Core::Json::GetStringValue(v, "key");
            r.size = Core::Json::GetLongValue(v, "size");
            r.etag = Core::Json::GetStringValue(v, "etag");
            r.versionId = Core::Json::GetStringValue(v, "versionId");
            r.sequencer = Core::Json::GetStringValue(v, "sequencer");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Object const &obj) {
            jv = {
                    {"key", obj.key},
                    {"size", obj.size},
                    {"etag", boost::json::value_from(obj.etag)},
                    {"versionId", obj.versionId},
                    {"sequencer", obj.sequencer},
            };
        }
    };

    struct NotificationBucket final : Common::BaseCounter<NotificationBucket> {

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
        Owner ownerIdentity;

      private:

        friend NotificationBucket tag_invoke(boost::json::value_to_tag<NotificationBucket>, boost::json::value const &v) {
            NotificationBucket r;
            r.name = Core::Json::GetStringValue(v, "s3SchemaVersion");
            r.arn = Core::Json::GetStringValue(v, "arn");
            r.ownerIdentity = boost::json::value_to<Owner>(v.at("ownerIdentity"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, NotificationBucket const &obj) {
            jv = {
                    {"name", obj.name},
                    {"arn", obj.arn},
                    {"ownerIdentity", boost::json::value_from(obj.ownerIdentity)},
            };
        }
    };

    struct S3 final : Common::BaseCounter<S3> {

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

      private:

        friend S3 tag_invoke(boost::json::value_to_tag<S3>, boost::json::value const &v) {
            S3 r;
            r.s3SchemaVersion = Core::Json::GetStringValue(v, "s3SchemaVersion");
            r.configurationId = Core::Json::GetStringValue(v, "configurationId");
            r.bucket = boost::json::value_to<NotificationBucket>(v.at("bucket"));
            r.object = boost::json::value_to<Object>(v.at("object"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, S3 const &obj) {
            jv = {
                    {"s3SchemaVersion", obj.s3SchemaVersion},
                    {"configurationId", obj.configurationId},
                    {"bucket", boost::json::value_from(obj.bucket)},
                    {"object", boost::json::value_from(obj.object)},
            };
        }
    };

    struct Record final : Common::BaseCounter<Record> {

        /**
         * Record version
         */
        std::string eventVersion = "2.2";

        /**
         * Event source
         */
        std::string eventSource = "aws:s3";

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

      private:

        friend Record tag_invoke(boost::json::value_to_tag<Record>, boost::json::value const &v) {
            Record r;
            r.eventVersion = Core::Json::GetStringValue(v, "eventVersion");
            r.eventSource = Core::Json::GetStringValue(v, "eventSource");
            r.eventTime = Core::Json::GetDatetimeValue(v, "eventTime");
            r.eventName = Core::Json::GetStringValue(v, "eventName");
            r.userIdentity = boost::json::value_to<UserIdentity>(v.at("eventName"));
            r.requestParameter = boost::json::value_to<RequestParameter>(v.at("requestParameter"));
            r.responseElements = boost::json::value_to<ResponseElements>(v.at("responseElements"));
            r.s3 = boost::json::value_to<S3>(v.at("s3"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Record const &obj) {
            jv = {
                    {"eventVersion", obj.eventVersion},
                    {"eventSource", obj.eventSource},
                    {"eventTime", Core::DateTimeUtils::ToISO8601(obj.eventTime)},
                    {"eventName", obj.eventName},
                    {"userIdentity", boost::json::value_from(obj.userIdentity)},
                    {"requestParameter", boost::json::value_from(obj.requestParameter)},
                    {"responseElements", boost::json::value_from(obj.responseElements)},
                    {"s3", boost::json::value_from(obj.s3)},
            };
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
    struct EventNotification final : Common::BaseCounter<EventNotification> {

        /**
         * S3 event record
         */
        std::vector<Record> records;

      private:

        friend EventNotification tag_invoke(boost::json::value_to_tag<EventNotification>, boost::json::value const &v) {
            EventNotification r;
            if (!r.records.empty()) {
                r.records = boost::json::value_to<std::vector<Record>>(v.at("Records"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, EventNotification const &obj) {
            jv = {
                    {"Records", boost::json::value_from(obj.records)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_EVENT_NOTIFICATION_H
