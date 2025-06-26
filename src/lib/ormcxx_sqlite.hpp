#include "ormcxx/ormcxx_db.hpp"
#include <sqlite3.h>

namespace ormcxx {

    struct SQLiteDriver : public DbDriver {
        tl::expected<Database*, Database::Error> open(const std::string& connInfo) override;
    };

    class Sqlite3Stmt {

        friend class Sqlite3Db;
        sqlite3_stmt* stmt;
        int prepare_rc;
        sql_result_impl<Sqlite3Stmt> result;

        public:
            Sqlite3Stmt(sqlite3* db, const std::string& sql_string);
            ~Sqlite3Stmt();

            tl::expected<sql_result*, int> execute();
    };

    class Sqlite3Db {
        sqlite3* hDb{ nullptr };
    public:
        ~Sqlite3Db();

        void open(const std::string& connInfo);

        void close();

        Sqlite3Stmt prepare(const std::string& sql_string);
    };
}
