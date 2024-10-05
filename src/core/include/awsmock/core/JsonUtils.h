//
// Created by vogje01 on 22/08/2022.
//

#ifndef AWS_MOCK_CORE_JSON_UTILS_H
#define AWS_MOCK_CORE_JSON_UTILS_H

// Standard C++ includes
#include <string>

// Poco includes
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>

// Boost includes
#include <boost/date_time/posix_time/posix_time.hpp>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>

#define JSON_DEFAULT_INDENT 4

namespace AwsMock::Core {

    using namespace boost::posix_time;

    /**
     * @brief JSON utilities.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class JsonUtils {

      public:

        /**
         * @brief Extracts the supplied JSON value from the object.
         *
         * @param name name of the JSON attribute
         * @param jsonObject JSON parentObject
         * @param attribute JSON attribute
         */
        static void GetJsonValueString(const std::string &name, Poco::JSON::Object::Ptr jsonObject, std::string &attribute);

        /**
         * @brief Sets the supplied JSON value in the object.
         *
         * @param jsonObject JSON parentObject
         * @param name name of the JSON attribute
         * @param value JSON attribute
         */
        static void SetJsonValueString(Poco::JSON::Object &jsonObject, const std::string &name, const std::string &value);

        /**
         * @brief Extracts the supplied JSON value from the object.
         *
         * @param name name of the JSON attribute
         * @param jsonObject JSON parentObject
         * @param attribute JSON attribute
         */
        static void GetJsonValueLong(const std::string &name, Poco::JSON::Object::Ptr jsonObject, long &attribute);

        /**
         * @brief Sets the supplied JSON value in the object.
         *
         * @param jsonObject JSON parentObject
         * @param name name of the JSON attribute
         * @param value JSON attribute
         */
        static void SetJsonValueLong(Poco::JSON::Object &jsonObject, const std::string &name, long value);

        /**
         * @brief Extracts the supplied JSON value from the object.
         *
         * @param name name of the JSON attribute
         * @param jsonObject JSON parentObject
         * @param attribute JSON attribute
         */
        static void GetJsonValueDouble(const std::string &name, Poco::JSON::Object::Ptr jsonObject, double &attribute);

        /**
         * @brief Sets the supplied JSON value in the object.
         *
         * @param jsonObject JSON parentObject
         * @param name name of the JSON attribute
         * @param value JSON attribute
         */
        static void SetJsonValueDouble(Poco::JSON::Object &jsonObject, const std::string &name, double value);

        /**
         * @brief Extracts the supplied JSON value from the object.
         *
         * @param name name of the JSON attribute
         * @param jsonObject JSON parentObject
         * @param attribute JSON attribute
         */
        static void GetJsonValueFloat(const std::string &name, Poco::JSON::Object::Ptr jsonObject, float &attribute);

        /**
         * @brief Sets the supplied JSON value in the object.
         *
         * @param jsonObject JSON parentObject
         * @param name name of the JSON attribute
         * @param value JSON attribute
         */
        static void SetJsonValueFloat(Poco::JSON::Object &jsonObject, const std::string &name, float value);

        /**
         * @brief Extracts the supplied JSON value from the object.
         *
         * @param name name of the JSON attribute
         * @param jsonObject JSON parentObject
         * @param attribute JSON attribute
         */
        static void GetJsonValueInt(const std::string &name, Poco::JSON::Object::Ptr jsonObject, int &attribute);

        /**
         * @brief Sets the supplied JSON value int the object.
         *
         * @param jsonObject JSON parentObject
         * @param name name of the JSON attribute
         * @param value JSON attribute
         */
        static void SetJsonValueInt(Poco::JSON::Object &jsonObject, const std::string &name, int value);

        /**
         * @brief Extracts the supplied JSON value from the object.
         *
         * @param name name of the JSON attribute
         * @param jsonObject JSON parentObject
         * @param attribute JSON attribute
         */
        static void GetJsonValueBool(const std::string &name, Poco::JSON::Object::Ptr jsonObject, bool &attribute);

        /**
         * @brief Sets the supplied JSON value in the object.
         *
         * @param jsonObject JSON parentObject
         * @param name name of the JSON attribute
         * @param value JSON attribute
         */
        static void SetJsonValueBool(Poco::JSON::Object &jsonObject, const std::string &name, bool value);

        /**
         * @brief Extracts the supplied JSON value from the object.
         *
         * @param name name of the JSON attribute
         * @param jsonObject JSON parentObject
         * @param attribute JSON attribute
         */
        static void GetJsonValueDate(const std::string &name, Poco::JSON::Object::Ptr jsonObject, Poco::DateTime &attribute);

        /**
         * @brief Extracts the supplied JSON value from the object.
         *
         * @param name name of the JSON attribute
         * @param jsonObject JSON object
         * @param attribute JSON attribute
         */
        static void GetJsonValueDate(const std::string &name, Poco::JSON::Object::Ptr jsonObject, std::chrono::system_clock::time_point &attribute);

        /**
         * @brief Sets the supplied JSON value int the object.
         *
         * @param jsonObject JSON parentObject
         * @param name name of the JSON attribute
         * @param value JSON attribute
         */
        static void SetJsonValueDate(Poco::JSON::Object &jsonObject, const std::string &name, const std::chrono::system_clock::time_point &value);

        /**
         * @brief Creates a JSON string array.
         *
         * @param values vector of string values
         * @return JSON string array
         */
        static Poco::JSON::Array GetJsonStringArray(const std::vector<std::string> &values);

        /**
         * @brief Creates a JSON object from a string map.
         *
         * <p>
         * Each key, value pair will be converted to a attribute with name key and value=value.
         * </p>
         *
         * Example map<std::string, std::string>:
         * @code{.json}
         * {
         *  "map.first": "map.second"
         * }
         * @endcode
         *
         * @param values vector of string values
         * @return JSON object
         */
        static Poco::JSON::Object GetJsonObject(const std::map<std::string, std::string> &values);

        /**
         * @brief Returns the JSON string for the document.
         *
         * @param jsonObject JSON object
         * @param prettyPrint use indentation
         * @param indent indentation size
         * @return JSON string
         */
        static std::string ToJsonString(const Poco::JSON::Object &jsonObject, bool prettyPrint = false, int indent = JSON_DEFAULT_INDENT);

        /**
         * @brief Returns the JSON string for the array.
         *
         * @param jsonArray JSON array
         * @param prettyPrint use indentation
         * @param indent indentation size
         * @return JSON string
         */
        static std::string ToJsonString(const Poco::JSON::Array &jsonArray, bool prettyPrint = false, int indent = JSON_DEFAULT_INDENT);
    };

}// namespace AwsMock::Core

#endif// AWS_MOCK_CORE_JSON_UTILS_H
