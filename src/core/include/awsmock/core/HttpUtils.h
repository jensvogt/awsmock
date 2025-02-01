//
// Created by vogje01 on 09/09/2023.
//

#ifndef AWS_MOCK_CORE_HTTP_UTILS_H
#define AWS_MOCK_CORE_HTTP_UTILS_H

// Standard C++ includes
#include <iostream>
#include <string>
#include <vector>

// Boost includes
#include <boost/beast.hpp>

// AwsMock includes
#include <awsmock/core/FieldAlloc.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/HttpSocketResponse.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/exception/ServiceException.h>

constexpr char SEPARATOR[] = "===============================================================================";

namespace AwsMock::Core {

    namespace http = boost::beast::http;
    using alloc_t = fields_alloc<char>;

    using request_body_t = http::string_body;

    /**
     * @brief HTTP utilities.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class HttpUtils {

      public:

        /**
         * @brief Returns the base path of the URI.
         *
         * @brief <p>Removes leading '/'.</p>
         *
         * @param uri HTTP request URI
         * @return base path of the HTTP request URI
         */
        static std::string GetBasePath(const std::string &uri);

        /**
         * @brief Returns a given path parameter by index.
         *
         * @param uri HTTP request URI
         * @param index path parameter index
         * @return path parameter with given index.
         */
        static std::string GetPathParameter(const std::string &uri, int index);

        /**
         * @brief Get the rest of the path after index.
         *
         * @param uri HTTP request URI
         * @param index path parameter index
         * @return rest of the path after index
         */
        static std::string GetPathParametersFromIndex(const std::string &uri, int index);

        /**
         * @brief Returns all path parameters.
         *
         * @param uri HTTP request URI
         * @return path parameters vector.
         */
        static std::vector<std::string> GetPathParameters(const std::string &uri);

        /**
         * @brief Checks whether the path parameter at index exists
         *
         * @param uri HTTP request URI
         * @param index path index
         * @return path parameters vector.
         */
        static bool HasPathParameters(const std::string &uri, int index);

        /**
         * @brief Returns the query string
         *
         * @param uri HTTP request URI
         * @return query string of the HTTP request URI
         */
        static std::string GetQueryString(const std::string &uri);

        /**
         * @brief Count parameters
         *
         * @param uri HTTP request URI
         * @return number of query parameters
         */
        static int CountQueryParameters(const std::string &uri);

        /**
         * @brief Count parameters by name prefix
         *
         * @param uri HTTP request URI
         * @param prefix HTTP parameter prefix
         * @return number of query parameters
         */
        static int CountQueryParametersByPrefix(const std::string &uri, const std::string &prefix);

        /**
         * @brief Returns the parameter value by prefix with the given index
         *
         * @param uri HTTP request URI
         * @param prefix HTTP parameter prefix
         * @param index path parameter index
         * @return number of query parameters
         */
        static std::string GetQueryParameterByPrefix(const std::string &uri, const std::string &prefix, int index);

        /**
         * @brief Adds a bool query parameter to the given URL.
         *
         * @param url url to add to
         * @param name parameter name
         * @param value parameter value
         * @return
         */
        static std::string AddQueryParameter(std::string &url, const std::string &name, bool value);

        /**
         * @brief Adds a string query parameter to the given URL.
         *
         * @param url url to add to
         * @param name parameter name
         * @param value parameter value
         * @return
         */
        static std::string AddQueryParameter(std::string &url, const std::string &name, const std::string &value);

        /**
         * @brief Adds a integer query parameter to the given URL.
         *
         * @param url url to add to
         * @param name parameter name
         * @param value parameter value
         * @return
         */
        static std::string AddQueryParameter(std::string &url, const std::string &name, int value);

        /**
         * @brief Returns an integer parameter
         *
         * @param body HTTP body
         * @param name parameter name
         * @param min minimum value
         * @param max maximum value
         * @param def default value
         * @return integer parameter
         */
        static int GetIntParameter(const std::string &body, const std::string &name, int min, int max, int def);

        /**
         * @brief Returns the name of a query parameter.
         *
         * @param parameter parameter in the form name=value
         * @return name of the parameter
         */
        static std::string GetQueryParameterName(const std::string &parameter);

        /**
         * @brief Returns the value of a query parameter.
         *
         * <p> if the value is URL encoded, the value will be decoded.</p>
         *
         * @param parameter parameter in the form name=value
         * @return value of the parameter
         */
        static std::string GetQueryParameterValue(const std::string &parameter);

        /**
         * @brief Returns a map of all query parameters.
         *
         * The resulting name/value pairs are not URL decoded.
         *
         * @param uri HTTP request URI
         * @return map of query parameters
         */
        static std::map<std::string, std::string> GetQueryParameters(const std::string &uri);

