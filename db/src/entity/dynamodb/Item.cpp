//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/dynamodb/Item.h>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/types/bson_value/value.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

template<typename T>
struct always_false : std::false_type {};

namespace AwsMock::Database::Entity::DynamoDb {
    bsoncxx::types::bson_value::value Item::KeyValueToBson(const KeyValue &kv) {

        return std::visit([]<typename T0>(const T0 &val) -> bsoncxx::types::bson_value::value {
            using T = std::decay_t<T0>;

            if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, double>) {
                return bsoncxx::types::bson_value::value{val};

            } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
                return bsoncxx::types::bson_value::value{
                        bsoncxx::types::b_binary{
                                bsoncxx::binary_sub_type::k_binary,
                                static_cast<uint32_t>(val.size()),
                                val.data()}};

            } else {
                static_assert(always_false<T>::value, "Unhandled KeyValue type");
                throw std::logic_error("Unhandled KeyValue type");
            }
        },
                          kv);
    }

    KeyValue Item::KeyValueFromBson(const bsoncxx::types::bson_value::view &val) {
        switch (val.type()) {
            case bsoncxx::type::k_string:
                return std::string{val.get_string().value};

            case bsoncxx::type::k_double:
                return val.get_double().value;

            case bsoncxx::type::k_binary: {
                const auto bin = val.get_binary();
                return std::vector<uint8_t>{bin.bytes, bin.bytes + bin.size};
            }

            default:
                throw std::runtime_error("Unexpected BSON type for KeyValue");
        }
    }

    bsoncxx::types::bson_value::value Item::DynamoValueToBson(const DynamoValue &dv) const {
        return std::visit([this]<typename T0>(const T0 &val) -> bsoncxx::types::bson_value::value {
            using T = std::decay_t<T0>;

            if constexpr (std::is_same_v<T, std::string>) {
                return bsoncxx::types::bson_value::value{val};

            } else if constexpr (std::is_same_v<T, double>) {
                return bsoncxx::types::bson_value::value{val};

            } else if constexpr (std::is_same_v<T, bool>) {
                return bsoncxx::types::bson_value::value{val};

            } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return bsoncxx::types::bson_value::value{bsoncxx::types::b_null{}};

            } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
                // Binary
                return bsoncxx::types::bson_value::value{
                        bsoncxx::types::b_binary{
                                bsoncxx::binary_sub_type::k_binary,
                                static_cast<uint32_t>(val.size()),
                                val.data()}};

            } else if constexpr (std::is_same_v<T, std::set<std::string>>) {
                array arr;
                for (const auto &s: val)
                    arr.append(s);
                return bsoncxx::types::bson_value::value{arr.view()};

            } else if constexpr (std::is_same_v<T, std::set<double>>) {
                array arr;
                for (const auto &n: val)
                    arr.append(n);
                return bsoncxx::types::bson_value::value{arr.view()};

            } else if constexpr (std::is_same_v<T, DynamoList>) {
                array arr;
                for (const auto &item: val)
                    arr.append(DynamoValueToBson(*item));// item is shared_ptr<DynamoValue>
                return bsoncxx::types::bson_value::value{arr.view()};

            } else if constexpr (std::is_same_v<T, DynamoMap>) {
                document doc;
                for (const auto &[k, v]: val)
                    doc.append(kvp(k, DynamoValueToBson(*v)));// v is shared_ptr<DynamoValue>
                return bsoncxx::types::bson_value::value{doc.view()};

            } else {
                //static_assert(always_false<T>::value, "Unhandled KeyValue type");
                throw std::logic_error("Unhandled KeyValue type");
            }
        },
                          dv.value);
    }

    DynamoValue Item::DynamoValueFromBson(const bsoncxx::types::bson_value::view &val) {
        switch (val.type()) {

            case bsoncxx::type::k_string:
                return DynamoValue{std::string{val.get_string().value}};

            case bsoncxx::type::k_double:
                return DynamoValue{val.get_double().value};

            case bsoncxx::type::k_bool:
                return DynamoValue{val.get_bool().value};

            case bsoncxx::type::k_null:
                return DynamoValue{nullptr};

            case bsoncxx::type::k_binary: {
                const auto bin = val.get_binary();
                return DynamoValue{std::vector<uint8_t>{bin.bytes, bin.bytes + bin.size}};
            }

            case bsoncxx::type::k_array: {
                auto arr = val.get_array().value;
                auto it = arr.begin();

                if (it == arr.end())
                    return DynamoValue{DynamoList{}};// empty — default to list

                if (it->type() == bsoncxx::type::k_string) {
                    std::set<std::string> ss;
                    for (const auto &el: arr)
                        ss.insert(std::string{el.get_string().value});
                    return DynamoValue{ss};

                } else if (it->type() == bsoncxx::type::k_double) {
                    std::set<double> ns;
                    for (const auto &el: arr)
                        ns.insert(el.get_double().value);
                    return DynamoValue{ns};

                } else {
                    DynamoList list;
                    for (const auto &el: arr)
                        list.push_back(std::make_shared<DynamoValue>(DynamoValueFromBson(el.get_value())));
                    return DynamoValue{list};
                }
            }

            case bsoncxx::type::k_document: {
                DynamoMap map;
                for (const auto &el: val.get_document().value)
                    map[std::string{el.key()}] = std::make_shared<DynamoValue>(
                            DynamoValueFromBson(el.get_value()));
                return DynamoValue{map};
            }

            default:
                throw std::runtime_error("Unexpected BSON type for DynamoValue");
        }
    }

    view_or_value<view, value> Item::ToDocument() const {

        try {

            // Attributes
            document attributesDoc;
            if (!attributes.empty()) {
                for (const auto &[fst, snd]: attributes) {
                    attributesDoc.append(kvp(fst, DynamoValueToBson(snd)));
                }
            }

            auto itemDoc = document{};
            Core::Bson::BsonUtils::SetStringValue(itemDoc, "oid", oid);
            Core::Bson::BsonUtils::SetStringValue(itemDoc, "region", region);
            Core::Bson::BsonUtils::SetStringValue(itemDoc, "tableName", tableName);
            Core::Bson::BsonUtils::SetLongValue(itemDoc, "size", size);
            Core::Bson::BsonUtils::SetDocumentValue(itemDoc, "attributes", attributesDoc);
            Core::Bson::BsonUtils::SetDateValue(itemDoc, "created", created);
            Core::Bson::BsonUtils::SetDateValue(itemDoc, "modified", modified);

            // Keys
            itemDoc.append(kvp("partitionKey", KeyValueToBson(partitionKey)));
            itemDoc.append(kvp("sortKey", KeyValueToBson(sortKey)));

            return itemDoc.extract();

        } catch (const std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::unordered_map<std::string, DynamoValue> Item::AttributesFromBson(const view &doc) {
        std::unordered_map<std::string, DynamoValue> attributes;
        for (const auto &el: doc) {
            attributes[std::string{el.key()}] = DynamoValueFromBson(el.get_value());
        }
        return attributes;
    }

    Item Item::FromDocument(const view_or_value<view, value> &mResult) {

        try {

            oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
            region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
            tableName = Core::Bson::BsonUtils::GetStringValue(mResult, "tableName");
            size = Core::Bson::BsonUtils::GetLongValue(mResult, "size");
            created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
            modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

            partitionKey = KeyValueFromBson(mResult.view()["partitionKey"].get_value());
            sortKey = KeyValueFromBson(mResult.view()["sortKey"].get_value());
            attributes = AttributesFromBson(mResult.view()["attributes"].get_document().value);
            return *this;

        } catch (const std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Database::Entity::DynamoDb