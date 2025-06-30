//
// Created by Gulliver on 29.06.25.
//
#include "ormcxx/ormcxx_db.hpp"
#include <iostream>
#include <cstring>

using namespace std;

using ormcxx::Database;

int main() {
  {
    auto db = Database::open(Database::BackendType::SQLITE, ":memory:");

    if (db) {
      auto query = (*db)->query("CREATE TABLE IF NOT EXISTS contacts ( \
                                contact_id INTEGER PRIMARY KEY,\
                                first_name TEXT NOT NULL,\
                                last_name TEXT NOT NULL,\
                                email TEXT NOT NULL UNIQUE,\
                                phone TEXT NOT NULL UNIQUE\
                                );"
      );

      auto result = (*query)->execute();

      query = (*db)->query("SELECT * FROM contacts;");

      result = (*query)->execute();

      for (auto i = 0; i < (*result)->column_count(); i++) {
        std::cout << (*result)->column_name(i) << std::endl;
      }
      std::cout << (*result)->column_name(5) << std::endl;

      query = (*db)->query(
        "INSERT INTO contacts(first_name,last_name,email,phone) VALUES (?,?,?,?)");

      std::cout << (*query)->bindings().parameter_count() << endl;
      (*query)->bindings().bind_text(0,"Frank",strlen("Frank"));
      (*query)->bindings().bind_text(1,"Frank",strlen("Frank"));
      (*query)->bindings().bind_text(2,"Frank",strlen("Frank"));
      (*query)->bindings().bind_text(3,"Frank",strlen("Frank"));
      (*query)->bindings().bind_text(4,"Frank",strlen("Frank"));

      for (auto i = 0; i < (*result)->column_count(); i++) {
        std::string name= (*result)->column_name(i);
        std::cout << name << std::endl;
      }
      result = (*query)->execute();
      if (!result) {
         std::cout << (int)result.error() <<endl;
      }
      else
      do {
        int id = (*result)->column_int(1);
        std::cout << (*result)->column_int(1)
        << '|' << (*result)->column_text(2)
        << '|' << (*result)->column_text(3)
        << '|' << (*result)->column_text(4)
        << endl;
      }
      while ((*result)->next_row());


    }
  }
}
