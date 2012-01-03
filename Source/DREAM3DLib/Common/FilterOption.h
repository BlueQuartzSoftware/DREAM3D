/*
 * FilterOption.h
 *
 *  Created on: Dec 30, 2011
 *      Author: mjackson
 */

#ifndef FILTEROPTION_H_
#define FILTEROPTION_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"


class FilterOption
{
  public:
    DREAM3D_SHARED_POINTERS(FilterOption)
    DREAM3D_STATIC_NEW_MACRO(FilterOption)
    DREAM3D_TYPE_MACRO(FilterOption)
    virtual ~FilterOption(){};

    enum WidgetType
    {
      StringWidget,
      IntWidget,
      DoubleWidget,
      InputFileWidget,
      OutputFileWidget,
      BooleanWidget,
      IntConstrainedWidget,
      DoubleContrainedWidget,
      ChoiceWidget,
      CustomWidget
    };

    DREAM3D_INSTANCE_STRING_PROPERTY(HumanLabel);
    DREAM3D_INSTANCE_STRING_PROPERTY(PropertyName);
    DREAM3D_INSTANCE_PROPERTY(WidgetType, WidgetType);
    DREAM3D_INSTANCE_STRING_PROPERTY(ValueType);
    DREAM3D_INSTANCE_STRING_PROPERTY(CastableValueType);

  protected:
    FilterOption() : m_ValueType("Not_Set"){}

  private:
    FilterOption(const FilterOption&); // Copy Constructor Not Implemented
    void operator=(const FilterOption&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
class ConstrainedFilterOption : public FilterOption
{
  public:
    DREAM3D_SHARED_POINTERS(ConstrainedFilterOption<T>)
    DREAM3D_STATIC_NEW_MACRO(ConstrainedFilterOption<T>)
    DREAM3D_TYPE_MACRO_SUPER(ConstrainedFilterOption<T>, FilterOption)

    virtual ~ConstrainedFilterOption(){};

    DREAM3D_INSTANCE_PROPERTY(T, Minimum)
    DREAM3D_INSTANCE_PROPERTY(T, Maximum)

  protected:
    ConstrainedFilterOption() {}

  private:
    ConstrainedFilterOption(const ConstrainedFilterOption&); // Copy Constructor Not Implemented
    void operator=(const ConstrainedFilterOption&); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class ChoiceFilterOption : public FilterOption
{
  public:
    DREAM3D_SHARED_POINTERS(ChoiceFilterOption)
    DREAM3D_STATIC_NEW_MACRO(ChoiceFilterOption)
    DREAM3D_TYPE_MACRO_SUPER(ChoiceFilterOption, FilterOption)

    virtual ~ChoiceFilterOption(){};

    DREAM3D_INSTANCE_PROPERTY(std::vector<std::string>, Choices);

  protected:
    ChoiceFilterOption() {}

  private:
    ChoiceFilterOption(const ChoiceFilterOption&); // Copy Constructor Not Implemented
    void operator=(const ChoiceFilterOption&); // Operator '=' Not Implemented
};


#endif /* FILTEROPTION_H_ */
