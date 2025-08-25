//
// Created by Gulliver on 07.06.25.
//

#ifndef ORMCXX_DB_HPP
#define ORMCXX_DB_HPP

#include <tl/expected.hpp>
#include <string>
#include <memory>
#include <map>

namespace ormcxx {

    using tl::expected;
    using tl::make_unexpected;

    enum class sql_error: int {OK, NOK};

    struct sql_bindings {

        virtual ~sql_bindings() = default;

        virtual size_t parameter_count() = 0;

        virtual size_t parameter_index(const char *zName) = 0;
        virtual const char* parameter_name(size_t index) = 0;

        virtual sql_error bind_blob(size_t index, const void*, size_t n)=0;
        virtual sql_error bind_double(size_t index, double)=0;
        virtual sql_error bind_int(size_t index, int)=0;
        virtual sql_error bind_int64(size_t index, int64_t)=0;
        virtual sql_error bind_null(size_t)=0;
        virtual sql_error bind_text(size_t index, const char* pszText, size_t pszText_Len)=0;
        virtual sql_error bind_text16(size_t index, const void*, size_t len)=0;
        virtual sql_error bind_text(size_t index,const std::string&);
    };

    struct sql_result {
        virtual ~sql_result() = default;

        virtual size_t column_count() const = 0;
        virtual std::string column_name(size_t iCol) const = 0;

        virtual const void* column_blob(size_t iCol) const = 0;
        virtual double column_double(size_t iCol) const = 0;
        virtual int column_int(size_t iCol) const = 0;
        virtual int64_t column_int64(size_t iCol) const = 0;
        virtual const unsigned char* column_text(size_t iCol) const = 0;
        virtual const void* column_text16(size_t iCol) const = 0;
        virtual int column_bytes(size_t iCol) const = 0;
        // int sqlite3_column_type(sqlite3_stmt*, size_t iCol);bool next_row();
        virtual int64_t last_inserted_id() const  = 0 ;
        virtual bool next_row() const = 0;
    };

    struct sql_stmt_base {
        virtual ~sql_stmt_base() = default;

        virtual sql_bindings& bindings() = 0;
        virtual const sql_result& result() const= 0;
        virtual sql_result& result() = 0;

        virtual sql_error prepare(const std::string& sql_string)=0;
        virtual sql_error execute() = 0;
        virtual sql_error execute(const std::string& sql_string)=0;
        virtual sql_error reset() = 0;
 };

    struct sql_stmt: public sql_stmt_base {

        explicit sql_stmt(sql_stmt_base* pImpl_);
        sql_stmt(sql_stmt&&) = default;
        sql_stmt(const sql_stmt&) = delete;

        sql_stmt& operator=(sql_stmt&&) = default;
        sql_stmt& operator=(const sql_stmt&) = delete;
        sql_bindings& bindings() override;
        const sql_result& result() const override;
        sql_result& result() override;
        sql_error prepare(const std::string& sql_string) override;
        sql_error execute(const std::string& sql_string) override;

        sql_error execute() override;
        sql_error reset() override;
    private:
        std::unique_ptr<sql_stmt_base> pImpl;

    };

    class Driver {
        public:
        enum class BackendType {
            SQLITE,
        //    ODBC,
        //    MARIA_DB,
        //    POSTGRESQL
        };

        static Driver* instance(BackendType backend);

        typedef std::map<std::type_info,std::string> type_map;
        virtual type_map typemap()=0;
    };

    class DatabaseImpl {
    public:
        enum class Error {
            OK,
            NO_DB_DRIVER,
            ERROR_NOT_FOUND
        };
        virtual ~DatabaseImpl() = default;

        virtual Error close()=0;

        virtual expected<sql_stmt,sql_error> query(const std::string& sql_string)=0;
    };
	class Database: public DatabaseImpl {
    public:

        enum class BackendType {
            SQLITE,
        //    ODBC,
        //    MARIA_DB,
            POSTGRESQL
        };


	    explicit Database(DatabaseImpl* pImpl_);
	    Database(Database&&) = default;
	    Database(const Database&) = delete;

	    Database& operator=(Database&&) = default;
	    Database& operator=(const Database&) = delete;

        static expected<Database,Error> open(BackendType backendType, const std::string& connInfo);
	    Error close() override;


        expected<sql_stmt,sql_error> query(const std::string& sql_string) override;
    protected:
	    Database()=default;

	private:
	    std::unique_ptr<DatabaseImpl> pImpl;
    };
} // ormcxx

#endif //ORMCXX_DB_HPP
