#include <iostream>

#include "include/ormcxx/ormcxx_sql_config.hpp"

using ormcxx::sql_table_definition;
using ormcxx::sql_column_definition;
using ormcxx::sql_type;
using ormcxx::sql_config;
using std::cout;
using std::endl;

void do_sql_config() {

  struct C {
    int id{0};
    std::string name;
  };

  sql_config<C>::table_name("C");
  sql_config<C>::setPrimaryField("id",&C::id);
  sql_config<C>::setField("name",&C::name);

  cout << sql_config<C>::to_ddl() << endl;
  cout << sql_config<C>::field_mapping().size() << endl;

    C c;
  for (size_t i=0; i < sql_config<C>::field_mapping().size();++i) {
    auto entry = sql_config<C>::field_mapping().at(i);
    cout << typeid(entry).name() << endl;
    entry->readFromDriver(&c,i);
  }

}

int main() {
  sql_table_definition table;
  table.columns.push_back({"id",{"int"},ormcxx::Nullable::NULLABLE,ormcxx::ePRIMARY_KEY::PRIMARY_KEY});
  table.columns.push_back({"name1",{"type1"},ormcxx::Nullable::NOT_NULL,ormcxx::ePRIMARY_KEY::NO_PRIMARY_KEY});

  cout << table.to_ddl() << endl;

  do_sql_config();
  return 0;
}
