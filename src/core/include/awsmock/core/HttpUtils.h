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
#include <boost/url.hpp>

// AwsMock includes
#include <awsmock/core/FieldAlloc.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/HttpSocketResponse.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/core/logging/LogStream.h>

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
         * @brief Returns a given path parameter by index.
         *
         * @param uri HTTP request URI
         * @param index path parameter index
         * @return path parameter with given index.
         */
        static std::string GetPathParameter(const std::string &uri, int index);

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
        static std::string AddQueryParameter(const std::string &url, const std::string &name, bool value);

        /**
         * @brief Adds a string query parameter to the given URL.
         *
         * @param url url to add to
         * @param name parameter name
         * @param value parameter value
         * @return
         */
        static std::string AddQueryParameter(const std::string &url, const std::string &name, const std::string &value);

        /**
         * @brief Adds an integer query parameter to the given URL.
         *
         * @param url url to add to
         * @param name parameter name
         * @param value parameter value
         * @return
         */
        static std::string AddQueryParameter(const std::string &url, const std::string &name, int value);

        /**
         * @brief Returns an integer parameter
         *
         * @par
         * The parameter uri a full URL or a payload, as AWS send for some modules (for instance: SNS) a query string without host and port
         * inside the payload. In this case the uri is appended by 'http://localhost:4566', otherwise boost::urls package cannot handle it.
         *
         * @param uri HTTP uri
         * @param name parameter name
         * @param min minimum value
         * @param max maximum value
         * @param def default value
         * @return integer parameter
         */
        static int GetIntParameter(const std::string &uri, const std::string &name, int min, int max, int def);

        /**
         * @brief Returns a long integer parameter
         *
         * @par
         * The parameter uri a full URL or a payload, as AWS send for some modules (for instance: SNS) a query string without host and port
         * inside the payload. In this case the uri is appended by 'http://localhost:4566', otherwise boost::urls package cannot handle it.
         *
         * @param uri HTTP uri
         * @param name parameter name
         * @param min minimum value
         * @param max maximum value
         * @param def default value
         * @return integer parameter
         */
        static long GetLongParameter(const std::string &uri, const std::string &name, long min, long max, long def);

        /**
         * @brief Returns a string parameter
         *
         * @par
         * The parameter uri a full URL or a payload, as AWS send for some modules (for instance: SNS) a query string without host and port
         * inside the payload. In this case the uri is appended by 'http://localhost:4566', otherwise boost::urls package cannot handle it.
         *
         * @param uri HTTP uri
         * @param name parameter name
         * @param def default value
         * @return integer parameter
         */
        static std::string GetStringParameter(const std::string &uri, const std::string &name, const std::string &def = {});

        /**
         * @brief Returns a boolean parameter
         *
         * @par
         * The parameter uri a full URL or a payload, as AWS send for some modules (for instance: SNS) a query string without host and port
         * inside the payload. In this case the uri is appended by 'http://localhost:4566', otherwise boost::urls package cannot handle it.
         *
         * @param uri HTTP uri
         * @param name parameter name
         * @param def default value
         * @return integer parameter
         */
        static bool GetBoolParameter(const std::string &uri, const std::string &name, const bool &def = false);

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
         * @brief Returns a vector of query parameters with the given index.
         *
         * @param uri HTTP request URI
         * @param prefix HTTP parameter prefix
         * @return vector of parameters
         */
        static std::vector<std::string> GetQueryParametersByPrefix(const std::string &uri, const std::string &prefix = {});

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
         * @brief Gets the body as a string from a boost dynamic_body request
         *
         * @param request HTTP serer request
         * @return HTTP body as string
         */
        static std::string GetBodyAsString(const http::request<http::dynamic_body> &request);

        /**
         * @brief Gets the body as a string from a boost dynamic_body response
         *
         * @param response HTTP serer response
         * @return HTTP body as string
         */
        static std::string GetBodyAsString(const http::response<http::dynamic_body> &response);

        /**
         * @brief Gets the body as a string from a boost string_body response
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
         * @return header value or empty string.
         */
        static bool HasHeader(const http::request<http::dynamic_body> &request, const std::string &name);

        /**
         * @brief Checks whether a header exists.
         *
         * @param request HTTP request
         * @param name header key
         * @return header value or empty string.
         */
        static bool HasHeader(const http::request<request_body_t, http::basic_fields<alloc_t>> &request, const std::string &name);

        /**
         * @brief Checks whether a header exists.
         *
         * @param request HTTP request
         * @param key header key
         * @return header value or empty string.
         */
        static bool HasHeader(const http::request<http::string_body> &request, const std::string &key);

        /**
         * @brief Checks whether a header exists and has a given value.
         *
         * @param request HTTP request
         * @param name header key
         * @param value header value
         * @return header value or empty string.
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
         * @return header value or empty string.
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
         * @return header value or empty string.
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
         * @return header value or empty string.
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

        /**
         * @brief Return a bad request response (400)
         *
         * @param request HTTP request
         * @param reason reason string
         * @return HTTP response
         */
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

        /**
         * @brief Return a not implemented response (501)
         *
         * @param status HTTP status
         * @return HTTP status as string
         */
        static std::string StatusCodeToString(const http::status &status);

        /**
         * @brief Return a not implemented response (501)
         *
         * @param status HTTP status
         * @return HTTP status as string
         */
        static http::status StatusCodeFromString(const std::string &status);

      private:

        /**
         * @brief Checks whether the query parameter value is URL encoded
         *
         * @param value HTTP query parameter
         * @return true if value is URL encoded
         */
        static bool IsUrlEncoded(const std::string &value);
    };

    template<bool isRequest, class SyncReadStream, class DynamicBuffer>
    void DumpChunks(std::ostream &os, SyncReadStream &stream, DynamicBuffer &buffer, boost::beast::error_code &ec) {
        // Declare the parser with an empty body since
        // we plan on capturing the chunks ourselves.
        http::parser<isRequest, http::empty_body> p;

        // First read the complete header
        read_header(stream, buffer, p, ec);
        if (ec)
            return;

        // This container will hold the extensions for each chunk
        http::chunk_extensions ce;

        // This string will hold the body of each chunk
        std::string chunk;

        // Declare our chunk header callback  This is invoked
        // after each chunk header and also after the last chunk.
        auto header_cb = [&](const std::uint64_t size, const boost::string_view extensions, boost::beast::error_code &ev)// We can set this to indicate an error
        {
            // Parse the chunk extensions so we can access them easily
            ce.parse(extensions, ev);
            if (ev)
                return;

            // See if the chunk is too big
            if (size > (std::numeric_limits<std::size_t>::max)()) {
                ev = http::error::body_limit;
                return;
            }

            // Make sure we have enough storage, and
            // reset the container for the upcoming chunk
            chunk.reserve(size);
            chunk.clear();
        };

        // Set the callback. The function requires a non-const reference so we
        // use a local variable, since temporaries can only bind to const refs.
        p.on_chunk_header(header_cb);

        // Declare the chunk body callback. This is called one or
        // more times for each piece of a chunk body.
        auto body_cb = [&](const std::uint64_t remain, const boost::string_view body, boost::beast::error_code &ev)// We can set this to indicate an error
        {
            // If this is the last piece of the chunk body,
            // set the error so that the call to `read` returns,
            // and we can process the chunk.
            if (remain == body.size())
                ev = http::error::end_of_chunk;

            // Append this piece to our container
            chunk.append(body.data(), body.size());

            // The return value informs the parser of how much of the body we
            // consumed. We will indicate that we consumed everything passed in.
            return body.size();
        };
        p.on_chunk_body(body_cb);

        while (!p.is_done()) {
            // Read as much as we can. When we reach the end of the chunk, the chunk
            // body callback will make the read return with the end_of_chunk error.
            read(stream, buffer, p, ec);
            if (!ec)
                continue;
            if (ec != http::error::end_of_chunk)
                return;
            ec = {};

            // We got a whole chunk, print the extensions:
            for (const auto &[fst, snd]: ce) {
                os << "Extension: " << fst;
                if (!snd.empty())
                    os << " = " << snd << std::endl;
                else
                    os << std::endl;
            }

            // Now print the chunk body
            os << "Chunk Body: " << chunk << std::endl;
        }

        // Get a reference to the parsed message, this is for convenience
        auto const &msg = p.get();

        // Check each field promised in the "Trailer" header and output it
        for (auto const &name: http::token_list{msg[http::field::trailer]}) {
            // Find the trailer field
            auto it = msg.find(name);
            if (it == msg.end()) {
                // Oops! They promised the field but failed to deliver it
                os << "Missing Trailer: " << name << std::endl;
                continue;
            }
            os << it->name() << ": " << it->value() << std::endl;
        }
    }
}// namespace AwsMock::Core

#endif// AWS_MOCK_CORE_HTTP_UTILS_H
