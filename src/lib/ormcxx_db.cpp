#include "ormcxx/ormcxx_db.hpp"
#include "ormcxx_sqlite.hpp"

namespace ormcxx {
    expected<sql_result *, sql_error> sql_stmt::execute(const std::string &sql_string) {
        sql_error r = prepare(sql_string);
        /// TBD check result of prepare before execute
        return execute();
    }

    tl::expected<Database*, Database::Error> Database::open(BackendType backendType, const std::string& connInfo)
    {
        Database* db = nullptr;
        switch (backendType) {
            case BackendType::SQLITE:
            db = Sqlite3Db::open(connInfo).value_or(nullptr);
            break;
        default:
            break;
        }
        if (!db)
            return make_unexpected(Error::NO_DB_DRIVER);
        else
            return db;
    }
}
