//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/core/HttpUtils.h>

namespace AwsMock::Core {

    std::string HttpUtils::GetBasePath(const std::string &uri) {
        std::string basePath = StringUtils::SubStringUntil(uri, "?");
        if (basePath[0] == '/') {
            basePath = basePath.substr(1);
        }
        return basePath;
    }

    std::string HttpUtils::GetPathParameter(const std::string &uri, int index) {

        const std::string basePath = GetBasePath(uri);

        std::vector<std::string> parameters = StringUtils::Split(basePath, '/');
        if (index >= static_cast<int>(parameters.size())) {
            return {};
        }
        if (IsUrlEncoded(parameters[index])) {
            return StringUtils::UrlDecode(parameters[index]);
        }
        return parameters[index];
    }

    std::string HttpUtils::GetPathParametersFromIndex(const std::string &uri, int index) {

        const std::string basePath = GetBasePath(uri);
        const std::vector<std::string> parameters = StringUtils::Split(basePath, '/');
        return StringUtils::UrlDecode(StringUtils::Join(parameters, index));
    }

    std::vector<std::string> HttpUtils::GetPathParameters(const std::string &uri) {

        const std::string basePath = GetBasePath(uri);
        return StringUtils::Split(basePath, '/');
    }

    bool HttpUtils::HasPathParameters(const std::string &uri, int index) {

        const std::string basePath = GetBasePath(uri);
        const std::vector<std::string> pathVector = StringUtils::Split(basePath, '/');
        return index < pathVector.size();
    }

    std::string HttpUtils::GetQueryString(const std::string &uri) {
        if (StringUtils::Contains(uri, "?")) {
            return StringUtils::SubStringAfter(uri, "?");
        }
        return uri;
    }

    int HttpUtils::CountQueryParameters(const std::string &uri) {
        const std::string queryString = GetQueryString(uri);
        const std::vector<std::string> parameters = StringUtils::Split(queryString, '&');
        return static_cast<int>(parameters.size());
    }

    int HttpUtils::CountQueryParametersByPrefix(const std::string &uri, const std::string &prefix) {

        const std::string queryString = GetQueryString(uri);
        const std::vector<std::string> parameters = StringUtils::Split(queryString, '&');

        int count = 0;
        for (auto &it: parameters) {
            if (it.starts_with(prefix)) {
                count++;
            }
        }
        return count;
    }

    std::string HttpUtils::GetQueryParameterName(const std::string &parameter) {
        return StringUtils::Split(parameter, '=')[0];
    }

    std::string HttpUtils::GetQueryParameterValue(const std::string &parameter) {
        if (const std::vector<std::string> parts = StringUtils::Split(parameter, '='); parts.size() > 1) {
            std::string value = parts[1];
            if (IsUrlEncoded(value)) {
                log_trace << "Found query parameter value, name: " << parameter << ", value: " << value;
                return StringUtils::UrlDecode(value);
            }
            log_trace << "Found query parameter value, name: " << parameter << ", value: " << value;
            return value;
        }
        log_debug << "Query parameter value not found, name: " << parameter;
        return {};
    }

    std::vector<std::string> HttpUtils::GetQueryParametersByPrefix(const std::string &uri, const std::string &prefix) {

        const std::string queryString = GetQueryString(uri);
        std::vector<std::string> parameters = StringUtils::Split(queryString, '&');

        std::vector<std::string> namedParameters;
        for (const auto &it: parameters) {
            if (!prefix.empty() && it.starts_with(prefix)) {
                namedParameters.emplace_back(GetQueryParameterValue(it));
            } else {
                namedParameters.emplace_back(GetQueryParameterValue(it));
            }
        }
        return namedParameters;
    }

    std::map<std::string, std::string> HttpUtils::GetQueryParameters(const std::string &uri) {

        if (!StringUtils::Contains(uri, "?")) {
            return {};
        }

        const std::string queryString = GetQueryString(uri);
        std::vector<std::string> parameters = StringUtils::Split(queryString, '&');

        std::map<std::string, std::string> queryParameters;
        for (const auto &it: parameters) {
            std::vector<std::string> namedValues = StringUtils::Split(it, '=');
            queryParameters[namedValues[0]] = namedValues[1];
        }
        return queryParameters;
    }

