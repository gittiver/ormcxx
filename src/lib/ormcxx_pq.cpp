#include "ormcxx_pq.hpp"
#include "libpq-fe.h"

namespace ormcxx {
  static sql_error int2error(int error);

  PostgresDb::~PostgresDb() {
    close();
  }

  expected<Database *, Database::Error> PostgresDb::open(const std::string &connInfo) {
    //auto db = new PostgresDb();


    //int r = sqlite3_open(connInfo.c_str(), &db->hDb);
    //switch (r) {
    //  case SQLITE_OK: return db;
    //  default: delete db;
        return tl::make_unexpected(Database::Error::ERROR_NOT_FOUND);
    //}
  }

  Database::Error PostgresDb::close() {
    if (hDb != nullptr) {
    //  int r = sqlite3_close(hDb);
      hDb = nullptr;
    }
    return Error::OK;
  }

  expected<sql_stmt, sql_error> PostgresDb::query(const std::string &sql_string) {
    sql_stmt stmt(new PostgresStmt(hDb));
    auto error = stmt.prepare(sql_string);
    if (error != sql_error::OK) {
      return make_unexpected(error);
    } else {
      return stmt;
    }
  }

  PostgresStmt::PostgresStmt(PGConn* db)
    : db_(db),
      prepare_rc(0/** TBD*/),
      exec_rc_(0 /** TBD*/)
       {
  }

  PostgresStmt::~PostgresStmt() {
    //sqlite3_finalize(stmt);
  }


  sql_error PostgresStmt::prepare(const std::string &sql_string) {
    prepare_rc = 0/** TBD*/; //sqlite3_prepare_v2(db_, sql_string.data(), sql_string.length(), &stmt, nullptr);
    return int2error(prepare_rc);
  }

  sql_error PostgresStmt::execute() {
    //if (prepare_rc != SQLITE_OK) {
      return int2error(prepare_rc);
    //} else {
    //  exec_rc_ = sqlite3_step(stmt);
    //  switch (exec_rc_) {
    //    case SQLITE_ROW: break;
    //    case SQLITE_DONE: break;
    //    case SQLITE_OK: break;
    //    default: return make_unexpected(int2error(exec_rc_));
    //  }
    //  return &result;
    // }
  }


  static sql_error int2error(int error) {
    switch (error) {
//      case SQLITE_OK: return sql_error::OK;
//        break;
      default: return sql_error::NOK;
        break;
    }
  }

  /*// sqlresult for sqlite3
  template<>
  size_t sql_result_impl<PostgresStmt>::column_count() {
    return sqlite3_column_count(stmt_->stmt);
  }

  template<>
  std::string sql_result_impl<PostgresStmt>::column_name(size_t iCol) {
    const char *pszName = sqlite3_column_name(stmt_->stmt, iCol);
    return pszName ? std::string(pszName) : std::string();
  }

  template<>
  const void *sql_result_impl<PostgresStmt>::column_blob(size_t iCol) {
    return sqlite3_column_blob(stmt_->stmt, iCol);
  }

  template<>
  double sql_result_impl<PostgresStmt>::column_double(size_t iCol) {
    return sqlite3_column_double(stmt_->stmt, iCol);
  }

  template<>
  int sql_result_impl<PostgresStmt>::column_int(size_t iCol) {
    return sqlite3_column_int(stmt_->stmt, iCol);
  }

  template<>
  int64_t sql_result_impl<PostgresStmt>::column_int64(size_t iCol) {
    return sqlite3_column_int64(stmt_->stmt, iCol);
  }

  template<>
  const unsigned char *sql_result_impl<PostgresStmt>::column_text(size_t iCol) {
    return sqlite3_column_text(stmt_->stmt, iCol);
  }

  template<>
  const void *sql_result_impl<PostgresStmt>::column_text16(size_t iCol) {
    return sqlite3_column_text16(stmt_->stmt, iCol);
  }

  template<>
  int sql_result_impl<PostgresStmt>::column_bytes(size_t iCol) {
    return sqlite3_column_bytes(stmt_->stmt, iCol);
  }

  template<>
  bool sql_result_impl<PostgresStmt>::next_row() {
    return sqlite3_step(stmt_->stmt) == SQLITE_ROW;
  }


  // sql_bindings for sqlite3
  template<>
  size_t sql_bindings_impl<PostgresStmt>::parameter_count() {
    return sqlite3_bind_parameter_count(stmt_->stmt);
  }

  template<>
  size_t sql_bindings_impl<PostgresStmt>::parameter_index(const char *zName) {
    return sqlite3_bind_parameter_index(stmt_->stmt, zName);
  }

  template<>
  const char *sql_bindings_impl<PostgresStmt>::parameter_name(size_t index) {
    return sqlite3_bind_parameter_name(stmt_->stmt, index);
  }

  template<>
  sql_error sql_bindings_impl<PostgresStmt>::bind_blob(size_t index, const void *pBlob, int n) {
    return int2error(sqlite3_bind_blob(stmt_->stmt, index, pBlob, n, nullptr));
  }

  template<>
  sql_error sql_bindings_impl<PostgresStmt>::bind_double(size_t index, double value) {
    return int2error(sqlite3_bind_double(stmt_->stmt, index, value));
  }

  template<>
  sql_error sql_bindings_impl<PostgresStmt>::bind_int(size_t index, int value) {
    return int2error(sqlite3_bind_int(stmt_->stmt, index, value));
  }

  template<>
  sql_error sql_bindings_impl<PostgresStmt>::bind_int64(size_t index, int64_t value) {
    return int2error(sqlite3_bind_int64(stmt_->stmt, index, value));
  }

  template<>
  sql_error sql_bindings_impl<PostgresStmt>::bind_null(size_t index) {
    return int2error(sqlite3_bind_null(stmt_->stmt, index));
  }

  template<>
  sql_error sql_bindings_impl<PostgresStmt>::bind_text(size_t index, const char *zText, int n) {
    return int2error(sqlite3_bind_text(stmt_->stmt, index, zText, n, nullptr));
  }

  template<>
  sql_error sql_bindings_impl<PostgresStmt>::bind_text16(size_t index, const void *zText16, int n) {
    return int2error(sqlite3_bind_text16(stmt_->stmt, index, zText16, n, nullptr));
  }*/
};
