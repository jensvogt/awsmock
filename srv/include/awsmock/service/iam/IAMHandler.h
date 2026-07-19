//
// Created by vogje01 on 19/07/2026.
//

#pragma once

#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/service/common/AbstractHandler.h>

namespace Awsmock::Service {

    namespace http = boost::beast::http;

    class IAMHandler final : public AbstractHandler {

      public:

        explicit IAMHandler(boost::asio::io_context &ioc) : AbstractHandler("iam-handler", ioc) {}

        http::response<http::dynamic_body> HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

      private:
        mutable logger_t _logger{boost::log::keywords::channel = "IAM"};
    };

}// namespace Awsmock::Service