    std::string HttpUtils::GetQueryParameterValueByName(const std::string &uri, const std::string &name) {

        std::string queryString = GetQueryString(uri);
        if (queryString.at(0) == '&') {
            queryString = queryString.substr(1);
        }

        for (std::vector<std::string> parameters = StringUtils::Split(queryString, '&'); const auto &it: parameters) {
            if (GetQueryParameterName(it) == name) {
                return GetQueryParameterValue(it);
            }
        }
        return {};
    }

    std::string HttpUtils::GetQueryParameterByPrefix(const std::string &uri, const std::string &prefix, int index) {

        std::vector<std::string> parameters = GetQueryParametersByPrefix(uri, prefix);

        if (index > parameters.size()) {
            throw ServiceException("Invalid path parameter index");
        }
        log_trace << "Query parameter found, prefix: " << prefix << " index: " << index;
        return parameters[index - 1];
    }

    int HttpUtils::GetIntParameter(const std::string &body, const std::string &name, int min, int max, int def) {
        int value = def;
        if (const std::string parameterValue = GetQueryParameterValueByName(body, name); !parameterValue.empty()) {
            value = std::stoi(parameterValue);
            value = value > min && value < max ? value : def;
        }
        log_trace << "Query parameter found, name: " << name << " value: " << value;
        return value;
    }

    bool HttpUtils::HasQueryParameter(const std::string &uri, const std::string &name) {
        const std::string queryString = GetQueryString(uri);
        for (std::vector<std::string> parameters = StringUtils::Split(queryString, '&'); const auto &it: parameters) {
            if (GetQueryParameterName(it) == name) {
                return true;
            }
        }
        log_trace << "Query parameter not found, name: " << name;
        return false;
    }

    std::string HttpUtils::AddQueryParameter(std::string &url, const std::string &name, bool value) {
        url = AddQueryDelimiter(url);
        url += name + "=" + (value ? "true" : "false");
        return url;
    }

    std::string HttpUtils::AddQueryParameter(std::string &url, const std::string &name, const std::string &value) {
        url = AddQueryDelimiter(url);
        url += name + "=" + value;
        return url;
    }

    std::string HttpUtils::AddQueryParameter(std::string &url, const std::string &name, int value) {
        url = AddQueryDelimiter(url);
        url += name + "=" + std::to_string(value);
        return url;
    }

    std::string HttpUtils::AddQueryDelimiter(std::string &url) {
        url += Core::StringUtils::Contains(url, "?") ? "&" : "?";
        return url;
    }

    bool HttpUtils::HasHeader(const http::request<http::string_body> &request, const std::string &name) {
        return request.base().find(name) != request.end();
    }

    bool HttpUtils::HasHeader(const http::request<http::dynamic_body> &request, const std::string &name) {
        return request.base().find(name) != request.end();
    }

    bool HttpUtils::HasHeader(const http::request<request_body_t, http::basic_fields<alloc_t>> &request, const std::string &name) {
        return request.base().find(name) != request.end();
    }

    bool HttpUtils::HasHeaderValue(const http::request<http::dynamic_body> &request, const std::string &name, const std::string &value) {
        return HasHeader(request, name) && GetHeaderValue(request, name) == value;
    }

    std::string HttpUtils::GetHeaderValue(const http::request<http::dynamic_body> &request, const std::string &name, const std::string &defaultValue) {
        if (request.base().find(name) == request.end()) {
            if (!defaultValue.empty()) {
                return defaultValue;
            }
        }
        return request.base()[name];
    }

    std::string HttpUtils::GetHeaderValue(const http::request<http::string_body> &request, const std::string &name, const std::string &defaultValue) {
        if (!HasHeader(request, name)) {
            if (!defaultValue.empty()) {
                return defaultValue;
            }
        }
        return request.base()[name];
    }

    std::string HttpUtils::GetHeaderValue(const http::request<request_body_t, http::basic_fields<alloc_t>> &request, const std::string &name, const std::string &defaultValue) {
        if (request.base().find(name) == request.end()) {
            if (!defaultValue.empty()) {
                return defaultValue;
            }
        }
        return request.base()[name];
    }

