//
// Created by vogje01 on 5/23/26.
//

#include <awsmock/core/config/Configuration.h>

namespace Awsmock::Core {

    // ── Load / Reload ─────────────────────────────────────────────────────────

    void Configuration::load(const std::filesystem::path &path) {
        if (!std::filesystem::exists(path)) {
            log_error << "Config file not found: " << path.string();
            throw CoreException("Config file not found: " + path.string());
        }

        const std::ifstream file(path);
        if (!file.is_open()) {
            log_error << "Cannot open config file: " << path.string();
            throw CoreException("Cannot open config file: " + path.string());
        }

        std::ostringstream buf;
        buf << file.rdbuf();

        boost::json::parse_options opts;
        opts.allow_comments = true;
        opts.allow_trailing_commas = true;

        boost::system::error_code ec;
        _root = boost::json::parse(buf.str(), ec, {}, opts);

        if (ec) {
            log_error << "JSON parse error in " << path.string() << ": " << ec.message();
        }

        _filePath = path;
    }

    void Configuration::reload() {
        if (_filePath.empty()) {
            log_error << "No config file loaded yet";
            throw CoreException("No config file loaded yet");
        }
        load(_filePath);
    }

    // ── Path resolution ───────────────────────────────────────────────────────

    static std::vector<std::string> splitPath(const std::string &path) {
        std::vector<std::string> segments;
        std::string seg;
        for (const char c: path) {
            if (c == '.') {
                if (!seg.empty()) segments.push_back(seg);
                seg.clear();
            } else {
                seg += c;
            }
        }
        if (!seg.empty()) segments.push_back(seg);
        return segments;
    }

    const boost::json::value *
    Configuration::resolvePath(const std::string &path) const noexcept {
        const boost::json::value *cur = &_root;

        for (const auto &seg: splitPath(path)) {
            if (!cur->is_object()) return nullptr;
            const auto &obj = cur->get_object();
            auto it = obj.find(seg);
            if (it == obj.end()) return nullptr;
            cur = &it->value();
        }
        return cur;
    }

    boost::json::value *
    Configuration::resolveOrCreatePath(const std::string &path) {
        boost::json::value *cur = &_root;

        // Ensure root is an object
        if (!cur->is_object()) *cur = boost::json::object{};

        for (const auto &seg: splitPath(path)) {
            auto &obj = cur->get_object();
            auto it = obj.find(seg);
            if (it == obj.end()) {
                obj.emplace(seg, boost::json::object{});
                it = obj.find(seg);
            }
            cur = &it->value();
        }
        return cur;
    }

    static ConfigValue toConfigValue(const boost::json::value &v, const std::string &path) {
        switch (v.kind()) {
            case boost::json::kind::bool_:
                return v.get_bool();

            case boost::json::kind::int64:
                return static_cast<long>(v.get_int64());

            case boost::json::kind::uint64:
                return static_cast<long>(v.get_uint64());

            case boost::json::kind::double_:
                return v.get_double();

            case boost::json::kind::string:
                return std::string(v.get_string());

            case boost::json::kind::object: {
                // Recursively convert nested objects
                auto obj = std::make_shared<ConfigObject>();
                for (const auto &[k, val]: v.get_object()) {
                    const std::string childPath = path + "." + std::string(k);
                    obj->values.emplace(std::string(k), toConfigValue(val, childPath));
                }
                return obj;
            }

            case boost::json::kind::array:
                // Store arrays as string representation for now
                return boost::json::serialize(v);

            case boost::json::kind::null:
                return std::string("null");

            default:
                throw std::runtime_error("Config key '" + path + "' has unsupported type");
        }
    }

    std::vector<std::string> Configuration::getKeys(const std::string &path) const noexcept {
        const auto *node = resolvePath(path);
        if (!node || !node->is_object()) return {};

        std::vector<std::string> keys;
        const auto &object = node->get_object();
        keys.reserve(object.size());
        for (const auto &[key, value]: object) keys.emplace_back(key);
        return keys;
    }

