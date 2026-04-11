//
// Created by jensv on 10/04/2026.
//

#ifndef AWSMOCK_CORE_BSON_CONVERTER_H
#define AWSMOCK_CORE_BSON_CONVERTER_H

#pragma once

#include <boost/json.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/element.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/array/view.hpp>
#include <stdexcept>
#include <string>

namespace AwsMock::Core {

    // Trait that is true for both bsoncxx element types.
    template<typename T>
    struct is_bson_element : std::false_type {
    };

    template<>
    struct is_bson_element<bsoncxx::document::element> : std::true_type {
    };

    template<>
    struct is_bson_element<bsoncxx::array::element> : std::true_type {
    };

    // ---------------------------------------------------------------------------
    // BsonToJson
    //
    // A stateless converter that turns a bsoncxx document/array view into a
    // boost::json::value.  The primary entry-points are:
    //
    //   boost::json::value v = BsonToJson::convert(doc_view);
    //   boost::json::value v = BsonToJson::convert(array_view);
    //
    // Individual BSON elements can also be converted:
    //
    //   boost::json::value v = BsonToJson::convert(element);
    // ---------------------------------------------------------------------------
    class BsonToJson {
    public:
        // -----------------------------------------------------------------------
        // Convert a BSON document view to a boost::json::object
        // -----------------------------------------------------------------------
        static boost::json::value convert(const bsoncxx::document::view doc) {
            boost::json::object obj;
            for (const auto &element: doc) {
                std::string key(element.key());
                obj.emplace(key, convertElement(element));
            }
            return obj;
        }

        // -----------------------------------------------------------------------
        // Convert a BSON array view to a boost::json::array
        // -----------------------------------------------------------------------
        static boost::json::value convert(const bsoncxx::array::view arr) {
            boost::json::array json_arr;
            for (const auto &element: arr) {
                json_arr.push_back(convertElement(element));
            }
            return json_arr;
        }

        // -----------------------------------------------------------------------
        // Convert a single BSON element to a boost::json::value.
        //
        // Accepts both bsoncxx::document::element and bsoncxx::array::element via
        // a constrained template — the two types are not related by inheritance
        // inside bsoncxx so a single concrete overload cannot cover both.
        // -----------------------------------------------------------------------
        template<typename Element, typename = std::enable_if_t<is_bson_element<Element>::value> >
        static boost::json::value convertElement(const Element &element) {
            using bsoncxx::type;

            switch (element.type()) {
                // --- Primitives ------------------------------------------------
                case type::k_double:
                    return boost::json::value(element.get_double().value);

                case type::k_int32:
                    return boost::json::value(
                        static_cast<std::int64_t>(element.get_int32().value));

                case type::k_int64:
                    return boost::json::value(element.get_int64().value);

                case type::k_bool:
                    return boost::json::value(element.get_bool().value);

                case type::k_null:
                    return {nullptr};

                // --- Strings ---------------------------------------------------
                case type::k_string: {
                    auto sv = element.get_string().value;
                    return boost::json::value(std::string(sv));
                }

                // --- Nested documents / arrays ---------------------------------
                case type::k_document:
                    return convert(element.get_document().value);

                case type::k_array:
                    return convert(element.get_array().value);

                // --- ObjectId → hex string ------------------------------------
                case type::k_oid: {
                    return boost::json::value(element.get_oid().value.to_string());
                }

                // --- Date → milliseconds since epoch (integer) ----------------
                case type::k_date: {
                    auto ms = element.get_date().value.count();
                    return boost::json::value(ms);
                }

                // --- Timestamp (BSON internal) → {t, i} object ----------------
                case type::k_timestamp: {
                    auto ts = element.get_timestamp();
                    boost::json::object obj;
                    obj["t"] = static_cast<std::int64_t>(ts.increment);
                    obj["i"] = static_cast<std::int64_t>(ts.timestamp);
                    return obj;
                }

                // --- Binary → { $binary, $subtype } object --------------------
                case type::k_binary: {
                    auto bin = element.get_binary();
                    auto *bytes = reinterpret_cast<const unsigned char *>(bin.bytes);
                    std::string encoded = base64Encode(bytes, bin.size);

                    boost::json::object obj;
                    obj["$binary"] = encoded;
                    obj["$subtype"] = static_cast<int>(bin.sub_type);
                    return obj;
                }

                // --- Regex → { $regex, $options } object ----------------------
                case type::k_regex: {
                    auto rx = element.get_regex();
                    boost::json::object obj;
                    obj["$regex"] = std::string(rx.regex);
                    obj["$options"] = std::string(rx.options);
                    return obj;
                }

                // --- Code → string --------------------------------------------
                case type::k_code: {
                    auto sv = element.get_code().code;
                    return boost::json::value(std::string(sv));
                }

                // --- Symbol → string ------------------------------------------
                case type::k_symbol: {
                    auto sv = element.get_symbol().symbol;
                    return boost::json::value(std::string(sv));
                }

                // --- Decimal128 → string representation ----------------------
                case type::k_decimal128: {
                    return boost::json::value(
                        element.get_decimal128().value.to_string());
                }

                // --- Undefined / MinKey / MaxKey → descriptive string ---------
                case type::k_undefined:
                    return {"$undefined"};

                case type::k_minkey:
                    return {"$minKey"};

                case type::k_maxkey:
                    return {"$maxKey"};

                // --- DBPointer → { $ref, $id } object -------------------------
                case type::k_dbpointer: {
                    auto dbp = element.get_dbpointer();
                    boost::json::object obj;
                    obj["$ref"] = std::string(dbp.collection);
                    obj["$id"] = dbp.value.to_string();
                    return obj;
                }

                // --- Anything else: throw rather than silently drop -----------
                default:
                    throw std::runtime_error(
                        std::string("BsonToJson: unsupported BSON type: ") +
                        bsoncxx::to_string(element.type()));
            }
        }

    private:
        // -----------------------------------------------------------------------
        // Minimal Base64 encoder (no external dependency required)
        // -----------------------------------------------------------------------
        static std::string base64Encode(const unsigned char *data, std::size_t len) {
            static constexpr char kTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

            std::string out;
            out.reserve(((len + 2) / 3) * 4);

            for (std::size_t i = 0; i < len; i += 3) {
                unsigned int b = static_cast<unsigned int>(data[i]) << 16;
                if (i + 1 < len) b |= static_cast<unsigned int>(data[i + 1]) << 8;
                if (i + 2 < len) b |= static_cast<unsigned int>(data[i + 2]);

                out.push_back(kTable[(b >> 18) & 0x3F]);
                out.push_back(kTable[(b >> 12) & 0x3F]);
                out.push_back(i + 1 < len ? kTable[(b >> 6) & 0x3F] : '=');
                out.push_back(i + 2 < len ? kTable[b & 0x3F] : '=');
            }
            return out;
        }
    };

} // namespace Awsmock::Core

#endif // AWSMOCK_CORE_BSON_CONVERTER_H
