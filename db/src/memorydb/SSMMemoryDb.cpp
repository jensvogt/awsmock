//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/memorydb/SSMMemoryDb.h>

namespace AwsMock::Database {

    boost::mutex SSMMemoryDb::_parameterMutex;

    bool SSMMemoryDb::ParameterExists(const std::string &name) {

        return std::ranges::find_if(_parameters, [name](const std::pair<std::string, Entity::SSM::Parameter> &topic) {
                   return topic.second.parameterName == name;
               }) != _parameters.end();
    }

    Entity::SSM::Parameter SSMMemoryDb::GetParameterById(const std::string &oid) {

        const auto it = std::ranges::find_if(_parameters, [oid](const std::pair<std::string, Entity::SSM::Parameter> &topic) {
            return topic.first == oid;
        });

        if (it != _parameters.end()) {
            it->second.oid = oid;
            return it->second;
        }

        log_warning << "Parameter not found, oid: " << oid;
        return {};
    }

    Entity::SSM::Parameter SSMMemoryDb::GetParameterByName(const std::string &name) {

        const auto it = std::ranges::find_if(_parameters, [name](const std::pair<std::string, Entity::SSM::Parameter> &topic) {
            return topic.second.parameterName == name;
        });

        if (it != _parameters.end()) {
            it->second.oid = it->first;
            return it->second;
        }

        log_warning << "Parameter not found, name: " << name;
        return {};
    }

    Entity::SSM::ParameterList SSMMemoryDb::ListParameters(const std::string &region) const {

        Entity::SSM::ParameterList parameterList;

        if (region.empty()) {

            for (const auto &val: _parameters | std::views::values) {
                parameterList.emplace_back(val);
            }

        } else {

            for (const auto &val: _parameters | std::views::values) {
                if (val.region == region) {
                    parameterList.emplace_back(val);
                }
            }
        }

        log_trace << "Got parameter list, size: " << parameterList.size();
        return parameterList;
    }

    Entity::SSM::ParameterList SSMMemoryDb::ListParameterCounters(const std::string &region, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) {
        Entity::SSM::ParameterList result;

        // Get values
        for (auto &val: _parameters | std::views::values) {
            result.push_back(val);
        }

        auto q = Core::from(result);
        q = q.order_by([](const Entity::SSM::Parameter &key1, const Entity::SSM::Parameter &key2) { return key1.oid < key2.oid; });

        for (const auto &[column, sortDirection]: sortColumns) {
            if (column == "name") {
                q = q.order_by([](const Entity::SSM::Parameter &key1, const Entity::SSM::Parameter &key2) { return key1.parameterName < key2.parameterName; });
            }
        }

        if (!region.empty()) {
            q.where([region](const Entity::SSM::Parameter &item) { return item.region == region; });
        }
        if (!prefix.empty()) {
            q.where([prefix](const Entity::SSM::Parameter &item) { return Core::StringUtils::StartsWith(item.parameterName, prefix); });
        }
        result = q.to_vector();

        // Create page iterators
        if (pageSize > 0) {
          auto start = result.begin() + pageIndex * pageSize;
          auto end = (pageIndex + 1) * pageSize < result.size() ? result.begin() + (pageIndex + 1) * pageSize : result.end();
          return {start, end};
        }
        return result;
    }

    long SSMMemoryDb::CountParameters() const {

        return static_cast<long>(_parameters.size());
    }

    Entity::SSM::Parameter SSMMemoryDb::CreateParameter(const Entity::SSM::Parameter &topic) {
        boost::mutex::scoped_lock loc(_parameterMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _parameters[oid] = topic;
        log_trace << "Parameter created, oid: " << oid;
        return GetParameterById(oid);
    }

    Entity::SSM::Parameter SSMMemoryDb::UpdateParameter(const Entity::SSM::Parameter &parameter) {
        boost::mutex::scoped_lock lock(_parameterMutex);

        std::string oid = parameter.oid;
        const auto it = std::ranges::find_if(_parameters,
                                             [oid](const std::pair<std::string, Entity::SSM::Parameter> &parameter) {
                                                 return parameter.second.oid == oid;
                                             });
        if (it != _parameters.end()) {
            _parameters[it->first] = parameter;
            return _parameters[it->first];
        }
        log_warning << "Parameter not found, oid: " << oid;
        return parameter;
    }

    void SSMMemoryDb::DeleteParameter(const Entity::SSM::Parameter &parameter) {
        boost::mutex::scoped_lock lock(_parameterMutex);

        std::string oid = parameter.oid;
        const auto count = std::erase_if(_parameters, [oid](const auto &item) {
            auto const &[k, v] = item;
            return k == oid;
        });
        log_debug << "Parameter deleted, count: " << count;
    }

    long SSMMemoryDb::DeleteAllParameters() {
        boost::mutex::scoped_lock lock(_parameterMutex);
        const long deleted = _parameters.size();
        _parameters.clear();
        log_debug << "All SSM parameters deleted, count: " << deleted;
        return deleted;
    }

}// namespace AwsMock::Database