#include "ormcxx_sqlite.hpp"
#include <sqlite3.h>

namespace ormcxx {

  Sqlite3Db::~Sqlite3Db() {
    close();
  }

  expected<Database*,Database::Error>  Sqlite3Db::open(const std::string &connInfo) {
    auto db = new Sqlite3Db();

    int r = sqlite3_open(connInfo.c_str(), &db->hDb);
    switch (r) {
      case SQLITE_OK: return db;
      default: delete db; return tl::make_unexpected(Database::Error::ERROR_NOT_FOUND);

    }
  }

  Database::Error Sqlite3Db::close() {
    if (hDb != nullptr) {
      int r = sqlite3_close(hDb);
      hDb = nullptr;
    }
    return Error::OK;
  }

  // expected<sql_stmt*,sql_result::Error> Sqlite3Db::prepare(const std::string &sql_string) {
  //   sql_stmt* s = new Sqlite3Stmt(this,sql_string);
  //   return s;
  // }
  //  sql_stmt::error Sqlite3Stmt::prepare(const std::string &sql_string) {
  //   sqlite3_prepare_v2()
  //
  //   Sqlite3Stmt stmt(hDb, sql_string);
  //   return stmt;
  // }

  expected<sql_stmt *, sql_stmt::error> Sqlite3Db::query(const std::string &sql_string) {
    Sqlite3Stmt* stmt =new Sqlite3Stmt(hDb);
    auto error = stmt->prepare(sql_string);
    if (error!=sql_stmt::error::OK) {
      delete stmt;
      return tl::make_unexpected(error);
    } else {
      return stmt;
    }
   }

  Sqlite3Stmt::Sqlite3Stmt(sqlite3 *db)
  : db_(db),
  prepare_rc(SQLITE_ERROR),
  result(this),
  bindings_(this) {}

  Sqlite3Stmt::~Sqlite3Stmt() { sqlite3_finalize(stmt); }

  sql_stmt::error Sqlite3Stmt::prepare(const std::string &sql_string) {
    prepare_rc = sqlite3_prepare_v2(db_, sql_string.data(), sql_string.length(), &stmt, nullptr);
    return sql_stmt::error::OK;
  }

  tl::expected<sql_result *, sql_stmt::error> Sqlite3Stmt::execute() {
    if (prepare_rc != SQLITE_OK) {
      return tl::make_unexpected(sql_stmt::error::NOK);
    } else {
      return &result;
    }
  }


  static sql_bindings::Error int2error(int error) {
    switch (error) {
      case SQLITE_OK: return sql_bindings::Error::OK;
        break;
      default: return sql_bindings::Error::NOK;
        break;
    }
  }

// sqlresult for sqlite3
  template<>
  size_t sql_result_impl<Sqlite3Stmt>::column_count() {
    return sqlite3_column_count(stmt_->stmt);
  }

  template<>
  const void* sql_result_impl<Sqlite3Stmt>::column_blob(size_t iCol) {
    return sqlite3_column_blob(stmt_->stmt, iCol);
  }

  template<>
    double sql_result_impl<Sqlite3Stmt>::column_double(size_t iCol) {
    return sqlite3_column_double(stmt_->stmt, iCol);
  }

  template<>
    int sql_result_impl<Sqlite3Stmt>::column_int(size_t iCol) {
    return sqlite3_column_int(stmt_->stmt, iCol);
  }

  template<>
    int64_t sql_result_impl<Sqlite3Stmt>::column_int64(size_t iCol) {
    return sqlite3_column_int64(stmt_->stmt, iCol);
  }

  template<>
    const unsigned char* sql_result_impl<Sqlite3Stmt>::column_text(size_t iCol) {
    return sqlite3_column_text(stmt_->stmt, iCol);
  }

  template<>
    const void* sql_result_impl<Sqlite3Stmt>::column_text16(size_t iCol) {
    return sqlite3_column_text16(stmt_->stmt, iCol);
  }

  template<>
    int sql_result_impl<Sqlite3Stmt>::column_bytes(size_t iCol) {
    return sqlite3_column_bytes(stmt_->stmt, iCol);
  }

  template<>
    bool sql_result_impl<Sqlite3Stmt>::next_row() {
    return sqlite3_step(stmt_->stmt) == SQLITE_ROW;
  }


  // sql_bindings for sqlite3
  template<>
  size_t sql_bindings_impl<Sqlite3Stmt>::parameter_count() {
    return sqlite3_bind_parameter_count(stmt_->stmt);
  }

  template<>
  size_t sql_bindings_impl<Sqlite3Stmt>::parameter_index(const char *zName) {
    return sqlite3_bind_parameter_index(stmt_->stmt, zName);
  }

  template<>
  const char *sql_bindings_impl<Sqlite3Stmt>::parameter_name(size_t index) {
    return sqlite3_bind_parameter_name(stmt_->stmt, index);
  }
  template<>
  sql_bindings::Error sql_bindings_impl<Sqlite3Stmt>::bind_blob(size_t index, const void *pBlob, int n) {
    return int2error(sqlite3_bind_blob(stmt_->stmt, index, pBlob, n, nullptr));
  }

  template<>
  sql_bindings::Error sql_bindings_impl<Sqlite3Stmt>::bind_double(size_t index, double value) {
    return int2error(sqlite3_bind_double(stmt_->stmt, index, value));
  }

  template<>
  sql_bindings::Error sql_bindings_impl<Sqlite3Stmt>::bind_int(size_t index, int value) {
    return int2error(sqlite3_bind_int(stmt_->stmt, index, value));
  }

  template<>
  sql_bindings::Error sql_bindings_impl<Sqlite3Stmt>::bind_int64(size_t index, int64_t value) {
    return int2error(sqlite3_bind_int64(stmt_->stmt, index, value));
  }

  template<>
  sql_bindings::Error sql_bindings_impl<Sqlite3Stmt>::bind_null(size_t index) {
    return int2error(sqlite3_bind_null(stmt_->stmt, index));
  }

  template<>
  sql_bindings::Error sql_bindings_impl<Sqlite3Stmt>::bind_text(size_t index, const char *zText, int n) {
    return int2error(sqlite3_bind_text(stmt_->stmt, index, zText, n, nullptr));
  }

  template<>
  sql_bindings::Error sql_bindings_impl<Sqlite3Stmt>::bind_text16(size_t index, const void *zText16, int n) {
    return int2error(sqlite3_bind_text16(stmt_->stmt, index, zText16, n, nullptr));
  }
};