        /**
         * @brief Returns a vector of query parameter with the given index.
         *
         * @param uri HTTP request URI
         * @param prefix HTTP parameter prefix
         * @return vector of parameters
         */
        static std::vector<std::string> GetQueryParametersByPrefix(const std::string &uri, const std::string &prefix = {});

        /**
         * @brief Returns the parameter value by prefix with the given index
         *
         * @param uri HTTP request URI
         * @param name HTTP parameter name
         * @return number of query parameters
         */
        static std::string GetQueryParameterValueByName(const std::string &uri, const std::string &name);

        /**
         * @brief Checks for existence of parameter with the given name.
         *
         * @param uri HTTP request URI
         * @param name HTTP parameter name
         * @return true if parameter exists
         */
        static bool HasQueryParameter(const std::string &uri, const std::string &name);

        /**
         * @brief Gets the version and action from an URI.
         *
         * @param uri HTTP request URI
         * @param version AWS request version
         * @param action AWS action
         */
        static void GetVersionAction(const std::string &uri, std::string &version, std::string &action);

        /**
         * @brief Gets the version and action from an URI.
         *
         * @param request HTTP serer request
         * @return HTTP body as string
         */
        static std::string GetBodyAsString(const http::request<http::string_body> &request);

        /**
         * @brief Gets the version and action from an URI.
         *
         * @param request HTTP serer request
         * @return HTTP body as string
         */
        static std::string GetBodyAsString(const http::request<request_body_t, http::basic_fields<alloc_t>> &request);

        /**
         * @brief Gets the body as string from a boost dynamic_body request
         *
         * @param request HTTP serer request
         * @return HTTP body as string
         */
        static std::string GetBodyAsString(const http::request<http::dynamic_body> &request);

        /**
         * @brief Gets the body as string from a boost dynamic_body response
         *
         * @param response HTTP serer response
         * @return HTTP body as string
         */
        static std::string GetBodyAsString(const http::response<http::dynamic_body> &response);

        /**
         * @brief Gets the body as string from a boost string_body response
         *
         * @param response HTTP serer response
         * @return HTTP body as string
         */
        static std::string GetBodyAsString(const http::response<http::string_body> &response);

        /**
         * @brief Checks whether a header exists.
         *
         * @param request HTTP request
         * @param name header key
         * @return header value of empty string.
         */
        static bool HasHeader(const http::request<http::dynamic_body> &request, const std::string &name);

        /**
         * @brief Checks whether a header exists.
         *
         * @param request HTTP request
         * @param name header key
         * @return header value of empty string.
         */
        static bool HasHeader(const http::request<request_body_t, http::basic_fields<alloc_t>> &request, const std::string &name);

        /**
         * @brief Checks whether a header exists.
         *
         * @param request HTTP request
         * @param key header key
         * @return header value of empty string.
         */
        static bool HasHeader(const http::request<http::string_body> &request, const std::string &key);

        /**
         * @brief Checks whether a header exists.
         *
         * @param request HTTP request
         * @param name header key
         * @param index index
         * @return header value of empty string.
         */
        static bool HasHeader(const http::request<http::string_body> &request, const std::string &name, int index);

        /**
         * @brief Checks whether a header exists and has a given value.
         *
         * @param request HTTP request
         * @param name header key
         * @param value header value
         * @return header value of empty string.
         */
        static bool HasHeaderValue(const http::request<http::dynamic_body> &request, const std::string &name, const std::string &value);

        /**
         * @brief Returns a header value by key.
         *
         * <p>
         * Returns the default value, if existent, otherwise logs a warning message, in case the request has no value for the given key.
         * </p>
         *
         * @param request HTTP request
         * @param key header key
         * @param defaultValue returned when the key was not found
         * @return header value of empty string.
         */
        static std::string GetHeaderValue(const http::request<http::dynamic_body> &request, const std::string &key, const std::string &defaultValue = {});

        /**
         * @brief Returns a header value by key.
         *
         * <p>
         * Returns the default value, if existent, otherwise logs a warning message, in case the request has no value for the given key.
         * </p>
         *
         * @param request HTTP request
         * @param key header key
         * @param defaultValue returned when the key was not found
         * @return header value of empty string.
         */
        static std::string GetHeaderValue(const http::request<request_body_t, http::basic_fields<alloc_t>> &request, const std::string &key, const std::string &defaultValue = {});

        /**
         * @brief Returns a header value by key.
         *
         * <p>
         * Returns the default value, if existent, otherwise logs a warning message, in case the request has no value for the given key.
         * </p>
         *
         * @param request HTTP request
         * @param key header key
         * @param defaultValue returned when the key was not found
         * @return header value of empty string.
         */
        static std::string GetHeaderValue(const http::request<http::string_body> &request, const std::string &key, const std::string &defaultValue = {});

