//
// Created by vogje01 on 22/08/2022.
//

#ifndef AWSMOCK_CORE_JSON_CONFIGURATION_H
#define AWSMOCK_CORE_JSON_CONFIGURATION_H

// Standard C++ includes
#include <ranges>
#include <string>

// Boost includes
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

// AwsMock includes
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/TypeName.h>
#include <awsmock/core/Version.h>
#include <awsmock/core/exception/CoreException.h>
#include <awsmock/core/logging/LogStream.h>

#ifdef _WIN32
#define DEFAULT_MAGIC_FILE "C:\\Program Files (x86)\\awsmock\\etc\\magic.mgc"
#else
#define DEFAULT_MAGIC_FILE "/usr/local/awsmock/etc/magic.mgc"
#endif

namespace AwsMock::Core {

    /**
     * @brief Configuration handler.
     *
     * @par
     * Configurations are read from the given configuration file and can be overruled by environment variables.
     * Properties in a configuration file are key-vale pairs.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class Configuration {

    public:

        /**
         * @brief Constructor
         */
        Configuration();

        /**
         * @brief Singleton instance
         */
        static Configuration &instance() {
            static Configuration configuration;
            return configuration;
        }

        /**
         * @brief Constructor
         *
         * @param basename basename of the configuration file.
         */
        explicit Configuration(const std::string &basename);

        /**
         * @brief Define a new configuration property.
         *
         * @par
         * If the system environment has a value for the given configuration key, the environment value is set.
         * If the configuration already has a value for the given key, the key is preserved; otherwise the default value is taken.
         *
         * @param key configuration key
         * @param envProperty environment variable name
         * @param defaultValue string default value
         */
        template<class T>
        void DefineProperty(const std::string &key, const std::string &envProperty, const T &defaultValue);

        /**
         * @brief Define a new string array property.
         *
         * <p>If the system environment has a value for the given configuration key, the environment value is set. If the configuration has already a value for the given
         * key, the key is preserved, otherwise the default value is taken.</p>
         *
         * @param key configuration key
         * @param envProperty environment variable name
         * @param defaultValue string default value
         */
        template<class T>
        void DefineArrayProperty(const std::string &key, const std::string &envProperty, const T &defaultValue);

        /**
         * @brief Sets a configuration value.
         *
         * @param key property key
         * @param value configuration value
         */
        template<class T>
        void SetValue(const std::string &key, const T &value);

        /**
         * @brief Returns a string configuration value
         *
         * @param key property key
         * @return configuration value
         */
        template<class T>
        T GetValue(const std::string &key) const;

        /**
         * @brief Returns a string configuration value
         *
         * @param key property key
         * @return configuration value
         */
        template<class T>
        std::vector<T> GetValueArray(const std::string &key) const;

        /**
         * @brief Returns the file name of the configuration file.
         *
         * @return file name of the configuration file.
         */
        [[nodiscard]] std::string GetFilename() const;

        /**
         * @brief Sets the file name of the configuration file.
         *
         * @param filename file name of the configuration file.
         */
        void SetFilename(const std::string &filename);

        /**
         * @brief Checks whether a value exists
         *
         * @param key configuration key
         * @return true if value exists
         */
        [[nodiscard]] bool HasValue(const std::string &key) const;

        /**
         * @brief Returns the application name
         *
         * @return application name
         */
        static std::string GetAppName();

        /**
         * @brief Returns the version of the library.
         *
         * @return library version
         */
        static std::string GetVersion();

        /**
         * @brief Writes the current configuration the given file
         *
         * @param filename name of the configuration file
         */
        void WriteFile(const std::string &filename) const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Dumps the configuration to std::err
         */
        void Dump() const;

    private:

        /**
         * @brief Initialize the base properties
         */
        void Initialize();

        /**
         * @brief Save the environment variables as a key/value pair
         *
         * @param key environment variable key
         * @param value environment variable value
         */
        void AddToEnvList(const std::string &key, const std::string &value);

        /**
         * @brief Reapply the environment variables to the properties, as environment variables have precedence over
         * file variables.
         */
        void ApplyEnvSettings();

