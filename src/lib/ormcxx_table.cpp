//
// Created by Gulliver on 07.06.25.
//

#include "../include/ormcxx/ormcxx_table.hpp"

namespace ormcxx {
  std::vector<std::string> sql_table_definition::primary_keys() const {
    std::vector<std::string> result;
    for (const auto& column : columns) {
      if (column.is_primary==ormcxx::ePRIMARY_KEY::PRIMARY_KEY) {
        result.push_back(column.name);
      }
    }
    return result;
  }

  std::string sql_table_definition::to_ddl() const {

    std::string ddl = "CREATE TABLE (";
    bool first = true;
    for (const auto column: columns ) {
      if (first) {
        first = false;
      } else {
        ddl.append(", ");
      }
      ddl.append(column.name).append(" ")
         .append(column.type.name)
         .append(column.nullable==ormcxx::Nullable::NOT_NULL ? " NOT NULL":"");
    }
    ddl.append(", PRIMARY KEY(");

    first = true;
    for (const auto& column: this->primary_keys()) {
      ddl.append(column);
      if (first) {
        first = false;
      } else {
        ddl.append(",");
      }
    };
    ddl.append(")");
    ddl.append(")");
    return ddl;
  }

} // ormcxx