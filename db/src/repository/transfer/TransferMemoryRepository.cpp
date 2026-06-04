//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/repository/transfer/TransferMemoryRepository.h>

namespace Awsmock::Database {

    boost::mutex TransferMemoryRepository::_transferMutex;
    boost::mutex TransferMemoryRepository::_userMutex;

    bool TransferMemoryRepository::transferExists(const std::string &region, const std::string &serverId) const {

        return std::ranges::find_if(_transfers,
                                    [region, serverId](const std::pair<std::string, Entity::Transfer::Transfer> &transfer) {
                                        return transfer.second.region == region && transfer.second.serverId == serverId;
                                    }) != _transfers.end();
    }

    bool TransferMemoryRepository::transferExists(const Entity::Transfer::Transfer &transfer) const {

        return transferExists(transfer.region, transfer.serverId);
    }

    bool TransferMemoryRepository::transferExists(const std::string &serverId) const {

        return std::ranges::find_if(_transfers,
                                    [serverId](const std::pair<std::string, Entity::Transfer::Transfer> &transfer) {
                                        return transfer.second.serverId == serverId;
                                    }) != _transfers.end();
    }

    bool TransferMemoryRepository::transferExists(const std::string &region, const std::vector<Entity::Transfer::Protocol> &protocols) const {

        return std::ranges::find_if(_transfers,
                                    [region, protocols](const std::pair<std::string, Entity::Transfer::Transfer> &transfer) {
                                        return transfer.second.region == region && std::equal(transfer.second.protocols.begin(), transfer.second.protocols.end(), protocols.begin());
                                    }) != _transfers.end();
    }

    std::vector<Entity::Transfer::Transfer> TransferMemoryRepository::listServers(const std::string &region, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const {

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

    std::vector<Entity::Transfer::Transfer> TransferMemoryRepository::listServers(const std::string &region, std::string &nextToken, long maxResults) const {

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

    std::vector<Entity::Transfer::User> TransferMemoryRepository::listUsers(const std::string &region, const std::string &serverId, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        if (Entity::Transfer::Transfer server = getTransferByServerId(region, serverId); !server.users.empty()) {
            log_trace << "Got user list, size: " << server.users.size();
            return server.users;
        }
        return {};
    }

    Entity::Transfer::Transfer TransferMemoryRepository::createTransfer(const Entity::Transfer::Transfer &transfer) const {
        boost::mutex::scoped_lock lock(_transferMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _transfers[oid] = transfer;
        log_trace << "Transfer created, oid: " << oid;
        return _transfers[oid];
    }

    Entity::Transfer::Transfer TransferMemoryRepository::createOrUpdateTransfer(const Entity::Transfer::Transfer &transfer) const {
        if (transferExists(transfer)) {
            return updateTransfer(transfer);
        }
        return createTransfer(transfer);
    }

    Entity::Transfer::Transfer TransferMemoryRepository::updateTransfer(const Entity::Transfer::Transfer &transfer) const {
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

    Entity::Transfer::Transfer TransferMemoryRepository::getTransferById(const std::string &oid) const {

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

    Entity::Transfer::Transfer TransferMemoryRepository::getTransferById(const bsoncxx::oid &oid) const {
        return getTransferById(oid.to_string());
    }

    Entity::Transfer::Transfer TransferMemoryRepository::getTransferByServerId(const std::string &region, const std::string &serverId) const {

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

    Entity::Transfer::Transfer TransferMemoryRepository::getTransferByArn(const std::string &arn) const {

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

    long TransferMemoryRepository::countServers(const std::string &region) const {

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

    long TransferMemoryRepository::countUsers(const std::string &region, const std::string &serverId) const {

        long count = 0;

        if (region.empty()) {

            count = static_cast<long>(_users.size());

        } else {

            return std::ranges::count_if(_transfers,
                                         [region](std::pair<std::string, Entity::Transfer::Transfer> const &p) {
                                             return p.second.region == region;
                                         });
        }
        log_trace << "Count users, result: " << count;
        return count;
    }

    void TransferMemoryRepository::deleteTransfer(const std::string &serverId) const {
        boost::mutex::scoped_lock lock(_transferMutex);

        const auto count = std::erase_if(_transfers, [serverId](const auto &item) {
            auto const &[key, value] = item;
            return value.serverId == serverId;
        });
        log_debug << "Transfer server deleted, count: " << count;
    }

    long TransferMemoryRepository::deleteAllTransfers() const {
        boost::mutex::scoped_lock lock(_transferMutex);

        const long count = _transfers.size();
        log_debug << "All transfer servers deleted, count: " << count;
        _transfers.clear();
        return count;
    }
}// namespace Awsmock::Database