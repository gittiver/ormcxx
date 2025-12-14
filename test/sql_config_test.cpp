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
using ormcxx::sql_config;


TEST_CASE("sql_table_definition") {
  using ormcxx::sql_table_definition;
  const std::string expected_result = "CREATE TABLE (id int, name1 type1 NOT NULL, PRIMARY KEY(id))";

  sql_table_definition table;
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

  REQUIRE(sql_config<C>::to_ddl()=="CREATE TABLE (id INTEGER NOT NULL, name VARCHAR, PRIMARY KEY(id))");
}

template<class C>
struct data_source {
  using sql_config = ormcxx::sql_config<C>;

  std::vector<C> select();

  void del(const C &c);

  ormcxx::sql_error ins(const C &c);

  data_source(ormcxx::Database *db) : db(db) {
  };

  Database *db;
};

template<class C>
std::vector<C> data_source<C>::select() {
  std::vector<C> result;
  auto query_result = db->query(ormcxx::sql_config<C>::select_all());
  if (query_result) {
    while (query_result->result().next_row()) {
      C c;
      for (auto mapped_field: ormcxx::sql_config<C>::field_mapping()) {
        cout << mapped_field->column() << endl;
        mapped_field->readFromResult(query_result->result(), &c);
      }
      // TBD sql_config map
      result.push_back(c);
    };
  }
  return result;
}

template<class C>
void data_source<C>::del(const C &c) {
}

template<class C>
ormcxx::sql_error data_source<C>::ins(const C &c) {
  ormcxx::sql_error error= ormcxx::sql_error::NOK;
  auto query = db->query(ormcxx::sql_config<C>::insert_into_values());
  if (query) {
    for (const auto mapped_field: ormcxx::sql_config<C>::field_mapping()) {
      cout << mapped_field->column() << endl;
      mapped_field->writeToBindings(query->bindings(), &c);

    }
    error = query->execute();
  }
  return error;
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
  if (db) {
    data_source<C> dsrc(&db.value<>());

    auto set = dsrc.select();
  }
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
  auto ddl = sql_config<C>::to_ddl();
  db->query(ddl)->execute();
  data_source<C> dsrc(&db.value<>());
  C c;
  c.id = 2;
  c.name = "second";
  auto error = dsrc.ins(c);
  c.id= 3 ;
  c.name = "third";
  error = dsrc.ins(c);

  auto set = dsrc.select();

  REQUIRE(error==ormcxx::sql_error::OK);

  REQUIRE(set.size()==2);

  REQUIRE(set[0].id==2);
  REQUIRE(set[0].name=="second");

  REQUIRE(set[1].id==3);
  REQUIRE(set[1].name=="third");
}
