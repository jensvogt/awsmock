//
// Created by vogje01 on 07/06/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_ITEM_H
#define AWSMOCK_DTO_DYNAMODB_ITEM_H

// C++ includes
#include <chrono>
#include <set>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::DynamoDb {

    using std::optional;
    using std::chrono::system_clock;

    // Forward declare for nested types
    struct DynamoValue;

    using KeyValue = std::variant<std::string, double, std::vector<uint8_t>>;
    using DynamoList = std::vector<std::shared_ptr<DynamoValue>>;
    using DynamoMap = std::unordered_map<std::string, std::shared_ptr<DynamoValue>>;

    // Wrapper to distinguish Binary from Binary Set
    struct BinarySet {
        std::vector<std::vector<uint8_t>> values;
    };
    struct DynamoValue {
        using DynamoVariant = std::variant<
                std::string,          // S
                double,               // N
                std::vector<uint8_t>, // B
                bool,                 // BOOL
                std::nullptr_t,       // NULL
                std::set<std::string>,// SS
                std::set<double>,     // NS
                BinarySet,            // BS
                DynamoList,           // L
                DynamoMap             // M
                >;

        DynamoVariant value;

      private:

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DynamoValue const &obj) {
            std::visit([&jv]<typename T0>(const T0 &val) {
                using T = std::decay_t<T0>;

                if constexpr (std::is_same_v<T, std::string>)
                    jv = {{"S", val}};

                else if constexpr (std::is_same_v<T, double>)
                    jv = {{"N", std::to_string(val)}};

                else if constexpr (std::is_same_v<T, bool>)
                    jv = {{"BOOL", val}};

                else if constexpr (std::is_same_v<T, std::nullptr_t>)
                    jv = {{"NULL", true}};

                else if constexpr (std::is_same_v<T, std::vector<uint8_t>>)
                    jv = {{"B", std::string(val.begin(), val.end())}};

                else if constexpr (std::is_same_v<T, std::set<std::string>>) {
                    boost::json::array arr;
                    for (const auto &s: val)
                        arr.emplace_back(s);
                    jv = {{"SS", arr}};

                } else if constexpr (std::is_same_v<T, std::set<double>>) {
                    boost::json::array arr;
                    for (const auto &n: val)
                        arr.emplace_back(std::to_string(n));
                    jv = {{"NS", arr}};

                } else if constexpr (std::is_same_v<T, BinarySet>) {
                    boost::json::array arr;
                    for (const auto &b: val.values)
                        arr.emplace_back(std::string(b.begin(), b.end()));
                    jv = {{"BS", arr}};

                } else if constexpr (std::is_same_v<T, DynamoList>) {
                    boost::json::array arr;
                    for (const auto &el: val)
                        arr.push_back(boost::json::value_from(*el));
                    jv = {{"L", arr}};

                } else if constexpr (std::is_same_v<T, DynamoMap>) {
                    boost::json::object obj;
                    for (const auto &[k, v]: val)
                        obj[k] = boost::json::value_from(*v);
                    jv = {{"M", obj}};

                } else {
                    throw std::runtime_error("Invalid attribute value");
                }
            },
                       obj.value);
        }

        friend DynamoValue tag_invoke(boost::json::value_to_tag<DynamoValue>, boost::json::value const &jv) {
            DynamoValue result;
            const auto &obj = jv.as_object();

            if (obj.contains("S"))
                result.value.emplace<std::string>(obj.at("S").as_string().c_str());

            else if (obj.contains("N"))
                result.value.emplace<double>(std::stod(obj.at("N").as_string().c_str()));

            else if (obj.contains("BOOL"))
                result.value.emplace<bool>(obj.at("BOOL").as_bool());

            else if (obj.contains("NULL"))
                result.value.emplace<std::nullptr_t>(nullptr);

            else if (obj.contains("B")) {
                auto s = obj.at("B").as_string();
                result.value.emplace<std::vector<uint8_t>>(s.begin(), s.end());

            } else if (obj.contains("SS")) {
                std::set<std::string> ss;
                for (const auto &el: obj.at("SS").as_array())
                    ss.insert(el.as_string().c_str());
                result.value.emplace<std::set<std::string>>(std::move(ss));

            } else if (obj.contains("NS")) {
                std::set<double> ns;
                for (const auto &el: obj.at("NS").as_array())
                    ns.insert(std::stod(el.as_string().c_str()));
                result.value.emplace<std::set<double>>(std::move(ns));

            } else if (obj.contains("BS")) {
                BinarySet bs;
                for (const auto &el: obj.at("BS").as_array()) {
                    auto s = el.as_string();
                    bs.values.push_back(std::vector<uint8_t>(s.begin(), s.end()));
                }
                result.value.emplace<BinarySet>(std::move(bs));

            } else if (obj.contains("L")) {
                DynamoList list;
                for (const auto &el: obj.at("L").as_array())
                    list.push_back(std::make_shared<DynamoValue>(
                            boost::json::value_to<DynamoValue>(el)));
                result.value.emplace<DynamoList>(std::move(list));

            } else if (obj.contains("M")) {
                DynamoMap map;
                for (const auto &[k, v]: obj.at("M").as_object())
                    map[std::string{k}] = std::make_shared<DynamoValue>(
                            boost::json::value_to<DynamoValue>(v));
                result.value.emplace<DynamoMap>(std::move(map));

            } else {
                throw std::runtime_error("Unknown DynamoValue JSON type");
            }

            return result;
        }
    };

    /**
     * @brief DynamoDB item primary key
     * @code(.json)
     * {
     *   "featureCustom": {
     *     "N": "1024"
     *   },
     *   "featureName": {
     *     "S": "ONIX_PARSING"
     *   },
     *   "featureState": {
     *     "S": "{\"enabled\":true,\"strategyId\":null,\"parameters\":{}}"
     *   }
     * }
     * @endcode
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Item final : Common::BaseCounter<Item> {

        /**
         * ID
         */
        std::string oid;

        /**
         * Table name
         */
        std::string tableName;

        /**
         * Attributes
         */
        std::unordered_map<std::string, AttributeValue> attributes;

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified = system_clock::now();

      private:

        friend Item tag_invoke(boost::json::value_to_tag<Item>, boost::json::value const &v) {
            Item r;
            r.oid = Core::Json::GetStringValue(v, "oid");
            r.tableName = Core::Json::GetStringValue(v, "tableName");
            r.created = Core::Json::GetDatetimeValue(v, "created");
            r.modified = Core::Json::GetDatetimeValue(v, "modified");

            if (Core::Json::AttributeExists(v, "attributes")) {
                r.attributes = boost::json::value_to<std::unordered_map<std::string, AttributeValue>>(v, "attributes");
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Item const &obj) {
            jv = {
                    {"oid", obj.oid},
                    {"tableName", obj.tableName},
                    {"attributes", boost::json::value_from(obj.attributes)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

    typedef std::vector<Item> ItemList;

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_ITEM_H
