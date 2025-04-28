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