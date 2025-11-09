#include <sys/stat.h>

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <type_traits>
#include <regex>

#include "catch2/catch_all.hpp"

#include "ormcxx/ormcxx_sql_config.hpp"
#include "ormcxx/ormcxx_db.hpp"

using namespace std;
using ormcxx::Database;

TEST_CASE("sql_table_definition")
{

  using ormcxx::sql_table_definition;
  const std::string expected_result = "CREATE TABLE (id int, name1 type1 NOT NULL, PRIMARY KEY(id))";

  sql_table_definition table;
  table.columns.push_back({"id",{"int"},ormcxx::Nullable::NULLABLE,ormcxx::ePRIMARY_KEY::PRIMARY_KEY});
  table.columns.push_back({"name1",{"type1"},ormcxx::Nullable::NOT_NULL,ormcxx::ePRIMARY_KEY::NO_PRIMARY_KEY});

  REQUIRE(expected_result == table.to_ddl());
}

TEST_CASE("sql_table_definition_sql_generation")
{
  using ormcxx::sql_table_definition;
  const auto expected_result = "SELECT id,name1 FROM select_table;";
  sql_table_definition table("select_table");
  table.columns.push_back({"id",{"int"},ormcxx::Nullable::NULLABLE,ormcxx::ePRIMARY_KEY::PRIMARY_KEY});
  table.columns.push_back({"name1",{"type1"},ormcxx::Nullable::NOT_NULL,ormcxx::ePRIMARY_KEY::NO_PRIMARY_KEY});

  REQUIRE(expected_result == table.select_all());
}

TEST_CASE("sql_generation_select_by_id")
{
  using ormcxx::sql_table_definition;
  const auto expected_result = "SELECT id,name1 FROM select_table WHERE id=?;";
  sql_table_definition table("select_table");
  table.columns.push_back({"id",{"int"},ormcxx::Nullable::NULLABLE,ormcxx::ePRIMARY_KEY::PRIMARY_KEY});
  table.columns.push_back({"name1",{"type1"},ormcxx::Nullable::NOT_NULL,ormcxx::ePRIMARY_KEY::NO_PRIMARY_KEY});

  REQUIRE(expected_result == table.select_by_id());
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

template <class C> struct data_source {
  using sql_config = ormcxx::sql_config<C>;
  std::vector<C> select();
  void del(const C& c);
  void ins(const C& c);

  data_source(ormcxx::Database* db) : db(db) {};

  Database* db;

};

template <class C> std::vector<C> data_source<C>::select() {
  std::vector<C> result;
  auto query = db->query(ormcxx::sql_config<C>::select_all());

  return result;
}
template <class C> void  data_source<C>::del(const C& c) {}
template <class C> void  data_source<C>::ins(const C& c) {

}



TEST_CASE("sql_config_driver_read") {
  using ormcxx::sql_config;

  struct C {
    int id;
    std::string name;
  };

  sql_config<C>::table_name("C");
  sql_config<C>::setPrimaryField("id",&C::id);
  sql_config<C>::setField("name",&C::name);

  auto db = Database::open(Database::BackendType::SQLITE,":memory:");

  if (db) {
    data_source<C> dsrc(&db.value<>());

    auto set = dsrc.select();
    C c;
    c.id = 1;
    c.name = "name";
    dsrc.ins(c);
    dsrc.del(c);

  }
  //template ds<C>::BackendType ds<C>::backend_type;

}

