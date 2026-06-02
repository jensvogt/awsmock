#pragma once

// C++ standard
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

// Boost
#include <boost/json.hpp>

// Awsmock includes
#include <awsmock/core/Version.h>
#include <awsmock/core/exception/CoreException.h>
#include <awsmock/core/logging/LogStream.h>

namespace Awsmock::Core {

    // Forward declare for recursive variant
    struct ConfigObject;

    // Recursive variant â€” supports nested objects
    using ConfigValue = std::variant<bool, long, double, std::string, std::shared_ptr<ConfigObject>>;

    /**
     * @brief Represents a configurable object with properties and behavior.
     *
     * This class encapsulates configuration settings and functionality for managing
     * and interacting with such settings. It serves as a central point for accessing
     * and possibly modifying parameters or options that influence the operation of
     * a system, component, or application feature.
     *
     * The class is designed to provide a structured and organized way to define,
     * retrieve, and update configuration-related data while maintaining consistency
     * and abstraction from implementation details.
     *
     * Instances of this class may serve as a container for personalized or system-wide
     * configuration data that can be utilized in multiple contexts.
     */
    struct ConfigObject {
        /**
         * @brief Stores the possible options or parameters relevant to a specific context or operation.
         *
         * This variable holds a collection of predefined values that may represent configuration settings,
         * operational states, or other data points. The exact nature and interpretation of these values
         * depend on the specific functionality or subsystem utilizing them.
         *
         * This serves as a standardized set of inputs or outputs that can be referenced during execution.
         *
         * @return A container encapsulating relevant predefined or calculated data points.
         */
        std::map<std::string, ConfigValue> values;

        /**
         * @brief Checks if the specified item exists in the collection.
         *
         * This method determines whether the given item is present in the collection.
         * It performs a search based on the item's characteristics and returns a boolean
         * indicating the result.
         *
         * @param key The key to check for existence in the collection.
         * @return true if the item exists in the collection; otherwise, false.
         */
        [[nodiscard]]
        bool has(const std::string &key) const {
            return values.contains(key);
        }

        /**
         * @brief Retrieves the value associated with a specified key from the data storage.
         *
         * This method searches for the provided key in the current data structure and returns
         * the corresponding value if the key exists.
         *
         * @param key The identifier for which the associated value needs to be retrieved.
         * @return The value associated with the specified key.
         * @throws std::invalid_argument If the key does not exist in the data storage.
         */
        template<typename T>
        T get(const std::string &key) const {
            return std::get<T>(values.at(key));
        }

        /**
         * @brief Retrieves the value associated with the specified key or returns a default value if the key is not found.
         *
         * This method attempts to fetch the value corresponding to a given key from a data store. If the key does
         * not exist in the store, the method returns the provided default value instead.
         *
         * @param key The key whose associated value is to be retrieved.
         * @param defaultValue The value to return if the specified key is not found.
         * @return The value associated with the key if it exists; otherwise, the provided default value.
         */
        template<typename T>
        T getOr(const std::string &key, T def) const noexcept {
            try {
                const auto it = values.find(key);
                if (it == values.end()) return def;
                return std::get<T>(it->second);
            } catch (...) {
                return def;
            }
        }
    };

    /**
     * @brief Retrieves all child objects under a specified path as named maps.
     *
     * This method processes the configuration structure to locate all direct children of the
     * specified path and structures them as named key-value maps, where each key is the name of
     * the child and the value is the corresponding configuration object.
     * Throws an exception if the path does not resolve to a valid object node.
     *
     * @param path The dot-separated path to the object node containing child objects.
     * @return A map where the keys represent the names of child objects, and values represent
     *         their corresponding configurations as nested key-value maps.
     * @throws std::runtime_error If the specified path does not resolve to an object node.
     */
    class Configuration {

      public:

        /**
         * @brief Provides a singleton instance of the Configuration class
         *
         * Ensures a single instance of the Configuration class is created and globally accessible.
         *
         * @return Reference to the singleton Configuration instance
         */
        static Configuration &instance() {
            static Configuration inst;
            return inst;
        }

