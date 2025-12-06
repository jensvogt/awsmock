//
// Created by vogje01 on 11/17/24.
//

#ifndef AWS_MOCK_CORE_BSON_UTILS_H
#define AWS_MOCK_CORE_BSON_UTILS_H

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifdef _WIN32
#define BOOST_ASIO_NO_WIN32_LEAN_AND_MEAN
#include <boost/asio.hpp>
#include <windows.h>
#endif
#endif

// C++ includes
#include <chrono>
#include <map>

// Bson includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/string/to_string.hpp>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/exception/JsonException.h>

using bsoncxx::view_or_value;
using bsoncxx::builder::basic::array;
using bsoncxx::builder::basic::document;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using bsoncxx::document::value;
using bsoncxx::document::view;
using std::chrono::system_clock;

namespace AwsMock::Core::Bson {

    inline bool FindBsonObject(const view &value, const std::string &name) {
        return value.find(name) != value.end();
    }

    template<class Element>
    void ToBsonArray(document &d, const std::string &name, std::vector<Element> a) {
        if (!a.empty()) {
            array jsonArray;
            for (const auto &e: a) {
                jsonArray.append(bsoncxx::types::b_document(e.ToDocument()));
            }
            d.append(kvp(name, bsoncxx::types::b_array(jsonArray)));
        }
    }

    template<class Element>
    void FromBsonArray(const view &value, const std::string &name, std::vector<Element> *a) {
        if (value.find(name) != value.end()) {
            for (const bsoncxx::array::view arrayView{value[name].get_array().value}; const bsoncxx::array::element &arrayElement: arrayView) {
                Element element;
                element.FromDocument(arrayElement.get_document().view());
                a->emplace_back(element);
            }
        }
    }

    template<class Element>
    void FromBsonArray(const value &value, const std::string &name, std::vector<Element> *a) {
        if (value.find(name) != value.end()) {
            for (const bsoncxx::array::view arrayView{value[name].get_array().value}; const bsoncxx::array::element &arrayElement: arrayView) {
                Element element;
                element.FromDocument(arrayElement.get_document().view());
                a->emplace_back(element);
            }
        }
    }

    inline void FromBsonStringArray(const value &value, const std::string &name, std::vector<std::string> *a) {
        if (value.find(name) != value.end()) {
            for (const bsoncxx::array::view arrayView{value[name].get_array().value}; const bsoncxx::array::element &arrayElement: arrayView) {
                a->emplace_back(arrayElement.get_string().value);
            }
        }
    }

    inline void FromBsonArray(const view &value, const std::string &name, std::vector<std::string> *a) {
        if (value.find(name) != value.end()) {
            for (const bsoncxx::array::view arrayView{value[name].get_array().value}; const bsoncxx::array::element &arrayElement: arrayView) {
                a->emplace_back(arrayElement.get_string().value);
            }
        }
    }

    template<class Object>
    Object FromBsonObject(const value &value, const std::string &name, Object *object) {
        if (FindBsonObject(value, name)) {
            return object->FromDocument(value[name].get_document().view());
        }
        return {};
    }

    inline std::map<std::string, std::string> MapFromBsonObject(const std::optional<view> &viewDocument, const std::string &name) {
        if (FindBsonObject(viewDocument.value(), name)) {
            std::map<std::string, std::string> valueMap;
            for (const view tagsView = viewDocument.value()[name].get_document().value; const bsoncxx::document::element
                                                                                                &tagElement: tagsView) {
                std::string key = bsoncxx::string::to_string(tagElement.key());
                std::string value = bsoncxx::string::to_string(tagsView[key].get_string().value);
                valueMap.emplace(key, value);
            }
            return valueMap;
        }
        return {};
    }

    struct BsonUtils {
        static void SetOidValue(document &document, const std::string &name, const std::string &value) {
            if (!value.empty()) {
                document.append(kvp(name, bsoncxx::oid(value)));
            }
        }
        static void SetStringValue(document &document, const std::string &name, const std::string &value) {
            if (!value.empty()) {
                document.append(kvp(name, value));
            }
        }

        static void SetIntValue(document &document, const std::string &name, const int value) {
            document.append(kvp(name, static_cast<bsoncxx::types::b_int32>(value)));
        }

        static void SetLongValue(document &document, const std::string &name, const long value) {
            document.append(kvp(name, static_cast<bsoncxx::types::b_int64>(value)));
        }

        static void SetBoolValue(document &document, const std::string &name, const bool value) {
            document.append(kvp(name, static_cast<bsoncxx::types::b_bool>(value)));
        }

        static void SetDateValue(document &document, const std::string &name, const system_clock::time_point &value) {
            document.append(kvp(name, bsoncxx::types::b_date(value)));
        }

        static void SetDocumentValue(document &rootDocument, const std::string &name, document &value) {
            rootDocument.append(kvp(name, value));
        }

        static void SetDocumentValue(document &rootDocument, const std::string &name, const view_or_value<view, value> &value) {
            rootDocument.append(kvp(name, value));
        }

        static void SetArrayValue(document &rootDocument, const std::string &name, array &value) {
            rootDocument.append(kvp(name, value));
        }

        static std::string GetOidValue(const std::optional<view> &view, const std::string &name) {
            if (view->find(name) != view->end()) {
                return GetOidValue(view.value()[name]);
            }
            return {};
        }

        static std::string GetOidValue(const bsoncxx::document::element &element) {
            switch (element.type()) {
                case bsoncxx::type::k_null:
                    return {};
                case bsoncxx::type::k_oid:
                    return element.get_oid().value.to_string();
                default:
                    break;
            }
            return {};
        }

