//
// Created by vogje01 on 07/02/2025.
//

#ifndef AWSMOCK_SAMPLES_S3_NOTIFICATION_LAMBDA_S3RECORD_H
#define AWSMOCK_SAMPLES_S3_NOTIFICATION_LAMBDA_S3RECORD_H

// C++ includes
#include <string>

// Libri includes
//#include <dto/Attribute.h>
//#include <dto/MessageAttribute.h>

struct S3Record {

    // Message ID
    std::string messageId;

    // Receipt handle
    std::string receiptHandle;

    // AWS region
    std::string awsRegion;

    // MD5 sum
    std::string md5Sum;

    // Event source
    std::string eventSource;

    // Event source ARN
    std::string eventSourceArn;

    // Attributes
    //Attribute attributes;

    // Message attributes
    //    std::map<std::string, Aws::SQS::Model::MessageAttributeValue> messageAttributes{};


    S3Record() = default;

    explicit S3Record(const Aws::Utils::Json::JsonView &record) {
        FromJson(record);
    }

    void FromJson(const Aws::Utils::Json::JsonView &record) {
        messageId = record.GetString("messageId");
        receiptHandle = record.GetString("receiptHandle");
        awsRegion = record.GetString("awsRegion");
        md5Sum = record.GetString("md5OfBody");
        eventSource = record.GetString("eventSource");
        eventSourceArn = record.GetString("eventSourceARN");
        //        attributes.FromJson(record.GetObject("attributes"));
        //        messageAttributes = MessageAttributesFromJson(record.GetObject("messageAttributes"));
        //        poco_information(Poco::Logger::get(LOGGER_NAME), "messageId: " + messageId + " region: " + awsRegion + " md5Sum: " + md5Sum + " eventSource: " + eventSource + " eventSourceArn: " + eventSourceArn);
        //        poco_information(Poco::Logger::get(LOGGER_NAME), "body: " + record.GetString("body"));
    }
};

#endif// AWSMOCK_SAMPLES_S3_NOTIFICATION_LAMBDA_S3RECORD_H