    std::map<std::string, std::string> HttpUtils::GetHeaders(const http::request<http::dynamic_body> &request) {

        std::map<std::string, std::string> headers;
        for (const auto &header: request) {
            headers[header.name_string()] = header.value();
            log_trace << header.name_string() << ": " << header.value();
        }
        return headers;
    }

    void HttpUtils::DumpHeaders(const http::request<http::dynamic_body> &request) {
        log_info << SEPARATOR;
        log_info << "Method: " << request.method_string();
        log_info << "Target: " << request.target();
        for (const auto &header: request.base()) {
            log_info << header.name_string() << ": " << header.value();
        }
        log_info << SEPARATOR;
    }

    void HttpUtils::DumpHeaders(const http::request<http::string_body> &request) {
        log_info << SEPARATOR;
        log_info << "Method: " << request.method_string();
        for (const auto &header: request.base()) {
            log_info << header.name_string() << ": " << header.value();
        }
        log_info << SEPARATOR;
    }

    void HttpUtils::DumpHeaders(const http::response<http::dynamic_body> &response) {
        log_info << SEPARATOR;
        for (const auto &header: response.base()) {
            log_info << header.name_string() << ": " << header.value();
        }
        log_info << SEPARATOR;
    }

    void HttpUtils::DumpRequest(const http::request<http::dynamic_body> &request, const int limit) {
        DumpHeaders(request);
        if (limit > 0) {
            const std::string tmp = GetBodyAsString(request);
            log_info << tmp.substr(0, limit);
        } else {
            log_info << GetBodyAsString(request);
        }
        log_info << SEPARATOR;
    }

    void HttpUtils::DumpRequest(const http::request<http::string_body> &request) {
        log_info << SEPARATOR;
        DumpHeaders(request);
        log_info << GetBodyAsString(request);
        log_info << SEPARATOR;
    }

    void HttpUtils::DumpHeaders(const http::response<http::string_body> &response) {
        log_info << SEPARATOR;
        for (const auto &header: response.base()) {
            log_info << header.name_string() << ": " << header.value();
        }
        log_info << SEPARATOR;
    }

    void HttpUtils::DumpResponse(const http::response<http::dynamic_body> &response) {
        log_info << SEPARATOR;
        DumpHeaders(response);
        log_info << GetBodyAsString(response);
        log_info << SEPARATOR;
    }

    void HttpUtils::DumpResponse(const http::response<http::string_body> &response) {
        log_info << SEPARATOR;
        DumpHeaders(response);
        log_info << GetBodyAsString(response);
        log_info << SEPARATOR;
    }

    std::string HttpUtils::GetContentType(const http::request<http::dynamic_body> &request) {

        return StringUtils::ContainsIgnoreCase(request.base()[http::field::content_type], "json") ? "json" : "xml";
    }

    long HttpUtils::GetContentLength(const http::request<http::dynamic_body> &request) {

        if (request.has_content_length()) {
            return std::stol(request.base()[http::field::content_length]);
        }
        return 0;
    }

    std::string HttpUtils::GetHost(const http::request<http::dynamic_body> &request) {
        return request.base()[http::field::host];
    }

    bool HttpUtils::IsUrlEncoded(const std::string &value) {
        return !StringUtils::Equals(StringUtils::UrlDecode(value), value);
    }

    void HttpUtils::GetVersionAction(const std::string &uri, std::string &version, std::string &action) {
        if (const std::vector<std::string> parameters = GetPathParameters(uri); !parameters.empty()) {
            version = GetPathParameters(uri)[0];
            action = GetPathParameters(uri)[1];
        }
    }

    std::string HttpUtils::GetBodyAsString(const http::request<http::dynamic_body> &request) {

        boost::beast::net::streambuf sb;
        sb.commit(boost::beast::net::buffer_copy(sb.prepare(request.body().size()), request.body().cdata()));

        return boost::beast::buffers_to_string(sb.data());
    }

    std::string HttpUtils::GetBodyAsString(const http::request<http::string_body> &request) {

        return request.body();
    }

    std::string HttpUtils::GetBodyAsString(const http::request<request_body_t, http::basic_fields<alloc_t>> &request) {

        return request.body();
    }

