//
// Created by Gulliver on 07.06.25.
//

#ifndef ORMCXX_SQLCONFIG_HPP
#define ORMCXX_SQLCONFIG_HPP

#include<ormcxx/ormcxx_table.hpp>
#include<ormcxx/ormcxx_fieldmap.hpp>
#include <typeinfo>

namespace ormcxx {
    std::string sql_type(const std::type_info& info);

  template<class ClassType>
  class sql_config {
    static sql_table_definition table_definition;
    static std::vector<SqlField<ClassType>*> field_map;
  public:
    static void table_name(const std::string &table_name) { table_definition.name = table_name; }
    static const std::string &table_name() { return table_definition.name; }

    static const sql_table_definition &table() { return table_definition; }

    static const std::vector<SqlField<ClassType>*>& field_mapping() { return field_map; }

    static std::string to_ddl() { return table_definition.to_ddl(); }
        
    template<typename AttributeType>
    static void setPrimaryField(const std::string &columnName, AttributeType attribute) {
      using FieldType = typename Traits<AttributeType>::AttributeType;
      auto pField = new SqlFieldImpl<ClassType,FieldType>;
      pField->mAccessWrapper = std::make_unique<AccessWrapperAttributeImpl<ClassType,FieldType,AttributeType>>(attribute);
      field_map.push_back(pField);
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
      auto pField = new SqlFieldImpl<ClassType,FieldType>;
      pField->mAccessWrapper = std::make_unique<AccessWrapperAttributeImpl<ClassType,FieldType,AttributeType>>(attribute);
      field_map.push_back(pField);
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
  template<typename ClassType>
  std::vector<SqlField<ClassType>*> sql_config<ClassType>::field_map;

} // ormcxx

#endif //ORMCXX_SQLCONFIG_HPP
