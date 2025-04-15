//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/memorydb/TransferMemoryDb.h>

namespace AwsMock::Database {

    boost::mutex TransferMemoryDb::_transferMutex;
    boost::mutex TransferMemoryDb::_userMutex;

    bool TransferMemoryDb::TransferExists(const std::string &region, const std::string &serverId) {

        return std::ranges::find_if(_transfers,
                                    [region, serverId](const std::pair<std::string, Entity::Transfer::Transfer> &transfer) {
                                        return transfer.second.region == region && transfer.second.serverId == serverId;
                                    }) != _transfers.end();
    }

    bool TransferMemoryDb::TransferExists(const Entity::Transfer::Transfer &transfer) {

        return TransferExists(transfer.region, transfer.serverId);
    }

    bool TransferMemoryDb::TransferExists(const std::string &serverId) {

        return std::ranges::find_if(_transfers,
                                    [serverId](const std::pair<std::string, Entity::Transfer::Transfer> &transfer) {
                                        return transfer.second.serverId == serverId;
                                    }) != _transfers.end();
    }

    bool TransferMemoryDb::TransferExists(const std::string &region, const std::vector<Entity::Transfer::Protocol> &protocols) {

        return std::ranges::find_if(_transfers,
                                    [region, protocols](const std::pair<std::string, Entity::Transfer::Transfer> &transfer) {
                                        return transfer.second.region == region && std::equal(transfer.second.protocols.begin(), transfer.second.protocols.end(), protocols.begin());
                                    }) != _transfers.end();
    }

    std::vector<Entity::Transfer::Transfer> TransferMemoryDb::ListServers(const std::string &region) {

        Entity::Transfer::TransferList transferList;
        if (region.empty()) {

            for (const auto &val: _transfers | std::views::values) {
                transferList.emplace_back(val);
            }

        } else {

            for (const auto &val: _transfers | std::views::values) {
                if (val.region == region) {
                    transferList.emplace_back(val);
                }
            }
        }

        log_trace << "Got transfer list, size: " << transferList.size();
        return transferList;
    }

    std::vector<Entity::Transfer::User> TransferMemoryDb::ListUsers(const std::string &region, const std::string &serverId) {

        Entity::Transfer::Transfer server = GetTransferByServerId(region, serverId);
        if (!server.users.empty()) {
            log_trace << "Got user list, size: " << server.users.size();
            return server.users;
        }
        return {};
    }

    Entity::Transfer::Transfer TransferMemoryDb::CreateTransfer(const Entity::Transfer::Transfer &transfer) {
        boost::mutex::scoped_lock lock(_transferMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _transfers[oid] = transfer;
        log_trace << "Transfer created, oid: " << oid;
        return GetTransferById(oid);
    }

    Entity::Transfer::Transfer TransferMemoryDb::UpdateTransfer(const Entity::Transfer::Transfer &transfer) {
        boost::mutex::scoped_lock lock(_transferMutex);

        std::string region = transfer.region;
        std::string serverId = transfer.serverId;
        const auto it = std::ranges::find_if(_transfers,
                                             [region, serverId](const std::pair<std::string, Entity::Transfer::Transfer> &transfer) {
                                                 return transfer.second.region == region && transfer.second.serverId == serverId;
                                             });

        if (it == _transfers.end()) {
            log_error << "Update transfer failed, serverId: " << serverId;
            throw Core::DatabaseException("Update transfer failed, serverId: " + serverId);
        }

        _transfers[it->first] = transfer;
        return _transfers[it->first];
    }

    Entity::Transfer::Transfer TransferMemoryDb::GetTransferById(const std::string &oid) {

        const auto it = std::ranges::find_if(_transfers,
                                             [oid](const std::pair<std::string, Entity::Transfer::Transfer> &transfer) {
                                                 return transfer.first == oid;
                                             });

        if (it == _transfers.end()) {
            log_error << "Get transfer by ID failed, oid: " << oid;
            throw Core::DatabaseException("Get transfer by ID failed, oid: " + oid);
        }

        it->second.oid = oid;
        return it->second;
    }

    Entity::Transfer::Transfer TransferMemoryDb::GetTransferByServerId(const std::string &region, const std::string &serverId) {

        const auto it = std::ranges::find_if(_transfers,
                                             [region, serverId](const std::pair<std::string, Entity::Transfer::Transfer> &transfer) {
                                                 return transfer.second.region == region && transfer.second.serverId == serverId;
                                             });

        if (it == _transfers.end()) {
            log_error << "Get transfer by serverId failed, serverId: " << serverId;
            throw Core::DatabaseException("Get transfer by serverId failed, serverId: " + serverId);
        }

        it->second.oid = it->first;
        return it->second;
    }

    Entity::Transfer::Transfer TransferMemoryDb::GetTransferByArn(const std::string &arn) {

        const auto it = std::ranges::find_if(_transfers,
                                             [arn](const std::pair<std::string, Entity::Transfer::Transfer> &transfer) {
                                                 return transfer.second.arn == arn;
                                             });

        if (it == _transfers.end()) {
            log_error << "Get transfer by arn failed, arn: " << arn;
            throw Core::DatabaseException("Get transfer by arn failed, arn: " + arn);
        }

        it->second.oid = it->first;
        return it->second;
    }

    long TransferMemoryDb::CountServers(const std::string &region) {

        long count = 0;

        if (region.empty()) {

            count = static_cast<long>(_transfers.size());

        } else {

            return std::ranges::count_if(_transfers,
                                         [region](std::pair<std::string, Entity::Transfer::Transfer> const &p) {
                                             return p.second.region == region;
                                         });
        }
        log_trace << "Count servers, result: " << count;
        return count;
    }


    void TransferMemoryDb::DeleteTransfer(const std::string &serverId) {
        boost::mutex::scoped_lock lock(_transferMutex);

        const auto count = std::erase_if(_transfers, [serverId](const auto &item) {
            auto const &[key, value] = item;
            return value.serverId == serverId;
        });
        log_debug << "Transfer server deleted, count: " << count;
    }

    long TransferMemoryDb::DeleteAllTransfers() {
        boost::mutex::scoped_lock lock(_transferMutex);

        const long count = _transfers.size();
        log_debug << "All transfer servers deleted, count: " << count;
        _transfers.clear();
        return count;
    }
}// namespace AwsMock::Database