#include "ormcxx/ormcxx_db.hpp"
/// TBD remove sqlite3 include here
#include <sqlite3.h>

namespace ormcxx {

    struct SQLiteDriver  {
        tl::expected<Database*, Database::Error> open(const std::string& connInfo);
    };

    class Sqlite3Stmt: public sql_stmt {
        friend class sql_bindings_impl<Sqlite3Stmt>;
        friend class sql_result_impl<Sqlite3Stmt>;
        friend class Sqlite3Db;
        sql_result_impl<Sqlite3Stmt> result;
        sql_bindings_impl<Sqlite3Stmt> bindings_;
        sqlite3* db_;
        sqlite3_stmt* stmt;
        int prepare_rc;
        int exec_rc_;

    public:
            Sqlite3Stmt(sqlite3* db);
            ~Sqlite3Stmt();
            sql_error prepare(const std::string &sql_string) override;

            sql_bindings& bindings() override { return bindings_; }
            expected<sql_result*, sql_error> execute() override;
    };

    class Sqlite3Db: public Database {
    private:
        sqlite3* hDb{ nullptr };
    public:
        ~Sqlite3Db();

        static expected<Database*,Database::Error> open(const std::string& connInfo);

        Error close() override;

        expected<sql_stmt *, sql_error> query(const std::string &sql_string) override;
    };
}
