//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/memorydb/CognitoMemoryDb.h>

namespace AwsMock::Database {

    boost::mutex CognitoMemoryDb::_userPoolMutex;
    boost::mutex CognitoMemoryDb::_userMutex;
    boost::mutex CognitoMemoryDb::_groupMutex;

    bool CognitoMemoryDb::UserPoolExists(const std::string &region, const std::string &name) {

        return std::ranges::find_if(_userPools,
                                    [region, name](const std::pair<std::string, Entity::Cognito::UserPool> &userPool) {
                                        return userPool.second.region == region && userPool.second.name == name;
                                    }) != _userPools.end();
    }

    bool CognitoMemoryDb::UserPoolExists(const std::string &userPoolId) {

        return std::ranges::find_if(_userPools,
                                    [userPoolId](const std::pair<std::string, Entity::Cognito::UserPool> &userPool) {
                                        return userPool.second.userPoolId == userPoolId;
                                    }) != _userPools.end();
    }

    Entity::Cognito::UserPoolList CognitoMemoryDb::ListUserPools(const std::string &region) {

        Entity::Cognito::UserPoolList userPoolList;
        if (region.empty()) {
            for (const auto &val: _userPools | std::views::values) {
                userPoolList.emplace_back(val);
            }
        } else {
            for (const auto &val: _userPools | std::views::values) {
                if (val.region == region) {
                    userPoolList.emplace_back(val);
                }
            }
        }

        log_trace << "Got user pool list, size: " << userPoolList.size();
        return userPoolList;
    }

    Entity::Cognito::UserPoolList CognitoMemoryDb::ExportUserPools(const std::vector<SortColumn> &sortColumns) const {

        Entity::Cognito::UserPoolList userPoolList;
        for (const auto &val: _userPools | std::views::values) {
            userPoolList.emplace_back(val);
        }

        log_trace << "Got user pool list, size: " << userPoolList.size();
        std::ranges::sort(userPoolList, [](const Entity::Cognito::UserPool &a, const Entity::Cognito::UserPool &b) {
            return a.name < b.name;
        });
        return userPoolList;
    }

