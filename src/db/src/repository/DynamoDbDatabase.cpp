//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/DynamoDbDatabase.h>

namespace AwsMock::Database {

  using bsoncxx::builder::basic::kvp;
  using bsoncxx::builder::basic::make_array;
  using bsoncxx::builder::basic::make_document;

  DynamoDbDatabase::DynamoDbDatabase(Core::Configuration &configuration) : Database(configuration), _logger(Poco::Logger::get("DynamoDbDatabase")), _memoryDb(DynamoDbMemoryDb::instance()) {

    if (HasDatabase()) {

      // Get collection
      _dynamodbCollection = GetConnection()["dynamo_db"];

    }
  }

  bool DynamoDbDatabase::DatabaseExists(const std::string &region, const std::string &name) {

    if (HasDatabase()) {

      try {

        int64_t count = _dynamodbCollection.count_documents(make_document(kvp("region", region), kvp("name", name)));
        log_trace_stream(_logger) << "DynamoDb database exists: " << (count > 0 ? "true" : "false") << std::endl;
        return count > 0;

      } catch (const mongocxx::exception &exc) {
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException("Database exception " + std::string(exc.what()), 500);
      }

    } else {

      return _memoryDb.DatabaseExists(region, name);

    }
  }

  Entity::DynamoDb::DynamoDb DynamoDbDatabase::GetDatabaseByName(const std::string &region, const std::string &name) {

    if (HasDatabase()) {

      try {

        mongocxx::stdx::optional<bsoncxx::document::value> mResult = _dynamodbCollection.find_one(make_document(kvp("region", region), kvp("name", name)));
        if (!mResult) {
          _logger.error() << "Database exception: DynamoDb not found " << std::endl;
          throw Core::DatabaseException("Database exception, DynamoDb not found ", 500);
        }

        Entity::DynamoDb::DynamoDb result;
        result.FromDocument(mResult);
        return result;

      } catch (mongocxx::exception::system_error &e) {
        log_error_stream(_logger) << "Get DynamoDb by name failed, error: " << e.what() << std::endl;
      }

    } else {

      return _memoryDb.GetDatabaseByName(region, name);
    }
    return {};
  }

