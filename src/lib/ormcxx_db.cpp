#include "ormcxx/ormcxx_db.hpp"
#include "ormcxx_sqlite.hpp"

namespace ormcxx {
    tl::expected<Database*, Database::Error> Database::open(BackendType backendType, const std::string& connInfo)
    {
        DbDriver* driver = nullptr;
        switch (backendType) {
        case BackendType::SQLITE:
            driver = new SQLiteDriver();
            break;
        default:
            break;
        }
        if (!driver)
            return tl::make_unexpected(Error::NO_DB_DRIVER);
        else
            return driver->open(connInfo);
    }
}
