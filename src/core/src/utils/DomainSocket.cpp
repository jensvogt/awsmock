//
// Created by vogje01 on 5/28/24.
//

#include <awsmock/core/DomainSocket.h>

namespace AwsMock::Core {

    request<string_body> DomainSocket::PrepareJsonMessage(const verb method, const std::string &path, const std::string &body, const std::map<std::string, std::string> &headers) {

        request<string_body> request;

        request.method(method);
        request.target(path);
        request.body() = body;
        request.prepare_payload();
        request.base().set(field::host, "localhost");
        request.base().set(field::content_type, "application/json");
        request.base().set(field::content_length, std::to_string(body.size()));

        if (!headers.empty()) {
            for (const auto &[fst, snd]: headers) {
                request.base().set(fst, snd);
            }
        }
        return request;
    }

    request<file_body> DomainSocket::PrepareBinaryMessage(const verb method, const std::string &path, const std::string &filename, const std::map<std::string, std::string> &headers) {

        boost::system::error_code ec;
        request<file_body> request;
        request.method(method);
        request.target(path);
        request.base().set("Host", "localhost");
        request.set(field::user_agent, BOOST_BEAST_VERSION_STRING);
        request.body().open(filename.c_str(), boost::beast::file_mode::read, ec);
        request.prepare_payload();

        if (!headers.empty()) {
            for (const auto &[fst, snd]: headers) {
                request.base().set(fst, snd);
            }
        }
        return request;
    }

    DomainSocketResult DomainSocket::PrepareResult(response<string_body> response) {

        DomainSocketResult domainSocketResult;
        domainSocketResult.body = response.body();
        domainSocketResult.statusCode = response.result();
        domainSocketResult.contentLength = static_cast<long>(response.payload_size().get());
        return domainSocketResult;
    }

}// namespace AwsMock::Core
