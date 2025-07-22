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
  {Database::BackendType::SQLITE, ":memory:"},
  {Database::BackendType::POSTGRESQL, "postgresql://testuser@localhost/test"}
};

TEST_CASE("open database") {
  for (auto const &c: connection_types) {
    auto db = Database::open(c.backend_type, c.connection_string);
    if (!db) {
      //REQUIRE(db.has_value()==true);
      FAIL("can not open connection:"+ std::to_string((int)c.backend_type)+"," + c.connection_string);
    }
    REQUIRE(db.has_value()==true);
  }
}


TEST_CASE("sql_query_and_execute") {
  for (auto const &c: connection_types) {
    auto db = Database::open(c.backend_type, c.connection_string);
    REQUIRE(db.has_value()==true);
    if (db.has_value()) {
      auto query = db->query(
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
    REQUIRE(db.has_value()==true);
    if (!db) {
      //REQUIRE(db.has_value()==true);
      FAIL("x" + c.connection_string);
    } else {
      auto query = db->query(
        "INSERT INTO contacts(first_name,last_name,email,phone) VALUES (?,?,?,?)");
      REQUIRE(query);
      std::cout << query->bindings().parameter_count() << endl;
      //query->bindings().bind_text(0,"Frank",strlen("Frank"));
      query->bindings().bind_text(1, "Frank", strlen("Frank"));
      query->bindings().bind_text(2, "Frank", strlen("Frank"));
      query->bindings().bind_text(3, "Frank", strlen("Frank"));
      query->bindings().bind_text(4, "Frank", strlen("Frank"));

      auto result = query->execute();
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
      std::cout << query2->result().column_name(5) << std::endl;
    }
  }
}