        /**
         * @brief Manages application configuration settings
         *
         * Contains methods and properties to handle configurable parameters
         * and settings of the application.
         *
         * @return Reference to the Configuration instance
         */
        Configuration(const Configuration &) = delete;

        /**
         * @brief Overloaded operator for performing a specific operation
         *
         * Implements a custom behavior when the operator is used with a particular class.
         *
         * @param configuration The left-hand side operand involved in the operation
         * @return Result of the operation as determined by the implementation
         */
        Configuration &operator=(const Configuration &configuration) = delete;

        /**
         * @brief Loads the configuration from a specified file path.
         *
         * Reads the configuration data from the given file path, parses it as JSON with
         * support for comments and trailing commas, and stores the information internally.
         * Throws an exception if the file does not exist, cannot be opened, or if there are
         * parsing errors.
         *
         * @param path The filesystem path to the configuration file to be loaded.
         * @throw std::runtime_error If the file does not exist, cannot be opened, or if JSON
         *        parsing fails.
         */
        void load(const std::filesystem::path &path);

        /**
         * @brief Reloads the configuration from the previously loaded file path.
         *
         * This method re-reads the configuration file and updates internal state.
         * Throws an exception if no configuration file path has been set prior to calling this method.
         *
         * @throws std::runtime_error if the configuration file path is empty.
         */
        void reload();

        /**
         * @brief Saves the current configuration to the previously loaded file.
         *
         * This method writes the configuration data to the file specified during
         * the initial load or a previous save operation. If no file path is
         * available, an exception is thrown to indicate that a file must be
         * specified using the `saveTo(path)` method instead.
         *
         * @throws std::runtime_error If no configuration file path has been set.
         */
        void save() const;

        /**
         * @brief Save the configuration to a file at the specified path.
         *
         * This method writes the configuration data to the given file path.
         * If the parent directories of the specified path do not exist, they are created automatically.
         * The configuration data is written in a pretty-printed, indented format.
         *
         * @param path  Filesystem path where the configuration should be saved
         * @throws std::runtime_error if the file cannot be opened for writing or if the write operation fails
         */
        void saveTo(const std::filesystem::path &path) const;

        /**
         * @brief Retrieves a value or resource.
         *
         * This method is used to fetch and return the requested value or resource.
         * The specifics of what is retrieved depend on the implementation.
         *
         * @return The requested value or resource
         */
        template<typename T>
        [[nodiscard]]
        T get(const std::string &path) const {
            if (auto ov = envOverride<T>(path)) return *ov;
            const auto *node = resolvePath(path);
            if (!node) throw std::runtime_error("Config key not found: " + path);
            return extractValue<T>(*node, path);
        }

        /**
         * @brief Retrieves the value at the specified configuration path or returns a default value.
         *
         * This method attempts to resolve the given path in the configuration structure. If the path is not found
         * or an error occurs during resolution, the provided default value is returned. The method is safe to use
         * as it does not throw exceptions.
         *
         * @tparam T The type of the value to retrieve.
         * @param path The configuration path to resolve.
         * @param def The default value to return if the path cannot be resolved or an error occurs.
         * @return The value at the specified path if it exists and is accessible, otherwise the default value.
         */
        template<typename T>
        T getOr(const std::string &path, T def) const noexcept {
            try {
                if (auto ov = envOverride<T>(path)) return *ov;
                const auto *node = resolvePath(path);
                if (!node) return def;
                return extractValue<T>(*node, path);
            } catch (...) {
                return def;
            }
        }

        /**
         * @brief Sets a value for the specified key in the configuration
         *
         * Updates the configuration by assigning the provided value to the given key.
         *
         * @param path The unique identifier for the configuration setting
         * @param value The value to associate with the given key
         */
        template<typename T>
        void set(const std::string &path, T value);

        /**
         * @brief Checks if a certain condition is met.
         *
         * The `has` method evaluates a certain criterion based on the provided input
         * and determines whether the corresponding condition is satisfied.
         *
         * @param path The key or identifier to check against the condition.
         * @return True if the condition is met for the given key, otherwise false.
         */
        [[nodiscard]]
        bool has(const std::string &path) const noexcept {
            return resolvePath(path) != nullptr;
        }

