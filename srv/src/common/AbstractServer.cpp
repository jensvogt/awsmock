//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/common/AbstractServer.h>

namespace AwsMock::Service {

    AbstractServer::AbstractServer(std::string name) : _name(std::move(name)), _moduleDatabase(Database::ModuleDatabase::instance()) {
    }

    bool AbstractServer::IsActive(const std::string &name) const {
        return _moduleDatabase.IsActive(name);
    }

    void AbstractServer::SetRunning() const {
        _moduleDatabase.SetState(_name, Database::Entity::Module::ModuleState::RUNNING);
    }

    void AbstractServer::SetStopped() const {
        _moduleDatabase.SetState(_name, Database::Entity::Module::ModuleState::STOPPED);
    }

    bool AbstractServer::IsRunning() const {
        return _moduleDatabase.GetState(_name) == Database::Entity::Module::ModuleState::RUNNING;
    }

    void AbstractServer::Shutdown() {
    }


}// namespace AwsMock::Service
