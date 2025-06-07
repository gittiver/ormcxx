//
// Created by Gulliver on 07.06.25.
//

#ifndef ORMCXX_TABLE_H
#define ORMCXX_TABLE_H

#include<string>
#include<vector>
#include<ormcxx/ormcxx_fieldmap.hpp>

namespace ormcxx {
  class sql_type {
  public:
    std::string name;
  };

  enum class Nullable {
    NULLABLE = 0,
    NOT_NULL = 1
  };

  enum class ePRIMARY_KEY { NO_PRIMARY_KEY, PRIMARY_KEY = 1 };

  class sql_column_definition {
  public:
    std::string name;
    sql_type type;

    Nullable nullable;
    ePRIMARY_KEY is_primary;
  };

  struct index_definition {
    std::string name;
    std::vector<size_t> column_indices;
  };

  class sql_table_definition {
    std::vector<index_definition> indices;

  public:
    sql_table_definition(const std::string &name = "") : name(name) {
    }

    std::string name;
    std::vector<sql_column_definition> columns;

    // TBD constraints and indices
    void add_index(const std::string &name, const std::vector<std::string> &column_names);

    std::vector<std::string> primary_keys() const;

    std::string to_ddl() const;
  };

  } // ormcxx

#endif //ORMCXX_TABLE_H
