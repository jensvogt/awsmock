//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_RESOURCE_TRANSFERHANDLER_H
#define AWSMOCK_RESOURCE_TRANSFERHANDLER_H

// Poco includes
#include "Poco/Logger.h"
#include "Poco/LogStream.h"
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"

// AwsMock includes
#include "awsmock/core/Configuration.h"
#include "awsmock/core/MetricService.h"
#include "awsmock/core/MetricServiceTimer.h"
#include "awsmock/core/MetricDefinition.h"
#include "awsmock/resource/HandlerException.h"
#include "awsmock/resource/AbstractResource.h"
#include "awsmock/service/S3Service.h"
#include "awsmock/dto/s3/CreateBucketRequest.h"
#include "awsmock/dto/s3/CompleteMultipartUploadResult.h"
#include "awsmock/dto/s3/DeleteObjectsRequest.h"
#include "awsmock/dto/s3/DeleteObjectsResponse.h"
#include "awsmock/dto/s3/InitiateMultipartUploadResult.h"
#include "awsmock/dto/s3/ListBucketRequest.h"
#include "awsmock/dto/s3/ListBucketResult.h"
#include "awsmock/dto/s3/ListAllBucketResponse.h"
#include "awsmock/dto/s3/PutObjectRequest.h"
#include "awsmock/dto/s3/PutBucketNotificationRequest.h"

#define TRANSFER_DEFAULT_HOST "localhost"
#define TRANSFER_DEFAULT_PORT 9504

namespace AwsMock {

    /**
     * AWS Transfer mock handler
     *
     * <p>AWS Transfer HTTP request handler. All Transfer related REST call are ending here. Depending on the request header the Transfer service will be selected in case the
     * authorization header contains the Transfer service. Default port is 9504.<p>
     *
     * <p><h3>GET Requests</h3>
     * <ul>
     * <li>S3 bucket list command: <pre>aws s3 ls --endpoint http://localhost:4567</pre></li>
     * <li>S3 object list command: <pre>aws s3 ls s3://example-bucket --recursive --endpoint http://localhost:4567</pre></li>
     * </ul>
     * </p>
     * <p><h3>POST Requests</h3>
     * <ul>
     * <li>Bigfile (>4MB) Initial Multipart upload: <pre>aws cp example.txt s3://example-bucket/test/example.txt --endpoint http://localhost:4567</pre></li>
     * <li>Upload part</li>
     * <li>Complete Multipart upload</li>
     * </ul>
     * <p>
     */
    class TransferHandler : public AwsMock::Resource::AbstractResource {

    public:

      /**
       * Constructor
       *
       * @param configuration application configuration
       * @param metricService monitoring service
       */
      TransferHandler(Core::Configuration &configuration, Core::MetricService &metricService);

    protected:

      /**
       * HTTP GET request.
       *
       * @param request HTTP request
       * @param response HTTP response
       * @param region AWS region name
       * @param user AWS user
       * @see AbstractResource::handleGet(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
       */
      void handleGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) override;

      /**
       * HTTP PUT request.
       *
       * @param request HTTP request
       * @param response HTTP response
       * @param region AWS region name
       * @param user AWS user
       * @see AbstractResource::handlePut(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
       */
      void handlePut(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) override;

      /**
       * HTTP POST request.
       *
       * @param request HTTP request
       * @param response HTTP response
       * @param region AWS region name
       * @param user AWS user
       * @see AbstractResource::handlePost(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
       */
      void handlePost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) override;

      /**
       * Delete DELETE request.
       *
       * @param request HTTP request
       * @param response HTTP response
       * @param region AWS region name
       * @param user AWS user
       * @see AbstractResource::handleDelete(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
       */
      void handleDelete(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) override;

      /**
       * Options request.
       *
       * @param response HTTP response
       * @see AbstractResource::handleOption(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
       */
      void handleOptions(Poco::Net::HTTPServerResponse &response) override;

      /**
       * Head request.
       *
       * @param request HTTP request
       * @param response HTTP response
       * @param region AWS region name
       * @param user AWS user
       * @see AbstractResource::handleHead(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
       */
      void handleHead(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response, const std::string &region, const std::string &user) override;

    private:

      /**
       * Logger
       */
      Core::LogStream _logger;

      /**
       * S3 handler configuration
       */
      Core::Configuration &_configuration;

      /**
       * Metric service
       */
      Core::MetricService &_metricService;

      /**
       * Transfer service port
       */
      int _transferServicePort;

      /**
       * Transfer service host
       */
      std::string _transferServiceHost;
    };
} // namespace AwsMock

#endif // AWSMOCK_RESOURCE_TRANSFERHANDLER_H