        /**
         * @brief Get all child key names under a dot-separated path
         *
         * @par Example:
         * @code
         * // JSON: { "awsmock": { "modules": { "s3": {...}, "sqs": {...} } } }
         * auto keys = cfg.getKeys("awsmock.modules");
         * // â†’ ["s3", "sqs"]
         * @endcode
         *
         * @param path  dot-separated path to an object node
         * @return vector of child key names, empty if path missing or not object
         */
        [[nodiscard]]
        std::vector<std::string> getKeys(const std::string &path) const noexcept;

        /**
         * @brief Get a single object node as a flat keyâ†’ConfigValue map
         *
         * @par Example:
         * @code
         * auto props = cfg.getObject("awsmock.modules.s3");
         * bool active = std::get<bool>(props["active"]);
         * auto socket = std::get<std::string>(props["socket"]);
         * @endcode
         *
         * @param path  dot-separated path to an object node
         * @throws std::runtime_error if a path isn't found or not an object
         */
        [[nodiscard]]
        std::map<std::string, ConfigValue> getObject(const std::string &path) const;

        /**
         * @brief Get all child objects under a path as named maps
         *
         * @par Example JSON:
         * @code
         * {
         *   "awsmock": {
         *     "modules": {
         *       "s3":  { "active": true,  "socket": "/var/run/awsmock/s3.sock",  "port": 4572 },
         *       "sqs": { "active": true,  "socket": "/var/run/awsmock/sqs.sock", "port": 4567 },
         *       "sns": { "active": false, "socket": "/var/run/awsmock/sns.sock", "port": 4568 }
         *     }
         *   }
         * }
         * @endcode
         *
         * @par Usage:
         * @code
         * for (auto& [name, props] : cfg.getObjects("awsmock.modules")) {
         *     bool active = std::get<bool>(props.at("active"));
         *     auto socket = std::get<std::string>(props.at("socket"));
         *     int  port   = static_cast<int>(std::get<long>(props.at("port")));
         * }
         * @endcode
         *
         * @param path  dot-separated path to a parent object node
         * @throws std::runtime_error if path not found or not an object
         */
        [[nodiscard]]
        std::vector<std::pair<std::string, std::map<std::string, ConfigValue>>>
        getObjects(const std::string &path) const;

        /**
         * @brief Get a JSON array node as a vector of typed values
         *
         * @par Example:
         * @code
         * // JSON: { "awsmock": { "tags": ["production", "eu-west-1"] } }
         * auto tags = cfg.getArray<std::string>("awsmock.tags");
         * @endcode
         *
         * @tparam T  int | long | double | bool | std::string
         * @throws std::runtime_error if path not found, not array, or type mismatch
         */
        template<typename T>
        [[nodiscard]]
        std::vector<T> getArray(const std::string &path) const {
            const auto *node = resolvePath(path);
            if (!node) throw std::runtime_error("Config key not found: " + path);
            if (!node->is_array()) throw std::runtime_error("Config key '" + path + "' is not an array");

            std::vector<T> result;
            result.reserve(node->get_array().size());
            for (const auto &elem: node->get_array()) result.push_back(extractValue<T>(elem, path + "[]"));
            return result;
        }

        /**
         * @brief Get a JSON array of objects as a vector of string maps.
         *
         * @par Example:
         * @code
         * // JSON: { "awsmock": { "channels": [{"name": "S3", "level": "info"}] } }
         * auto channels = cfg.getArrayOfObjects("awsmock.channels");
         * @endcode
         *
         * @param path dot-separated config path to the array
         * @return vector of string key/value maps, one per array element
         */
        [[nodiscard]]
        std::vector<std::map<std::string, std::string>> getArrayOfObjects(const std::string &path) const;

        /**
         * @brief Serializes the current configuration state to a JSON-formatted string.
         *
         * This method converts the internal configuration data into a string
         * representation using the JSON format. The serialized output reflects the
         * current state of the configuration, making it suitable for debugging,
         * logging, or exporting purposes.
         *
         * @return A JSON-formatted string representing the serialized configuration.
         */
        [[nodiscard]]
        std::string dump() const;