    std::string HttpUtils::GetBodyAsString(const http::response<http::dynamic_body> &response) {

        boost::beast::net::streambuf sb;
        sb.commit(boost::beast::net::buffer_copy(sb.prepare(response.body().size()), response.body().cdata()));

        return boost::beast::buffers_to_string(sb.data());
    }

    std::string HttpUtils::GetBodyAsString(const http::response<http::string_body> &response) {

        return response.body();
    }

    http::response<http::dynamic_body> HttpUtils::Ok(const http::request<http::dynamic_body> &request) {

        http::response<http::dynamic_body> response{http::status::ok, request.version()};
        response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        response.set(http::field::content_type, "text/html");
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "cache-control,content-type,x-amz-target,x-amz-user-agent");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");

        return response;
    }

    http::response<http::string_body> HttpUtils::Ok(const http::request<http::string_body> &request) {

        http::response<http::string_body> response{http::status::ok, request.version()};
        response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        response.set(http::field::content_type, "text/html");
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "cache-control,content-type,x-amz-target,x-amz-user-agent");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");

        return response;
    }

    http::response<http::dynamic_body> HttpUtils::BadRequest(const http::request<http::dynamic_body> &request, const std::string &reason) {

        http::response<http::dynamic_body> response{http::status::bad_request, request.version()};
        response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        response.set(http::field::content_type, "text/html");
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "cache-control,content-type,x-amz-target,x-amz-user-agent");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");

        ostream(response.body()) << reason;
        response.prepare_payload();
        return response;
    }

    http::response<http::dynamic_body> HttpUtils::BadRequest(const http::request<request_body_t, http::basic_fields<alloc_t>> &request, const std::string &reason) {

        http::response<http::dynamic_body> response{http::status::bad_request, request.version()};
        response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        response.set(http::field::content_type, "text/html");
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "cache-control,content-type,x-amz-target,x-amz-user-agent");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");

        ostream(response.body()) << reason;
        response.prepare_payload();
        return response;
    }

    //

    http::response<http::string_body> HttpUtils::BadRequest(const http::request<http::string_body> &request, const std::string &reason) {

        http::response<http::string_body> response{http::status::bad_request, request.version()};
        response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        response.set(http::field::content_type, "text/html");
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "cache-control,content-type,x-amz-target,x-amz-user-agent");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");

        response.body() = reason;
        response.prepare_payload();
        return response;
    }

    http::response<http::dynamic_body> HttpUtils::Unauthorized(const http::request<http::dynamic_body> &request, const std::string &reason) {

        http::response<http::dynamic_body> response{http::status::unauthorized, request.version()};
        response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        response.set(http::field::content_type, "text/html");
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "cache-control,content-type,x-amz-target,x-amz-user-agent");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");

        boost::beast::ostream(response.body()) << reason;
        response.prepare_payload();
        return response;
    }

    http::response<http::dynamic_body> HttpUtils::InternalServerError(const http::request<http::dynamic_body> &request, const std::string &reason) {

        http::response<http::dynamic_body> response{http::status::internal_server_error, request.version()};
        response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        response.set(http::field::content_type, "text/html");
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "cache-control,content-type,x-amz-target,x-amz-user-agent");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");

        boost::beast::ostream(response.body()) << reason;
        response.prepare_payload();
        return response;
    }

    http::response<http::dynamic_body> HttpUtils::NotImplemented(const http::request<http::dynamic_body> &request, const std::string &reason) {

        http::response<http::dynamic_body> response{http::status::not_implemented, request.version()};
        response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        response.set(http::field::content_type, "text/html");
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "cache-control,content-type,x-amz-target,x-amz-user-agent");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");

        boost::beast::ostream(response.body()) << reason;
        response.prepare_payload();
        return response;
    }

    http::response<http::string_body> HttpUtils::NotImplemented(const http::request<http::string_body> &request, const std::string &reason) {

        http::response<http::string_body> response{http::status::not_implemented, request.version()};
        response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        response.set(http::field::content_type, "text/html");
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "cache-control,content-type,x-amz-target,x-amz-user-agent");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");

        response.body() = reason;
        response.prepare_payload();
        return response;
    }

}// namespace AwsMock::Core