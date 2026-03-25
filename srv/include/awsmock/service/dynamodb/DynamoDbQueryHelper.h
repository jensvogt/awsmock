//
// Created by vogje01 on 3/22/26.
//

#ifndef AWSMOCK_SERVICE_DYNAMODB_QUERY_HELPER_H
#define AWSMOCK_SERVICE_DYNAMODB_QUERY_HELPER_H

// C++ standard includes
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>

// AwsMock DTO includes
#include <awsmock/dto/dynamodb/model/AttributeValue.h>

namespace AwsMock::Service {

    // =========================================================================
    // AST nodes
    // =========================================================================

    struct Eq {
        std::string field;
        std::string placeholder;
    };

    struct Gte {
        std::string field;
        std::string placeholder;
    };

    struct Lte {
        std::string field;
        std::string placeholder;
    };

    struct Gt {
        std::string field;
        std::string placeholder;
    };

    struct Lt {
        std::string field;
        std::string placeholder;
    };

    struct Between {
        std::string field;
        std::string low;
        std::string high;
    };

    using Condition = std::variant<Eq, Gte, Lte, Gt, Lt, Between>;

    struct And {
        std::vector<Condition> conditions;
    };

    // ExpressionAttributeValues maps placeholder (e.g. ":pk") to an AttributeValue
    using ExpressionAttributeValues = std::unordered_map<std::string, Dto::DynamoDb::AttributeValue>;

    // =========================================================================
    // Hand-rolled recursive descent parser
    //
    // Grammar:
    //   expr        ::= condition (' AND ' condition)*
    //   condition   ::= field SP? ('='|'>='|'<='|'>'|'<') SP? placeholder
    //                 | field ' BETWEEN ' placeholder ' AND ' placeholder
    //   field       ::= '#'? alpha (alnum | '_')*
    //   placeholder ::= ':' (alnum | '_')+
    // =========================================================================

    class ExpressionParser {
      public:

        explicit ExpressionParser(const std::string_view input) : src_(input), pos_(0) {}

        And parse() {
            And result;
            result.conditions.push_back(parse_condition());

            while (match_keyword_and()) {
                result.conditions.push_back(parse_condition());
            }

            if (pos_ != src_.size()) {
                log_error << "Unexpected trailing input at position " << pos_ << ": '" << std::string(src_.substr(pos_)) << "'";
                throw Core::DynamoDbParseException("Unexpected trailing input at position " + std::to_string(pos_) + ": '" + std::string(src_.substr(pos_)) + "'");
            }
            return result;
        }

      private:

        std::string_view src_;
        std::size_t pos_;

        // -----------------------------------------------------------------
        // Low-level helpers
        // -----------------------------------------------------------------

        char peek() const {
            return pos_ < src_.size() ? src_[pos_] : '\0';
        }

        void expect(char c) {
            if (pos_ >= src_.size() || src_[pos_] != c) {
                log_error << "Expected '" << c << "' at position " << pos_ << ", got '" << (pos_ < src_.size() ? std::string(1, src_[pos_]) : "EOF") << "'";
                throw Core::DynamoDbParseException(std::string("Expected '") + c + "' at position " + std::to_string(pos_) + ", got '" + (pos_ < src_.size() ? std::string(1, src_[pos_]) : "EOF") + "'");
            }
            ++pos_;
        }

        void expect_str(std::string_view s) {
            if (src_.substr(pos_, s.size()) != s) {
                throw Core::DynamoDbParseException(
                        "Expected '" + std::string(s) + "' at position " +
                        std::to_string(pos_));
            }
            pos_ += s.size();
        }

        void skip_spaces() {
            while (pos_ < src_.size() && std::isspace(static_cast<unsigned char>(src_[pos_])))
                ++pos_;
        }