        /**
         * @brief Returns the headers as a map of strings
         *
         * @param request HTTP request
         * @return map of strings
         */
        static std::map<std::string, std::string> GetHeaders(const http::request<http::dynamic_body> &request);

        /**
         * @brief Dumps the headers to the logger as info messages
         *
         * @param request HTTP request
         */
        static void DumpHeaders(const http::request<http::dynamic_body> &request);

        /**
         * @brief Dumps the headers to the logger as info messages
         *
         * @param request HTTP request
         */
        static void DumpHeaders(const http::request<http::string_body> &request);

        /**
         * @brief Dumps the headers to the logger as info messages
         *
         * @param response HTTP response
         */
        static void DumpHeaders(const http::response<http::dynamic_body> &response);

        /**
         * @brief Dumps the headers to the logger as info messages
         *
         * @param response HTTP response
         */
        static void DumpHeaders(const http::response<http::string_body> &response);

        /**
         * @brief Dumps the whole request
         *
         * @param request HTTP request
         * @param limit maximum number of bytes to print
         */
        static void DumpRequest(const http::request<http::dynamic_body> &request, int limit = 0);

        /**
         * @brief Dumps the headers to the logger as info messages
         *
         * @param request HTTP request
         */
        static void DumpRequest(const http::request<http::string_body> &request);

        /**
         * @brief Dumps the headers to the logger as info messages
         *
         * @param response HTTP response
         */
        static void DumpResponse(const http::response<http::dynamic_body> &response);

        /**
         * @brief Dumps the headers to the logger as info messages
         *
         * @param response HTTP response
         */
        static void DumpResponse(const http::response<http::string_body> &response);

        /**
         * @brief Returns the content type
         *
         * @param request HTTP request
         * @return reduced content type, either 'json' or 'xml.
         */
        static std::string GetContentType(const http::request<http::dynamic_body> &request);

        /**
         * @brief Returns the content length
         *
         * @param request HTTP request
         * @return content length in bytes
         */
        static long GetContentLength(const http::request<http::dynamic_body> &request);

        /**
         * @brief Returns the host
         *
         * @param request HTTP request
         * @return request host
         */
        static std::string GetHost(const http::request<http::dynamic_body> &request);

        /**
         * @brief Return a simple ok response (200)
         *
         * @param request HTTP request
         * @return HTTP response
         */
        static http::response<http::dynamic_body> Ok(const http::request<http::dynamic_body> &request);

        /**
         * @brief Return a simple ok response (200)
         *
         * @param request HTTP request
         * @return HTTP response
         */
        static http::response<http::string_body> Ok(const http::request<http::string_body> &request);

        /**
         * @brief Return a bad request response (400)
         *
         * @param request HTTP request
         * @param reason reason string
         * @return HTTP response
         */
        static http::response<http::dynamic_body> BadRequest(const http::request<http::dynamic_body> &request, const std::string &reason);

        static http::response<http::dynamic_body> BadRequest(const http::request<request_body_t, http::basic_fields<alloc_t>> &request, const std::string &reason);

        /**
         * @brief Return a bad request response (400)
         *
         * @param request HTTP request
         * @param reason reason string
         * @return HTTP response
         */
        static http::response<http::string_body> BadRequest(const http::request<http::string_body> &request, const std::string &reason);

        /**
         * @brief Return a bad request response (401)
         *
         * @param request HTTP request
         * @param reason reason string
         * @return HTTP response
         */
        static http::response<http::dynamic_body> Unauthorized(const http::request<http::dynamic_body> &request, const std::string &reason);

        /**
         * @brief Return a not implemented response (500)
         *
         * @param request HTTP request
         * @param reason reason string
         * @return HTTP response
         */
        static http::response<http::dynamic_body> InternalServerError(const http::request<http::dynamic_body> &request, const std::string &reason);

        /**
         * @brief Return a not implemented response (501)
         *
         * @param request HTTP request
         * @param reason reason string
         * @return HTTP response
         */
        static http::response<http::dynamic_body> NotImplemented(const http::request<http::dynamic_body> &request, const std::string &reason);

        /**
         * @brief Return a not implemented response (501)
         *
         * @param request HTTP request
         * @param reason reason string
         * @return HTTP response
         */
        static http::response<http::string_body> NotImplemented(const http::request<http::string_body> &request, const std::string &reason);

      private:

        /**
         * @brief Checks whether the query parameter value is URL encoded
         *
         * @param value HTTP query parameter
         * @return true if value is URL encoded
         */
        static bool IsUrlEncoded(const std::string &value);

        /**
         * @brief Add query delimiter
         *
         * @param url HTTP request URL
         * @return URL with delimiter
         */
        static std::string AddQueryDelimiter(std::string &url);
    };

}// namespace AwsMock::Core

#endif// AWS_MOCK_CORE_HTTP_UTILS_H