        /**
         * @brief Replace environment variables with their value.
         *
         * @param value configuration value
         * @return value with replaced environment variables
         */
        static std::string ReplaceEnvironmentVariables(const std::string &value);

        /**
         * @brief Checks the existence of a property key
         *
         * @return true if a property key exists
         */
        [[nodiscard]] bool HasProperty(const std::string &key) const;

        /**
         * Name of the configuration file
         */
        std::string _filename;

        /**
         * Profile configuration file
         */
        std::string _profile;

        /**
         * List of defined environment variables and their value. After a configuration has been read, the environment variables need to be reapplied,
         * as environment variables have precedence over config file variables.
         */
        std::map<std::string, std::string> _envList;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &, const Configuration &);

        /**
         * Boost a property tree
         */
        boost::property_tree::ptree _treeConfiguration;
    };

    template<class T>
    void Configuration::DefineProperty(const std::string &key, const std::string &envProperty, const T &defaultValue) {
        const auto typeName = std::string(TypeName<T>());

        T value = defaultValue;
        if (getenv(envProperty.c_str()) != nullptr) {
            const auto envVariable = std::string(getenv(envProperty.c_str()));
            AddToEnvList(key, envVariable);
            if (typeName == TYPE_NAME_INT) {
                value = std::stoi(envVariable);
            } else if (typeName == TYPE_NAME_LONG) {
                value = std::stol(envVariable);
            } else if (typeName == TYPE_NAME_FLOAT) {
                value = std::stof(envVariable);
            } else if (typeName == TYPE_NAME_DOUBLE) {
                value = std::stod(envVariable);
            } else if (typeName == TYPE_NAME_BOOL) {
                value = envVariable == "true" || envVariable == "True" || envVariable == "TRUE" || envVariable == "1";
            }
        }
        _treeConfiguration.put<T>(key, value);
        //log_info << "Defined property, key: " << key << ", property: " << envProperty << ", default: " << defaultValue;
    }

    template<class T>
    void Configuration::DefineArrayProperty(const std::string &key, const std::string &envProperty, const T &defaultValue) {
        if (const auto typeName = std::string(TypeName<T>()); typeName != TYPE_NAME_STRING) {
            return;
        }
        T value = defaultValue;
        if (getenv(envProperty.c_str()) != nullptr) {
            const std::string envVariable = getenv(envProperty.c_str());
            AddToEnvList(key, envVariable);
        }

        boost::property_tree::ptree array;
        for (const std::vector<std::string> values = StringUtils::Split(value, ";"); auto &v: values) {
            boost::property_tree::ptree child;
            child.put("", v);
            array.push_back(std::make_pair("", child));
        }
        _treeConfiguration.add_child(key, array);
        //log_info << "Defined property, key: " << key << ", property: " << envProperty << ", default: " << defaultValue;
    }

    template<class T>
    void Configuration::SetValue(const std::string &key, const T &value) {
        if (!HasProperty(key)) {
            log_error << "Property not found, key: " << key;
            throw CoreException("Property not found, key: " + key);
        }
        _treeConfiguration.get_child(key).put_value<T>(value);
        log_debug << "Value set, key: " << key << ", value: " << _treeConfiguration.get<T>(key);
    }

    template<class T>
    T Configuration::GetValue(const std::string &key) const {
        if (!HasProperty(key)) {
            log_error << "Property not found, key: " << key;
            throw CoreException("Property not found, key: " + key);
        }
        return _treeConfiguration.get_child(key).get_value<T>();
    }

    template<class T>
    std::vector<T> Configuration::GetValueArray(const std::string &key) const {
        if (!HasProperty(key)) {
            log_error << "Property not found, key: " << key;
            throw CoreException("Property not found, key: " + key);
        }
        std::vector<std::string> r;
        for (const auto &val: _treeConfiguration.get_child(key) | std::views::values) {
            r.push_back(val.get_value<std::string>());
        }
        log_debug << "Value aray set, key: " << key << ", count: " << r.size();
        return r;
    }

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_JSON_CONFIGURATION_H
