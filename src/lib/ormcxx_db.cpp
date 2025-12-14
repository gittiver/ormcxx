#include "ormcxx/ormcxx_db.hpp"
#include "ormcxx_sqlite.hpp"

namespace ormcxx {

  sql_stmt::sql_stmt(sql_stmt_base *pImpl_)
    : pImpl(pImpl_) {
  }

  sql_bindings &sql_stmt::bindings() { return pImpl->bindings(); }

  sql_error sql_stmt::prepare(const std::string &sql_string) {
    return pImpl->prepare(sql_string);
  }

  sql_error sql_stmt::execute() {
    return pImpl->execute();
  }

  sql_error sql_stmt::execute(const std::string &sql_string) {
    return pImpl->execute(sql_string);
  }


  sql_error sql_bindings::bind_text(size_t index, const std::string &text) {
    return bind_text(index, text.data(), text.length());
  }
  const sql_result& sql_stmt::result() const {
    return pImpl->result();
  }


  Database::Database(DatabaseImpl *pImpl_)
    : pImpl(pImpl_) {
  }

  tl::expected<Database, Database::Error> Database::open(BackendType backendType, const std::string &connInfo) {
    switch (backendType) {
      case BackendType::SQLITE:
        {
          auto db = Sqlite3Db::open(connInfo);
          if (!db) {
            return make_unexpected(Error::ERROR_NOT_FOUND);
          } else {
            return db;
          }
        }
        break;
      default:
        return make_unexpected(Error::NO_DB_DRIVER);
        break;
    }
  }

  Database::Error Database::close() {
    return pImpl->close();
  }

  expected<sql_stmt, sql_error> Database::query(const std::string &sql_string) {
    return pImpl->query(sql_string);
  }
}
