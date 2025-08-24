//
// Created by vogje01 on 8/24/25.
//

#ifndef AWSMOCK_MONGODBINSTANCE_H
#define AWSMOCK_MONGODBINSTANCE_H

#include <iostream>
#include <memory>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>

namespace AwsMOck::Database {
    class MongoDBInstance {

      public:

        static MongoDBInstance *GetInstance() {
            static MongoDBInstance objMongoDBInstance;
            return &objMongoDBInstance;
        }

        /* Create a pool object only once from MongoDB URI */
        void createPool(const std::string &uri_) {
            if (!m_client_pool) {
                m_client_pool = std::make_unique<mongocxx::pool>(mongocxx::uri{uri_});
            }
        }

        /* Acquire a client from the pool */
        [[nodiscard]] mongocxx::pool::entry getClientFromPool() const { return m_client_pool->acquire(); }
        ~MongoDBInstance();

      private:

        mongocxx::instance m_dbInstance{};
        std::unique_ptr<mongocxx::pool> m_client_pool;
        MongoDBInstance() = default;
    };

}// namespace AwsMOck::Database

#endif//AWSMOCK_MONGODBINSTANCE_H
