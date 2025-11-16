//
// Created by Gulliver on 29.06.25.
//
#include "ormcxx/logging.hpp"
#include "ormcxx/ormcxx_db.hpp"
#include <cstring>

using namespace std;

using ormcxx::Database;

int main() {
  {
    auto db = Database::open(Database::BackendType::SQLITE, ":memory:");

    if (db) {
      auto query = db->query("CREATE TABLE IF NOT EXISTS contacts ( \
                                contact_id INTEGER PRIMARY KEY,\
                                first_name TEXT NOT NULL,\
                                last_name TEXT NOT NULL,\
                                email TEXT NOT NULL,\
                                phone TEXT NOT NULL\
                                );"
      );

      auto error = query->execute();

      query = db->query("SELECT * FROM contacts;");

      error = query->execute();

      for (auto i = 0; i < query->result().column_count(); i++) {
          ORMCXX_LOG_DEBUG << query->result().column_name(i);
      }
      ORMCXX_LOG_DEBUG << query->result().column_name(5);

      query = db->query(
        "INSERT INTO contacts(first_name,last_name,email,phone) VALUES (?,?,?,?)");

      ORMCXX_LOG_DEBUG << query->bindings().parameter_count();
      //query->bindings().bind_text(0,"Frank",strlen("Frank"));
      query->bindings().bind_text(1,"Frank",strlen("Frank"));
      query->bindings().bind_text(2,"Frank",strlen("Frank"));
      query->bindings().bind_text(3,"Frank",strlen("Frank"));
      query->bindings().bind_text(4,"Frank",strlen("Frank"));

      error = query->execute();
      error = query->execute();
      error = query->execute();
      error = query->execute();
      error = query->execute();
      error = query->execute();
      error = query->execute();

      query = db->query("SELECT * FROM contacts;");
      error = query->execute();
      for (auto i = 0; i < query->result().column_count(); i++) {
        std::string name= query->result().column_name(i);
        ORMCXX_LOG_DEBUG << name;
      }
      if (error!=ormcxx::sql_error::OK) {
          ORMCXX_LOG_DEBUG << (int)error;
      }
      else
      do {
        auto& r = query->result();
        ORMCXX_LOG_DEBUG << query->result().column_int(0)
        << '|' << query->result().column_text(1)
        << '|' << query->result().column_text(2)
        << '|' << query->result().column_text(3)
        << '|' << query->result().column_text(4);
      }
      while (query->result().next_row());


    }
  }
}
