//
// Created by vogje01 on 19/07/2026.
//

#pragma once

#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/STSClientCommand.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/sts/STSService.h>

namespace Awsmock::Service {

    namespace http = boost::beast::http;

    class STSHandler final : public AbstractHandler {

      public:

        explicit STSHandler(boost::asio::io_context &ioc) : AbstractHandler("sts-handler", ioc) {}

        http::response<http::dynamic_body> HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

      private:
        mutable logger_t _logger{boost::log::keywords::channel = "STS"};
        Service::STSService _stsService;
    };

}// namespace Awsmock::Service
