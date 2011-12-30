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

    virtual ~FilterOption(){};

    enum WidgetType
    {
      StringWidget,
      IntWidget,
      DoubleWidget,
      InputFileWidget,
      OutputFileWidget,
      BooleanWidget,
      CustomWidget
    };


    DREAM3D_INSTANCE_STRING_PROPERTY(Label);
    DREAM3D_INSTANCE_PROPERTY(WidgetType, WidgetType);

  protected:
    FilterOption() {}

  private:
    FilterOption(const FilterOption&); // Copy Constructor Not Implemented
    void operator=(const FilterOption&); // Operator '=' Not Implemented
};



#endif /* FILTEROPTION_H_ */
