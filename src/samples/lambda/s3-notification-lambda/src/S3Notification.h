//
// Created by vogje01 on 16/09/2023.
//

#ifndef AWSMOCK_SAMPLES_S3_NOTIFICATION_LAMBDA_S3NOTIFICATION_H
#define AWSMOCK_SAMPLES_S3_NOTIFICATION_LAMBDA_S3NOTIFICATION_H

// C++ includes
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

// AWS SDK C++ includes
#include <aws/core/Aws.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/core/utils/logging/LogMacros.h>

// Libri includes
#include "S3Record.h"

/**
 * S3 notification struct.
 *
 * Example:
 * <pre>
 * {
 *    "Records": [
 *        {
 *            "messageId": "5a29ba27-2298-43c2-85be-3c508e3c248e",
 *            "receiptHandle": "AQEBUrUXsOIWEjB.....",
 *            "body": "{\n\t\"lieferantenId\": \"ftpuser1\",\n\t\"dateinameOriginal\": \"9783986360115.png\",\n\t\"eingangsverzeichnis\": \"ftpuser1/9783986360115.png\",\n\t\"eingangsdatum\": \"2023-09-15T15:08:17.4\",\n\t\"dateinameS3\": \"ftpuser1/9783986360115.png\",\n\t\"ablageortS3\": \"pim-dev-file-delivery\",\n\t\"fileType\": PNG\n}",
 *            "attributes": {
 *                "ApproximateReceiveCount": "1",
 *                "SentTimestamp": "1694803240999",
 *                "SenderId": "AROATJV5PDKHWVEJOFZQZ:jens.vogt@opitz-consulting.com",
 *                "ApproximateFirstReceiveTimestamp": "1694803241000"
 *            },
 *            "messageAttributes": {
 *              "retryContext": {
 *                  "dataType": "string",
 *                  "stringValue": "[{\"verarbeitungsschritt\":\"blabluh\",\"internalId\":\"blabluh\",\"queueName\":\"blabluh\"}",
 *              },
 *             "fileType":{
 *                  "dataType":"string"
 *                  "stringValue": "blahbluh"
 *              }
 *            },
 *            "md5OfBody": "a387545f8b24b1fa46e94d5ed62f52b8",
 *            "eventSource": "aws:sqs",
 *            "eventSourceARN": "arn:aws:sqs:eu-central-1:226956876431:pim-dev-ftp-file-distribution-image-queue",
 *            "awsRegion": "eu-central-1"
 *        }
 *    ]
 * }
 * </pre>
 */
struct S3Notification {

    /**
     * S3 record
     */
    std::vector<S3Record> records;

    /**
     * Constructor
     */
    S3Notification() = default;

    /**
     * Constructor
     *
     * @param payload message body
     */
    explicit S3Notification(const std::string &payload) {
        FromJson(payload);
    }

    static void FromJson(const std::string &payload) {

        std::cout << "From S3 notification JSON, payload: " << payload << std::endl;

        const Aws::Utils::Json::JsonValue json(payload);
        if (!json.WasParseSuccessful()) {

            return;
        }

        // Get the record array
        auto recordsArray = json.View().GetArray("Records");
        for (int i = 0; i < recordsArray.GetLength(); i++) {

            // Get one record element
            auto record = recordsArray.GetItem(i);

            // Deserialize record
            auto sqsRecord = S3Record(record);
        }
    }
};
#endif// AWSMOCK_SAMPLES_S3_NOTIFICATION_LAMBDA_S3NOTIFICATION_H
