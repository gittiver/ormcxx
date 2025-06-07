//
// Created by Gulliver on 07.06.25.
//

#ifndef ORMCXX_SQLCONFIG_HPP
#define ORMCXX_SQLCONFIG_HPP

#include<ormcxx/ormcxx_table.hpp>
#include<ormcxx/ormcxx_fieldmap.hpp>

namespace ormcxx {

  template<class ClassType>
  class sql_config {
    static sql_table_definition table_definition;
    //static std::vector<AccessWrapper<ClassType>> field_map;
  public:
    static void table_name(const std::string &table_name) { sql_config<ClassType>::table_definition.name = table_name; }
    static const std::string &table_name() { return sql_config<ClassType>::table_definition.name; }
    static const sql_table_definition &table() { return sql_config<ClassType>::table_definition; }
    static std::string to_ddl() { return sql_config<ClassType>::table_definition.to_ddl(); }

    template<typename AttributeType>
    static void setPrimaryField(const std::string &columnName, AttributeType attribute) {
      //SALSABIL_LOG_DEBUG("Setting primary field (attribute): " + columnName);
      //using FieldType = typename Utility::Traits<AttributeType>::AttributeType;
      //mPrimaryFieldList.push_back(new SqlFieldImpl<ClassType, FieldType>(columnName, fieldColumnIndex(columnName), new AccessWrapperAttributeImpl<ClassType, FieldType, AttributeType>(attribute)));

      sql_column_definition column{
        columnName,
        {typeid(typename Traits<AttributeType>::AttributeType).name()},
        Nullable::NOT_NULL,
        ePRIMARY_KEY::PRIMARY_KEY
      };
      table_definition.columns.push_back(column);
    }

    template<typename AttributeType>
    static void setField(const std::string &columnName,
                         AttributeType attribute,
                         Nullable nullable = Nullable::NULLABLE) {
      //SALSABIL_LOG_DEBUG("Setting field (attribute): " + columnName);
      //using FieldType = typename Traits<AttributeType>::AttributeType;
      //mFieldList.push_back(new SqlFieldImpl<ClassType, FieldType>(columnName, fieldColumnIndex(columnName), new AccessWrapperAttributeImpl<ClassType, FieldType, AttributeType>(attribute)));
      sql_column_definition column{
        columnName,
        {typeid(typename Traits<AttributeType>::AttributeType).name()},
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
