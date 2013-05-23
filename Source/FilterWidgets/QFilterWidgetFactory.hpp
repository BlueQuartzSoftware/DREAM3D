/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef QFILTERWIDGETFACTORY_H_
#define QFILTERWIDGETFACTORY_H_

#include "PipelineBuilder/IFilterWidgetFactory.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

template <class Widget>
class QFilterWidgetFactory : public IFilterWidgetFactory
{
  public:
    DREAM3D_SHARED_POINTERS(QFilterWidgetFactory<Widget> )
    DREAM3D_TYPE_MACRO_SUPER(QFilterWidgetFactory<Widget>, IFilterWidgetFactory)
    DREAM3D_STATIC_NEW_MACRO(QFilterWidgetFactory<Widget>)

    /**
     * @brief Creates a new widget for this filter. The Calling method MUST set
     * a parent Widget OR take responsibility for deleting this object.
     * @return
     */
    QFilterWidget* createWidget()
    {
      return new Widget;
    }

    std::string getFilterGroup()
    {
      return m_GroupName;
    }

    std::string getFilterSubGroup()
    {
      return m_SubGroupName;
    }

    std::string getFilterHumanLabel()
    {
      return m_HumanName;
    }

    AbstractFilter::Pointer getFilterInstance()
    {
      Widget w;
      return w.getFilter();
    }

  protected:
    QFilterWidgetFactory() {
      Widget w;
      m_GroupName = w.getFilter()->getGroupName();
      m_SubGroupName = w.getFilter()->getSubGroupName();
      m_HumanName = w.getFilter()->getHumanLabel();
    }

  private:
    std::string m_GroupName;
    std::string m_SubGroupName;
    std::string m_HumanName;

    QFilterWidgetFactory(const QFilterWidgetFactory&); // Copy Constructor Not Implemented
    void operator=(const QFilterWidgetFactory&); // Operator '=' Not Implemented
};
#endif /* QFILTERWIDGETFACTORY_H_ */