    std::map<std::string, ConfigValue> Configuration::getObject(const std::string &path) const {
        const auto *node = resolvePath(path);
        if (!node) throw std::runtime_error("Config key not found: " + path);
        if (!node->is_object()) throw std::runtime_error("Config key '" + path + "' is not an object");

        std::map<std::string, ConfigValue> result;
        for (const auto &[key, value]: node->get_object()) {
            const std::string childPath = path + "." + std::string(key);
            result.emplace(std::string(key), toConfigValue(value, childPath));
        }
        return result;
    }

    std::vector<std::pair<std::string, std::map<std::string, ConfigValue>>>
    Configuration::getObjects(const std::string &path) const {
        const auto *node = resolvePath(path);
        if (!node) throw std::runtime_error("Config key not found: " + path);
        if (!node->is_object()) throw std::runtime_error("Config key '" + path + "' is not an object");

        std::vector<std::pair<std::string, std::map<std::string, ConfigValue>>> result;
        const auto &object = node->get_object();
        result.reserve(object.size());
        for (const auto &[key, value]: object) {
            const std::string name(key);
            const std::string childPath = path + "." + name;
            if (!value.is_object()) throw std::runtime_error("Config key '" + childPath + "' is not an object");

            std::map<std::string, ConfigValue> values;
            for (const auto &[childKey, childValue]: value.get_object()) {
                const std::string valuePath = childPath + "." + std::string(childKey);
                values.emplace(std::string(childKey), toConfigValue(childValue, valuePath));
            }
            result.emplace_back(name, std::move(values));
        }
        return result;
    }

    // ── Dump ──────────────────────────────────────────────────────────────────

    std::string Configuration::dump() const {
        return boost::json::serialize(_root);
    }

    // ── Save ──────────────────────────────────────────────────────────────────

    void Configuration::save() const {
        if (_filePath.empty()) {
            throw std::runtime_error("No config file loaded, use saveTo(path) instead");
        }
        saveTo(_filePath);
    }

    void Configuration::saveTo(const std::filesystem::path &path) const {

        // Create parent directories if they don't exist
        if (path.has_parent_path()) std::filesystem::create_directories(path.has_parent_path() ? path.parent_path() : std::filesystem::path{"."});

        std::ofstream file(path, std::ios::out | std::ios::trunc);
        if (!file.is_open()) throw std::runtime_error("Cannot open config file for writing: " + path.string());

        // Pretty-print with indentation
        file << prettyPrint(_root);

        if (file.fail()) {
            throw std::runtime_error("Write failed for config file: " + path.string());
        }
    }

    void Configuration::prettyPrintValue(const boost::json::value &v, std::ostream &out, int indent) {

        const std::string pad(indent * 2, ' ');
        const std::string childPad((indent + 1) * 2, ' ');

        switch (v.kind()) {

            case boost::json::kind::object: {
                const auto &obj = v.get_object();
                if (obj.empty()) {
                    out << "{}";
                    return;
                }
                out << "{\n";
                std::size_t i = 0;
                for (const auto &[key, val]: obj) {
                    out << childPad << boost::json::serialize(boost::json::value(key)) << ": ";
                    prettyPrintValue(val, out, indent + 1);
                    if (++i < obj.size()) out << ",";
                    out << "\n";
                }
                out << pad << "}";
                break;
            }

            case boost::json::kind::array: {
                const auto &arr = v.get_array();
                if (arr.empty()) {
                    out << "[]";
                    return;
                }
                out << "[\n";
                for (std::size_t i = 0; i < arr.size(); ++i) {
                    out << childPad;
                    prettyPrintValue(arr[i], out, indent + 1);
                    if (i + 1 < arr.size()) out << ",";
                    out << "\n";
                }
                out << pad << "]";
                break;
            }

            default:
                // Scalar — let Boost handle serialization
                out << boost::json::serialize(v);
                break;
        }
    }

    std::string Configuration::prettyPrint(const boost::json::value &root) {
        std::ostringstream out;
        prettyPrintValue(root, out, 0);
        out << "\n";
        return out.str();
    }

}// namespace Awsmock::Core
