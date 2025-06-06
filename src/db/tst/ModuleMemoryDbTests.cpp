//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_MODULE_MEMORYDB_TEST_H
#define AWMOCK_CORE_MODULE_MEMORYDB_TEST_H

// C++ standard includes
#include <vector>

// AwsMock includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/ModuleDatabase.h>

#define MODULE "test-module"

namespace AwsMock::Database {

    struct ModuleMemoryDbTest {

        ModuleMemoryDbTest() {
            _region = _configuration.GetValue<std::string>("awsmock.region");
        }

        ~ModuleMemoryDbTest() {
            const long count = _moduleDatabase.DeleteAllModules();
            log_debug << "Modules deleted, count: " << count;
        }

        std::string _region;
        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration();
        ModuleDatabase &_moduleDatabase = ModuleDatabase::instance();
    };

    BOOST_FIXTURE_TEST_CASE(ModuleCreateMTest, ModuleMemoryDbTest) {

        // arrange
        Entity::Module::Module module = {.name = MODULE, .state = Entity::Module::ModuleState::RUNNING};

        // act
        const Entity::Module::Module result = _moduleDatabase.CreateModule(module);

        // assert
        BOOST_CHECK_EQUAL(result.name, MODULE);
        BOOST_CHECK_EQUAL(result.state == Entity::Module::ModuleState::RUNNING, true);
    }

    BOOST_FIXTURE_TEST_CASE(ModuleExistsMTest, ModuleMemoryDbTest) {

        // arrange
        Entity::Module::Module module = {.name = MODULE, .state = Entity::Module::ModuleState::RUNNING};
        module = _moduleDatabase.CreateModule(module);

        // act
        const bool result1 = _moduleDatabase.ModuleExists(module.name);
        const bool result2 = _moduleDatabase.ModuleExists("blabla");

        // assert
        BOOST_CHECK_EQUAL(result1, true);
        BOOST_CHECK_EQUAL(result2, false);
    }

    BOOST_FIXTURE_TEST_CASE(ModuleActiveMTest, ModuleMemoryDbTest) {

        // arrange
        Entity::Module::Module module =
                {.name = MODULE, .state = Entity::Module::ModuleState::RUNNING, .status = Entity::Module::ModuleStatus::ACTIVE};
        module = _moduleDatabase.CreateModule(module);

        // act
        const bool result = _moduleDatabase.IsActive(module.name);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_FIXTURE_TEST_CASE(ModuleGetByNameMTest, ModuleMemoryDbTest) {

        // arrange
        Entity::Module::Module module = {.name = MODULE, .state = Entity::Module::ModuleState::RUNNING};
        module = _moduleDatabase.CreateModule(module);

        // act
        const Entity::Module::Module result = _moduleDatabase.GetModuleByName(module.name);

        // assert
        BOOST_CHECK_EQUAL(result.name, MODULE);
    }

    BOOST_FIXTURE_TEST_CASE(ModuleGetByIdMTest, ModuleMemoryDbTest) {

        // arrange
        Entity::Module::Module module = {.name = MODULE, .state = Entity::Module::ModuleState::RUNNING};
        module = _moduleDatabase.CreateModule(module);

        // act
        const Entity::Module::Module result = _moduleDatabase.GetModuleById(module.oid);

        // assert
        BOOST_CHECK_EQUAL(result.name, MODULE);
    }

    BOOST_FIXTURE_TEST_CASE(ModuleUpdateMTest, ModuleMemoryDbTest) {

        // arrange
        Entity::Module::Module module = {.name = MODULE, .state = Entity::Module::ModuleState::RUNNING};
        module = _moduleDatabase.CreateModule(module);
        module.state = Entity::Module::ModuleState::STOPPED;

        // act
        const Entity::Module::Module result = _moduleDatabase.UpdateModule(module);

        // assert
        BOOST_CHECK_EQUAL(result.state == Entity::Module::ModuleState::STOPPED, true);
    }

    BOOST_FIXTURE_TEST_CASE(ModuleListMTest, ModuleMemoryDbTest) {

        // arrange
        Entity::Module::Module module = {.name = MODULE, .state = Entity::Module::ModuleState::RUNNING};
        _moduleDatabase.CreateModule(module);

        // act
        const Entity::Module::ModuleList result = _moduleDatabase.ListModules();

        // assert
        BOOST_CHECK_EQUAL(1, result.size());
    }

    BOOST_FIXTURE_TEST_CASE(ModuleDeleteMTest, ModuleMemoryDbTest) {

        // arrange
        Entity::Module::Module module = {.name = MODULE, .state = Entity::Module::ModuleState::RUNNING};
        module = _moduleDatabase.CreateModule(module);

        // act
        _moduleDatabase.DeleteModule(module);
        const int count = _moduleDatabase.ModuleCount();

        // assert
        BOOST_CHECK_EQUAL(0, count);
    }

    BOOST_FIXTURE_TEST_CASE(ModuleSetStatusMTest, ModuleMemoryDbTest) {

        // arrange
        Entity::Module::Module module = {.name = MODULE, .state = Entity::Module::ModuleState::RUNNING};
        module = _moduleDatabase.CreateModule(module);

        // act
        _moduleDatabase.SetState(MODULE, Entity::Module::ModuleState::STOPPED);
        const Entity::Module::Module updatedModule = _moduleDatabase.GetModuleByName(MODULE);

        // assert
        BOOST_CHECK_EQUAL(updatedModule.state == Entity::Module::ModuleState::STOPPED, true);
    }

    BOOST_FIXTURE_TEST_CASE(ModuleSetPortMTest, ModuleMemoryDbTest) {

        // arrange
        Entity::Module::Module module = {.name = MODULE, .state = Entity::Module::ModuleState::RUNNING};
        module = _moduleDatabase.CreateModule(module);

        // act
        _moduleDatabase.SetPort(MODULE, 9999);
        const Entity::Module::Module updatedModule = _moduleDatabase.GetModuleByName(MODULE);

        // assert
        BOOST_CHECK_EQUAL(updatedModule.port, 9999);
    }

}// namespace AwsMock::Database

#endif// AWMOCK_CORE_MODULE_MEMORYDB_TEST_H