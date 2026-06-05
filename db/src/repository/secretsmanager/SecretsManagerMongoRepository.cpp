//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/secretsmanager/SecretsManagerMongoRepository.h>

namespace Awsmock::Database {

    bool SecretsManagerMongoRepository::SecretExists(const std::string &region, const std::string &name) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _secretCollection = (*client)[_databaseName][_parameterCollectionName];

            // Set limit to 1 (Very important for performance!)
            mongocxx::options::count options;
            options.limit(1);

            return _secretCollection.count_documents(make_document(kvp("region", region), kvp("name", name)), options) > 0;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    bool SecretsManagerMongoRepository::SecretExistsByArn(const std::string &arn) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _secretCollection = (*client)[_databaseName][_parameterCollectionName];

            // Set limit to 1 (Very important for performance!)
            mongocxx::options::count options;
            options.limit(1);

            return _secretCollection.count_documents(make_document(kvp("arn", arn)), options) > 0;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    bool SecretsManagerMongoRepository::SecretExists(const Entity::SecretsManager::Secret &secret) const {
        return SecretExists(secret.region, secret.name);
    }

    bool SecretsManagerMongoRepository::SecretExists(const std::string &secretId) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _secretCollection = (*client)[_databaseName][_parameterCollectionName];

            // Set limit to 1 (Very important for performance!)
            mongocxx::options::count options;
            options.limit(1);

            return _secretCollection.count_documents(make_document(kvp("secretId", secretId)), options) > 0;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SecretsManager::Secret SecretsManagerMongoRepository::GetSecretById(bsoncxx::oid oid) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _secretCollection = (*client)[_databaseName][_parameterCollectionName];

        const auto mResult = _secretCollection.find_one(make_document(kvp("_id", oid)));
        Entity::SecretsManager::Secret result;
        result.FromDocument(mResult->view());

        return result;
    }

    Entity::SecretsManager::Secret SecretsManagerMongoRepository::GetSecretById(const std::string &oid) const {
        return GetSecretById(bsoncxx::oid(oid));
    }

    Entity::SecretsManager::Secret SecretsManagerMongoRepository::GetSecretByRegionName(const std::string &region, const std::string &name) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _bucketCollection = (*client)[_databaseName][_parameterCollectionName];
        const auto mResult = _bucketCollection.find_one(make_document(kvp("region", region), kvp("name", name)));
        if (!mResult) {
            return {};
        }

        Entity::SecretsManager::Secret result;
        result.FromDocument(mResult->view());
        log_trace << "Got secret: " << result.ToString();
        return result;
    }

    Entity::SecretsManager::Secret SecretsManagerMongoRepository::GetSecretBySecretId(const std::string &secretId) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _bucketCollection = (*client)[_databaseName][_parameterCollectionName];
        const auto mResult = _bucketCollection.find_one(make_document(kvp("secretId", secretId)));
        if (mResult->empty()) {
            return {};
        }

        Entity::SecretsManager::Secret result;
        result.FromDocument(mResult->view());
        log_trace << "Got secret: " << result.ToString();
        return result;
    }

    Entity::SecretsManager::Secret SecretsManagerMongoRepository::GetSecretByArn(const std::string &arn) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _bucketCollection = (*client)[_databaseName][_parameterCollectionName];
        const auto mResult = _bucketCollection.find_one(make_document(kvp("arn", arn)));
        if (mResult->empty()) {
            return {};
        }

        Entity::SecretsManager::Secret result;
        result.FromDocument(mResult->view());
        log_trace << "Got secret: " << result.ToString();
        return result;
    }

    Entity::SecretsManager::Secret SecretsManagerMongoRepository::CreateSecret(Entity::SecretsManager::Secret &secret) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _secretCollection = (*client)[_databaseName][_parameterCollectionName];

        try {

            const auto insert_one_result = _secretCollection.insert_one(secret.ToDocument());
            log_trace << "Secret created, oid: " << insert_one_result->inserted_id().get_oid().value.to_string();

            secret.oid = insert_one_result->inserted_id().get_oid().value.to_string();
            return secret;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SecretsManager::Secret SecretsManagerMongoRepository::UpdateSecret(Entity::SecretsManager::Secret &secret) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _secretCollection = (*client)[_databaseName][_parameterCollectionName];

        mongocxx::options::find_one_and_update opts{};
        opts.return_document(mongocxx::options::return_document::k_after);
        opts.upsert(true);

        try {

            const auto mResult = _secretCollection.find_one_and_update(make_document(kvp("secretId", secret.secretId)), secret.ToDocument(), opts);
            log_trace << "Secret updated: " << secret.ToString();

            if (!mResult->empty()) {
                log_trace << "Secret user updated: " << secret.ToString();
                secret.FromDocument(mResult->view());
                return secret;
            }
            return {};

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    Entity::SecretsManager::Secret SecretsManagerMongoRepository::CreateOrUpdateSecret(Entity::SecretsManager::Secret &secret) const {

        if (SecretExists(secret)) {

            return UpdateSecret(secret);
        }
        return CreateSecret(secret);
    }

    Entity::SecretsManager::SecretList SecretsManagerMongoRepository::ListSecrets() const {

        Entity::SecretsManager::SecretList secretList;
        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _secretCollection = (*client)[_databaseName][_parameterCollectionName];

        for (auto secretCursor = _secretCollection.find({}); auto secret: secretCursor) {
            Entity::SecretsManager::Secret result;
            result.FromDocument(secret);
            secretList.push_back(result);
        }
        return secretList;
    }

    long SecretsManagerMongoRepository::CountSecrets(const std::string &region) const {

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _secretsCollection = (*client)[_databaseName][_parameterCollectionName];

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
        return 0;
    }

    void SecretsManagerMongoRepository::DeleteSecret(const Entity::SecretsManager::Secret &secret) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _bucketCollection = (*client)[_databaseName][_parameterCollectionName];

        try {

            const auto delete_many_result = _bucketCollection.delete_one(make_document(kvp("region", secret.region), kvp("name", secret.name)));
            log_debug << "Secret deleted, count: " << delete_many_result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

    long SecretsManagerMongoRepository::DeleteAllSecrets() const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _secretCollection = (*client)[_databaseName][_parameterCollectionName];

        try {

            const auto result = _secretCollection.delete_many({});
            log_debug << "Secrets deleted, count: " << result->deleted_count();
            return result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

}// namespace Awsmock::Database
