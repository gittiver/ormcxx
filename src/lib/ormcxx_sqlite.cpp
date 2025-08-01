#include "ormcxx_sqlite.hpp"
#include <sqlite3.h>
#include <chrono>
#include <iostream>

namespace ormcxx {
  static sql_error int2error(int error);

  Sqlite3Db::Sqlite3Db()
    : hDb(nullptr) {
  }

  Sqlite3Db::~Sqlite3Db() {
    close();
  }

  expected<Database*, Database::Error> Sqlite3Db::open(const std::string &connInfo) {
    auto db = new Sqlite3Db();

    int r = sqlite3_open(connInfo.c_str(), &db->hDb);
    switch (r) {
      case SQLITE_OK: return db;
      default: delete db;
        return tl::make_unexpected(Database::Error::ERROR_NOT_FOUND);
    }
  }

  Database::Error Sqlite3Db::close() {
    if (hDb != nullptr) {
      int r = sqlite3_close(hDb);
      hDb = nullptr;
    }
    return Error::OK;
  }

  expected<sql_stmt, sql_error> Sqlite3Db::query(const std::string &sql_string) {
    sql_stmt stmt(new Sqlite3Stmt(hDb));
    auto error = stmt.prepare(sql_string);
    if (error != sql_error::OK) {
      return make_unexpected(error);
    } else {
      return stmt;
    }
  }

  Sqlite3Stmt::Sqlite3Stmt(sqlite3 *db)
    : db_(db),
      stmt(nullptr),
      prepare_rc(SQLITE_ERROR),
      exec_rc_(SQLITE_ERROR) {
  }

  Sqlite3Stmt::~Sqlite3Stmt() { sqlite3_finalize(stmt); }


  sql_error Sqlite3Stmt::prepare(const std::string &sql_string) {
    prepare_rc = sqlite3_prepare_v2(db_, sql_string.data(), sql_string.length(), &stmt, nullptr);
    return int2error(prepare_rc);
  }

  sql_error Sqlite3Stmt::execute() {
    if (prepare_rc != SQLITE_OK) {
      return int2error(prepare_rc);
    } else {
      const auto start{std::chrono::steady_clock::now()};
      exec_rc_ = sqlite3_step(stmt);
      const auto finish{std::chrono::steady_clock::now()};
      std::chrono::duration<double> duration = finish - start;
      std::cout << duration.count() << std::endl;
      switch (exec_rc_) {
        case SQLITE_ROW:
        case SQLITE_DONE:
        case SQLITE_OK: return sql_error::OK;
        default: return int2error(exec_rc_);
      }
    }
  }

  sql_error Sqlite3Stmt::execute(const std::string &sql_string) {
    sql_error rc = prepare(sql_string);
    if (rc != sql_error::OK) {
      return rc;
    } else {
      return execute();
    }
  }


  static sql_error int2error(int error) {
    switch (error) {
      case SQLITE_OK: return sql_error::OK;
        break;
      default: return sql_error::NOK;
        break;
    }
  }

  // sqlresult for sqlite3
  size_t Sqlite3Stmt::column_count() const {
    return sqlite3_column_count(stmt);
  }

  std::string Sqlite3Stmt::column_name(size_t iCol) const {
    const char *pszName = sqlite3_column_name(stmt, iCol);
    return pszName ? std::string(pszName) : std::string();
  }

  const void *Sqlite3Stmt::column_blob(size_t iCol) const {
    return sqlite3_column_blob(stmt, iCol);
  }

  double Sqlite3Stmt::column_double(size_t iCol) const {
    return sqlite3_column_double(stmt, iCol);
  }

  int Sqlite3Stmt::column_int(size_t iCol) const {
    return sqlite3_column_int(stmt, iCol);
  }

  int64_t Sqlite3Stmt::column_int64(size_t iCol)  const{
    return sqlite3_column_int64(stmt, iCol);
  }

  const unsigned char *Sqlite3Stmt::column_text(size_t iCol)  const{
    return sqlite3_column_text(stmt, iCol);
  }

  const void *Sqlite3Stmt::column_text16(size_t iCol)  const{
    return sqlite3_column_text16(stmt, iCol);
  }

  int Sqlite3Stmt::column_bytes(size_t iCol)  const{
    return sqlite3_column_bytes(stmt, iCol);
  }

  bool Sqlite3Stmt::next_row()  {
    return sqlite3_step(stmt) == SQLITE_ROW;
  }


  // sql_bindings for sqlite3
  size_t Sqlite3Stmt::parameter_count() {
    return sqlite3_bind_parameter_count(stmt);
  }


  size_t Sqlite3Stmt::parameter_index(const char *zName) {
    return sqlite3_bind_parameter_index(stmt, zName);
  }


  const char *Sqlite3Stmt::parameter_name(size_t index) {
    return sqlite3_bind_parameter_name(stmt, index);
  }


  sql_error Sqlite3Stmt::bind_blob(size_t index, const void *pBlob, size_t n) {
    return int2error(sqlite3_bind_blob(stmt, index, pBlob, n, nullptr));
  }


  sql_error Sqlite3Stmt::bind_double(size_t index, double value) {
    return int2error(sqlite3_bind_double(stmt, index, value));
  }


  sql_error Sqlite3Stmt::bind_int(size_t index, int value) {
    return int2error(sqlite3_bind_int(stmt, index, value));
  }


  sql_error Sqlite3Stmt::bind_int64(size_t index, int64_t value) {
    return int2error(sqlite3_bind_int64(stmt, index, value));
  }


  sql_error Sqlite3Stmt::bind_null(size_t index) {
    return int2error(sqlite3_bind_null(stmt, index));
  }


  sql_error Sqlite3Stmt::bind_text(size_t index, const char *zText, size_t n) {
    return int2error(sqlite3_bind_text(stmt, index, zText, n, nullptr));
  }


  sql_error Sqlite3Stmt::bind_text16(size_t index, const void *zText16, size_t len) {
    return int2error(sqlite3_bind_text16(stmt, index, zText16, len, nullptr));
  }
};
