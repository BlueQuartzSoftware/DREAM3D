/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#ifndef _FilterFACTORY_H_
#define _FilterFACTORY_H_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/IFilterFactory.hpp"


template <class T>
class FilterFactory : public IFilterFactory
{
  public:
    SIMPL_SHARED_POINTERS(FilterFactory<T> )
    SIMPL_TYPE_MACRO_SUPER(FilterFactory<T>, IFilterFactory)
    SIMPL_STATIC_NEW_MACRO(FilterFactory<T>)

    /**
     * @brief Creates a new  for this filter. The Calling method MUST set
     * a parent  OR take responsibility for deleting this object.
     * @return
     */
    AbstractFilter::Pointer create()
    {
      typename T::Pointer w = T::New();
      return w;
    }

    QString getFilterGroup()
    {
      return m_GroupName;
    }

    QString getFilterSubGroup()
    {
      return m_SubGroupName;
    }

    QString getFilterHumanLabel()
    {
      return m_HumanName;
    }



  protected:
    FilterFactory()
    {
      typename T::Pointer w = T::New();
      m_GroupName = w->getGroupName();
      m_SubGroupName = w->getSubGroupName();
      m_HumanName = w->getHumanLabel();
    }

  private:
    QString m_GroupName;
    QString m_SubGroupName;
    QString m_HumanName;

    FilterFactory(const FilterFactory&); // Copy Constructor Not Implemented
    void operator=(const FilterFactory&); // Operator '=' Not Implemented
};
#endif /* FilterFACTORY_H_ */

