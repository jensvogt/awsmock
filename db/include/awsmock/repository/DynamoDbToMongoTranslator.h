//
// Created by vogje01 on 3/8/26.
//

#ifndef AWSMOCK_REPOSITORY_DYNAMODB_TO_MONGO_TRANSLATOR_H
#define AWSMOCK_REPOSITORY_DYNAMODB_TO_MONGO_TRANSLATOR_H

// C++ includes
#include <iostream>
#include <map>
#include <regex>
#include <string>

// Mongo DB includes
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

namespace AwsMock::Database {

    class DynamoToMongoTranslator {

      public:

        struct DynamoRequest {
            std::string expression;
            std::map<std::string, std::string> attrNames; // #n1 -> realName
            std::map<std::string, std::string> attrValues;// :v1 -> realValue (unmarshalled)
        };

        static bsoncxx::document::value translate(const DynamoRequest &req) {
            auto filter = bsoncxx::builder::basic::document{};

            // 1. Handle Equality: "Field = :value"
            std::regex eqRegex(R"((\#?\w+)\s*=\s*(\:?\w+))");
            auto words_begin = std::sregex_iterator(req.expression.begin(), req.expression.end(), eqRegex);
            auto words_end = std::sregex_iterator();

            for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
                const std::smatch &match = *i;
                std::string field = resolve(match[1], req.attrNames);
                std::string val = resolve(match[2], req.attrValues);
                filter.append(kvp(field, val));
            }

            // 2. Handle begins_with: "begins_with(#field, :val)"
            std::regex bwRegex(R"(begins_with\s*\(\s*(\#?\w+)\s*,\s*(\:?\w+)\s*\))");
            if (std::smatch bwMatch; std::regex_search(req.expression, bwMatch, bwRegex)) {
                std::string field = resolve(bwMatch[1], req.attrNames);
                std::string val = resolve(bwMatch[2], req.attrValues);

                // MongoDB Regex for "starts with" is /^value/
                filter.append(kvp(field, make_document(kvp("$regex", "^" + val))));
            }

            return filter.extract();
        }

      private:

        static std::string resolve(std::string token, const std::map<std::string, std::string> &mapping) {
            if (mapping.contains(token)) {
                return mapping.at(token);
            }
            return token;// Return as-is if not in map (e.g. raw field name)
        }
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_DYNAMODB_TO_MONGO_TRANSLATOR_H