        /**
         * @brief Retrieves the file path associated with the configuration.
         *
         * This method returns the file path currently used by the configuration
         * manager. The file path typically points to the location of the
         * configuration file on the filesystem.
         *
         * @return The file path as a `std::filesystem::path` object.
         */
        [[nodiscard]]
        std::filesystem::path filePath() const { return _filePath; }

        /**
         * @brief Sets the file path for the associated resource.
         *
         * This method assigns a specific file path that will be used for operations
         * such as loading, saving, or processing the associated resource. The file
         * path is expected to be a valid path that the system has access to.
         *
         * @param path A string representing the absolute or relative path to the file.
         * @throws std::invalid_argument If the provided path is empty or invalid.
         */
        void setFilePath(const std::filesystem::path &path) { _filePath = path; }

        /**
         * @brief Retrieves the unique identifier of an account.
         *
         * This method provides access to the account's unique identifier, which is typically
         * used for identifying and interacting with the account in a system.
         *
         * @return The unique identifier of the account as a string.
         */
        [[nodiscard]]
        std::string getAccountId() const { return get<std::string>("awsmock.access.account-id"); }

        /**
         * @brief Retrieves the AWS region configured in the system.
         *
         * This method fetches the region setting from the configuration using the key "awsmock.region".
         * It ensures that the stored value is retrieved as a string.
         *
         * @return A string representing the configured AWS region.
         */
        [[nodiscard]]
        std::string getRegion() const { return get<std::string>("awsmock.region"); }

        /**
         * @brief Retrieves the version of the application or component.
         *
         * This method provides the current version information, which can be used for
         * display or comparison purposes. The version is returned as a string in a
         * predefined format.
         *
         * @return A string representing the version of the application or component.
         */
        static std::string getVersion() { return APP_VERSION; }

        /**
         * @brief Retrieves the name of the application.
         *
         * This method returns the name of the application as a string. The application name is typically
         * configured during initialization and remains constant throughout the application's lifetime.
         *
         * @return A string representing the name of the application.
         */
        static std::string getAppName() { return "awsmockmgr"; }

      private:

        mutable logger_t _logger{boost::log::keywords::channel = "Core"};

        /**
         * @brief Default constructor for the Configuration class.
         *
         * Initializes a Configuration object with the default state.
         */
        Configuration() = default;

        /**
         * @brief Converts a given string into an environment variable-friendly name.
         *
         * This method transforms an input string into a format suitable for use as
         * an environment variable name. It typically involves replacing invalid characters,
         * modifying casing, or applying other adjustments to adhere to conventions
         * and constraints of environment variable naming standards.
         *
         * @param path The string to be converted into an environment variable name.
         * @return A formatted string that conforms to environment variable naming conventions.
         */
        static std::string toEnvVarName(const std::string &path) {
            std::string name;
            name.reserve(path.size());
            for (const char c: path) {
                name += (c == '.') ? '_' : static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
            }
            return name;
        }

        /**
         * @brief Returns the env-var value parsed as T, or nullopt if the var is unset.
         *
         * @param path configuration node path
         */
        template<typename T>
        static std::optional<T> envOverride(const std::string &path) { return std::nullopt; }

        /**
         * @brief Resolves a JSON value by navigating a hierarchical path in the configuration data.
         *
         * This method traverses the configuration tree based on a dotted path string, returning
         * a pointer to the JSON value at the specified location. If the path is invalid or points
         * to a non-existent key, the method returns nullptr.
         *
         * @param path The dotted path string used to locate a value in the hierarchical configuration.
         *             Each segment is separated by a dot (e.g., "foo.bar.baz").
         * @return A pointer to the `boost::json::value` found at the specified path, or nullptr
         *         if the path cannot be resolved.
         */
        [[nodiscard]]
        const boost::json::value *resolvePath(const std::string &path) const noexcept;

        /**
         * @brief Resolves or creates a nested JSON object path within the configuration.
         *
         * This method traverses the JSON structure starting from the root, using the
         * provided path. Each segment of the path represents a key in the JSON object.
         * If a key does not exist, it will create a nested object for that segment.
         * The resulting JSON value (at the end of the path) is returned as a pointer.
         *
         * @param path The dot-separated path representing the hierarchy to resolve or create.
         * @return A pointer to the nested JSON value at the specified path.
         */
        boost::json::value *resolveOrCreatePath(const std::string &path);

