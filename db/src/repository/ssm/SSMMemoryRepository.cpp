//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/repository/ssm/SSMMemoryRepository.h>

namespace Awsmock::Database {

    boost::mutex SSMMemoryRepository::_parameterMutex;

    bool SSMMemoryRepository::parameterExists(const std::string &name) const {

        return std::ranges::find_if(_parameters, [name](const std::pair<std::string, Entity::SSM::Parameter> &topic) {
                   return topic.second.parameterName == name;
               }) != _parameters.end();
    }

    Entity::SSM::Parameter SSMMemoryRepository::getParameterById(const std::string &oid) const {

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

    Entity::SSM::Parameter SSMMemoryRepository::getParameterById(const bsoncxx::oid &oid) const {
        return getParameterById(oid.to_string());
    }

    Entity::SSM::Parameter SSMMemoryRepository::getParameterByName(const std::string &name) const {

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

    Entity::SSM::ParameterList SSMMemoryRepository::listParameters(const std::string &region, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const {
        Entity::SSM::ParameterList result;

        // Get values
        for (auto &val: _parameters | std::views::values) {
            result.push_back(val);
        }

        auto q = Core::from(result);
        q = q.order_by([](const Entity::SSM::Parameter &key1, const Entity::SSM::Parameter &key2) { return key1.oid < key2.oid; });

        for (const auto &sc: sortColumns) {
            if (sc.column == "name") {
                q = q.order_by([](const Entity::SSM::Parameter &key1, const Entity::SSM::Parameter &key2) { return key1.parameterName < key2.parameterName; });
            }
        }

        if (!region.empty()) {
            q = q.where([region](const Entity::SSM::Parameter &item) { return item.region == region; });
        }
        if (!prefix.empty()) {
            q = q.where([prefix](const Entity::SSM::Parameter &item) { return Core::StringUtils::StartsWith(item.parameterName, prefix); });
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

    long SSMMemoryRepository::countParameters(const std::string &region, const std::string &prefix) const {

        Entity::SSM::ParameterList result;
        for (auto &val: _parameters | std::views::values) {
            result.push_back(val);
        }
        auto q = Core::from(result);
        if (!region.empty()) {
            q = q.where([region](const Entity::SSM::Parameter &item) { return item.region == region; });
        }
        if (!prefix.empty()) {
            q = q.where([prefix](const Entity::SSM::Parameter &item) { return Core::StringUtils::StartsWith(item.parameterName, prefix); });
        }
        return static_cast<long>(q.count());
    }

    Entity::SSM::Parameter SSMMemoryRepository::createParameter(Entity::SSM::Parameter &parameter) const {
        boost::mutex::scoped_lock loc(_parameterMutex);

        parameter.oid = Core::StringUtils::CreateRandomUuid();
        _parameters[parameter.oid] = parameter;
        log_trace << "Parameter created, oid: " << parameter.oid;
        return _parameters[parameter.oid];
    }

    Entity::SSM::Parameter SSMMemoryRepository::updateParameter(Entity::SSM::Parameter &parameter) const {
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

    Entity::SSM::Parameter SSMMemoryRepository::importParameter(Entity::SSM::Parameter &parameter) const {
        if (parameterExists(parameter.parameterName)) {
            return updateParameter(parameter);
        }
        return createParameter(parameter);
    }

    long SSMMemoryRepository::deleteParameter(const Entity::SSM::Parameter &parameter) const {
        boost::mutex::scoped_lock lock(_parameterMutex);

        std::string oid = parameter.oid;
        const auto count = std::erase_if(_parameters, [oid](const auto &item) {
            auto const &[k, v] = item;
            return k == oid;
        });
        log_debug << "Parameter deleted, count: " << count;
        return static_cast<long>(count);
    }

    long SSMMemoryRepository::deleteAllParameters() const {
        boost::mutex::scoped_lock lock(_parameterMutex);
        const auto deleted = _parameters.size();
        _parameters.clear();
        log_debug << "All SSM parameters deleted, count: " << deleted;
        return static_cast<long>(deleted);
    }

}// namespace Awsmock::Database