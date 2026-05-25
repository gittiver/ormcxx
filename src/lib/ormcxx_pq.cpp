#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib") // Link against Ws2_32.lib on MSVC
#else
#include <netinet/in.h>
#ifndef ntohll
#define ntohll(x) ( ((uint64_t)ntohl(x & 0xFFFFFFFF) << 32) | ntohl(x >> 32) )
#endif
#endif
#include <cstring>
#include <iostream>

#include "libpq-fe.h"

#include "ormcxx_pq.hpp"

namespace ormcxx {
  static sql_error status2error(int error);

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
    this->stmtName = sql_string;
    res = PQprepare(db_,this->stmtName.c_str(),sql_string.c_str(),0,nullptr);
    prepare_rc = PQresultStatus(res);

    switch (prepare_rc) {
      case PGRES_COMMAND_OK:
      case PGRES_TUPLES_OK:
        return sql_error::OK;
      default:
        std::cout << PQerrorMessage(db_);
        return status2error(prepare_rc);
    }
  }

  sql_error PostgresStmt::execute() {
    PQclear(res);
    res = PQexecPrepared(db_,
      this->stmtName.c_str(),
      this->parameters.size(),
      this->parameters.values.data(),
      this->parameters.lengths.data(),
      this->parameters.formats.data(),
      1);

    exec_rc_ = PQresultStatus(res);

    switch (exec_rc_) {
      case PGRES_TUPLES_OK:
        {
          char *pszTuples = PQcmdTuples(res);
          tuples = atoll(pszTuples);
        }
      case PGRES_COMMAND_OK:
        return sql_error::OK;
      default:
        std::cout << PQerrorMessage(db_);
        return status2error(prepare_rc);
    }
  }

  sql_error PostgresStmt::execute(const std::string &sql_string) {
    res = PQexec(this->db_, sql_string.c_str());
    auto execStatus = PQresultStatus(res);

    switch (execStatus) {
      case PGRES_TUPLES_OK:
      {
        char *pszTuples = PQcmdTuples(res);
        tuples = atoll(pszTuples);
      }

      case PGRES_COMMAND_OK:
        return sql_error::OK;
      default:
        std::cout << PQerrorMessage(db_);
        return status2error(prepare_rc);
    }
  }

  int64_t PostgresStmt::last_inserted_id() const {
    throw std::runtime_error("not implemented");
    return 0; /// TODO
  }

  sql_error PostgresStmt::reset() {
    return sql_error::OK;
  }


  static sql_error status2error(int error) {
    switch (error) {
      case PGRES_COMMAND_OK:
      case PGRES_TUPLES_OK:
        return sql_error::OK;
      default: return sql_error::NOK;
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
    throw std::runtime_error("conversion not implemented");
    return PQgetvalue(res, row, iCol);
  }

  double PostgresStmt::column_double(size_t iCol) const {
    int isnull = PQgetisnull(res, row, iCol);
    if (isnull) {
      return 0.0;
    } else {
      uint64_t v = 0;
      int l= PQgetlength(res, row, iCol);

      if (l!=sizeof v)
        throw std::runtime_error("conversion error");
      memcpy(&v, PQgetvalue(res, row, iCol), sizeof v);
      v = ntohll(v);
      double result;
      std::memcpy(&result, &v, sizeof(result));
      return result;
    }
  }

  int PostgresStmt::column_int(size_t iCol) const {
    int32_t v = 0;
    int isnull = PQgetisnull(res, row, iCol);
    if (isnull) {

    } else {
      int l= PQgetlength(res, row, iCol);
      if (l!=sizeof v)
        throw std::runtime_error("conversion error");
      memcpy(&v, PQgetvalue(res, row, iCol), sizeof v);
      v = ntohl(v);
    }
    return v;
  }

  int64_t PostgresStmt::column_int64(size_t iCol) const {
    int64_t v = 0;
    int isnull = PQgetisnull(res, row, iCol);
    if (isnull) {

    } else {
      int l= PQgetlength(res, row, iCol);
      if (l!=sizeof v)
        throw std::runtime_error("conversion error");
      memcpy(&v, PQgetvalue(res, row, iCol), sizeof v);
      v = ntohll(v);
    }
    return v;
  }

  const unsigned char *PostgresStmt::column_text(size_t iCol) const {
    int isnull = PQgetisnull(res, row, iCol);
    if (isnull) {
      return nullptr;
    } else {
      return reinterpret_cast<unsigned char *>(PQgetvalue(res, row, iCol));
    }
  }

  const void *PostgresStmt::column_text16(size_t iCol) const {
    int isnull = PQgetisnull(res, row, iCol);
    if (isnull) {
      return nullptr;
    } else {
      return PQgetvalue(res, row, iCol);
    }
  }

  bool PostgresStmt::has_next_row() const {
    return row < tuples;
  }

  bool PostgresStmt::next_row()  {
    if (has_next_row()) {
      ++row;
      return true;
    } else
      return false;
  }

  void  PostgresStmt::Params::bind(size_t index, const char *value, int length, int format) {
    if (index==values.size()) {
      values.push_back(value);
      lengths.push_back(length);
      formats.push_back(format);
    } else if (index<values.size()) {
      values[index] = value;
      lengths[index] = length;
      formats[index] = format;
    } else {
      values.insert(values.end(),index-values.size(),nullptr);
      values.push_back(value);

      lengths.insert(lengths.end(),index-lengths.size(),0);
      lengths.push_back(length);

      formats.insert(formats.end(),index-formats.size(),0);
      formats.push_back(format);
    }
  }

  int PostgresStmt::Params::size() const {
    return values.size();
  }

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
    parameters.bind(index,static_cast<const char*>(pBlob),n,1);
    return sql_error::OK;
  }

  sql_error PostgresStmt::bind_double(size_t index, double value) {
    std::string s = std::to_string(value);
    parameters.bind(index,s.data(),s.size());
    return sql_error::OK;
  }

  sql_error PostgresStmt::bind_int(size_t index, int value) {
    std::string s = std::to_string(value);
    parameters.bind(index,s.data(),s.size());
    return sql_error::OK;
  }

  sql_error PostgresStmt::bind_int64(size_t index, int64_t value) {
    std::string s = std::to_string(value);
    parameters.bind(index,s.data(),s.size());
    return sql_error::OK;
  }

  sql_error PostgresStmt::bind_null(size_t index) {
    parameters.bind(index,nullptr,0);
    return sql_error::OK;
  }

  sql_error PostgresStmt::bind_text(size_t index, const char *zText, size_t n) {
    parameters.bind(index,zText,n,0);
    return sql_error::OK;
  }

  sql_error PostgresStmt::bind_text16(size_t index, const void *zText16, size_t len) {
    parameters.bind(index,static_cast<const char*>(zText16),len,0);
    return sql_error::OK;
  }
};
