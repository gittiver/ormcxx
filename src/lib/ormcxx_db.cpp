#include "ormcxx/ormcxx_db.hpp"
#include <sqlite3.h>

namespace ormcxx {



    struct sql_result {
        virtual size_t column_count()=0;

        virtual int column_int()=0;
        virtual const void *column_blob(int iCol)=0;
        virtual double column_double(int iCol)=0;
        virtual int column_int(int iCol)=0;
        virtual int64_t column_int64(int iCol)=0;
        virtual const unsigned char * column_text(int iCol)=0;
        virtual const void *column_text16(int iCol)=0;
        virtual int column_bytes(int iCol)=0;
        // int sqlite3_column_type(sqlite3_stmt*, int iCol);bool next_row();

        virtual bool next()=0;
    };

    template<class T>
    class sql_result_impl : public sql_result {
    public:
        size_t column_count() override;

        int column_int() override;

        const void * column_blob(int iCol) override;

        double column_double(int iCol) override;

        int column_int(int iCol) override;

        int64_t column_int64(int iCol) override;

        const unsigned char * column_text(int iCol) override;

        const void * column_text16(int iCol) override;

        int column_bytes(int iCol) override;

        bool next() override;

    private:
        const T* stmt_;
    public:

        sql_result_impl(const T * stmt): stmt_(stmt) {}; ;
    };
    class Sqlite3Stmt {

        friend class Sqlite3Db;
        sqlite3_stmt* stmt;
        int prepare_rc;
        sql_result_impl<Sqlite3Stmt> result;

        public:
            Sqlite3Stmt(sqlite3* db,const std::string& sql_string) : result(this) {
                prepare_rc = sqlite3_prepare_v2(db,sql_string.data(),sql_string.length(),&stmt, nullptr);
            }
            ~Sqlite3Stmt() { sqlite3_finalize(stmt); }

            tl::expected<sql_result*,int> execute() {

                if (prepare_rc != SQLITE_OK) {
                    return tl::make_unexpected(prepare_rc);
                } else {
                    return &result;
                }
            }
    };
    
    class Sqlite3Db {
        sqlite3* hDb{nullptr};
    public:
        ~Sqlite3Db() {
            close();
        }

        void open(const std::string& connInfo) {
            sqlite3_open(connInfo.c_str(), &hDb);
        }
        
        void close() {
            if (hDb != nullptr) {
                int r = sqlite3_close(hDb);
            }

        }

        Sqlite3Stmt prepare(const std::string& sql_string) {
            Sqlite3Stmt stmt(hDb,sql_string);
            return stmt;
        }
    };

    class DbDriver { 
    public:
        virtual tl::expected<Database*, Database::Error> open(const std::string& connInfo) = 0;

    };

    struct SQLiteDriver : public DbDriver {
        tl::expected<Database*, Database::Error> open(const std::string& connInfo) override;
       
    };


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