        // Returns true and advances past " AND " if it is a top-level separator.
        // Distinguishes from the AND inside BETWEEN by checking that what follows
        // is a field name (alpha or '#') and not a placeholder (':').
        bool match_keyword_and() {
            std::size_t tmp = pos_;

            // skip leading spaces
            while (tmp < src_.size() && src_[tmp] == ' ') ++tmp;

            if (src_.substr(tmp, 3) != "AND") return false;
            tmp += 3;

            // must be followed by a space
            if (tmp >= src_.size() || src_[tmp] != ' ') return false;
            ++tmp;

            if (tmp >= src_.size()) return false;
            const char next = src_[tmp];

            // Next token must be a field (alpha or '#'), not a placeholder ':'
            if (std::isalpha(static_cast<unsigned char>(next)) || next == '#') {
                pos_ = tmp;// commit
                return true;
            }
            return false;
        }

        // -----------------------------------------------------------------
        // Token parsers
        // -----------------------------------------------------------------

        // field ::= '#'? alpha (alnum | '_')*
        std::string parse_field() {
            std::string result;

            if (peek() == '#') {
                result += '#';
                ++pos_;
            }

            if (!std::isalpha(static_cast<unsigned char>(peek()))) {
                log_error << "Expected field name at position " << pos_;
                throw Core::DynamoDbParseException("Expected field name at position " + std::to_string(pos_));
            }

            while (pos_ < src_.size() && (std::isalnum(static_cast<unsigned char>(src_[pos_])) || src_[pos_] == '_')) {
                result += src_[pos_++];
            }
            return result;
        }

        // placeholder ::= ':' (alnum | '_')+
        std::string parse_placeholder() {
            expect(':');
            std::string result = ":";

            if (!std::isalnum(static_cast<unsigned char>(peek())) && peek() != '_') {
                log_error << "Expected placeholder name after ':' at position " << pos_;
                throw Core::DynamoDbParseException("Expected placeholder name after ':' at position " + std::to_string(pos_));
            }
            while (pos_ < src_.size() && (std::isalnum(static_cast<unsigned char>(src_[pos_])) || src_[pos_] == '_')) {
                result += src_[pos_++];
            }
            return result;
        }

        // operator ::= '=' | '>=' | '<=' | '>' | '<'
        std::string parse_operator() {
            if (pos_ >= src_.size()) {
                log_error << "Expected operator at position " << pos_;
                throw Core::DynamoDbParseException("Expected operator at position " + std::to_string(pos_));
            }
            // Log exactly what we see (remove after debugging)
            log_info << "parse_operator pos=" << std::to_string(pos_) << " char='" + std::string(1, src_[pos_]) << "'" << " context='" << std::string(src_.substr(pos_, 15)) + "'";

            const char c = src_[pos_++];

            if (c == '=') return "=";

            if (c == '>' || c == '<') {
                if (pos_ < src_.size() && src_[pos_] == '=') {
                    ++pos_;
                    return std::string(1, c) + "=";
                }
                return std::string(1, c);
            }

            log_error << "Unknown operator '" << c << "' at position " << pos_;
            throw Core::DynamoDbParseException("Unknown operator '" + std::to_string(c) + "' at position " + std::to_string(pos_));
        }

        // Returns true if " BETWEEN " follows the current position
        bool peek_between() const {
            return src_.substr(pos_, 9) == " BETWEEN ";
        }

        // -----------------------------------------------------------------
        // Condition parser
        // -----------------------------------------------------------------

        Condition parse_condition() {
            const std::string field = parse_field();

            if (peek_between()) {
                pos_ += 9;// consume " BETWEEN "
                std::string low = parse_placeholder();
                expect_str(" AND ");
                std::string high = parse_placeholder();
                return Between{field, std::move(low), std::move(high)};
            }

            // EQ / comparison branch — skip optional spaces around operator
            skip_spaces();
            const std::string op = parse_operator();
            skip_spaces();
            std::string val = parse_placeholder();

            if (op == "=") return Eq{field, std::move(val)};
            if (op == ">=") return Gte{field, std::move(val)};
            if (op == "<=") return Lte{field, std::move(val)};
            if (op == ">") return Gt{field, std::move(val)};
            if (op == "<") return Lt{field, std::move(val)};

            log_error << "Unknown operator: " << op;
            throw Core::DynamoDbParseException("Unknown operator: " + op);
        }
    };

    // =========================================================================
    // DynamoDbQueryHelper — public API
    // =========================================================================

    class DynamoDbQueryHelper {
      public:

