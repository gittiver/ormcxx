//
// Created by Gulliver on 29.06.25.
//
#include "ormcxx/ormcxx_db.hpp"
#include "ormcxx/ormcxx_sql_config.hpp"
#include <iostream>

#include "ormcxx/data_source.hpp"

using namespace std;

using ormcxx::Database;
using ormcxx::sql_config;
using ormcxx::data_source;

struct C {
  int id;
  std::string name;
};

void configure() {
  sql_config<C>::table_name("C");
  sql_config<C>::setPrimaryField("id", &C::id);
  sql_config<C>::setField("name", &C::name);
}

void migrate(Database& db) {
  db.query("DROP table IF EXISTS C;")->execute();
  auto ddl = sql_config<C>::to_ddl();
  db.query(ddl)->execute();
}

int main() {
  {
    configure();

    auto db = Database::open(Database::BackendType::SQLITE, ":memory:");

    if (db) {
      migrate(*db);

      data_source<C> dsrc(&db.value<>());

      C c;
      c.id = 2;
      c.name = "second";
      auto error = dsrc.insert(c);

      c.id= 3 ;
      c.name = "third";
      error = dsrc.insert(c);

      /// select the inserted data
      auto set = dsrc.select();

      std::cout << "| id      | name               "
                << std::endl
                << "+---------+--------------------"
                << std::endl;

      for (auto c : set) {
        std::cout << "| "  << c.id
                  << " | " << c.name << std::endl;
      }

      dsrc.delete_by_id(c);

      C c3 {5,"fifth"};
      dsrc.insert(c3);

      c3.name = "fourth";
      dsrc.update_by_id(c3);

      set = dsrc.select();

      std::cout << "| id      | name               "
                << std::endl
                << "+---------+--------------------"
                << std::endl;

      for (auto c : set) {
        std::cout << "| "  << c.id
                  << " | " << c.name << std::endl;
      }


    }
  }
}
