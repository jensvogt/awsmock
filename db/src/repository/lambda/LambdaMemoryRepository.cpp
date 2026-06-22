//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/repository/lambda/LambdaMemoryRepository.h>

namespace Awsmock::Database {

    boost::mutex LambdaMemoryRepository::_lambdaMutex;
    boost::mutex LambdaMemoryRepository::_lambdaResultMutex;

    bool LambdaMemoryRepository::lambdaExists(const std::string &function) const {

        return std::ranges::find_if(_lambdas,
                                    [function](const std::pair<std::string, Entity::Lambda::Lambda> &lambda) {
                                        return lambda.second.function == function;
                                    }) != _lambdas.end();
    }

    bool LambdaMemoryRepository::lambdaExists(const Entity::Lambda::Lambda &lambda) const {

        std::string region = lambda.region;
        std::string function = lambda.function;
        return std::ranges::find_if(_lambdas,
                                    [region, function](const std::pair<std::string, Entity::Lambda::Lambda> &lambda) {
                                        return lambda.second.region == region && lambda.second.function == function;
                                    }) != _lambdas.end();
    }

    bool LambdaMemoryRepository::lambdaExists(const std::string &region, const std::string &function, const std::string &runtime) const {

        return std::ranges::find_if(_lambdas,
                                    [region, function, runtime](const std::pair<std::string, Entity::Lambda::Lambda> &lambda) {
                                        return lambda.second.region == region && lambda.second.function == function && lambda.second.runtime == runtime;
                                    }) != _lambdas.end();
    }

    bool LambdaMemoryRepository::lambdaExistsByArn(const std::string &arn) const {

        return std::ranges::find_if(_lambdas, [arn](const std::pair<std::string, Entity::Lambda::Lambda> &lambda) {
                   return lambda.second.arn == arn;
               }) != _lambdas.end();
    }

    Entity::Lambda::LambdaList LambdaMemoryRepository::listLambdasWithEventSource(const std::string &eventSourceArn) const {

        auto q = Core::from(_lambdas | std::views::values | std::ranges::to<std::vector>());
        if (!eventSourceArn.empty()) {
            q = q.where([eventSourceArn](const Entity::Lambda::Lambda &lambda) { return lambda.HasEventSource(eventSourceArn); });
        }
        return q.to_vector();
    }

    Entity::Lambda::LambdaList LambdaMemoryRepository::listLambdas(const std::string &region) const {

        auto q = Core::from(_lambdas | std::views::values | std::ranges::to<std::vector>());
        if (!region.empty()) {
            q = q.where([region](const Entity::Lambda::Lambda &lambda) { return lambda.region == region; });
        }
        return q.to_vector();
    }

    std::vector<Entity::Lambda::Lambda> LambdaMemoryRepository::listLambdaCounters(const std::string &region, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        auto q = Core::from(_lambdas | std::views::values | std::ranges::to<std::vector>());
        if (!region.empty()) {
            q = q.where([region](const Entity::Lambda::Lambda &item) { return item.region == region; });
        }
        if (!prefix.empty()) {
            q = q.where([prefix](const Entity::Lambda::Lambda &item) { return Core::StringUtils::StartsWith(item.function, prefix); });
        }

        q = q.order_by([](const Entity::Lambda::Lambda &key1, const Entity::Lambda::Lambda &key2) { return key1.oid < key2.oid; });

        for (const auto &sc: sortColumns) {
            if (sc.column == "name") {
                q = q.order_by([](const Entity::Lambda::Lambda &key1, const Entity::Lambda::Lambda &key2) { return key1.function < key2.function; });
            }
        }

        // Create page iterators
        return Core::PageVector<Entity::Lambda::Lambda>(q.to_vector(), pageSize, pageIndex);
    }

