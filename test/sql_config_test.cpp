#include <sys/stat.h>

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <type_traits>
#include <regex>
#include <filesystem>

#include "catch2/catch_all.hpp"

#include "ormcxx/data_source.hpp"

using namespace std;
using ormcxx::Database;
using ormcxx::sql_config;
using ormcxx::data_source;


TEST_CASE("sql_table_definition") {
  using ormcxx::sql_table_definition;
  const std::string expected_result = "CREATE TABLE C (id int, name1 type1 NOT NULL, PRIMARY KEY(id))";

  sql_table_definition table;
  table.name = "C";
  table.columns.push_back({"id", {"int"}, ormcxx::Nullable::NULLABLE, ormcxx::ePRIMARY_KEY::PRIMARY_KEY});
  table.columns.push_back({"name1", {"type1"}, ormcxx::Nullable::NOT_NULL, ormcxx::ePRIMARY_KEY::NO_PRIMARY_KEY});

  REQUIRE(expected_result == table.to_ddl());
}

TEST_CASE("sql_table_definition_sql_generation") {
  using ormcxx::sql_table_definition;
  const auto expected_result = "SELECT id,name1 FROM select_table;";
  sql_table_definition table("select_table");
  table.columns.push_back({"id", {"int"}, ormcxx::Nullable::NULLABLE, ormcxx::ePRIMARY_KEY::PRIMARY_KEY});
  table.columns.push_back({"name1", {"type1"}, ormcxx::Nullable::NOT_NULL, ormcxx::ePRIMARY_KEY::NO_PRIMARY_KEY});

  REQUIRE(expected_result == table.select_all());
}

TEST_CASE("sql_generation_select_by_id") {
  using ormcxx::sql_table_definition;
  const auto expected_result = "SELECT id,name1 FROM select_table WHERE id=?;";
  sql_table_definition table("select_table");
  table.columns.push_back({"id", {"int"}, ormcxx::Nullable::NULLABLE, ormcxx::ePRIMARY_KEY::PRIMARY_KEY});
  table.columns.push_back({"name1", {"type1"}, ormcxx::Nullable::NOT_NULL, ormcxx::ePRIMARY_KEY::NO_PRIMARY_KEY});

  REQUIRE(expected_result == table.select_by_id());
}

TEST_CASE("sql_config_table_map") {
  using ormcxx::sql_config;

  struct C {
    int id;
    std::string name;
  };

  REQUIRE(sql_config<C>::table_name().empty());
  REQUIRE(sql_config<C>::table().columns.empty());

  sql_config<C>::table_name("C");

  REQUIRE(sql_config<C>::table_name()=="C");

  sql_config<C>::setPrimaryField("id", &C::id);
  sql_config<C>::setField("name", &C::name);

  REQUIRE(sql_config<C>::table().columns.size()==2);
  REQUIRE(sql_config<C>::table().columns[0].name=="id");
  REQUIRE(sql_config<C>::table().columns[0].is_primary==ormcxx::ePRIMARY_KEY::PRIMARY_KEY);

  REQUIRE(sql_config<C>::table().columns[1].name=="name");
  REQUIRE(sql_config<C>::table().columns[1].is_primary==ormcxx::ePRIMARY_KEY::NO_PRIMARY_KEY);

  REQUIRE(sql_config<C>::table().primary_keys()==std::vector<std::string>({"id"}));

  REQUIRE(sql_config<C>::to_ddl()=="CREATE TABLE C (id INTEGER NOT NULL, name VARCHAR, PRIMARY KEY(id))");
}



TEST_CASE("sql_config_driver_select") {
  struct C {
    int id;
    std::string name;
  };

  sql_config<C>::table_name("C");
  sql_config<C>::setPrimaryField("id", &C::id);
  sql_config<C>::setField("name", &C::name);
  auto db = Database::open(Database::BackendType::SQLITE, ":memory:");
  auto ddl = sql_config<C>::to_ddl();
  db->query(ddl)->execute();

  db->query("INSERT INTO C(id,name) VALUES (1,'first')")->execute();
  db->query("INSERT INTO C(id,name) VALUES (3,'third')")->execute();
  db->query("INSERT INTO C(id,name) VALUES (2,'second')")->execute();
  if (db) {
    data_source<C> dsrc(&db.value<>());

    auto set = dsrc.select();
  }
}

