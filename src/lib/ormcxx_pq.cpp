#include "ormcxx_pq.hpp"
#include "libpq-fe.h"
#include <iostream>

namespace ormcxx {
  static sql_error int2error(int error);

  PostgresDb::~PostgresDb() {
    close();
  }

  expected<Database *, Database::Error> PostgresDb::open(const std::string &connInfo) {
    pg_conn *conn = PQconnectdb(connInfo.c_str());
    switch (PQstatus(conn)) {
      case CONNECTION_OK: {
        auto db = new PostgresDb();
        db->hDb = conn;
        return db;
      }
      case CONNECTION_BAD:
      default:
        PQfinish(conn);
        return make_unexpected(Database::Error::ERROR_NOT_FOUND);
    }
  }

  Database::Error PostgresDb::close() {
    if (hDb != nullptr) {
      PQfinish(hDb);
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

  PostgresStmt::PostgresStmt(PGconn *db)
    : db_(db),
      prepare_rc(0/** TBD*/),
      exec_rc_(0 /** TBD*/) {
  }

  PostgresStmt::~PostgresStmt() {
    PQclear(res);
    PQclear(res_describe_prepared);
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

  sql_error PostgresStmt::execute(const std::string &sql_string) {
    res = PQexec(this->db_, sql_string.c_str());
    auto execStatus = PQresultStatus(res);

    switch (execStatus) {
      case PGRES_COMMAND_OK:
      case PGRES_TUPLES_OK:
        return sql_error::OK;
      default:
        std::cout << PQerrorMessage(db_);
        return int2error(prepare_rc);
    }
  }


  static sql_error int2error(int error) {
    switch (error) {
      case PGRES_COMMAND_OK:
      case PGRES_TUPLES_OK:
        return sql_error::OK;
      default: return sql_error::NOK;
        break;
    }
  }

  // sqlresult for sqlite3
  size_t PostgresStmt::column_count() const {
    return PQnfields(res);
  }

  std::string PostgresStmt::column_name(size_t iCol) const {
    const char *pszName = PQfname(res, iCol);
    return pszName ? std::string(pszName) : std::string();
  }

  const void *PostgresStmt::column_blob(size_t iCol) const {
    return PQgetvalue(res, row, iCol);
  }

  double PostgresStmt::column_double(size_t iCol) const {
    double d = 0.0;
    char *pval = PQgetvalue(res, row, iCol);
    d = *pval;
    return d;
  }

  int PostgresStmt::column_int(size_t iCol) const {
    int v = 0.0;
    char *pval = PQgetvalue(res, row, iCol);
    v = *pval;
    return v;
  }

  int64_t PostgresStmt::column_int64(size_t iCol) const {
    int64_t v = 0.0;
    char *pval = PQgetvalue(res, row, iCol);
    v = *pval;
    return v;
  }

  const unsigned char *PostgresStmt::column_text(size_t iCol) const {
    return reinterpret_cast<unsigned char *>(PQgetvalue(res, row, iCol));
  }

  const void *PostgresStmt::column_text16(size_t iCol) const {
    return PQgetvalue(res, row, iCol);
  }

  int PostgresStmt::column_bytes(size_t iCol) const {
    return PQgetlength(res, row, iCol);
  }

  bool PostgresStmt::next_row() {
    char *pszTuples = PQcmdTuples(res);
    long long tuples = atoll(pszTuples);
    if (row < tuples - 1) {
      ++row;
      return true;
    } else
      return false;
  }


  // sql_bindings for sqlite3
  size_t PostgresStmt::parameter_count() {
    return PQnparams(res_describe_prepared);
  }

  size_t PostgresStmt::parameter_index(const char *pszName) {
    return PQfnumber(res_describe_prepared, pszName);
  }

  const char *PostgresStmt::parameter_name(size_t index) {
    return PQfname(res_describe_prepared, index);
  }

  sql_error PostgresStmt::bind_blob(size_t index, const void *pBlob, size_t n) {
    return sql_error::NOK;
  }

  sql_error PostgresStmt::bind_double(size_t index, double value) {
    return sql_error::NOK;
  }

  sql_error PostgresStmt::bind_int(size_t index, int value) {
    return sql_error::NOK;
  }

  sql_error PostgresStmt::bind_int64(size_t index, int64_t value) {
    return sql_error::NOK;
  }

  sql_error PostgresStmt::bind_null(size_t index) {
    return sql_error::NOK;
  }

  sql_error PostgresStmt::bind_text(size_t index, const char *zText, size_t n) {
    return sql_error::NOK;
  }

  sql_error PostgresStmt::bind_text16(size_t index, const void *zText16, size_t len) {
    return sql_error::NOK;
  }
};
