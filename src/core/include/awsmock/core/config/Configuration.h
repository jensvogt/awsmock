//
// Created by vogje01 on 22/08/2022.
//

#ifndef AWSMOCK_CORE_YAML_CONFIGURATION_H
#define AWSMOCK_CORE_YAML_CONFIGURATION_H

/*#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define BOOST_ASIO_NO_WIN32_LEAN_AND_MEAN
#include <boost/asio.hpp>
#include <windows.h>
#endif
*/

// Standard C++ includes
#include <ranges>
#include <string>

// Boost includes
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

// AwsMock includes
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/Version.h>
#include <awsmock/core/exception/CoreException.h>

#ifdef _WIN32
#define DEFAULT_MAGIC_FILE "C:\\Program Files (x86)\\awsmock\\etc\\magic.mgc"
#else
#define DEFAULT_MAGIC_FILE "/usr/local/awsmock/etc/magic.mgc"
#endif

namespace AwsMock::Core {

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
        void (Configuration::*get_value())(const std::string &, const std::string &, bool);

        /**
         * @brief Define a new configuration property.
         *
         * <p>If the system environment has a value for the given configuration key, the environment value is set. If the configuration has already a value for the given
         * key, the key is preserved, otherwise the default value is taken.</p>
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
         * key, the key is preserved, otherwise the default value is taken.</p>
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
         * key, the key is preserved, otherwise the default value is taken.</p>
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
         * key, the key is preserved, otherwise the default value is taken.</p>
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
         * key, the key is preserved, otherwise the default value is taken.</p>
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
         * key, the key is preserved, otherwise the default value is taken.</p>
         *
         * @param key configuration key
         * @param envProperty environment variable name
         * @param defaultValue integer default value
         */
        void DefineFloatProperty(const std::string &key, const std::string &envProperty, float defaultValue);

        /**
         * @brief Define a new configuration property.
         *
         * <p>If the system environment has a value for the given configuration key, the environment value is set. If the configuration has already a value for the given
         * key, the key is preserved, otherwise the default value is taken.</p>
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
        static std::string ReplaceEnvironmentVariables(const std::string &value);

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
         * Boost property tree
         */
        boost::property_tree::ptree _treeConfiguration;
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_YAML_CONFIGURATION_H
