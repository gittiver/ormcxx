//
// Created by Gulliver on 03.02.26.
//

#ifndef ORMCXX_DATA_SOURCE_HPP
#define ORMCXX_DATA_SOURCE_HPP

#include "ormcxx/ormcxx_sql_config.hpp"
#include "ormcxx/ormcxx_db.hpp"

namespace ormcxx {
  using ormcxx::Database;
  using ormcxx::sql_config;

  template<class C>
  struct data_source {
    using sql_config = ormcxx::sql_config<C>;

    std::vector<C> select();

    ormcxx::sql_error delete_by_id(const C &c);

    ormcxx::sql_error update_by_id(const C &c);

    ormcxx::sql_error insert(const C &c);

    data_source(ormcxx::Database *db) : db(db) {
    };

    virtual ~data_source() = default;

    Database *db;
  };

  template<class C>
  std::vector<C> data_source<C>::select() {
    std::vector<C> result;
    auto query_result = db->query(ormcxx::sql_config<C>::select_all());
    if (query_result) {
      while (query_result->result().next_row()) {
        C c;
        for (auto mapped_field: ormcxx::sql_config<C>::field_mapping()) {
          // cout << mapped_field->column() << endl;
          mapped_field->readFromResult(query_result->result(), &c);
        }
        // TBD sql_config map
        result.push_back(c);
      };
    }
    return result;
  }

  template<class C>
  ormcxx::sql_error data_source<C>::delete_by_id(const C &c) {
    ormcxx::sql_error error = ormcxx::sql_error::NOK;
    auto query = db->query(ormcxx::sql_config<C>::table().delete_by_id());
    if (query) {
      size_t i = 1;
      for (const auto mapped_field: ormcxx::sql_config<C>::field_mapping()) {
        if (ormcxx::sql_config<C>::table().columns[mapped_field->column()].is_primary ==
            ormcxx::ePRIMARY_KEY::PRIMARY_KEY) {
          mapped_field->writeToBindings(query->bindings(), &c, i++);
        }
      }
      error = query->execute();
      query->reset();
    }
    return error;
  }

  template<class C>
  ormcxx::sql_error data_source<C>::update_by_id(const C &c) {
    ormcxx::sql_error error = ormcxx::sql_error::NOK;
    auto query = db->query(ormcxx::sql_config<C>::table().update_by_id());
    if (query) {
      size_t i = 1;

      for (const auto mapped_field: ormcxx::sql_config<C>::field_mapping()) {
        auto col = ormcxx::sql_config<C>::table().columns[mapped_field->column()];
        if (col.is_primary != ormcxx::ePRIMARY_KEY::PRIMARY_KEY) {
          mapped_field->writeToBindings(query->bindings(), &c, i++);
        }
      }
      for (const auto mapped_field: ormcxx::sql_config<C>::field_mapping()) {
        auto col = ormcxx::sql_config<C>::table().columns[mapped_field->column()];
        if (col.is_primary == ormcxx::ePRIMARY_KEY::PRIMARY_KEY) {
          mapped_field->writeToBindings(query->bindings(), &c, i++);
        }
      }
      error = query->execute();
      query->reset();
    }
    return error;
  }

  template<class C>
  ormcxx::sql_error data_source<C>::insert(const C &c) {
    ormcxx::sql_error error = ormcxx::sql_error::NOK;
    auto query = db->query(ormcxx::sql_config<C>::insert_into_values());
    if (query) {
      for (const auto mapped_field: ormcxx::sql_config<C>::field_mapping()) {
        // cout << mapped_field->column() << endl;
        mapped_field->writeToBindings(query->bindings(), &c, mapped_field->column() + 1);
      }
      error = query->execute();
      query->reset();
    }
    return error;
  }
}

#endif //ORMCXX_DATA_SOURCE_HPP
