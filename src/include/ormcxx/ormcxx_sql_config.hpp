//
// Created by Gulliver on 07.06.25.
//

#ifndef ORMCXX_SQLCONFIG_HPP
#define ORMCXX_SQLCONFIG_HPP

#include<ormcxx/ormcxx_table.hpp>
#include<ormcxx/ormcxx_fieldmap.hpp>
#include <typeinfo>

namespace ormcxx {

  template<class ClassType>
  class sql_config {
    static sql_table_definition table_definition;
    //static std::vector<AccessWrapper<ClassType>> field_map;
  public:
    static void table_name(const std::string &table_name) { table_definition.name = table_name; }
    static const std::string &table_name() { return table_definition.name; }
    static const sql_table_definition &table() { return table_definition; }
    static std::string to_ddl() { return table_definition.to_ddl(); }

    static std::string sql_type(const std::type_info& info) {
      if (info ==typeid(std::string)) {
        return "VARCHAR";
      } else if (info ==typeid(int)) {
        return "INTEGER";
      } else {
        return "";
      }
    }

    template<typename AttributeType>
    static void setPrimaryField(const std::string &columnName, AttributeType attribute) {
      using FieldType = typename Traits<AttributeType>::AttributeType;
      AccessWrapperAttributeImpl<ClassType,FieldType,AttributeType> a(attribute);

      const sql_column_definition column{
        columnName,
        {sql_type(typeid(FieldType))},
        Nullable::NOT_NULL,
        ePRIMARY_KEY::PRIMARY_KEY
      };
      table_definition.columns.push_back(column);
    }


    template<typename AttributeType>
    static void setField(const std::string &columnName,
                         AttributeType attribute,
                         const Nullable nullable = Nullable::NULLABLE) {
      using FieldType = typename Traits<AttributeType>::AttributeType;
      const sql_column_definition column{
        columnName,
        { sql_type(typeid(FieldType))},
        nullable,
        ePRIMARY_KEY::NO_PRIMARY_KEY
      };
      table_definition.columns.push_back(column);
    }
  };

  template<typename ClassType>
  sql_table_definition sql_config<ClassType>::table_definition;
} // ormcxx

#endif //ORMCXX_SQLCONFIG_HPP
