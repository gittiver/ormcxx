#include <iostream>

#include "catch2/catch_all.hpp"
#include "ormcxx/ormcxx_db.hpp"
using namespace std;

using ormcxx::Database;

struct descriptor {
    Database::BackendType backend_type;
    std::string connection_string;
};

const std::vector<descriptor> connection_types = {
    {Database::BackendType::SQLITE, "test_db"},
    {Database::BackendType::POSTGRESQL, "postgresql://testuser:mypassword@localhost/test_db"}
};

TEST_CASE("open database") {
    for (auto const &c: connection_types) {
        auto db = Database::open(c.backend_type, c.connection_string);
        if (!db) {
            FAIL("can not open connection:"+ std::to_string((int)c.backend_type)+"," + c.connection_string);
        }
        REQUIRE(db.has_value()==true);
    }
}


TEST_CASE("sql_query_and_execute") {
    for (auto const &c: connection_types) {
        auto db = Database::open(c.backend_type, c.connection_string);
        if (!db) {
            FAIL("can not open connection:"+ std::to_string((int)c.backend_type)+"," + c.connection_string);
        } else {
            auto query = db->query("DROP TABLE IF EXISTS contacts;");

            query = db->query(
                "CREATE TABLE IF NOT EXISTS contacts ( "
                "contact_id INTEGER PRIMARY KEY,"
                "first_name TEXT NOT NULL,"
                "last_name TEXT NOT NULL,"
                "email TEXT NOT NULL,"
                "phone TEXT NOT NULL"
                ");"
            );
            REQUIRE(query.has_value());
            query->execute();
        }
    }
}

TEST_CASE("sql_query_bindings") {
    for (auto const &c: connection_types) {
        auto db = Database::open(c.backend_type, c.connection_string);
        if (!db) {
            FAIL("can not open connection:"+ std::to_string((int)c.backend_type)+"," + c.connection_string);
        } else {
            const std::string CREATE_TABLE_QUERY = (c.backend_type==Database::BackendType::SQLITE) ?

                "CREATE TABLE IF NOT EXISTS contacts ( "
                 "contact_id INTEGER PRIMARY KEY,"
                 "first_name TEXT NOT NULL,"
                 "last_name TEXT NOT NULL,"
                 "email TEXT NOT NULL,"
                 "phone TEXT NOT NULL"
                 ");"
            :     "CREATE TABLE IF NOT EXISTS contacts ( "
                 "contact_id INTEGER PRIMARY KEY GENERATED ALWAYS as IDENTITY,"
                 "first_name TEXT NOT NULL,"
                 "last_name TEXT NOT NULL,"
                 "email TEXT NOT NULL,"
                 "phone TEXT NOT NULL"
                 ");";
            auto query = db->query("DROP TABLE IF EXISTS contacts;");
            query->execute();

            query = db->query( CREATE_TABLE_QUERY);
            query->execute();

            query = db->query("DELETE from contacts;");
            query->execute();

            query = db->query(
                "INSERT INTO contacts(first_name,last_name,email,phone) VALUES ($1,$2,$3,$4);");
            if (!query) {
                auto error = query.error();
                FAIL("can not do query:");
            }
            REQUIRE(query.has_value()==true);
            std::cout << "n_of_parameters: " << query->bindings().parameter_count() << endl;
            query->bindings().bind_text(0,"Frank",strlen("Frank"));
            query->bindings().bind_text(1, "Frank", strlen("Frank"));
            query->bindings().bind_text(2, "Frank", strlen("Frank"));
            query->bindings().bind_text(3, "Frank", strlen("Frank"));

            auto result = query->execute();
            REQUIRE(result==ormcxx::sql_error::OK);
            result = query->execute();

            REQUIRE(result==ormcxx::sql_error::OK);
            auto query2 = db->query("SELECT * FROM contacts;");
            REQUIRE(query2.has_value());

            result = query2->execute();

            REQUIRE(result==ormcxx::sql_error::OK);
            REQUIRE(query2->result().column_count()==5);
            for (auto i = 0; i < 5; i++) {
                std::cout << query2->result().column_name(i) << std::endl;
            }
            size_t row_count =0;
            for (
                ; query2->result().has_next_row()
                ; query2->result().next_row(),row_count++) {
                int id = query2->result().column_int(0);
                const char* firstname =reinterpret_cast<const char*>(query2->result().column_text(1));
                std::cout << row_count << '|' << id << "|" << (firstname==NULL ? "NULL" : firstname) << std::endl;
            }
            REQUIRE(row_count==2);
        }
    }
}
