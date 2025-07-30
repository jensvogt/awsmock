//
// Created by vogje01 on 10/23/23.
//

#ifndef AWSMOCK_DTO_MODULE_MODULE_H
#define AWSMOCK_DTO_MODULE_MODULE_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/module/Module.h>
#include <awsmock/entity/module/ModuleState.h>

namespace AwsMock::Dto::Module {

    using std::chrono::system_clock;

    /**
     * @brief AwsMock module
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Module {

        /**
         * Name
         */
        std::string name;

        /**
         * Status
         */
        Database::Entity::Module::ModuleState status;

        /**
         * Executable
         */
        std::string executable = {};

        /**
         * HTTP port
         */
        int port = -1;

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified = system_clock::now();

        /**
         * @brief Convert to JSON representation
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Convert to JSON representation
         *
         * @param moduleList module entity list
         * @return JSON string
         */
        static std::string ToJson(const Database::Entity::Module::ModuleList &moduleList);

        /**
         * @brief Convert to JSON representation
         *
         * @param moduleList module entity list
         * @return JSON string
         */
        typedef std::vector<Module> ModuleList;
        static std::string ToJson(const ModuleList &moduleList);

        /**
         * @brief Convert from JSON representation
         *
         * @param payload JSON representation
         * @return Module
         */
        static Module FromJson(const std::string &payload);

        /**
         * @brief Convert from BSON representation
         *
         * @param document BSON representation
         * @return Module
         */
        static Module FromDocument(const view &document);

        /**
         * @brief Convert from JSON representation
         *
         * @param payload JSON representation
         * @return ModuleList
         */
        static std::vector<Module> FromJsonList(const std::string &payload);

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @param os output stream
         * @param m module struct
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const Module &m);
    };

}// namespace AwsMock::Dto::Module

#endif// AWSMOCK_DTO_MODULE_MODULE_H
