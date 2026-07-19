//
// Created by vogje01 on 19/07/2026.
//

#include <awsmock/service/cloudwatch/CloudWatchLogsHandler.h>

namespace Awsmock::Service {

    http::response<http::dynamic_body> CloudWatchLogsHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_trace << "CWL POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        // Extract command from X-Amz-Target: Logs_20140328.CreateLogGroup
        std::string target = Core::HttpUtils::GetHeaderValue(request, "X-Amz-Target");
        std::string action;
        const auto dot = target.find('.');
        if (dot != std::string::npos) {
            action = target.substr(dot + 1);
        }

        const std::string requestId = Core::AwsUtils::CreateRequestId();

        try {
            log_info << "CWL action: " << action;

            if (action == "CreateLogGroup") {
                return SendResponse(request, http::status::ok);
            }

            if (action == "DeleteLogGroup") {
                return SendResponse(request, http::status::ok);
            }

            if (action == "DescribeLogGroups") {
                const std::string responseBody = R"({"logGroups":[],"nextToken":null})";
                return SendResponse(request, http::status::ok, responseBody);
            }

            if (action == "CreateLogStream") {
                return SendResponse(request, http::status::ok);
            }

            if (action == "DeleteLogStream") {
                return SendResponse(request, http::status::ok);
            }

            if (action == "DescribeLogStreams") {
                const std::string responseBody = R"({"logStreams":[],"nextToken":null})";
                return SendResponse(request, http::status::ok, responseBody);
            }

            if (action == "PutLogEvents") {
                const std::string responseBody = R"({"nextSequenceToken":"1"})";
                return SendResponse(request, http::status::ok, responseBody);
            }

            if (action == "FilterLogEvents") {
                const std::string responseBody = R"({"events":[],"searchedLogStreams":[],"nextToken":null})";
                return SendResponse(request, http::status::ok, responseBody);
            }

            if (action == "PutRetentionPolicy") {
                return SendResponse(request, http::status::ok);
            }

            if (action == "DeleteRetentionPolicy") {
                return SendResponse(request, http::status::ok);
            }

            if (action == "ListTagsLogGroup" || action == "ListTagsForResource") {
                const std::string responseBody = R"({"tags":{}})";
                return SendResponse(request, http::status::ok, responseBody);
            }

            if (action == "TagLogGroup" || action == "TagResource") {
                return SendResponse(request, http::status::ok);
            }

            if (action == "UntagLogGroup" || action == "UntagResource") {
                return SendResponse(request, http::status::ok);
            }

            if (action == "PutMetricFilter") {
                return SendResponse(request, http::status::ok);
            }

            if (action == "DeleteMetricFilter") {
                return SendResponse(request, http::status::ok);
            }

            if (action == "DescribeMetricFilters") {
                const std::string responseBody = R"({"metricFilters":[],"nextToken":null})";
                return SendResponse(request, http::status::ok, responseBody);
            }

            log_error << "Unknown CWL action: " << action;
            return SendResponse(request, http::status::not_implemented, R"({"__type":"NotImplementedException","message":"Action not implemented"})");

        } catch (std::exception &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.what());
        }
    }

}// namespace Awsmock::Service
