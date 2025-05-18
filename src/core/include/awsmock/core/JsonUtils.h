//
// Created by vogje01 on 11/17/24.
//

#ifndef AWS_MOCK_CORE_JSON_UTILS_H
#define AWS_MOCK_CORE_JSON_UTILS_H

// C++ includes
#include <chrono>
#include <map>

// Boost JSON includes
#include <boost/json.hpp>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/JsonException.h>

using std::chrono::system_clock;

namespace AwsMock::Core::Json {

    inline boost::json::value ParseJsonString(const std::string &jsonString) {
        boost::system::error_code ec;
        boost::json::value result = boost::json::parse(jsonString, ec);
        if (ec) {
            throw JsonException("JSON exception, error: " + ec.message());
        }
        return result;
    }

    inline bool AttributeExists(const boost::json::value &value, const std::string &name) {
        return value.as_object().if_contains(name);
    }

    inline std::string GetStringValue(const boost::json::value &value, const std::string &name) {
        if (AttributeExists(value, name)) {
            return value.at(name).as_string().data();
        }
        return {};
    }

    inline long GetLongValue(const boost::json::value &value, const std::string &name, const long defaultValue = 0) {
        if (AttributeExists(value, name)) {
            return value.at(name).as_int64();
        }
        return defaultValue;
    }

    inline int GetIntValue(const boost::json::value &value, const std::string &name, const int defaultValue = 0) {
        return static_cast<int>(GetLongValue(value, name, defaultValue));
    }

    inline bool GetBoolValue(const boost::json::value &value, const std::string &name) {
        if (AttributeExists(value, name)) {
            return value.at("name").as_bool();
        }
        return false;
    }

    inline float GetFloatValue(const boost::json::value &value, const std::string &name) {
        if (AttributeExists(value, name)) {
            return static_cast<float>(value.at("name").as_double());
        }
        return 0.0;
    }

    inline double GetDoubleValue(const boost::json::value &value, const std::string &name) {
        if (AttributeExists(value, name)) {
            return value.at("name").as_double();
        }
        return 0.0;
    }

    inline std::vector<std::string> GetStringArrayValue(const boost::json::value &value, const std::string &name, const std::string &defaultValue = {}) {
        if (AttributeExists(value, name)) {
            return boost::json::value_to<std::vector<std::string>>(value.at(name));
        }
        return {defaultValue};
    }

    inline bool findObject(boost::json::value &value, const std::string &name) {
        return value.as_object().if_contains(name);
    }
}// namespace AwsMock::Core::Json

#endif// AWS_MOCK_CORE_JSON_UTILS_H