  /*bool LambdaDatabase::LambdaExists(const Entity::Lambda::Lambda &lambda) {

    return LambdaExists(lambda.region, lambda.function, lambda.runtime);
  }

  bool LambdaDatabase::LambdaExists(const std::string &functionName) {

    if (HasDatabase()) {

      try {

        int64_t count = _lambdaCollection.count_documents(make_document(kvp("function", functionName)));
        log_trace_stream(_logger) << "lambda function exists: " << (count > 0 ? "true" : "false") << std::endl;
        return count > 0;

      } catch (const mongocxx::exception &exc) {
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException("Database exception " + std::string(exc.what()), 500);
      }

    } else {

      return _memoryDb.LambdaExists(functionName);

    }
  }

  bool LambdaDatabase::LambdaExistsByArn(const std::string &arn) {

    if (HasDatabase()) {

      try {

        int64_t count = _lambdaCollection.count_documents(make_document(kvp("arn", arn)));
        log_trace_stream(_logger) << "lambda function exists: " << (count > 0 ? "true" : "false") << std::endl;
        return count > 0;

      } catch (const mongocxx::exception &exc) {
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException("Database exception " + std::string(exc.what()), 500);
      }

    } else {

      return _memoryDb.LambdaExistsByArn(arn);

    }
  }

  long LambdaDatabase::LambdaCount(const std::string &region) {

    if (HasDatabase()) {

      bsoncxx::builder::basic::document builder;
      if (!region.empty()) {
        builder.append(bsoncxx::builder::basic::kvp("region", region));
      }
      bsoncxx::document::value filter = builder.extract();

      try {
        long count = _lambdaCollection.count_documents({filter});
        log_trace_stream(_logger) << "lambda count: " << count << std::endl;
        return count;

      } catch (mongocxx::exception::system_error &exc) {
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException("Database exception " + std::string(exc.what()), 500);
      }

    } else {

      return _memoryDb.LambdaCount(region);

    }
    return -1;
  }

  Entity::Lambda::Lambda LambdaDatabase::CreateLambda(const Entity::Lambda::Lambda &lambda) {

    if (HasDatabase()) {

      try {
        auto result = _lambdaCollection.insert_one(lambda.ToDocument());
        log_trace_stream(_logger) << "Bucket created, oid: " << result->inserted_id().get_oid().value.to_string() << std::endl;
        return GetLambdaById(result->inserted_id().get_oid().value);

      } catch (const mongocxx::exception &exc) {
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException("Database exception " + std::string(exc.what()), 500);
      }

    } else {

      return _memoryDb.CreateLambda(lambda);

    }
  }

  Entity::Lambda::Lambda LambdaDatabase::GetLambdaById(bsoncxx::oid oid) {

    try {

      mongocxx::stdx::optional<bsoncxx::document::value> mResult = _lambdaCollection.find_one(make_document(kvp("_id", oid)));
      if (!mResult) {
        _logger.error() << "Database exception: Lambda not found " << std::endl;
        throw Core::DatabaseException("Database exception, Lambda not found ", 500);
      }

      Entity::Lambda::Lambda result;
      result.FromDocument(mResult);
      return result;

    } catch (const mongocxx::exception &exc) {
      _logger.error() << "Database exception " << exc.what() << std::endl;
      throw Core::DatabaseException("Database exception " + std::string(exc.what()), 500);
    }

  }

  Entity::Lambda::Lambda LambdaDatabase::GetLambdaById(const std::string &oid) {

    if (HasDatabase()) {

      return GetLambdaById(bsoncxx::oid(oid));

    } else {

      return _memoryDb.GetLambdaById(oid);
    }
  }

  Entity::Lambda::Lambda LambdaDatabase::GetLambdaByArn(const std::string &arn) {

    if (HasDatabase()) {

      try {

        mongocxx::stdx::optional<bsoncxx::document::value> mResult = _lambdaCollection.find_one(make_document(kvp("arn", arn)));
        if (!mResult) {
          _logger.error() << "Database exception: Lambda not found " << std::endl;
          throw Core::DatabaseException("Database exception, Lambda not found ", 500);
        }

        Entity::Lambda::Lambda result;
        result.FromDocument(mResult);
        return result;

      } catch (mongocxx::exception::system_error &e) {
        log_error_stream(_logger) << "Get lambda by ARN failed, error: " << e.what() << std::endl;
      }

    } else {

      return _memoryDb.GetLambdaByArn(arn);
    }
    return {};
  }

  Entity::Lambda::Lambda LambdaDatabase::CreateOrUpdateLambda(const Entity::Lambda::Lambda &lambda) {

    if (LambdaExists(lambda)) {
      return UpdateLambda(lambda);
    } else {
      return CreateLambda(lambda);
    }
  }

  Entity::Lambda::Lambda LambdaDatabase::UpdateLambda(const Entity::Lambda::Lambda &lambda) {

    if (HasDatabase()) {

      try {
        auto result = _lambdaCollection.replace_one(make_document(kvp("region", lambda.region), kvp("function", lambda.function), kvp("runtime", lambda.runtime)), lambda.ToDocument());

        log_trace_stream(_logger) << "lambda updated: " << lambda.ToString() << std::endl;

        return GetLambdaByArn(lambda.arn);

      } catch (const mongocxx::exception &exc) {
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException("Database exception " + std::string(exc.what()), 500);
      }

    } else {

      return _memoryDb.UpdateLambda(lambda);

    }
  }

  std::vector<Entity::Lambda::Lambda> LambdaDatabase::ListLambdas(const std::string &region) {

    std::vector<Entity::Lambda::Lambda> lambdas;
    if (HasDatabase()) {

      try {

        if(region.empty()) {

          auto lambdaCursor = _lambdaCollection.find({});
          for (auto lambda : lambdaCursor) {
            Entity::Lambda::Lambda result;
            result.FromDocument(lambda);
            lambdas.push_back(result);
          }
        } else{
          auto lambdaCursor = _lambdaCollection.find(make_document(kvp("region", region)));
          for (auto lambda : lambdaCursor) {
            Entity::Lambda::Lambda result;
            result.FromDocument(lambda);
            lambdas.push_back(result);
          }

        }

      } catch (const mongocxx::exception &exc) {
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException("Database exception " + std::string(exc.what()), 500);
      }

    } else {

      lambdas = _memoryDb.ListLambdas(region);
    }

    log_trace_stream(_logger) << "Got lamda list, size:" << lambdas.size() << std::endl;
    return lambdas;
  }

  void LambdaDatabase::DeleteLambda(const std::string &functionName) {

    if (HasDatabase()) {

      try {

        auto result = _lambdaCollection.delete_many(make_document(kvp("function", functionName)));
        log_debug_stream(_logger) << "lambda deleted, function: " << functionName << " count: " << result->deleted_count() << std::endl;

      } catch (const mongocxx::exception &exc) {
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException("Database exception " + std::string(exc.what()), 500);
      }

    } else {

      _memoryDb.DeleteLambda(functionName);

    }
  }

  void LambdaDatabase::DeleteAllLambdas() {

    if (HasDatabase()) {

      try {

        auto result = _lambdaCollection.delete_many({});
        log_debug_stream(_logger) << "All lambdas deleted, count: " << result->deleted_count() << std::endl;

      } catch (const mongocxx::exception &exc) {
        _logger.error() << "Database exception " << exc.what() << std::endl;
        throw Core::DatabaseException("Database exception " + std::string(exc.what()), 500);
      }

    } else {

      _memoryDb.DeleteAllLambdas();

    }
  }*/

} // namespace AwsMock::Database