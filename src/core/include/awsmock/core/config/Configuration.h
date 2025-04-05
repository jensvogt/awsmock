//
// Created by vogje01 on 22/08/2022.
//

#ifndef AWSMOCK_CORE_YAML_CONFIGURATION_H
#define AWSMOCK_CORE_YAML_CONFIGURATION_H

// Standard C++ includes
#include <string>

// YAML-cpp includes
#include <yaml-cpp/yaml.h>

// AwsMock includes
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/Version.h>
#include <awsmock/core/exception/CoreException.h>

#ifdef _WIN32
#define DEFAULT_MAGIC_FILE "C:/Program Files (x86)/awsmock/etc/magic.mgc"
#else
#define DEFAULT_MAGIC_FILE "/usr/looal/awsmock/etc/magic.mgc"
#endif

namespace AwsMock::Core {

    // Node lookup
    template<typename Iter>
    YAML::Node lookup(const YAML::Node &node, Iter start, Iter end) {
        if (start == end) {
            return node;
        }
        return lookup(node[*start], next(start), end);
    }

    template<typename T>
    void SetValueByPath(YAML::Node &_yamlConfig, const std::string &key, T value) {

        const std::vector<std::string> tags = StringUtils::Split(key, '.');
        const int numTags = static_cast<int>(tags.size());

        // Reads root node.
        assert(numTags > 0);
        assert(_yamlConfig);
        YAML::Node *parentNode = &_yamlConfig;

        // Determines index of first non-existing node.
        int i = 0;
        for (; i < numTags - 1; i++) {
            if (const std::string &tag = tags.at(i); (*parentNode)[tag]) {
                auto childNode = (*parentNode)[tag];
                parentNode = &childNode;
            } else {
                break;
            }
        }

        // Note: necessary because *parentNode will later point to a different node due to lib behavior .
        YAML::Node lastExistingNode = *parentNode;

        // Sets node value and creates missing nodes.
        if (i == numTags - 1) {
            lastExistingNode[tags.back()] = value;
        } else {
            YAML::Node newNode;
            newNode = value;
            for (int j = numTags - 1; j > i; j--) {
                YAML::Node tmpNode;
                tmpNode[tags.at(j)] = newNode;
                newNode = tmpNode;
            }
            // Inserts missing nodes.
            lastExistingNode[tags.at(i)] = newNode;
        }
    }

    /**
     * @brief Configuration handler.
     *
     * Configuration are read from the given configuration file and can be overruled by environment variables. Environment variables
     * are replaced by the system environment.
     *
     * Properties in a configuration file are key-vale pairs. The following list shows all supported keys with their default values:
     * @code{.yaml}
     * awsmock.monitoring.port=9100
     * awsmock.monitoring.timeout=60000
     * awsmock.logging.level=debug
     * @endcode
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
         * <p>If the system environment has a value for the given configuration key, the environment value is set. If the configuration has already a value for the given
         * key, the key is preserved, otherwise the default value is taken. </p>
         *
         * @param key configuration key
         * @param envProperty environment variable name
         * @param defaultValue string default value
         */
        void DefineStringProperty(const std::string &key, const std::string &envProperty, const std::string &defaultValue);

        /**
         * @brief Define a new string array property.
         *
         * <p>If the system environment has a value for the given configuration key, the environment value is set. If the configuration has already a value for the given
         * key, the key is preserved, otherwise the default value is taken. </p>
         *
         * @param key configuration key
         * @param envProperty environment variable name
         * @param defaultValue string default value
         */
        void DefineStringArrayProperty(const std::string &key, const std::string &envProperty, const std::string &defaultValue);

        /**
         * @brief Define a new configuration property.
         *
         * <p>If the system environment has a value for the given configuration key, the environment value is set. If the configuration has already a value for the given
         * key, the key is preserved, otherwise the default value is taken. </p>
         *
         * @param key configuration key
         * @param envProperty environment variable name
         * @param defaultValue boolean default value
         */
        void DefineBoolProperty(const std::string &key, const std::string &envProperty, bool defaultValue);