TEST_CASE("sql_insert_parameterized") {
  struct C {
    int id;
    std::string name;
  };

  sql_config<C>::table_name("C");
  sql_config<C>::setPrimaryField("id", &C::id);
  sql_config<C>::setField("name", &C::name);

  //std::filesystem::remove("testcase.db");
  auto db = Database::open(Database::BackendType::SQLITE, "test.db");
  db->query("DROP table  IF EXISTS C;")->execute();
  auto ddl = sql_config<C>::to_ddl();
  db->query(ddl)->execute();

  auto query = db->query("INSERT INTO C(id,name) VALUES (?,?)");
  size_t i = 1;
  query->bindings().bind_int(1,1);
  query->bindings().bind_text(2,"text",strlen("text"));
  auto result = query->execute();
  query->reset();
  query->bindings().bind_int(1,3);
  query->bindings().bind_text(2,"third",strlen("third"));
  result = query->execute();

  auto query2 = db->query("INSERT INTO C(name) VALUES (?)");
  query2->bindings().bind_text(1,"third",strlen("third"));
  result = query2->execute();
  query2->reset();
  int newId = query2->result().last_inserted_id();

  result = query2->execute();
  query2->reset();

  newId = query2->result().last_inserted_id();
  std::string name = "std::string";
  query2->bindings().bind_text(1,name);
  result = query2->execute();
  newId = query2->result().last_inserted_id();
}

TEST_CASE("sql_config_insert") {
  struct C {
    int id;
    std::string name;
  };

  sql_config<C>::table_name("C");
  sql_config<C>::setPrimaryField("id", &C::id);
  sql_config<C>::setField("name", &C::name);

  auto db = Database::open(Database::BackendType::SQLITE, ":memory:");

  db->query("DROP table IF EXISTS C;")->execute();
  auto ddl = sql_config<C>::to_ddl();
  db->query(ddl)->execute();

  data_source<C> dsrc(&db.value<>());
  C c;
  c.id = 2;
  c.name = "second";
  auto error = dsrc.insert(c);
  c.id= 3 ;
  c.name = "third";
  error = dsrc.insert(c);

  auto set = dsrc.select();

  REQUIRE(error==ormcxx::sql_error::OK);

  REQUIRE(set.size()==2);

  REQUIRE(set[0].id==2);
  REQUIRE(set[0].name=="second");

  REQUIRE(set[1].id==3);
  REQUIRE(set[1].name=="third");
}


TEST_CASE("sql_update_by_id") {
  struct C {
    int id;
    std::string name;
  };

  sql_config<C>::table_name("C");
  sql_config<C>::setPrimaryField("id", &C::id);
  sql_config<C>::setField("name", &C::name);

  auto db = Database::open(Database::BackendType::SQLITE, ":memory:");

  db->query("DROP table  IF EXISTS C;")->execute();
  auto ddl = sql_config<C>::to_ddl();
  db->query(ddl)->execute();

  data_source<C> dsrc(&db.value<>());
  C c;
  c.id = 2;
  c.name = "second";
  auto error = dsrc.insert(c);
  REQUIRE(error==ormcxx::sql_error::OK);
  c.id= 3 ;
  c.name = "third";
  error = dsrc.insert(c);
  REQUIRE(error==ormcxx::sql_error::OK);


  C update_item = c;
  update_item.name = "updated_third";
  error = dsrc.update_by_id(update_item);
  REQUIRE(error==ormcxx::sql_error::OK);

  auto set = dsrc.select();

  REQUIRE(set.size()==2);

  REQUIRE(set[0].id==2);
  REQUIRE(set[0].name=="second");

  REQUIRE(set[1].id==3);
  REQUIRE(set[1].name=="updated_third");
}
