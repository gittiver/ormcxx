#include <sys/stat.h>

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <type_traits>
#include <regex>

#include "catch2/catch_all.hpp"
#include "ormcxx/ormcxx_db.hpp"
using namespace std;

using ormcxx::Database;

TEST_CASE("database")
{
  auto db = Database::open(Database::BackendType::SQLITE,":memory:");

  REQUIRE(db.has_value());
  if (db.has_value()) {
    auto query = (*db)->query("CREATE TABLE x(id int, name1 type1 NOT NULL, PRIMARY KEY(id))");
    REQUIRE(query.has_value());

    auto result = (*query)->execute();
    REQUIRE(result.has_value());

    query = (*db)->query("SELECT * FROM x");
    REQUIRE(query.has_value());

    result = (*query)->execute();

    REQUIRE(result.has_value());
    REQUIRE((*result)->column_count()==2);
  }

}