        /**
         * @brief Parse a DynamoDB KeyConditionExpression / FilterExpression
         *        and convert it to a MongoDB filter document.
         *
         * Supported syntax:
         *   field = :placeholder
         *   field > :placeholder
         *   field >= :placeholder
         *   field < :placeholder
         *   field <= :placeholder
         *   field BETWEEN :lo AND :hi
         *   <condition> AND <condition> ...
         *
         * Fields may be plain identifiers or expression attribute names (#name).
         *
         * @param expression  e.g. "pk = :pk AND sk BETWEEN :lo AND :hi"
         * @param attr_values ExpressionAttributeValues from the DynamoDB request
         * @return bsoncxx::document::value  MongoDB filter document
         * @throws Core::DynamoDbParseException on parse failure or missing placeholder
         */
        static value ToMongoFilter(const std::string &expression, const ExpressionAttributeValues &attr_values) {
            const And ast = parse_expression(expression);
            return build_mongo_filter(ast, attr_values);
        }

        /**
         * @brief Same as to_mongo_filter but returns a JSON string.
         *        Useful for logging / debugging.
         */
        static std::string to_mongo_filter_json(const std::string &expression, const ExpressionAttributeValues &attr_values) {
            return bsoncxx::to_json(ToMongoFilter(expression, attr_values).view());
        }

      private:

        // -----------------------------------------------------------------
        // Parse step
        // -----------------------------------------------------------------

        static And parse_expression(const std::string &input) {
            try {// Normalize line endings — remove \r in case of Windows-style input
                std::string normalized;
                normalized.reserve(input.size());
                for (const unsigned char c: input)
                    if (c >= 0x20) normalized += static_cast<char>(c);
                return ExpressionParser(normalized).parse();
            } catch (const std::exception &e) {
                log_error << std::string("DynamoDB expression parse error: ") << e.what() << " | input: " << input;
                throw Core::DynamoDbParseException("DynamoDB expression parse error: " + std::string(e.what()) + " | input: " + input);
            }
        }

        // -----------------------------------------------------------------
        // Resolve a placeholder to its AttributeValue.
        // -----------------------------------------------------------------
        static Dto::DynamoDb::AttributeValue resolve(const std::string &placeholder, const ExpressionAttributeValues &attr_values) {

            const std::string key = placeholder.front() == ':' ? placeholder : ':' + placeholder;

            const auto it = attr_values.find(key);
            if (it == attr_values.end()) {
                log_error << "Missing ExpressionAttributeValue for: " << key;
                throw Core::DynamoDbParseException("Missing ExpressionAttributeValue for: " + key);
            }

            return it->second;
        }

        // -----------------------------------------------------------------
        // Build step  AST -> bsoncxx document
        //
        // DynamoDB EQ:       { "field": value }
        // DynamoDB GT/GTE:   { "field": { "$gt/$gte": value } }
        // DynamoDB LT/LTE:   { "field": { "$lt/$lte": value } }
        // DynamoDB BETWEEN:  { "field": { "$gte": lo, "$lte": hi } }
        // Multiple (AND):    { "$and": [ {cond1}, {cond2}, ... ] }
        // -----------------------------------------------------------------

        static bsoncxx::document::value build_mongo_filter(const And &ast, const ExpressionAttributeValues &attr_values) {

            using bsoncxx::builder::basic::kvp;

            // Single condition — no $and wrapper needed
            if (ast.conditions.size() == 1)
                return build_single(ast.conditions.front(), attr_values);

            // Multiple conditions — wrap in $and array
            document doc{};
            array arr{};

            for (const auto &cond: ast.conditions)
                arr.append(build_single(cond, attr_values));

            doc.append(kvp("$and", arr.extract()));
            return doc.extract();
        }

        // -----------------------------------------------------------------
        // Helper: build a single comparison document for a given mongo op.
        // -----------------------------------------------------------------
        static value make_comparison(
                const std::string &field,
                const std::string &mongo_op,
                const Dto::DynamoDb::AttributeValue &value) {

            using bsoncxx::builder::basic::kvp;
            using bsoncxx::builder::basic::make_document;

            if (value.type == "N")
                return make_document(kvp(field, make_document(kvp(mongo_op, stoi(value.numberValue)))));

            return make_document(kvp(field, make_document(kvp(mongo_op, value.stringValue))));
        }

