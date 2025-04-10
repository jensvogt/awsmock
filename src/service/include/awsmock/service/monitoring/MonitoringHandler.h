//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_MONITORING_HANDLER_H
#define AWSMOCK_SERVICE_MONITORING_HANDLER_H

// Boost includes
#include <boost/beast.hpp>

// AwsMock includes
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/MonitoringClientCommand.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/monitoring/MonitoringService.h>

namespace AwsMock::Service {

    namespace http = boost::beast::http;
    namespace ip = boost::asio::ip;

    /**
     * @brief AWS mock monitoring handler.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class MonitoringHandler final : public AbstractHandler {

      public:

        /**
         * @brief Constructor
         */
        explicit MonitoringHandler() : AbstractHandler("monitoring-handler"){};

        /**
         * @brief HTTP POST request.
         *
         * @param request HTTP request
         * @param region AWS region
         * @param user AWS user
         * @return HTTP response
         * @see AbstractResource::handlePost(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
         */
        http::response<http::dynamic_body> HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

      private:

        /**
         * Monitoring service
         */
        MonitoringService _monitoringService;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_SQS_HANDLER_H
