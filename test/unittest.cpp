#define CROW_ENABLE_DEBUG
#define CROW_LOG_LEVEL 0
#include <sys/stat.h>

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <type_traits>
#include <regex>

#include "catch2/catch_all.hpp"
#include "ormcxx/ormcxx_sql_config.hpp"
using namespace std;


TEST_CASE("sql_table_definition")
{
  using ormcxx::sql_table_definition;
  const std::string expected_result = "CREATE TABLE (id int, name1 type1 NOT NULL, PRIMARY KEY(id))";

  sql_table_definition table;
  table.columns.push_back({"id",{"int"},ormcxx::Nullable::NULLABLE,ormcxx::ePRIMARY_KEY::PRIMARY_KEY});
  table.columns.push_back({"name1",{"type1"},ormcxx::Nullable::NOT_NULL,ormcxx::ePRIMARY_KEY::NO_PRIMARY_KEY});

  REQUIRE(expected_result == table.to_ddl());
}

TEST_CASE("sql_config_table_map")
{
  using ormcxx::sql_config;

  struct C {
    int id;
    std::string name;
  };

  REQUIRE(sql_config<C>::table_name().empty());
  REQUIRE(sql_config<C>::table().columns.empty());

  sql_config<C>::table_name("C");

  REQUIRE(sql_config<C>::table_name()=="C");

  sql_config<C>::setPrimaryField("id",&C::id);
  sql_config<C>::setField("name",&C::name);

  REQUIRE(sql_config<C>::table().columns.size()==2);
  REQUIRE(sql_config<C>::table().columns[0].name=="id");
  REQUIRE(sql_config<C>::table().columns[0].is_primary==ormcxx::ePRIMARY_KEY::PRIMARY_KEY);

  REQUIRE(sql_config<C>::table().columns[1].name=="name");
  REQUIRE(sql_config<C>::table().columns[1].is_primary==ormcxx::ePRIMARY_KEY::NO_PRIMARY_KEY);

  REQUIRE(sql_config<C>::table().primary_keys()==std::vector<std::string>({"id"}));

  REQUIRE(sql_config<C>::to_ddl()=="CREATE TABLE (id INTEGER NOT NULL, name VARCHAR, PRIMARY KEY(id))");

}