        // -----------------------------------------------------------------
        // Convert a single Condition variant to a bsoncxx document.
        // -----------------------------------------------------------------
        static value build_single(
                const Condition &cond,
                const ExpressionAttributeValues &attr_values) {

            using bsoncxx::builder::basic::kvp;
            using bsoncxx::builder::basic::make_document;

            return std::visit([&]<typename T0>(T0 &&c) -> value {
                using T = std::decay_t<T0>;

                if constexpr (std::is_same_v<T, Eq>) {
                    if (Dto::DynamoDb::AttributeValue value = resolve(c.placeholder, attr_values); value.type == "S") {
                        return make_document(kvp(getField(c.field, value.type), value.stringValue));
                    } else if (value.type == "N") {
                        return make_document(kvp(getField(c.field, value.type), std::stoi(value.numberValue)));
                    }
                }

                if constexpr (std::is_same_v<T, Gte>) {
                    if (Dto::DynamoDb::AttributeValue value = resolve(c.placeholder, attr_values); value.type == "S") {
                        return make_document(
                                kvp(getField(c.field, value.type), make_document(
                                                                           kvp("$gte", value.stringValue))));
                    } else if (value.type == "N") {
                        return make_document(
                                kvp(getField(c.field, value.type), make_document(
                                                                           kvp("$gte", std::stoi(value.numberValue)))));
                    }
                }

                if constexpr (std::is_same_v<T, Lte>) {
                    if (Dto::DynamoDb::AttributeValue value = resolve(c.placeholder, attr_values); value.type == "S") {
                        return make_document(
                                kvp(getField(c.field, value.type), make_document(
                                                                           kvp("$lte", value.stringValue))));
                    } else if (value.type == "N") {
                        return make_document(
                                kvp(getField(c.field, value.type), make_document(
                                                                           kvp("$lte", std::stoi(value.numberValue)))));
                    }
                }

                if constexpr (std::is_same_v<T, Gt>) {
                    if (Dto::DynamoDb::AttributeValue value = resolve(c.placeholder, attr_values); value.type == "S") {
                        return make_document(
                                kvp(getField(c.field, value.type), make_document(
                                                                           kvp("$gt", value.stringValue))));
                    } else if (value.type == "N") {
                        return make_document(
                                kvp(getField(c.field, value.type), make_document(
                                                                           kvp("$gt", std::stoi(value.numberValue)))));
                    }
                }

                if constexpr (std::is_same_v<T, Lt>) {
                    if (Dto::DynamoDb::AttributeValue value = resolve(c.placeholder, attr_values); value.type == "S") {
                        return make_document(
                                kvp(getField(c.field, value.type), make_document(
                                                                           kvp("$lt", value.stringValue))));
                    } else if (value.type == "N") {
                        return make_document(
                                kvp(getField(c.field, value.type), make_document(
                                                                           kvp("$lt", std::stoi(value.numberValue)))));
                    }
                }

                if constexpr (std::is_same_v<T, Between>) {
                    const Dto::DynamoDb::AttributeValue valueLow = resolve(c.low, attr_values);
                    if (const Dto::DynamoDb::AttributeValue valueHigh = resolve(c.high, attr_values); valueLow.type == "N" && valueHigh.type == "N") {
                        return make_document(kvp(getField(c.field, valueLow.type), make_document(
                                                                                           kvp("$gte", std::stoi(valueLow.numberValue)),
                                                                                           kvp("$lte", std::stoi(valueHigh.numberValue)))));
                    }
                }

                // Unreachable — silences -Wreturn-type for non-exhaustive if constexpr chains
                throw Core::DynamoDbParseException("Unhandled condition variant in build_single");
            },
                              cond);
        }

        static std::string getField(const std::string &inputField, const std::string &type) {
            if (inputField != "tableName" && inputField != "region") {
                return "attributes." + inputField;
            }
            return inputField;
        }
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_DYNAMODB_QUERY_HELPER_H
