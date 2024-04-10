//
// Created by vogje01 on 3/23/24.
//

#ifndef AWSMOCK_DTO_COMMON_HTTPMETHOD_H
#define AWSMOCK_DTO_COMMON_HTTPMETHOD_H

// C++ includes
#include <string>
#include <map>

// AwsMock includes
#include <awsmock/core/StringUtils.h>

namespace AwsMock::Dto::Common {

  enum class HttpMethod {
    GET,
    PUT,
    POST,
    DELETE,
    UNKNOWN
  };

  static std::map<HttpMethod, std::string> HttpMethodNames{
    {HttpMethod::GET, "GET"},
    {HttpMethod::PUT, "PUT"},
    {HttpMethod::POST, "POST"},
    {HttpMethod::DELETE, "DELETE"},
  };

  [[maybe_unused]]static std::string HttpMethodToString(HttpMethod requestType) {
    return HttpMethodNames[requestType];
  }

  [[maybe_unused]]static HttpMethod HttpMethodFromString(const std::string &requestType) {
    for (auto &it : HttpMethodNames) {
      if (Core::StringUtils::StartsWith(requestType, it.second)) {
        return it.first;
      }
    }
    return HttpMethod::UNKNOWN;
  }

}

#endif // AWSMOCK_DTO_COMMON_HTTPMETHOD_H