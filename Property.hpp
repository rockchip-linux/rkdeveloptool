//-- Property.hpp --

/*--------------------------------------------------------------------------
                         Class Library

     Copyrights Emad Barsoum (ebarsoum@msn.com) 2003. All rights reserved.
     ________________________________________________________________


     PROJECT   : General
     MODULE    : property
     FILENAME  : Property.hpp
	   BUILD     : 1

     History of Modifications:

     Date(dd/mm/yyyy)Person                Description
     ----            ------                -----------
     25/03/2003      Emad Barsoum          Initial design and coding

     CLASS NAME: property
     VERSION: 1.0

     DESCRIPTION:
        This class try to simulate property for C++, using template technique.

     LICENSE:
        You are free to change or modify or redistribute the code, just keep the header.
      And you can use this class in any application you want without any warranty.
*/
#include <assert.h>
#include <stdlib.h>
#if !defined INC_PROPERTY_HPP
#define INC_PROPERTY_HPP

#define READ_ONLY 1
#define WRITE_ONLY 2
#define READ_WRITE 3

template<typename Container, typename ValueType, int nPropType>
class property
{
public:
  property()
  {
    m_cObject = NULL;
    Set = NULL;
    Get = NULL;
  }
  //-- This to set a pointer to the class that contain the property --
  void setContainer(Container* cObject)
  {
    m_cObject = cObject;
  }
  //-- Set the set member function that will change the value --
  void setter(void (Container::*pSet)(ValueType value))
  {
    if((nPropType == WRITE_ONLY) || (nPropType == READ_WRITE))
      Set = pSet;
    else
      Set = NULL;
  }
  //-- Set the get member function that will retrieve the value --
  void getter(ValueType (Container::*pGet)())
  {
    if((nPropType == READ_ONLY) || (nPropType == READ_WRITE))
     Get = pGet;
    else
      Get = NULL;
  }
  //-- Overload the '=' sign to set the value using the set member --
  ValueType operator =(const ValueType& value)
  {
    assert(m_cObject != NULL);
    assert(Set != NULL);
    (m_cObject->*Set)(value);
    return value;
  }

  //-- To make possible to cast the property class to the internal type --
  operator ValueType()
  {
    assert(m_cObject != NULL);
    assert(Get != NULL);
    return (m_cObject->*Get)();
  }

private:
  Container* m_cObject;//-- Pointer to the module that contain the property --
  void (Container::*Set)(ValueType value);//-- Pointer to set member function --
  ValueType (Container::*Get)();//-- Pointer to get member function --
};

#endif