    Entity::Lambda::Lambda LambdaMemoryRepository::createLambda(Entity::Lambda::Lambda &lambda) const {
        boost::mutex::scoped_lock lock(_lambdaMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _lambdas[oid] = lambda;
        log_trace << "Lambda created, oid: " << oid;
        return getLambdaById(oid);
    }

    Entity::Lambda::Lambda LambdaMemoryRepository::createOrUpdateLambda(Entity::Lambda::Lambda &lambda) const {
        if (lambdaExists(lambda)) {
            return updateLambda(lambda);
        }
        return createLambda(lambda);
    }

    Entity::Lambda::Lambda LambdaMemoryRepository::getLambdaById(const std::string &oid) const {

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

    Entity::Lambda::Lambda LambdaMemoryRepository::getLambdaById(const bsoncxx::oid &oid) const {
        return getLambdaById(oid.to_string());
    }

    Entity::Lambda::Lambda LambdaMemoryRepository::getLambdaByArn(const std::string &arn) const {

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

    Entity::Lambda::Lambda LambdaMemoryRepository::getLambdaByName(const std::string &region, const std::string &name) const {

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

    long LambdaMemoryRepository::lambdaCount(const std::string &region) const {

        auto q = Core::from(_lambdas | std::views::values | std::ranges::to<std::vector>());
        if (!region.empty()) {
            q = q.where([region](const Entity::Lambda::Lambda &item) { return item.region == region; });
        }
        return q.count();
    }

    Entity::Lambda::Lambda LambdaMemoryRepository::updateLambda(Entity::Lambda::Lambda &lambda) const {
        boost::mutex::scoped_lock lock(_lambdaMutex);

        std::string region = lambda.region;
        std::string function = lambda.function;
        const auto it = std::ranges::find_if(_lambdas,
                                             [lambda](const std::pair<std::string, Entity::Lambda::Lambda> &l) {
                                                 return l.second.region == lambda.region && l.second.function == lambda.function;
                                             });

        if (it == _lambdas.end()) {
            log_error << "Update lambda failed, region: " << lambda.region << " function: " << lambda.function;
            throw Core::DatabaseException("Update lambda failed, region: " + lambda.region + " function: " + lambda.function);
        }
        _lambdas[it->first] = lambda;
        return _lambdas[it->first];
    }

    Entity::Lambda::Lambda LambdaMemoryRepository::importLambda(Entity::Lambda::Lambda &lambda) const {
        if (lambdaExists(lambda)) {
            const Entity::Lambda::Lambda existing = getLambdaByArn(lambda.arn);
            lambda.modified = system_clock::now();
            lambda.instances = existing.instances;
            return updateLambda(lambda);
        }
        return createLambda(lambda);
    }

    std::vector<Entity::Lambda::Lambda> LambdaMemoryRepository::exportLambdas(const std::vector<SortColumn> &sortColumns) const {
        return _lambdas | std::views::values | std::ranges::to<std::vector>();
    }

    void LambdaMemoryRepository::setInstanceValues(const std::string &containerId, const Entity::Lambda::RuntimeStatus &status) const {
        boost::mutex::scoped_lock lock(_lambdaMutex);

        for (auto &val: _lambdas | std::views::values) {
            for (auto &instance: val.instances) {
                if (instance.containerId == containerId) {
                    instance.status = status;
                    instance.lastInvocation = system_clock::now();
                }
            }
        }
    }

    void LambdaMemoryRepository::setLambdaValues(const Entity::Lambda::Lambda &lambda, long invocations, long avgRuntime) const {
        boost::mutex::scoped_lock lock(_lambdaMutex);

        for (auto &val: _lambdas | std::views::values) {
            val.invocations = invocations;
            val.averageRuntime = avgRuntime;
        }
    }

    Entity::Lambda::LambdaResult LambdaMemoryRepository::createLambdaResult(Entity::Lambda::LambdaResult &lambdaResult) const {
        boost::mutex::scoped_lock lock(_lambdaResultMutex);

        lambdaResult.oid = Core::StringUtils::CreateRandomUuid();
        _lambdaResults[lambdaResult.oid] = lambdaResult;
        log_trace << "Lambda created, oid: " << lambdaResult.oid;
        return _lambdaResults[lambdaResult.oid];
    }

    bool LambdaMemoryRepository::lambdaResultExists(const std::string &oid) const {
        return std::ranges::find_if(_lambdaResults, [oid](const std::pair<std::string, Entity::Lambda::LambdaResult> &lambdaResult) {
                   return lambdaResult.first == oid;
               }) != _lambdaResults.end();
    }

    Entity::Lambda::LambdaResult LambdaMemoryRepository::getLambdaResultCounter(const std::string &oid) const {
        const auto it = std::ranges::find_if(_lambdaResults, [oid](const std::pair<std::string, Entity::Lambda::LambdaResult> &lambdaResult) {
            return lambdaResult.first == oid;
        });
        if (it == _lambdaResults.end()) {
            log_error << "Get lambda result by ID failed, oid: " << oid;
            throw Core::DatabaseException("Get lambda result by ID failed, oid: " + oid);
        }
        it->second.oid = oid;
        return it->second;
    }

    long LambdaMemoryRepository::deleteResultsCounter(const std::string &oid) const {
        boost::mutex::scoped_lock lock(_lambdaResultMutex);

        const auto count = std::erase_if(_lambdas, [oid](const auto &l) {
            return l.first == oid;
        });
        log_debug << "Lambda results deleted, count: " << count;
        return count;
    }

    long LambdaMemoryRepository::deleteResultsCounters(const std::string &lambdaArn) const {
        boost::mutex::scoped_lock lock(_lambdaResultMutex);

        const auto count = std::erase_if(_lambdas, [lambdaArn](const auto &l) {
            return l.second.arn == lambdaArn;
        });
        log_debug << "Lambda results deleted, count: " << count;
        return static_cast<long>(count);
    }

    long LambdaMemoryRepository::deleteAllResultsCounters() const {
        boost::mutex::scoped_lock lock(_lambdaResultMutex);
        const long deleted = static_cast<long>(_lambdaResults.size());
        log_debug << "All lambda results deleted, count: " << deleted;
        _lambdaResults.clear();
        return deleted;
    }

    long LambdaMemoryRepository::removeExpiredLambdaLogs(const system_clock::time_point &cutOff) const {
        boost::mutex::scoped_lock lock(_lambdaMutex);

        const auto count = std::erase_if(_lambdaResults, [cutOff](const auto &l) {
            return l.second.timestamp < cutOff;
        });
        log_debug << "Lambda results deleted, count: " << count;
        return static_cast<long>(count);
    }

    void LambdaMemoryRepository::deleteLambda(const std::string &functionName) const {
        boost::mutex::scoped_lock lock(_lambdaMutex);

        const auto count = std::erase_if(_lambdas, [functionName](const auto &l) {
            return l.second.function == functionName;
        });
        log_debug << "Lambda deleted, count: " << count;
    }

    long LambdaMemoryRepository::deleteAllLambdas() const {
        boost::mutex::scoped_lock lock(_lambdaMutex);
        const long deleted = static_cast<long>(_lambdas.size());
        log_debug << "All lambdas deleted, count: " << _lambdas.size();
        _lambdas.clear();
        return deleted;
    }

    std::vector<Entity::Lambda::LambdaResult> LambdaMemoryRepository::listLambdaResultCounters(const std::string &lambdaArn, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        auto q = Core::from(_lambdaResults | std::views::values | std::ranges::to<std::vector>());
        if (!lambdaArn.empty()) {
            q = q.where([lambdaArn](const Entity::Lambda::LambdaResult &item) { return item.lambdaArn == lambdaArn; });
        }
        if (!prefix.empty()) {
            q = q.where([prefix](const Entity::Lambda::LambdaResult &item) { return Core::StringUtils::StartsWith(item.lambdaArn, prefix); });
        }
        return Core::PageVector<Entity::Lambda::LambdaResult>(q.to_vector(), pageSize, pageIndex);
    }

    long LambdaMemoryRepository::lambdaResultsCount(const std::string &lambdaArn) const {

        auto q = Core::from(_lambdaResults | std::views::values | std::ranges::to<std::vector>());
        if (!lambdaArn.empty()) {
            q = q.where([lambdaArn](const Entity::Lambda::LambdaResult &item) { return item.lambdaArn == lambdaArn; });
        }
        return q.count();
    }
}// namespace Awsmock::Database