        static long GetLongValue(const std::optional<view> &view, const std::string &name) {
            if (view->find(name) != view->end()) {
                return GetLongValue(view.value()[name]);
            }
            return {};
        }

        static long GetLongValue(const value &value, const std::string &name) {
            if (value.find(name) != value.end()) {
                return GetLongValue(value[name]);
            }
            return {};
        }

        static long GetLongValue(const bsoncxx::document::element &element) {
            if (!element) {
                return 0;
            }
            switch (element.type()) {
                case bsoncxx::type::k_int32:
                    return element.get_int32().value;
                case bsoncxx::type::k_int64:
                    return static_cast<long>(element.get_int64().value);
                case bsoncxx::type::k_string:
                    return std::stol(bsoncxx::string::to_string(element.get_string().value));
                case bsoncxx::type::k_null:
                    return 0;
                default:
                    break;
            }
            return 0;
        }

        static long long GetLongLongValue(const std::optional<view> &view, const std::string &name) {
            if (view->find(name) != view->end()) {
                return GetLongLongValue(view.value()[name]);
            }
            return {};
        }

        static long long GetLongLongValue(const value &value, const std::string &name) {
            if (value.find(name) != value.end()) {
                return GetLongLongValue(value[name]);
            }
            return {};
        }

        static long long GetLongLongValue(const bsoncxx::document::element &element) {
            if (!element) {
                return 0;
            }
            switch (element.type()) {
                case bsoncxx::type::k_int32:
                    return element.get_int32().value;
                case bsoncxx::type::k_int64:
                    return element.get_int64().value;
                case bsoncxx::type::k_string:
                    return std::stol(bsoncxx::string::to_string(element.get_string().value));
                case bsoncxx::type::k_null:
                    return 0;
                default:
                    break;
            }
            return 0;
        }

        static int GetIntValue(const std::optional<view> &view, const std::string &name) {
            if (view->find(name) != view->end()) {
                return GetIntValue(view.value()[name]);
            }
            return {};
        }

        static int GetIntValue(const value &value, const std::string &name) {
            if (value.find(name) != value.end()) {
                return GetIntValue(value[name]);
            }
            return {};
        }

        static int GetIntValue(const bsoncxx::document::element &element) {
            if (!element) {
                return 0;
            }
            switch (element.type()) {
                case bsoncxx::type::k_int32:
                    return element.get_int32().value;
                case bsoncxx::type::k_int64:
                    return static_cast<int>(element.get_int64().value);
                case bsoncxx::type::k_string:
                    return std::stoi(bsoncxx::string::to_string(element.get_string().value));
                case bsoncxx::type::k_null:
                    return 0;
                default:
                    break;
            }
            return 0;
        }

        static double GetDoubleValue(const std::optional<view> &view, const std::string &name) {
            if (view.value().find(name) != view.value().end()) {
                return GetDoubleValue(view.value()[name]);
            }
            return {};
        }

        static double GetDoubleValue(const bsoncxx::document::element &element) {
            if (!element) {
                return 0;
            }
            switch (element.type()) {
                case bsoncxx::type::k_null:
                    return 0.0;
                case bsoncxx::type::k_double:
                    return element.get_double().value;
                default:
                    break;
            }
            return 0;
        }

        static std::string GetStringValue(const std::optional<view> &view, const std::string &name) {
            if (view->find(name) != view->end()) {
                return GetStringValue(view.value()[name]);
            }
            return {};
        }

        static std::string GetStringValue(const value &value, const std::string &name) {
            if (value.find(name) != value.end()) {
                return GetStringValue(value[name]);
            }
            return {};
        }

        static std::string GetStringValue(const bsoncxx::document::element &element) {
            if (!element) {
                return {};
            }

            switch (element.type()) {
                case bsoncxx::type::k_null:
                    return {};
                case bsoncxx::type::k_string:
                    return bsoncxx::string::to_string(element.get_string().value);
                default:
                    break;
            }
            return {};
        }

        static std::string GetStringValue(const bsoncxx::array::element &element, const std::string &name) {
            return GetStringValue(element[name]);
        }

        static bool GetBoolValue(const std::optional<view> &view, const std::string &name) {
            if (view->find(name) != view->end()) {
                return GetBoolValue(view.value()[name]);
            }
            return {};
        }

        static bool GetBoolValue(const bsoncxx::document::element &element) {
            if (!element) {
                return false;
            }

            switch (element.type()) {
                case bsoncxx::type::k_null:
                    return false;
                case bsoncxx::type::k_bool:
                    return element.get_bool().value;
                default:
                    break;
            }
            return {};
        }

        static system_clock::time_point GetDateValue(const std::optional<view> &view, const std::string &name) {
            if (view->find(name) != view->end()) {
                return GetDateValue(view.value()[name]);
            }
            return {};
        }

        static system_clock::time_point GetDateValue(const bsoncxx::document::element &element) {
            if (!element) {
                return {};
            }

            switch (element.type()) {
                case bsoncxx::type::k_null:
                    return system_clock::now();
                case bsoncxx::type::k_date:
                    return bsoncxx::types::b_date(element.get_date());
                case bsoncxx::type::k_timestamp:
                    return std::chrono::time_point<system_clock, std::chrono::milliseconds>{
                            std::chrono::milliseconds{element.get_timestamp().timestamp}};
                default:
                    break;
            }
            return {};
        }

        static std::string ToJsonString(const view &document) {
            return bsoncxx::to_json(document);
        }

        static std::string ToJsonString(const array &array) {
            return bsoncxx::to_json(array);
        }
    };
}// namespace AwsMock::Core::Bson

#endif// AWS_MOCK_CORE_BSON_UTILS_H
