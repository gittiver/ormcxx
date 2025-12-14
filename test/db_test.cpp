#include <iostream>

#include "catch2/catch_all.hpp"
#include "ormcxx/ormcxx_db.hpp"
using namespace std;

using ormcxx::Database;

TEST_CASE("database") {
  auto db = Database::open(Database::BackendType::SQLITE, ":memory:");

  REQUIRE(db.has_value());
  if (db.has_value()) {
    auto query = db->query("CREATE TABLE IF NOT EXISTS contacts ( \
  contact_id INTEGER PRIMARY KEY,\
  first_name TEXT NOT NULL,\
  last_name TEXT NOT NULL,\
  email TEXT NOT NULL,\
  phone TEXT NOT NULL\
);");
    REQUIRE(query.has_value());
    query->execute();

    query = db->query(
      "INSERT INTO contacts(first_name,last_name,email,phone) VALUES (?,?,?,?)");

    std::cout << query->bindings().parameter_count() << endl;
    //query->bindings().bind_text(0,"Frank",strlen("Frank"));
    query->bindings().bind_text(1, "Frank", strlen("Frank"));
    query->bindings().bind_text(2, "Frank", strlen("Frank"));
    query->bindings().bind_text(3, "Frank", strlen("Frank"));
    query->bindings().bind_text(4, "Frank");

    auto result = query->execute();

    REQUIRE(result==ormcxx::sql_error::OK);
    auto query2 = db->query("SELECT * FROM contacts;");
    REQUIRE(query2.has_value());

    result = query2->execute();

    REQUIRE(result==ormcxx::sql_error::OK);
    REQUIRE(query2->result().column_count()==5);
    for (auto i = 3; i < 5; i++) {
      std::cout << query2->result().column_name(i)
          << ":"
          << query2->result().column_text(i - 1)
          << std::endl;
    }
    std::cout << query2->result().column_name(5) << std::endl;
    std::cout << query2->result().column_text(5 - 1) << std::endl;
  }
}
