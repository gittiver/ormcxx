#include "ormcxx/ormcxx_db.hpp"

namespace ormcxx {

    struct PGConn;
    struct PQDriver  {
        tl::expected<Database*, Database::Error> open(const std::string& connInfo);
    };

    class PostgresStmt: public sql_stmt {
        friend class sql_bindings_impl<PostgresStmt>;
        friend class sql_result_impl<PostgresStmt>;
        friend class PostgresDb;
        sql_result_impl<PostgresStmt> result;
        sql_bindings_impl<PostgresStmt> bindings_;
        PGConn* db_;
        const char* stmt;
        int prepare_rc;
        int exec_rc_;

    public:
            PostgresStmt(PGConn* db);
            ~PostgresStmt();
            sql_error prepare(const std::string &sql_string) override;

            sql_bindings& bindings() override { return bindings_; }
            expected<sql_result*, sql_error> execute() override;
    };

    class PostgresDb: public Database {
    private:
        PGConn* hDb{ nullptr };
    public:
        ~PostgresDb();

        static expected<Database*,Database::Error> open(const std::string& connInfo);

        Error close() override;

        expected<sql_stmt *, sql_error> query(const std::string &sql_string) override;
    };
}
