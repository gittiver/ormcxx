#include "ormcxx/ormcxx_db.hpp"
#include <sqlite3.h>

namespace ormcxx {


    
    
    class SqliteDb {
        sqlite3* hDb{nullptr};
    public:
        ~SqliteDb() {
            close();
        }

        void open(const std::string& connInfo) {
            sqlite3_open(connInfo.c_str(), &hDb);
        }
        
        void close() {
            if (hDb != nullptr) {
                sqlite3_close(hDb);
            }

        }

        void prepare() {
        //    sqlite3_prepare_v2(hDb,)
        }
    };

    class DbDriver { 
    public:
        virtual tl::expected<Database*, Database::Error> open(const std::string& connInfo) = 0;
    };

    struct SQLiteDriver : public DbDriver {
        virtual tl::expected<Database*, Database::Error> open(const std::string& connInfo);
       
    };


    tl::expected<Database*, Database::Error> Database::open(BackendType backendType, const std::string& connInfo)
    {
        DbDriver* driver = nullptr;
        switch (backendType) {
        case BackendType::SQLITE:
            driver = new SQLiteDriver();
            break;

        }
        if (!driver)
            return tl::make_unexpected(Error::NO_DB_DRIVER);
        else
            return driver->open(connInfo);
    }


}
