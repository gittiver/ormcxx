//
// Created by Gulliver on 07.06.25.
//

#ifndef ORMCXX_FIELDMAP_H
#define ORMCXX_FIELDMAP_H

#include<string>
#include<vector>

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


  template<typename ClassType>
  class SqlField {
  public:
  };

  template<typename ClassType, typename FieldType>
  class SqlFieldImpl : SqlField<ClassType> {
  public:
  };


  template<class ClassType, typename FieldType>
  class AccessWrapper {
  public:
    virtual void get(const ClassType *, FieldType *) = 0;

    virtual void set(ClassType *, FieldType *) = 0;
  };

  template<class ClassType, typename FieldType, typename AttributeType>
  class AccessWrapperAttributeImpl : public AccessWrapper<ClassType, FieldType> {
    AttributeType mAttributePtr;

  public:
    AccessWrapperAttributeImpl(AttributeType attribute) : mAttributePtr(attribute) {
    }

    virtual void get(const ClassType *classInstance, FieldType *fieldInstance) {
      *fieldInstance = classInstance->*mAttributePtr;
    }

    virtual void set(ClassType *classInstance, FieldType *fieldInstance) {
      classInstance->*mAttributePtr = *fieldInstance;
    }
  };


} // ormcxx

#endif //ORMCXX_TABLE_H
