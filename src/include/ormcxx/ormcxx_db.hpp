//
// Created by Gulliver on 07.06.25.
//

#ifndef ORMCXX_DB_HPP
#define ORMCXX_DB_HPP
#include <tl/expected.hpp>
#include <string>

namespace ormcxx {

    using tl::expected;

    struct sql_result {
        enum class Error {
            OK,
            NOT_OK
        };

        virtual size_t column_count() = 0;

        virtual int column_int() = 0;
        virtual const void* column_blob(int iCol) = 0;
        virtual double column_double(int iCol) = 0;
        virtual int column_int(int iCol) = 0;
        virtual int64_t column_int64(int iCol) = 0;
        virtual const unsigned char* column_text(int iCol) = 0;
        virtual const void* column_text16(int iCol) = 0;
        virtual int column_bytes(int iCol) = 0;
        // int sqlite3_column_type(sqlite3_stmt*, int iCol);bool next_row();

        virtual bool next() = 0;
    };

    template<class T>
    class sql_result_impl : public sql_result {
    public:
        size_t column_count() override;

        int column_int() override;

        const void* column_blob(int iCol) override;

        double column_double(int iCol) override;

        int column_int(int iCol) override;

        int64_t column_int64(int iCol) override;

        const unsigned char* column_text(int iCol) override;

        const void* column_text16(int iCol) override;

        int column_bytes(int iCol) override;

        bool next() override;

    private:
        const T* stmt_;
    public:

        sql_result_impl(const T* stmt) : stmt_(stmt) {}; ;
    };

	class Database {
    public:
        enum class Error {
            OK,
            NO_DB_DRIVER,
            ERROR_NOT_FOUND
        };
        enum class BackendType {
            SQLITE,
        //    ODBC,
        //    MARIA_DB,
        //    POSTGRESQL
        };


        /** closes database */
        virtual ~Database()=default;
        
        static expected<Database*,Error> open(BackendType backendType, const std::string& connInfo);

        virtual Error open() = 0;
        virtual Error close() = 0;

        virtual expected<sql_result*,sql_result::Error> execute(const std::string& sql_string)=0;
        struct Result;


    };
    class DbDriver {
    public:
        virtual expected<Database*, Database::Error> open(const std::string& connInfo) = 0;
    };


} // ormcxx

#endif //ORMCXX_DB_HPP