        /**
         * @brief Extracts a value of type T from a JSON object using a specified path.
         *
         * This method traverses a JSON object based on the provided path and attempts
         * to extract the value at the specified location. The path uses a dot-separated
         * notation to denote hierarchy within the JSON structure.
         *
         * @tparam T The type to which the extracted value will be converted.
         * @param v The JSON object from which the value will be extracted.
         * @param path The dot-separated path specifying the location of the value to be extracted.
         * @return The value extracted from the JSON object, converted to the specified type T.
         */
        template<typename T>
        static T extractValue(const boost::json::value &v, const std::string &path);

        /**
         * @brief Formats a JSON value into a human-readable, pretty-printed string.
         *
         * This method converts the supplied JSON value into a string representation
         * with proper indentation for better readability. The formatting is applied
         * recursively to all nested JSON objects and arrays.
         *
         * @param root The root JSON value to be pretty-printed.
         * @return A pretty-printed string representation of the given JSON value.
         */
        static std::string prettyPrint(const boost::json::value &root);

        /**
         * @brief Outputs a formatted, human-readable representation of a JSON value to a stream.
         *
         * This method recursively processes a JSON value, formatting it with proper indentation
         * to improve readability. It handles objects, arrays, and scalar values, adjusting
         * the indentation for nested structures.
         *
         * @param v The JSON value to be pretty-printed.
         * @param out The output stream where the formatted JSON will be written.
         * @param indent The current indentation level (used internally for recursive calls).
         */
        static void prettyPrintValue(const boost::json::value &v, std::ostream &out, int indent);

        /**
         * @brief Internal storage for the hierarchical configuration data.
         *
         * The `_root` variable holds the primary JSON structure representing all configuration
         * details of the application. It serves as the core data container for parsing,
         * accessing, and manipulating configuration settings. This variable is typically
         * utilized as the central point for managing serialized configuration input and output.
         */
        boost::json::value _root{};

        /**
         * @brief Stores the file system path used for configuration purposes.
         *
         * The `_filePath` variable holds the path to a file used by the application
         * for storing or retrieving configuration settings. It uses the `std::filesystem::path`
         * type to ensure compatibility and ease of interaction with filesystem operations.
         */
        std::filesystem::path _filePath;
    };

    template<>
    inline int Configuration::extractValue<int>(const boost::json::value &v, const std::string &path) {
        if (v.is_int64()) return static_cast<int>(v.get_int64());
        if (v.is_uint64()) return static_cast<int>(v.get_uint64());
        if (v.is_double()) return static_cast<int>(v.get_double());
        throw std::runtime_error("Config key '" + path + "' is not an integer");
    }

    template<>
    inline unsigned int Configuration::extractValue<unsigned int>(const boost::json::value &v, const std::string &path) {
        if (v.is_int64()) return static_cast<unsigned int>(v.get_int64());
        if (v.is_uint64()) return static_cast<unsigned int>(v.get_uint64());
        if (v.is_double()) return static_cast<unsigned int>(v.get_double());
        throw std::runtime_error("Config key '" + path + "' is not an unsigned integer");
    }

    template<>
    inline long Configuration::extractValue<long>(const boost::json::value &v, const std::string &path) {
        if (v.is_int64()) return v.get_int64();
        if (v.is_uint64()) return static_cast<long>(v.get_uint64());
        if (v.is_double()) return static_cast<long>(v.get_double());
        throw std::runtime_error("Config key '" + path + "' is not a long");
    }

    template<>
    inline double Configuration::extractValue<double>(const boost::json::value &v, const std::string &path) {
        if (v.is_double()) return v.get_double();
        if (v.is_int64()) return static_cast<double>(v.get_int64());
        if (v.is_uint64()) return static_cast<double>(v.get_uint64());
        throw std::runtime_error("Config key '" + path + "' is not a double");
    }

    template<>
    inline bool Configuration::extractValue<bool>(const boost::json::value &v, const std::string &path) {
        if (v.is_bool()) return v.get_bool();
        throw std::runtime_error("Config key '" + path + "' is not a bool");
    }

