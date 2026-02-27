//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/SecretsManagerDatabase.h>

namespace AwsMock::Database {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;

    SecretsManagerDatabase::SecretsManagerDatabase() : _databaseName(GetDatabaseName()), _collectionName("secretsmanager_secret"), _memoryDb(SecretsManagerMemoryDb::instance()) {}

    bool SecretsManagerDatabase::SecretExists(const std::string &region, const std::string &name) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _secretCollection = (*client)[_databaseName][_collectionName];

                // Set limit to 1 (Very important for performance!)
                mongocxx::options::count options;
                options.limit(1);

                return _secretCollection.count_documents(make_document(kvp("region", region), kvp("name", name)), options) > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.SecretExists(region, name);
    }

    bool SecretsManagerDatabase::SecretExistsByArn(const std::string &arn) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _secretCollection = (*client)[_databaseName][_collectionName];

                // Set limit to 1 (Very important for performance!)
                mongocxx::options::count options;
                options.limit(1);

                return _secretCollection.count_documents(make_document(kvp("arn", arn)), options) > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.SecretExistsByArn(arn);
    }

    bool SecretsManagerDatabase::SecretExists(const Entity::SecretsManager::Secret &secret) const {
        return SecretExists(secret.region, secret.name);
    }

    bool SecretsManagerDatabase::SecretExists(const std::string &secretId) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _secretCollection = (*client)[_databaseName][_collectionName];

                // Set limit to 1 (Very important for performance!)
                mongocxx::options::count options;
                options.limit(1);

                return _secretCollection.count_documents(make_document(kvp("secretId", secretId)), options) > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.SecretExists(secretId);
    }

    Entity::SecretsManager::Secret SecretsManagerDatabase::GetSecretById(bsoncxx::oid oid) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _secretCollection = (*client)[_databaseName][_collectionName];

        const auto mResult = _secretCollection.find_one(make_document(kvp("_id", oid)));
        Entity::SecretsManager::Secret result;
        result.FromDocument(mResult->view());

        return result;
    }

    Entity::SecretsManager::Secret SecretsManagerDatabase::GetSecretById(const std::string &oid) const {

        if (HasDatabase()) {

            return GetSecretById(bsoncxx::oid(oid));
        }
        return _memoryDb.GetSecretById(oid);
    }

    Entity::SecretsManager::Secret SecretsManagerDatabase::GetSecretByRegionName(const std::string &region, const std::string &name) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_collectionName];
            const auto mResult = _bucketCollection.find_one(make_document(kvp("region", region), kvp("name", name)));
            if (!mResult) {
                return {};
            }

            Entity::SecretsManager::Secret result;
            result.FromDocument(mResult->view());
            log_trace << "Got secret: " << result.ToString();
            return result;
        }
        return _memoryDb.GetSecretByRegionName(region, name);
    }

    Entity::SecretsManager::Secret SecretsManagerDatabase::GetSecretBySecretId(const std::string &secretId) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_collectionName];
            const auto mResult = _bucketCollection.find_one(make_document(kvp("secretId", secretId)));
            if (mResult->empty()) {
                return {};
            }

            Entity::SecretsManager::Secret result;
            result.FromDocument(mResult->view());
            log_trace << "Got secret: " << result.ToString();
            return result;
        }
        return _memoryDb.GetSecretBySecretId(secretId);
    }

    Entity::SecretsManager::Secret SecretsManagerDatabase::GetSecretByArn(const std::string &arn) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_collectionName];
            const auto mResult = _bucketCollection.find_one(make_document(kvp("arn", arn)));
            if (mResult->empty()) {
                return {};
            }

            Entity::SecretsManager::Secret result;
            result.FromDocument(mResult->view());
            log_trace << "Got secret: " << result.ToString();
            return result;
        }
        return _memoryDb.GetSecretByArn(arn);
    }

    Entity::SecretsManager::Secret SecretsManagerDatabase::CreateSecret(Entity::SecretsManager::Secret &secret) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _secretCollection = (*client)[_databaseName][_collectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto insert_one_result = _secretCollection.insert_one(secret.ToDocument());
                session.commit_transaction();
                log_trace << "Secret created, oid: " << insert_one_result->inserted_id().get_oid().value.to_string();

                secret.oid = insert_one_result->inserted_id().get_oid().value.to_string();
                return secret;

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.CreateSecret(secret);
    }

    Entity::SecretsManager::Secret SecretsManagerDatabase::UpdateSecret(Entity::SecretsManager::Secret &secret) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _secretCollection = (*client)[_databaseName][_collectionName];
            auto session = client->start_session();

            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);
            opts.upsert(true);

            try {

                session.start_transaction();
                const auto mResult = _secretCollection.find_one_and_update(make_document(kvp("secretId", secret.secretId)), secret.ToDocument(), opts);
                session.commit_transaction();
                log_trace << "Secret updated: " << secret.ToString();

                if (!mResult->empty()) {
                    log_trace << "Secret user updated: " << secret.ToString();
                    secret.FromDocument(mResult->view());
                    return secret;
                }
                return {};

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.UpdateSecret(secret);
    }

    Entity::SecretsManager::Secret SecretsManagerDatabase::CreateOrUpdateSecret(Entity::SecretsManager::Secret &secret) const {

        if (SecretExists(secret)) {

            return UpdateSecret(secret);
        }
        return CreateSecret(secret);
    }

    Entity::SecretsManager::SecretList SecretsManagerDatabase::ListSecrets() const {

        Entity::SecretsManager::SecretList secretList;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _secretCollection = (*client)[_databaseName][_collectionName];

            for (auto secretCursor = _secretCollection.find({}); auto secret: secretCursor) {
                Entity::SecretsManager::Secret result;
                result.FromDocument(secret);
                secretList.push_back(result);
            }

        } else {

            secretList = _memoryDb.ListSecrets();
        }
        log_trace << "Got secret list, size:" << secretList.size();
        return secretList;
    }

    long SecretsManagerDatabase::CountSecrets(const std::string &region) const {

        if (HasDatabase()) {

            try {
                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _secretsCollection = (*client)[_databaseName][_collectionName];

                long count;
                if (region.empty()) {
                    count = _secretsCollection.count_documents(make_document());
                } else {
                    count = _secretsCollection.count_documents(make_document(kvp("region", region)));
                }
                log_trace << "Secrets count: " << count;
                return count;

            } catch (mongocxx::exception::system_error &e) {
                log_error << "Secrets count failed, error: " << e.what();
            }

        } else {

            return _memoryDb.CountSecrets(region);
        }
        return -1;
    }

    void SecretsManagerDatabase::DeleteSecret(const Entity::SecretsManager::Secret &secret) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_collectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto delete_many_result = _bucketCollection.delete_one(make_document(kvp("region", secret.region), kvp("name", secret.name)));
                session.commit_transaction();
                log_debug << "Secret deleted, count: " << delete_many_result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }

        } else {

            _memoryDb.DeleteSecret(secret);
        }
    }

    long SecretsManagerDatabase::DeleteAllSecrets() const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _secretCollection = (*client)[_databaseName][_collectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = _secretCollection.delete_many({});
                session.commit_transaction();
                log_debug << "Secrets deleted, count: " << result->deleted_count();
                return result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        return _memoryDb.DeleteAllSecrets();
    }

}// namespace AwsMock::Database
