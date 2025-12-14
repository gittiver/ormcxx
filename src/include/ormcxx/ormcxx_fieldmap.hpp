//
// Created by Gulliver on 07.06.25.
//

#ifndef ORMCXX_FIELDMAP_H
#define ORMCXX_FIELDMAP_H

#include <memory>
#include <iostream>
#include "ormcxx/ormcxx_db.hpp"

namespace ormcxx {

  // remove all qualifiers of a type

  template<class F>
  struct Traits;

  // pointer to member object

  template<class C, class R>
  struct Traits<R(C::*)> : public Traits<R(C &)> {
    using AttributeType = R;
  };

  template<class C>
  struct Traits {
    using UnqualifiedType = typename std::remove_const<typename std::remove_cv<typename std::remove_pointer<typename
      std::remove_reference<C>::type>::type>::type>::type;
  };




  template<class ClassType, typename FieldType>
  class AccessWrapper {
  public:
    virtual ~AccessWrapper()=default;
    virtual void get(const ClassType *, FieldType *) = 0;

    virtual void set(ClassType *, FieldType *) = 0;
  };

  template<class ClassType, typename FieldType, typename AttributeType>
  class AccessWrapperAttributeImpl : public AccessWrapper<ClassType, FieldType> {
    AttributeType mAttributePtr;

  public:
    explicit AccessWrapperAttributeImpl(AttributeType attribute) : mAttributePtr(attribute) {
    }

    virtual void get(const ClassType *classInstance, FieldType *fieldInstance) {
      *fieldInstance = classInstance->*mAttributePtr;
    }

    virtual void set(ClassType *classInstance, FieldType *fieldInstance) {
      classInstance->*mAttributePtr = *fieldInstance;
    }
  };

  inline void result2Variable(const sql_result& sql_result, size_t column, int* to) {
    *to = sql_result.column_int(column);
  }

  inline void result2Variable(const sql_result& sql_result, size_t column, std::string* to) {
    const char* text = reinterpret_cast<const char*>(sql_result.column_text(column));
    (*to) = text !=nullptr ? text:"";
  }

  inline void result2Variable(const sql_result& sql_result, size_t column, float* to) {
    *to = sql_result.column_double(column);
  }

  inline void result2Variable(const sql_result& sql_result, size_t column, double* to) {
    *to = sql_result.column_double(column);
  }

  inline sql_error bindVariable(sql_bindings& bindings, size_t column, int* from) {
    return bindings.bind_int(column, *from);
  }

  inline sql_error bindVariable(sql_bindings& bindings, size_t column, const std::string* const from) {
    return bindings.bind_text(column, *from);
  }

  inline sql_error bindVariable(sql_bindings& bindings, size_t column, double* from) {
    return bindings.bind_double(column, *from);
  }

  template<typename ClassType>
  class SqlField {
    size_t m_column;
  public:
    explicit SqlField(size_t column) : m_column(column) {}
    virtual ~SqlField() = default;

    [[nodiscard]] size_t column() const { return m_column; }
    /* Reads the data at the corresponding column from <i>driver</i> and inject it in <i>instance</i> using its setter method. */
    virtual void readFromResult(const ormcxx::sql_result& result,ClassType* instance) = 0;
    virtual void writeToBindings(ormcxx::sql_bindings& sql_query,const ClassType* instance) = 0;

  };

  template<typename ClassType, typename FieldType>
  class SqlFieldImpl : public SqlField<ClassType> {
  public:
    explicit SqlFieldImpl(size_t column)
      : SqlField<ClassType>(column) {
    }

    std::unique_ptr<AccessWrapper<ClassType, FieldType>> mAccessWrapper;
    void readFromResult(const ormcxx::sql_result& result,ClassType* instance) {
      FieldType t{};
      std::cout << "read " << typeid(instance).name() << ":" << this->column() << std::endl;
      result2Variable(result,this->column(),&t);
      std::cout << "read value " << t << std::endl;

      mAccessWrapper->set(instance, &t);
    }
    void writeToBindings(ormcxx::sql_bindings& b,const ClassType* instance) {

      FieldType t{};
      mAccessWrapper->get(instance,&t);
      bindVariable(b,this->column()+1,&t);
      std::cout << "bind " << typeid(instance).name() << ":" << this->column() << std::endl;
    }
  };


} // ormcxx

#endif //ORMCXX_TABLE_H