    template<>
    inline std::string Configuration::extractValue<std::string>(const boost::json::value &v, const std::string &path) {
        if (v.is_string()) return std::string(v.get_string());
        if (v.is_int64()) return std::to_string(v.get_int64());
        if (v.is_uint64()) return std::to_string(v.get_uint64());
        if (v.is_double()) return std::to_string(v.get_double());
        if (v.is_bool()) return v.get_bool() ? "true" : "false";
        throw std::runtime_error("Config key '" + path + "' is not a string");
    }

    inline std::vector<std::map<std::string, std::string>> Configuration::getArrayOfObjects(const std::string &path) const {
        const auto *node = resolvePath(path);
        if (!node || !node->is_array()) return {};
        std::vector<std::map<std::string, std::string>> result;
        for (const auto &elem: node->get_array()) {
            if (!elem.is_object()) continue;
            std::map<std::string, std::string> obj;
            for (const auto &kv: elem.get_object()) {
                try {
                    obj[std::string(kv.key())] = extractValue<std::string>(kv.value(), std::string(kv.key()));
                } catch (...) {}
            }
            result.push_back(std::move(obj));
        }
        return result;
    }

    template<>
    inline void Configuration::set<int>(const std::string &p, const int v) {
        *resolveOrCreatePath(p) = boost::json::value(static_cast<std::int64_t>(v));
    }

    template<>
    inline void Configuration::set<long>(const std::string &p, const long v) {
        *resolveOrCreatePath(p) = boost::json::value(v);
    }

    template<>
    inline void Configuration::set<double>(const std::string &p, const double v) {
        *resolveOrCreatePath(p) = boost::json::value(v);
    }

    template<>
    inline void Configuration::set<bool>(const std::string &p, const bool v) {
        *resolveOrCreatePath(p) = boost::json::value(v);
    }

    template<>
    inline void Configuration::set<std::string>(const std::string &p, const std::string v) {
        *resolveOrCreatePath(p) = boost::json::value(boost::json::string_view(v));
    }

    // â”€â”€ envOverride specializations â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    template<>
    inline std::optional<std::string> Configuration::envOverride<std::string>(const std::string &path) {
        const char *val = std::getenv(toEnvVarName(path).c_str());
        if (!val) return std::nullopt;
        return std::string(val);
    }

    template<>
    inline std::optional<int> Configuration::envOverride<int>(const std::string &path) {
        const char *val = std::getenv(toEnvVarName(path).c_str());
        if (!val) return std::nullopt;
        try {
            return std::stoi(val);
        } catch (...) {
            return std::nullopt;
        }
    }

    template<>
    inline std::optional<unsigned int> Configuration::envOverride<unsigned int>(const std::string &path) {
        const char *val = std::getenv(toEnvVarName(path).c_str());
        if (!val) return std::nullopt;
        try {
            return static_cast<unsigned int>(std::stoul(val));
        } catch (...) {
            return std::nullopt;
        }
    }

    template<>
    inline std::optional<long> Configuration::envOverride<long>(const std::string &path) {
        const char *val = std::getenv(toEnvVarName(path).c_str());
        if (!val) return std::nullopt;
        try {
            return std::stol(val);
        } catch (...) {
            return std::nullopt;
        }
    }

    template<>
    inline std::optional<double> Configuration::envOverride<double>(const std::string &path) {
        const char *val = std::getenv(toEnvVarName(path).c_str());
        if (!val) return std::nullopt;
        try {
            return std::stod(val);
        } catch (...) {
            return std::nullopt;
        }
    }

    template<>
    inline std::optional<bool> Configuration::envOverride<bool>(const std::string &path) {
        const char *val = std::getenv(toEnvVarName(path).c_str());
        if (!val) return std::nullopt;
        std::string s(val);
        std::ranges::transform(s, s.begin(), [](unsigned char c) { return std::tolower(c); });
        if (s == "1" || s == "true" || s == "yes" || s == "on") return true;
        if (s == "0" || s == "false" || s == "no" || s == "off") return false;
        return std::nullopt;
    }

}// namespace Awsmock::Core
