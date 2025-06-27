//
// Created by Gulliver on 07.06.25.
//

#ifndef ORMCXX_DB_HPP
#define ORMCXX_DB_HPP

#include <tl/expected.hpp>
#include <string>

namespace ormcxx {

    using tl::expected;

    struct sql_bindings {
        enum class Error: int {OK, NOK};

        virtual ~sql_bindings() = default;

        virtual size_t parameter_count() = 0;

        virtual size_t parameter_index(const char *zName) = 0;
        virtual const char* parameter_name(size_t index) = 0;

        virtual Error bind_blob(size_t index, const void*, int n)=0;
        virtual Error bind_double(size_t index, double)=0;
        virtual Error bind_int(size_t index, int)=0;
        virtual Error bind_int64(size_t index, int64_t)=0;
        virtual Error bind_null(size_t)=0;
        virtual Error bind_text(size_t index,const char*,int)=0;
        virtual Error bind_text16(size_t index, const void*, int)=0;
    };

    struct sql_result {
        virtual ~sql_result() = default;

        virtual size_t column_count() = 0;

        virtual const void* column_blob(size_t iCol) = 0;
        virtual double column_double(size_t iCol) = 0;
        virtual int column_int(size_t iCol) = 0;
        virtual int64_t column_int64(size_t iCol) = 0;
        virtual const unsigned char* column_text(size_t iCol) = 0;
        virtual const void* column_text16(size_t iCol) = 0;
        virtual int column_bytes(size_t iCol) = 0;
        // int sqlite3_column_type(sqlite3_stmt*, size_t iCol);bool next_row();

        virtual bool next_row() = 0;
    };

    struct sql_stmt {
        enum class error {
            OK, NOK
        };
        virtual ~sql_stmt() = default;

       virtual sql_bindings& bindings() = 0;

        virtual error prepare(const std::string& sql_string)=0;
        virtual expected<sql_result*,error> execute() = 0;
        virtual expected<sql_result*,error> execute(const std::string& sql_string);
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

        virtual Error close() = 0;

        // virtual Error open(const std::string& connInfo) = 0;
	    virtual expected<sql_stmt*,sql_stmt::error> query(const std::string& sql_string) =0;
        expected<sql_result*,Error> execute(const std::string& sql_string);
        struct Result;


    };

    template<class T>
    class sql_bindings_impl : public sql_bindings {
    public:
        size_t parameter_count() override;

        size_t parameter_index(const char *zName) override;
        const char* parameter_name(size_t index) override;

        Error bind_blob(size_t index, const void*, int n) override;
        Error bind_double(size_t index, double) override;
        Error bind_int(size_t index, int) override;
        Error bind_int64(size_t index, int64_t) override;
        Error bind_null(size_t) override;
        Error bind_text(size_t index,const char*,int) override;
        Error bind_text16(size_t index, const void*, int) override;

    private:
        const T* stmt_;
    public:

        sql_bindings_impl(const T* stmt) : stmt_(stmt) {}; ;
    };
    template<class T>
    class sql_result_impl : public sql_result {
    public:
        size_t column_count() override;

        const void* column_blob(size_t iCol) override;

        double column_double(size_t iCol) override;

        int column_int(size_t iCol) override;

        int64_t column_int64(size_t iCol) override;

        const unsigned char* column_text(size_t iCol) override;

        const void* column_text16(size_t iCol) override;

        int column_bytes(size_t iCol) override;

        bool next_row() override;

    private:
        const T* stmt_;
    public:

        sql_result_impl(const T* stmt) : stmt_(stmt) {}; ;
    };

} // ormcxx

#endif //ORMCXX_DB_HPP
