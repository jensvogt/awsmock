//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_TRANSFER_HANDLER_H
#define AWSMOCK_SERVICE_TRANSFER_HANDLER_H

// Boost includes
#include <boost/beast.hpp>
#include <boost/beast/http/impl/message.hpp>

// AwsMock includes
#include "awsmock/core/config/Configuration.h"
#include <awsmock/core/LogStream.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/transfer/TransferService.h>

namespace AwsMock::Service {

    namespace http = boost::beast::http;
    namespace ip = boost::asio::ip;

    /**
     * @brief Transfer mock handler
     *
     * <p>AWS Transfer HTTP request handler. All Transfer related REST call are ending here.<p>
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class TransferHandler : public AbstractHandler {

      public:

        /**
         * @brief Constructor
         */
        explicit TransferHandler() = default;

        /**
         * @brief HTTP POST request.
         *
         * @param request HTTP request
         * @param region AWS region name
         * @param user AWS user
         * @return HTTP response
         * @see AbstractResource::handlePost(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
         */
        http::response<http::dynamic_body> HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

      private:

        /**
         * @brief Get the request target.
         *
         * @param request HTTP request
         * @return target string
         */
        static std::string GetTarget(const http::request<http::dynamic_body> &request);

        /**
         * Transfer module
         */
        Service::TransferService _transferService;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_TRANSFER_HANDLER_H
