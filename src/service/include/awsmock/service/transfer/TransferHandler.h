//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_TRANSFER_HANDLER_H
#define AWSMOCK_SERVICE_TRANSFER_HANDLER_H

// Boost includes
#include <boost/beast.hpp>

// AwsMock includes
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
    class TransferHandler final : public AbstractHandler {

      public:

        /**
         * @brief Constructor
         */
        explicit TransferHandler(boost::asio::io_context &ioc) : AbstractHandler("transfer-handler", ioc) {}

        /**
         * @brief HTTP POST request.
         *
         * @param request HTTP request
         * @param region AWS region name
         * @param user AWS user
         * @return HTTP response
         * @see AbstractResource::handlePost(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user)
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
        TransferService _transferService;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_TRANSFER_HANDLER_H
