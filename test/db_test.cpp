
#include <iostream>

#include "catch2/catch_all.hpp"
#include "ormcxx/ormcxx_db.hpp"
using namespace std;

using ormcxx::Database;

TEST_CASE("database")
{
  auto db = Database::open(Database::BackendType::SQLITE,":memory:");

  REQUIRE(db.has_value());
  if (db.has_value()) {
    auto query = (*db)->query("CREATE TABLE IF NOT EXISTS contacts ( \
  contact_id INTEGER PRIMARY KEY,\
  first_name TEXT NOT NULL,\
  last_name TEXT NOT NULL,\
  email TEXT NOT NULL UNIQUE,\
  phone TEXT NOT NULL UNIQUE\
);");
    REQUIRE(query.has_value());

    auto result = (*query)->execute();
    REQUIRE(result.has_value());
    auto query2 = (*db)->query("SELECT * FROM contacts;");
    REQUIRE(query2.has_value());

    result = (*query2)->execute();

    REQUIRE(result.has_value());
    REQUIRE((*result)->column_count()==5);
    for (auto i = 0; i < 5; i++) {
      std::cout << (*result)->column_name(i)<<std::endl;
    }
    std::cout << (*result)->column_name(5)<<std::endl;

  }

}
