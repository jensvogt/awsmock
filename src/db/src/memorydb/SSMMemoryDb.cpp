//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/memorydb/SSMMemoryDb.h>

namespace AwsMock::Database {

    Poco::Mutex SSMMemoryDb::_parameterMutex;

    bool SSMMemoryDb::ParameterExists(const std::string &name) {

        return find_if(_parameters.begin(), _parameters.end(), [name](const std::pair<std::string, Entity::SSM::Parameter> &topic) {
                   return topic.second.parameterName == name;
               }) != _parameters.end();
    }

    Entity::SSM::Parameter SSMMemoryDb::GetParameterById(const std::string &oid) {

        auto it = find_if(_parameters.begin(), _parameters.end(), [oid](const std::pair<std::string, Entity::SSM::Parameter> &topic) {
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

        auto it = find_if(_parameters.begin(), _parameters.end(), [name](const std::pair<std::string, Entity::SSM::Parameter> &topic) {
            return topic.second.parameterName == name;
        });

        if (it != _parameters.end()) {
            it->second.oid = it->first;
            return it->second;
        }

        log_warning << "Parameter not found, name: " << name;
        return {};
    }

    Entity::SSM::ParameterList SSMMemoryDb::ListParameters(const std::string &region) {

        Entity::SSM::ParameterList parameterList;

        if (region.empty()) {

            for (const auto &parameter: _parameters) {
                parameterList.emplace_back(parameter.second);
            }

        } else {

            for (const auto &parameter: _parameters) {
                if (parameter.second.region == region) {
                    parameterList.emplace_back(parameter.second);
                }
            }
        }

        log_trace << "Got parameter list, size: " << parameterList.size();
        return parameterList;
    }

    long SSMMemoryDb::CountParameters() {

        return (long) _parameters.size();
    }

    Entity::SSM::Parameter SSMMemoryDb::CreateParameter(const Entity::SSM::Parameter &topic) {
        Poco::ScopedLock loc(_parameterMutex);

        std::string oid = Poco::UUIDGenerator().createRandom().toString();
        _parameters[oid] = topic;
        log_trace << "Parameter created, oid: " << oid;
        return GetParameterById(oid);
    }

    Entity::SSM::Parameter SSMMemoryDb::UpdateParameter(const Entity::SSM::Parameter &parameter) {
        Poco::ScopedLock lock(_parameterMutex);

        std::string oid = parameter.oid;
        auto it = find_if(_parameters.begin(),
                          _parameters.end(),
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
        Poco::ScopedLock lock(_parameterMutex);

        std::string oid = parameter.oid;
        const auto count = std::erase_if(_parameters, [oid](const auto &item) {
            auto const &[k, v] = item;
            return k == oid;
        });
        log_debug << "Parameter deleted, count: " << count;
    }

    void SSMMemoryDb::DeleteAllParameters() {
        Poco::ScopedLock lock(_parameterMutex);
        _parameters.clear();
        log_debug << "All ssm parameters deleted";
    }

}// namespace AwsMock::Database