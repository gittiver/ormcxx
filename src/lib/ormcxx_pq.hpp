#include "ormcxx/ormcxx_db.hpp"
struct pg_conn;
struct pg_result;
namespace ormcxx {
  struct PQDriver {
    static expected<Database *, Database::Error> open(const std::string &connInfo);
  };

  class PostgresStmt : public sql_stmt_base,
                       private sql_bindings,
                       private sql_result {
    friend class PostgresDb;
    pg_conn *db_;
    const char *stmt;
    int prepare_rc;
    int exec_rc_;
    pg_result *res;
    pg_result *res_describe_prepared;
    size_t row{0};
  public:
    PostgresStmt(pg_conn *db);

    ~PostgresStmt();

    sql_error prepare(const std::string &sql_string) override;

    sql_bindings &bindings() override { return *this; }
    sql_result &result() { return *this; }
    const sql_result &result() const override { return *this; }

    sql_error execute() override;

    sql_error execute(const std::string &sql_string) override;

  private:
    size_t parameter_count() override;

    size_t parameter_index(const char *zName) override;

    const char *parameter_name(size_t index) override;

    sql_error bind_blob(size_t index, const void *, int n) override;

    sql_error bind_double(size_t index, double) override;

    sql_error bind_int(size_t index, int) override;

    sql_error bind_int64(size_t index, int64_t) override;

    sql_error bind_null(size_t) override;

    sql_error bind_text(size_t index, const char *, int) override;

    sql_error bind_text16(size_t index, const void *, int) override;

    // implementation of sql_result_base interface
    size_t column_count() const override;

    std::string column_name(size_t iCol) const override;

    const void *column_blob(size_t iCol) const override;

    double column_double(size_t iCol) const override;

    int column_int(size_t iCol) const override;

    int64_t column_int64(size_t iCol) const override;

    const unsigned char *column_text(size_t iCol) const override;

    const void *column_text16(size_t iCol) const override;

    int column_bytes(size_t iCol) const override;

    bool next_row() override;
  };

  class PostgresDb : public Database {
  private:
    struct pg_conn *hDb{nullptr};

  public:
    ~PostgresDb();

    static expected<Database *, Database::Error> open(const std::string &connInfo);

    Error close() override;

    expected<sql_stmt, sql_error> query(const std::string &sql_string) override;
  };
}