    Entity::Cognito::UserPool CognitoMemoryDb::CreateUserPool(const Entity::Cognito::UserPool &userPool) {
        boost::mutex::scoped_lock lock(_userPoolMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _userPools[oid] = userPool;
        log_trace << "Cognito user pool created, oid: " << oid;
        return GetUserPoolByOid(oid);
    }

    Entity::Cognito::UserPool CognitoMemoryDb::GetUserPoolByOid(const std::string &oid) {

        const auto it = std::ranges::find_if(_userPools,
                                             [oid](const std::pair<std::string, Entity::Cognito::UserPool> &userPool) {
                                                 return userPool.first == oid;
                                             });

        if (it == _userPools.end()) {
            log_error << "Get cognito user pool by oid failed, oid: " << oid;
            throw Core::DatabaseException("Get cognito user pool by oid failed, oid: " + oid);
        }

        it->second.oid = oid;
        return it->second;
    }

    Entity::Cognito::UserPool CognitoMemoryDb::GetUserPoolByUserPoolId(const std::string &userPoolId) {

        const auto it = std::ranges::find_if(_userPools,
                                             [userPoolId](const std::pair<std::string, Entity::Cognito::UserPool> &userPool) {
                                                 return userPool.second.userPoolId == userPoolId;
                                             });

        if (it == _userPools.end()) {
            log_error << "Get cognito user pool by userPoolId failed, region: " << userPoolId;
            throw Core::DatabaseException("Get cognito user pool by userPoolId failed, userPoolId: " + userPoolId);
        }

        it->second.oid = it->first;
        return it->second;
    }

    Entity::Cognito::UserPool CognitoMemoryDb::GetUserPoolByClientId(const std::string &clientId) {

        const auto it = std::ranges::find_if(_userPools,
                                             [clientId](const std::pair<std::string, Entity::Cognito::UserPool> &userPool) {
                                                 const auto it1 = std::ranges::find_if(userPool.second.userPoolClients,
                                                                                       [clientId](const Entity::Cognito::UserPoolClient &userPoolClient) {
                                                                                           return userPoolClient.clientId == clientId;
                                                                                       });
                                                 return it1 != userPool.second.userPoolClients.end();
                                             });

        if (it == _userPools.end()) {
            log_error << "Get cognito user pool by clientId failed, clientId: " << clientId;
            throw Core::DatabaseException("Get cognito user pool by clientId failed, clientId: " + clientId);
        }

        it->second.oid = it->first;
        return it->second;
    }

    Entity::Cognito::UserPool CognitoMemoryDb::GetUserPoolByRegionName(const std::string &region, const std::string &name) {

        const auto it = std::ranges::find_if(_userPools,
                                             [region, name](const std::pair<std::string, Entity::Cognito::UserPool> &userPool) {
                                                 return userPool.second.region == region && userPool.second.name == name;
                                             });

        if (it == _userPools.end()) {
            log_error << "Get cognito user pool by region and name failed, region: " << region << " name: " << name;
            throw Core::DatabaseException("Get cognito user pool by region and name failed, region: " + region + " name: " + name);
        }

        it->second.oid = it->first;
        return it->second;
    }

    long CognitoMemoryDb::CountUserPools(const std::string &region) const {
        long count = 0;
        if (region.empty()) {
            count = static_cast<long>(_userPools.size());
        } else {
            for (const auto &val: _userPools | std::views::values) {
                if (val.region == region) {
                    count++;
                }
            }
        }

        log_trace << "Count user pools, size: " << count;
        return count;
    }

    Entity::Cognito::UserPool CognitoMemoryDb::UpdateUserPool(const Entity::Cognito::UserPool &userPool) {

        boost::mutex::scoped_lock lock(_userPoolMutex);

        std::string region = userPool.region;
        std::string name = userPool.name;
        const auto it = std::ranges::find_if(_userPools,
                                             [region, name](const std::pair<std::string, Entity::Cognito::UserPool> &userPool) {
                                                 return userPool.second.region == region && userPool.second.name == name;
                                             });

        if (it == _userPools.end()) {
            log_error << "Update user pool failed, region: " << userPool.region << " name: " << userPool.name;
            throw Core::DatabaseException("Update cognito user pool failed, region: " + userPool.region + " name: " + userPool.name);
        }
        _userPools[it->first] = userPool;
        return _userPools[it->first];
    }

    void CognitoMemoryDb::DeleteUserPool(const std::string &userPoolId) {
        boost::mutex::scoped_lock lock(_userPoolMutex);

        const auto count = std::erase_if(_userPools, [userPoolId](const auto &item) {
            auto const &[key, value] = item;
            return value.userPoolId == userPoolId;
        });
        log_debug << "Cognito user pool deleted, count: " << count;
    }

    void CognitoMemoryDb::DeleteAllUserPools() {
        boost::mutex::scoped_lock lock(_userPoolMutex);

        log_debug << "All cognito user pools deleted, count: " << _userPools.size();
        _userPools.clear();
    }

    bool CognitoMemoryDb::UserExists(const std::string &region, const std::string &userPoolId, const std::string &userName) {

        return std::ranges::find_if(_users,
                                    [region, userPoolId, userName](const std::pair<std::string, Entity::Cognito::User> &user) {
                                        return user.second.region == region && user.second.userPoolId == userPoolId && user.second.userName == userName;
                                    }) != _users.end();
    }

    bool CognitoMemoryDb::UserExists(const std::string &region, const std::string &userName) {

        return std::ranges::find_if(_users,
                                    [region, userName](const std::pair<std::string, Entity::Cognito::User> &user) {
                                        return user.second.region == region && user.second.userName == userName;
                                    }) != _users.end();
    }

    Entity::Cognito::User CognitoMemoryDb::CreateUser(const Entity::Cognito::User &user) {
        boost::mutex::scoped_lock lock(_userMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _users[oid] = user;
        log_trace << "Cognito user created, oid: " << oid;
        return GetUserByOid(oid);
    }

    Entity::Cognito::User CognitoMemoryDb::GetUserByOid(const std::string &oid) {

        const auto it = std::ranges::find_if(_users, [oid](const std::pair<std::string, Entity::Cognito::User> &user) {
            return user.first == oid;
        });

        if (it == _users.end()) {
            log_error << "Get cognito user by oid failed, oid: " << oid;
            throw Core::DatabaseException("Get cognito user by oid failed, oid: " + oid);
        }

        it->second.oid = oid;
        return it->second;
    }

    Entity::Cognito::User CognitoMemoryDb::GetUserByUserName(const std::string &region, const std::string &userPoolId, const std::string &userName) {

        const auto it = std::ranges::find_if(_users,
                                             [region, userPoolId, userName](const std::pair<std::string, Entity::Cognito::User> &user) {
                                                 return user.second.region == region && user.second.userPoolId == userPoolId && user.second.userName == userName;
                                             });

        if (it == _users.end()) {
            log_error << "Get cognito user by user name failed, userName: " << userName;
            throw Core::DatabaseException("Get cognito user by user name failed, userName: " + userName);
        }

        it->second.oid = it->first;
        return it->second;
    }

    long CognitoMemoryDb::CountUsers(const std::string &region, const std::string &userPoolId, const std::string &groupName) const {

        long count = 0;

        count = std::ranges::count_if(_users,
                                      [region, userPoolId](const std::pair<std::string, Entity::Cognito::User> &obj) {
                                          if (!region.empty() && obj.second.region == region) {
                                              return true;
                                          }
                                          if (!region.empty() && obj.second.region == region && !userPoolId.empty() && obj.second.userPoolId == userPoolId) {
                                              return true;
                                          }
                                          return false;
                                      });

        log_trace << "Count user pools, size: " << count;
        return count;
    }

    std::vector<Entity::Cognito::User> CognitoMemoryDb::ListUsers(const std::string &region, const std::string &userPoolId) const {

        Entity::Cognito::UserList userList;

        if (!region.empty() && !userPoolId.empty()) {

            for (const auto &val: _users | std::views::values) {
                if (val.region == region && val.userPoolId == userPoolId) {
                    userList.emplace_back(val);
                }
            }

        } else if (!region.empty()) {

            for (const auto &val: _users | std::views::values) {
                if (val.region == region) {
                    userList.emplace_back(val);
                }
            }

        } else {

            for (const auto &val: _users | std::views::values) {
                userList.emplace_back(val);
            }
        }

        log_trace << "Got user list, size: " << userList.size();
        return userList;
    }

    std::vector<Entity::Cognito::User> CognitoMemoryDb::ExportUsers(const std::vector<SortColumn> &sortColumns) const {

        Entity::Cognito::UserList userList;
        for (const auto &val: _users | std::views::values) {
            userList.emplace_back(val);
        }

        log_trace << "Got user list, size: " << userList.size();
        std::ranges::sort(userList, [](const Entity::Cognito::User &a, const Entity::Cognito::User &b) {
            return a.userName < b.userName;
        });
        return userList;
    }

    std::vector<Entity::Cognito::User> CognitoMemoryDb::ListUsersInGroup(const std::string &region, const std::string &userPoolId, const std::string &groupName) {

        Entity::Cognito::UserList userList;

        for (const auto &val: _users | std::views::values) {
            if (val.region == region && val.userPoolId == userPoolId && std::ranges::find_if(val.groups, [&groupName](const Entity::Cognito::Group &g) {
                                                                            return g.groupName == groupName;
                                                                        }) != val.groups.end()) {
                userList.emplace_back(val);
            }
        }

        log_trace << "Got user in group list, size: " << userList.size();
        return userList;
    }

    Entity::Cognito::User CognitoMemoryDb::UpdateUser(const Entity::Cognito::User &user) {

        boost::mutex::scoped_lock lock(_userMutex);

        std::string region = user.region;
        std::string userPoolId = user.userPoolId;
        std::string userName = user.userName;
        const auto it = std::ranges::find_if(_users,
                                             [region, userPoolId, userName](const std::pair<std::string, Entity::Cognito::User> &user) {
                                                 return user.second.region == region && user.second.userPoolId == userPoolId && user.second.userName == userName;
                                             });

        if (it == _users.end()) {
            log_error << "Update user failed, region: " << user.region << " name: " << user.userName;
            throw Core::DatabaseException("Update cognito user failed, region: " + user.region + " name: " + user.userName);
        }
        _users[it->first] = user;
        return _users[it->first];
    }

    void CognitoMemoryDb::DeleteUser(const Entity::Cognito::User &user) {
        boost::mutex::scoped_lock lock(_userMutex);

        const auto count = std::erase_if(_users, [user](const std::pair<std::string, Entity::Cognito::User> &u) {
            return u.second.region == user.region && u.second.userPoolId == user.userPoolId && u.second.userName == user.userName;
        });
        log_debug << "Cognito user deleted, count: " << count;
    }

    void CognitoMemoryDb::DeleteAllUsers() {
        boost::mutex::scoped_lock lock(_userMutex);

        log_debug << "All cognito users deleted, count: " << _userPools.size();
        _users.clear();
    }

    bool CognitoMemoryDb::GroupExists(const std::string &region, const std::string &groupName) {

        return std::ranges::find_if(_groups,
                                    [region, groupName](const std::pair<std::string, Entity::Cognito::Group> &group) {
                                        return group.second.region == region && group.second.groupName == groupName;
                                    }) != _groups.end();
    }

    Entity::Cognito::Group CognitoMemoryDb::GetGroupByGroupName(const std::string &region, const std::string &groupPoolId, const std::string &groupName) {

        const auto it = std::ranges::find_if(_groups,
                                             [region, groupPoolId, groupName](const std::pair<std::string, Entity::Cognito::Group> &group) {
                                                 return group.second.region == region && group.second.userPoolId == groupPoolId && group.second.groupName == groupName;
                                             });

        if (it == _groups.end()) {
            log_error << "Get cognito group by group name failed, groupName: " << groupName;
            throw Core::DatabaseException("Get cognito group by group name failed, groupName: " + groupName);
        }

        it->second.oid = it->first;
        return it->second;
    }

    Entity::Cognito::Group CognitoMemoryDb::CreateGroup(const Entity::Cognito::Group &group) {
        boost::mutex::scoped_lock lock(_groupMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _groups[oid] = group;
        log_trace << "Cognito user pool created, oid: " << oid;
        return _groups[oid];
    }

    std::vector<Entity::Cognito::Group> CognitoMemoryDb::ListGroups(const std::string &region, const std::string &userPoolId) {

        Entity::Cognito::GroupList groupList;
        if (!region.empty() && !userPoolId.empty()) {

            for (const auto &val: _groups | std::views::values) {
                if (val.region == region && val.userPoolId == userPoolId) {
                    groupList.emplace_back(val);
                }
            }

        } else if (!region.empty()) {

            for (const auto &val: _groups | std::views::values) {
                if (val.region == region) {
                    groupList.emplace_back(val);
                }
            }

        } else {

            for (const auto &val: _groups | std::views::values) {
                groupList.emplace_back(val);
            }
        }

        log_trace << "Got group list, size: " << groupList.size();
        return groupList;
    }

    std::vector<Entity::Cognito::Group> CognitoMemoryDb::ExportGroups(const std::vector<SortColumn> &sortColumns) {

        Entity::Cognito::GroupList groupList;
        for (const auto &val: _groups | std::views::values) {
            groupList.emplace_back(val);
        }

        std::ranges::sort(groupList, [](const Entity::Cognito::Group &g1, const Entity::Cognito::Group &g2) {
            return g1.groupName < g2.groupName;
        });
        log_trace << "Got group list, size: " << groupList.size();
        return groupList;
    }

    void CognitoMemoryDb::DeleteGroup(const std::string &region, const std::string &userPoolId, const std::string &groupName) {
        boost::mutex::scoped_lock lock(_groupMutex);

        const auto count = std::erase_if(_groups, [region, userPoolId, groupName](const std::pair<std::string, Entity::Cognito::Group> &g) {
            return g.second.region == region && g.second.userPoolId == userPoolId && g.second.groupName == groupName;
        });
        log_debug << "Cognito group deleted, groupName: " << groupName << " count: " << count;
    }

    void CognitoMemoryDb::DeleteAllGroups(const std::string &region) {
        boost::mutex::scoped_lock lock(_groupMutex);

        if (region.empty()) {

            _groups.clear();
            log_debug << "All cognito groups deleted";

        } else {
            const auto count = std::erase_if(_groups, [region](const std::pair<std::string, Entity::Cognito::Group> &g) {
                return g.second.region == region;
            });
            log_debug << "All cognito groups deleted, region: " << region << " count: " << count;
        }
    }

    bool CognitoMemoryDb::ClientIdExists(const std::string &region, const std::string &clientId) {

        return std::ranges::find_if(_userPools,
                                    [region, clientId](const std::pair<std::string, Entity::Cognito::UserPool> &userPool) {
                                        return userPool.second.region == region && std::ranges::find_if(userPool.second.userPoolClients,
                                                                                                        [clientId](const Entity::Cognito::UserPoolClient &userPoolClient) {
                                                                                                            return userPoolClient.clientId == clientId;
                                                                                                        }) != userPool.second.userPoolClients.end();
                                    }) != _userPools.end();
    }

}// namespace AwsMock::Database