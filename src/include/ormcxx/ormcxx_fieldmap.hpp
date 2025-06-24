//
// Created by Gulliver on 07.06.25.
//

#ifndef ORMCXX_FIELDMAP_H
#define ORMCXX_FIELDMAP_H

#include <memory>
#include <iostream>

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

  template<typename ClassType>
  class SqlField {
  public:
    /* Reads the data at the corresponding column from <i>driver</i> and inject it in <i>instance</i> using its setter method. */
    virtual void readFromDriver(ClassType* instance, int column) = 0;

  };

  template<typename ClassType, typename FieldType>
  class SqlFieldImpl : public SqlField<ClassType> {
  public:
    std::unique_ptr<AccessWrapper<ClassType, FieldType>> mAccessWrapper;
    void readFromDriver(ClassType* instance, int columnIndex) {
      FieldType t;
      std::cout << "read " << typeid(instance).name() << ":" << columnIndex << std::endl;
      //Utility::driverToVariable(SqlEntityConfigurer<ClassType>::driver(), columnIndex, Utility::initializeInstance(&t));
      mAccessWrapper->set(instance, &t);
    }

  };


} // ormcxx

#endif //ORMCXX_TABLE_H
