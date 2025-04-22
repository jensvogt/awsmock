//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/memorydb/LambdaMemoryDb.h>

namespace AwsMock::Database {

    boost::mutex LambdaMemoryDb::_lambdaMutex;

    bool LambdaMemoryDb::LambdaExists(const std::string &function) {

        return std::ranges::find_if(_lambdas,
                                    [function](const std::pair<std::string, Entity::Lambda::Lambda> &lambda) {
                                        return lambda.second.function == function;
                                    }) != _lambdas.end();
    }

    bool LambdaMemoryDb::LambdaExists(const Entity::Lambda::Lambda &lambda) {

        std::string region = lambda.region;
        std::string function = lambda.function;
        return std::ranges::find_if(_lambdas,
                                    [region, function](const std::pair<std::string, Entity::Lambda::Lambda> &lambda) {
                                        return lambda.second.region == region && lambda.second.function == function;
                                    }) != _lambdas.end();
    }

    bool LambdaMemoryDb::LambdaExists(const std::string &region,
                                      const std::string &function,
                                      const std::string &runtime) {

        return std::ranges::find_if(_lambdas,
                                    [region, function, runtime](const std::pair<std::string, Entity::Lambda::Lambda> &lambda) {
                                        return lambda.second.region == region && lambda.second.function == function && lambda.second.runtime == runtime;
                                    }) != _lambdas.end();
    }

    bool LambdaMemoryDb::LambdaExistsByArn(const std::string &arn) {

        return std::ranges::find_if(_lambdas, [arn](const std::pair<std::string, Entity::Lambda::Lambda> &lambda) {
                   return lambda.second.arn == arn;
               }) != _lambdas.end();
    }

    Entity::Lambda::LambdaList LambdaMemoryDb::ListLambdasWithEventSource(const std::string &eventSourceArn) {

        Entity::Lambda::LambdaList lambdaList;
        for (const auto &val: _lambdas | std::views::values) {
            if (val.HasEventSource(eventSourceArn)) {
                lambdaList.emplace_back(val);
            }
        }

        log_trace << "Got lambda list, size: " << lambdaList.size();
        return lambdaList;
    }

    Entity::Lambda::LambdaList LambdaMemoryDb::ListLambdas(const std::string &region) {

        Entity::Lambda::LambdaList lambdaList;
        if (region.empty()) {
            for (const auto &val: _lambdas | std::views::values) {
                lambdaList.emplace_back(val);
            }
        } else {
            for (const auto &val: _lambdas | std::views::values) {
                if (val.region == region) {
                    lambdaList.emplace_back(val);
                }
            }
        }

        log_trace << "Got lambda list, size: " << lambdaList.size();
        return lambdaList;
    }

    Entity::Lambda::Lambda LambdaMemoryDb::CreateLambda(const Entity::Lambda::Lambda &lambda) {
        boost::mutex::scoped_lock lock(_lambdaMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _lambdas[oid] = lambda;
        log_trace << "Lambda created, oid: " << oid;
        return GetLambdaById(oid);
    }

    Entity::Lambda::Lambda LambdaMemoryDb::GetLambdaById(const std::string &oid) {

        const auto it =
                std::ranges::find_if(_lambdas, [oid](const std::pair<std::string, Entity::Lambda::Lambda> &lambda) {
                    return lambda.first == oid;
                });

        if (it == _lambdas.end()) {
            log_error << "Get lambda by ID failed, arn: " << oid;
            throw Core::DatabaseException("Get lambda by ID failed, arn: " + oid);
        }

        it->second.oid = oid;
        return it->second;
    }

    Entity::Lambda::Lambda LambdaMemoryDb::GetLambdaByArn(const std::string &arn) {

        const auto it =
                std::ranges::find_if(_lambdas, [arn](const std::pair<std::string, Entity::Lambda::Lambda> &lambda) {
                    return lambda.second.arn == arn;
                });

        if (it == _lambdas.end()) {
            log_error << "Get lambda by ARN failed, arn: " << arn;
            throw Core::DatabaseException("Get lambda by ARN failed, arn: " + arn);
        }

        it->second.oid = arn;
        return it->second;
    }

    Entity::Lambda::Lambda LambdaMemoryDb::GetLambdaByName(const std::string &region, const std::string &name) {

        const auto it =
                std::ranges::find_if(_lambdas, [region, name](const std::pair<std::string, Entity::Lambda::Lambda> &lambda) {
                    return lambda.second.region == region && lambda.second.function == name;
                });

        if (it == _lambdas.end()) {
            log_error << "Get lambda by name failed, name: " << name;
            throw Core::DatabaseException("Get lambda by name failed, name: " + name);
        }

        it->second.oid = it->first;
        return it->second;
    }

    long LambdaMemoryDb::LambdaCount(const std::string &region) const {

        long count = 0;
        if (region.empty()) {
            return static_cast<long>(_lambdas.size());
        }
        for (const auto &val: _lambdas | std::views::values) {
            if (val.region == region) {
                count++;
            }
        }
        return count;
    }

    Entity::Lambda::Lambda LambdaMemoryDb::UpdateLambda(const Entity::Lambda::Lambda &lambda) {
        boost::mutex::scoped_lock lock(_lambdaMutex);

        std::string region = lambda.region;
        std::string function = lambda.function;
        const auto it = std::ranges::find_if(_lambdas,
                                             [region, function](const std::pair<std::string, Entity::Lambda::Lambda> &lambda) {
                                                 return lambda.second.region == region && lambda.second.function == function;
                                             });

        if (it == _lambdas.end()) {
            log_error << "Update lambda failed, region: " << lambda.region << " function: " << lambda.function;
            throw Core::DatabaseException("Update lambda failed, region: " + lambda.region + " function: " + lambda.function);
        }
        _lambdas[it->first] = lambda;
        return _lambdas[it->first];
    }

    void LambdaMemoryDb::SetInstanceStatus(const std::string &containerId, const Entity::Lambda::LambdaInstanceStatus &status) {
        boost::mutex::scoped_lock lock(_lambdaMutex);

        for (auto &val: _lambdas | std::views::values) {
            for (auto &instance: val.instances) {
                if (instance.containerId == containerId) {
                    instance.status = status;
                }
            }
        }
    }

    void LambdaMemoryDb::DeleteLambda(const std::string &functionName) {
        boost::mutex::scoped_lock lock(_lambdaMutex);

        const auto count = std::erase_if(_lambdas, [functionName](const auto &item) {
            auto const &[key, value] = item;
            return value.function == functionName;
        });
        log_debug << "Lambda deleted, count: " << count;
    }

    long LambdaMemoryDb::DeleteAllLambdas() {
        boost::mutex::scoped_lock lock(_lambdaMutex);
        const long deleted = _lambdas.size();
        log_debug << "All lambdas deleted, count: " << _lambdas.size();
        _lambdas.clear();
        return deleted;
    }
}// namespace AwsMock::Database