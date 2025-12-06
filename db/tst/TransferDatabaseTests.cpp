//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_TRANSFER_DATABASE_TEST_H
#define AWMOCK_CORE_TRANSFER_DATABASE_TEST_H

// C++ standard includes
#include <vector>

// AwsMock includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/TransferDatabase.h>

#define BUCKET "test-bucket"
#define OWNER "test-owner"

namespace AwsMock::Database {

    struct TransferDatabaseTests {

        TransferDatabaseTests() {
            _region = _configuration.GetValue<std::string>("awsmock.region");
        }

        ~TransferDatabaseTests() {
            const long deleted = _transferDatabase.DeleteAllTransfers();
            log_debug << "Database cleaned, count:" << deleted;
        }

        std::string _region;
        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration(true);
        TransferDatabase &_transferDatabase = TransferDatabase::instance();
    };

    BOOST_FIXTURE_TEST_CASE(TransferCreateTest, TransferDatabaseTests) {

        // arrange
        const Entity::Transfer::Transfer transfer = {.region = _region, .protocols = {Entity::Transfer::FTP, Entity::Transfer::SFTP}};

        // act
        const Entity::Transfer::Transfer result = _transferDatabase.CreateTransfer(transfer);

        // assert
        BOOST_CHECK_EQUAL(result.protocols[0] == Database::Entity::Transfer::FTP, true);
        BOOST_CHECK_EQUAL(result.region, _region);
    }

    BOOST_FIXTURE_TEST_CASE(TransferExistsUniqueTest, TransferDatabaseTests) {

        // arrange
        Entity::Transfer::Transfer transfer1 = {.region = _region, .serverId = "s_3456af45e", .protocols = {Entity::Transfer::FTP}};
        Entity::Transfer::Transfer transfer2 = {.region = _region, .serverId = "s_abc6af45e", .protocols = {Entity::Transfer::SFTP}};
        transfer1 = _transferDatabase.CreateTransfer(transfer1);
        transfer2 = _transferDatabase.CreateTransfer(transfer2);

        // act
        const bool result1 = _transferDatabase.TransferExists(_region, transfer1.protocols);
        const bool result2 = _transferDatabase.TransferExists(_region, transfer2.protocols);

        // assert
        BOOST_CHECK_EQUAL(result1, false);
        BOOST_CHECK_EQUAL(result2, false);
    }

    BOOST_FIXTURE_TEST_CASE(TransferExistsByServerIdTest, TransferDatabaseTests) {

        // arrange
        Entity::Transfer::Transfer transfer = {.region = _region, .serverId = "s_3456af45e", .protocols = {Entity::Transfer::FTP, Entity::Transfer::SFTP}};
        transfer = _transferDatabase.CreateTransfer(transfer);

        // act
        const bool result1 = _transferDatabase.TransferExists(_region, transfer.serverId);
        const bool result2 = _transferDatabase.TransferExists(transfer.serverId);

        // assert
        BOOST_CHECK_EQUAL(result1, true);
        BOOST_CHECK_EQUAL(result2, true);
    }

    BOOST_FIXTURE_TEST_CASE(TransferGetByServerIdTest, TransferDatabaseTests) {

        // arrange
        Entity::Transfer::Transfer transfer = {.region = _region, .protocols = {Entity::Transfer::FTP, Entity::Transfer::SFTP}};
        transfer = _transferDatabase.CreateTransfer(transfer);

        // act
        const Entity::Transfer::Transfer result = _transferDatabase.GetTransferByServerId(_region, transfer.serverId);

        // assert
        BOOST_CHECK_EQUAL(result.serverId, transfer.serverId);
    }

    BOOST_FIXTURE_TEST_CASE(TransferGetByServerArnTest, TransferDatabaseTests) {

        // arrange
        const std::string serverId = "s_3456af45e";
        const std::string arn = "arn:aws:transfer:" + _region + ":" + OWNER + ":server/" + serverId;
        Entity::Transfer::Transfer transfer = {.region = _region, .serverId = serverId, .arn = arn, .protocols = {Entity::Transfer::FTP, Entity::Transfer::SFTP}};
        transfer = _transferDatabase.CreateTransfer(transfer);

        // act
        const Entity::Transfer::Transfer result = _transferDatabase.GetTransferByArn(transfer.arn);

        // assert
        BOOST_CHECK_EQUAL(result.serverId, transfer.serverId);
    }

    BOOST_FIXTURE_TEST_CASE(TransferUpdateTest, TransferDatabaseTests) {

        // arrange
        Entity::Transfer::Transfer transfer = {.region = _region, .serverId = "s_3456af45e", .protocols = {Entity::Transfer::FTP, Entity::Transfer::SFTP}};
        transfer = _transferDatabase.CreateTransfer(transfer);

        // act
        transfer.users.push_back({.userName = "test", .password = "test", .homeDirectory = "/"});
        const Entity::Transfer::Transfer result = _transferDatabase.UpdateTransfer(transfer);

        // assert
        BOOST_CHECK_EQUAL(result.users[0].userName, "test");
    }

    BOOST_FIXTURE_TEST_CASE(TransferDeleteTest, TransferDatabaseTests) {

        // arrange
        Entity::Transfer::Transfer transfer = {.region = _region, .serverId = "s_3456af45e", .protocols = {Entity::Transfer::FTP, Entity::Transfer::SFTP}};
        transfer = _transferDatabase.CreateTransfer(transfer);

        // act
        _transferDatabase.DeleteTransfer(transfer.serverId);
        const bool result = _transferDatabase.TransferExists(transfer.serverId);

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_FIXTURE_TEST_CASE(TransferDeleteAllTest, TransferDatabaseTests) {

        // arrange
        Entity::Transfer::Transfer transfer1 = {.region = _region, .serverId = "s_3456af45e", .protocols = {Entity::Transfer::FTP, Entity::Transfer::SFTP}};
        Entity::Transfer::Transfer transfer2 = {.region = _region, .serverId = "s_14eab3422", .protocols = {Entity::Transfer::FTP, Entity::Transfer::SFTP}};
        transfer1 = _transferDatabase.CreateTransfer(transfer1);
        transfer2 = _transferDatabase.CreateTransfer(transfer2);

        // act
        long deleted = _transferDatabase.DeleteAllTransfers();
        const bool result1 = _transferDatabase.TransferExists(transfer1.serverId);
        const bool result2 = _transferDatabase.TransferExists(transfer2.serverId);

        // assert
        BOOST_CHECK_EQUAL(result1, false);
        BOOST_CHECK_EQUAL(result2, false);
    }

}// namespace AwsMock::Database

#endif// AWMOCK_CORE_TRANSFER_DATABASE_TEST_H