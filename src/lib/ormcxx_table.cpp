//
// Created by Gulliver on 07.06.25.
//

#include "../include/ormcxx/ormcxx_table.hpp"

namespace ormcxx {
  void sql_table_definition::append_column_list(std::string &sql, const std::string &separator) const {
    bool first = true;
    for (const auto &column: columns ) {
      if (first) {
        first = false;
      } else {
        sql.append(separator);
      }
      sql.append(column.name);
    }
  }

  std::vector<std::string> sql_table_definition::primary_keys() const {
    std::vector<std::string> result;
    for (const auto& column : columns) {
      if (column.is_primary==ePRIMARY_KEY::PRIMARY_KEY) {
        result.push_back(column.name);
      }
    }
    return result;
  }

  std::string sql_table_definition::to_ddl() const {

    std::string ddl = "CREATE TABLE " + name +" (";
    bool first = true;
    for (const auto &column: columns ) {
      if (first) {
        first = false;
      } else {
        ddl.append(", ");
      }
      ddl.append(column.name).append(" ")
         .append(column.type.name)
         .append(column.nullable==Nullable::NOT_NULL ? " NOT NULL":"");
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
    }
    ddl.append(")");
    ddl.append(")");
    return ddl;
  }


  std::string sql_table_definition::select_all() const {
    std::string sql = "SELECT ";
    append_column_list(sql);
    sql.append(" FROM ").append(name).append(";");
    return sql;
  }

  std::string sql_table_definition::select_by_id() const {
    std::string sql = "SELECT ";
    append_column_list(sql);
    sql.append(" FROM ").append(name)
        .append(" WHERE ");
    for (auto pkey : primary_keys()) {
      sql.append(pkey).append("=?");
    }
    sql.append(";");
    return sql;
  }

  std::string sql_table_definition::delete_by_id() const {
    std::string sql = "DELETE FROM  ";
    sql.append(name)
        .append(" WHERE ");
    bool first = false;
    for (auto const & pkey : primary_keys()) {
      if (first) {
        first = false;
      } else {
        sql.append(pkey).append("= ?");
      }
    };
    sql.append(";");
    return sql;
  }

  std::string sql_table_definition::update_by_id() const {
    std::string sql = "UPDATE ";
    sql.append(name).append(" SET ");
    std::string where;
    bool first = true;

    for (auto const & col : columns) {
      if (col.is_primary==ePRIMARY_KEY::PRIMARY_KEY) {
        if (where.empty()) {
        } else {
          where.append(" AND ");
        }
        where.append(col.name).append("= ?");

      } else {
        if (first) {
          first = false;
        } else {
          sql.append(",");
        }
        sql.append(col.name).append("= ?");

      }

    };
    sql.append(" WHERE ").append(where).append(";");
    return sql;
  }

  std::string sql_table_definition::insert() const {
    std::string sql = "INSERT INTO ";
    sql.append(name);
    sql.append(" (");

    bool first = true;

    for (const auto & column : columns) {
      //if (column.is_primary==ePRIMARY_KEY::NO_PRIMARY_KEY) {
          if (first) {
            first = false;
          } else {
            sql.append(",");
          }
          sql.append(column.name);
      //}
    };
    sql.append(")");
    sql.append(" VALUES (");
    first = true;
    for (const auto & column : columns) {
      //if (column.is_primary==ePRIMARY_KEY::NO_PRIMARY_KEY) {
        if (first) {
          first = false;
        } else {
          sql.append(",");
        }
        sql.append("?");
      //}
    };
    sql.append(")");


    return sql;
  }
} // ormcxx