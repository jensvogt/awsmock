//
// Created by vogje01 on 5/9/25.
//

// AWS C++ SDK
#include <aws/core/Aws.h>
#include <aws/core/utils/logging/ConsoleLogSystem.h>
#include <aws/core/utils/logging/LogLevel.h>
#include <aws/lambda-runtime/runtime.h>
#include <aws/s3/S3Client.h>

// AwsMock includes
#include "S3Notification.h"

/**
 * Amazon memory allocation tag.
 */
constexpr char TAG[] = "ImageTransformation";
constexpr auto logLevelAws = Aws::Utils::Logging::LogLevel::Info;

/**
 * AWS lambda console logger.
 *
 * @return return AWS lambda console logger.
 */
std::function<std::shared_ptr<Aws::Utils::Logging::LogSystemInterface>()> GetConsoleLoggerFactory() {
    return [] {
        return Aws::MakeShared<Aws::Utils::Logging::ConsoleLogSystem>(TAG, logLevelAws);
    };
}

/**
 * Lambda request handler. As we get the actual SQS message nested in a event
 * notification records array, with possibly several other records, we need to
 * loop over all incoming SQS records and extract the individual SQS messages.
 *
 * <pre>
 * {
 *      "Records": [
 *          {
 *            ...
 *            "body": "{\"lieferantenId\": \"DLI145\", ...}",
 *            ...
 *          }
 *      ]
 * }
 * </pre>
 *
 * @param req invocation request
 * @param s3Client Amazon S3 client.
 * @return invocation_response::success if successful,
 * invocation_response::failure otherwise
 */
static aws::lambda_runtime::invocation_response QueueHandler(aws::lambda_runtime::invocation_request const &req, Aws::S3::S3Client const &s3Client) {

    // Get and deserialize the S3 notification SQS messages, could be more than
    // one message, per SQS message bunch.
    S3Notification s3Notification(req.payload);

    // Loop over all SQS message records (there could be more than one) and
    // process one record at a time.
    /*    for (auto &fileInfoDto : sqsNotification.fileInformationDtos) {

        aws::lambda_runtime::invocation_response result =
            ProcessRecord(sqsClient, snsClient, s3Client, fileInfoDto);
        if (!result.is_success()) {
            return aws::lambda_runtime::invocation_response::failure(
                "failure", "application/base64");
        }
    }*/
    return aws::lambda_runtime::invocation_response::success("success", "foobar");
}

int main() {
    using namespace Aws;

    // Initialize AWS SDK
    SDKOptions options;
    options.loggingOptions.logLevel = logLevelAws;
    options.loggingOptions.logger_create_fn = GetConsoleLoggerFactory();
    options.httpOptions.initAndCleanupCurl = true;
    InitAPI(options);

    // AWS S3 clients
    Client::ClientConfiguration config;
    config.endpointOverride = "http://localhost:4566";
    S3::S3Client s3Client(config, Client::AWSAuthV4Signer::PayloadSigningPolicy::Never, false);

    // Lambda request handler
    auto handler_fn = [&s3Client](aws::lambda_runtime::invocation_request const &req) {
        return QueueHandler(req, s3Client);
    };
    aws::lambda_runtime::run_handler(handler_fn);

    // Cleanup
    ShutdownAPI(options);
    return 0;
}