        /**
         * @brief Define a new configuration property.
         *
         * <p>If the system environment has a value for the given configuration key, the environment value is set. If the configuration has already a value for the given
         * key, the key is preserved, otherwise the default value is taken. </p>
         *
         * @param key configuration key
         * @param envProperty environment variable name
         * @param defaultValue integer default value
         */
        void DefineIntProperty(const std::string &key, const std::string &envProperty, int defaultValue);

        /**
         * @brief Define a new configuration property.
         *
         * <p>If the system environment has a value for the given configuration key, the environment value is set. If the configuration has already a value for the given
         * key, the key is preserved, otherwise the default value is taken. </p>
         *
         * @param key configuration key
         * @param envProperty environment variable name
         * @param defaultValue integer default value
         */
        void DefineLongProperty(const std::string &key, const std::string &envProperty, long defaultValue);

        /**
         * @brief Define a new configuration property.
         *
         * <p>If the system environment has a value for the given configuration key, the environment value is set. If the configuration has already a value for the given
         * key, the key is preserved, otherwise the default value is taken. </p>
         *
         * @param key configuration key
         * @param envProperty environment variable name
         * @param defaultValue integer default value
         */
        void DefineDoubleProperty(const std::string &key, const std::string &envProperty, double defaultValue);

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
         * @brief Returns a string configuration value
         *
         * @param key property key
         * @return configuration value
         */
        [[nodiscard]] std::string GetValueString(const std::string &key) const;

        /**
         * @brief Returns a string array configuration value
         *
         * @param key property key
         * @return configuration value
         */
        [[nodiscard]] std::vector<std::string> GetValueStringArray(const std::string &key) const;

        /**
         * @brief Returns a integer configuration value
         *
         * @param key property key
         * @return configuration value
         */
        [[nodiscard]] int GetValueInt(const std::string &key) const;

        /**
         * @brief Returns a long integer configuration value
         *
         * @param key property key
         * @return configuration value
         */
        [[nodiscard]] long GetValueLong(const std::string &key) const;

        /**
         * @brief Returns a boolean configuration value
         *
         * @param key property key
         * @return configuration value
         */
        [[nodiscard]] bool GetValueBool(const std::string &key) const;

        /**
         * @brief Returns a float configuration value
         *
         * @param key property key
         * @return configuration value
         */
        [[nodiscard]] float GetValueFloat(const std::string &key) const;

        /**
         * @brief Returns a double configuration value
         *
         * @param key property key
         * @return configuration value
         */
        [[nodiscard]] double GetValueDouble(const std::string &key) const;

        /**
         * @brief Sets a string configuration value
         *
         * @param key property key
         * @param value configuration value
         */
        void SetValueString(const std::string &key, const std::string &value);

        /**
         * @brief Sets a bool configuration value
         *
         * @param key property key
         * @param value configuration value
         */
        void SetValueBool(const std::string &key, bool value);

        /**
         * @brief Sets an integer configuration value
         *
         * @param key property key
         * @param value configuration value
         */
        void SetValueInt(const std::string &key, int value);

        /**
         * @brief Sets an long integer configuration value
         *
         * @param key property key
         * @param value configuration value
         */
        void SetValueLong(const std::string &key, const long value);

        /**
         * @brief Sets an double configuration value
         *
         * @param key property key
         * @param value configuration value
         */
        void SetValueFloat(const std::string &key, const float value);

        /**
         * @brief Sets an double configuration value
         *
         * @param key property key
         * @param value configuration value
         */
        void SetValueDouble(const std::string &key, const double value);

        /**
         * @brief Checks whether a value exists
         *
         * @param key configuration key
         * @return true if value exists
         */
        bool HasValue(const std::string &key) const;

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
         * @brief Dumps the configuration to std:cerr
         */
        void Dump() const;

      private:

        /**
         * @brief Initialize the base properties
         */
        void Initialize();

        /**
         * @brief Save the environment variables as key/value pair
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
        static std::string ReplaceEnvironmentVariables(std::string &value);

        /**
         * @brief Checks existence of a property key
         *
         * @return true if property key exists
         */
        bool HasProperty(const std::string &key) const;

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
         * YAML config
         */
        YAML::Node _yamlConfig;
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_YAML_CONFIGURATION_H
