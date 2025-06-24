//
// Created by Gulliver on 07.06.25.
//

#ifndef ORMCXX_DB_HPP
#define ORMCXX_DB_HPP
#include <tl/expected.hpp>
#include <string>

namespace ormcxx {
	
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
        
        static tl::expected<Database*,Error> open(BackendType backendType, const std::string& connInfo);

        virtual Error open() = 0;
        virtual Error close() = 0;

        struct Result;


    };

} // ormcxx

#endif //ORMCXX_DB_HPP
