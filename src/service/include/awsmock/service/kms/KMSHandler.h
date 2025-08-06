//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_KMS_HANDLER_H
#define AWSMOCK_SERVICE_KMS_HANDLER_H

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/KMSClientCommand.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/kms/KMSService.h>
#include <awsmock/service/monitoring/MetricService.h>

namespace AwsMock::Service {

    namespace http = http;
    namespace ip = boost::asio::ip;

    /**
     * @brief KMS request handler
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class KMSHandler final : public AbstractHandler {

      public:

        /**
         * @brief Constructor
         */
        explicit KMSHandler(boost::beast::tcp_stream &stream) : AbstractHandler("kms-handler", stream) {}

        /**
         * @brief HTTP POST request.
         *
         * @param request HTTP request
         * @param region AWS region
         * @param user AWS user
         * @param isDone response already sent
         * @return HTTP response
         * @see AbstractResource::handlePost(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
         */
        http::response<http::dynamic_body> HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user, bool &isDone) override;

      private:

        /**
         * KMS service
         */
        KMSService _kmsService;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_KMS_HANDLER_H
