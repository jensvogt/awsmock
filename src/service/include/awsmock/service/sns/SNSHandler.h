//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVER_LAMBDA_SERVER_H
#define AWSMOCK_SERVER_LAMBDA_SERVER_H

// AwsMock includes
#include <awsmock/dto/common/SNSClientCommand.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/sns/SNSService.h>
#include <awsmock/dto/sns/model/DeleteTopicRequest.h>

namespace AwsMock::Service {

    /**
     * @brief SNS request handler
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SNSHandler final : public AbstractHandler, public std::enable_shared_from_this<SNSHandler> {

    public:

        /**
         * @brief Constructor
         */
        explicit SNSHandler(boost::asio::io_context &ioc) : AbstractHandler("sns-handler", ioc), _snsService(ioc) {}

        /**
         * @brief HTTP POST request.
         *
         * @param request HTTP request
         * @param region AWS region
         * @param user AWS user
         * @return HTTP response
         * @see AbstractResource::handlePost(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user)
         */
        http::response<http::dynamic_body> HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

    private:

        /**
         * Get the message attributes.
         *
         * @param payload HTTP body
         * @return list of message userAttributes
         */
        static std::map<std::string, Dto::SNS::MessageAttribute> GetMessageAttributes(const std::string &payload);

        /**
         * SNS module
         */
        SNSService _snsService;

        /**
         * Default account ID
         */
        std::string _accountId;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVER_LAMBDA_SERVER_H
