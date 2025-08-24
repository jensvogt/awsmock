//
// Created by vogje01 on 8/24/25.
//

#ifndef AWSMOCK_MONGODBACCESS_H
#define AWSMOCK_MONGODBACCESS_H

#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/json.hpp>
#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <mongocxx/pool.hpp>

namespace AwsMOck::Database {

    class MongoDBAccess {

      public:

        MongoDBAccess(mongocxx::client &client_, const std::string &dbName_, const std::string &collName_) : m_client(client_), m_dbName(dbName_), m_collectionName(collName_) {
            m_db = m_client[dbName_];
            m_collection = m_db[collName_];
        }

        int insert(std::string jsonDoc_) {
            try {
                // Convert JSON data to document
                auto doc_value = bsoncxx::from_json(jsonDoc_);
                //Insert the document
                auto result = m_collection.insert_one(std::move(doc_value));
            } catch (const bsoncxx::exception &e) {
                std::string errInfo = std::string("Error in converting JSONdata,Err Msg : ") + e.what();
                return -1;
            } catch (mongocxx::bulk_write_exception &e) {
                std::string errInfo = std::string("Error in inserting document, Err Msg : ") + e.what();
                return -1;
            }
            return 0;
        }

      private:

        mongocxx::client &m_client;
        std::string m_dbName;
        std::string m_collectionName;
        mongocxx::database m_db;
        mongocxx::collection m_collection;
    };
}// namespace AwsMOck::Database
#endif//AWSMOCK_MONGODBACCESS_H
