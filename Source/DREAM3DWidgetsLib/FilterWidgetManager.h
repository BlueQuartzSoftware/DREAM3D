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
#ifndef PipelineFilterWidgetMANAGER_H_
#define PipelineFilterWidgetMANAGER_H_


#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QSet>

#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLib.h"
#include "DREAM3DWidgetsLib/IFilterWidgetFactory.h"


/**
 * @class FilterWidgetManager FilterWidgetManager.h FilterWidgets/FilterWidgetManager.h
 * @brief This class is a singleton based manager for creating Qt based widgets
 * that expose the various options for each filter.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 6, 2012
 * @version 1.0
 */
class DREAM3DWidgetsLib_EXPORT FilterWidgetManager
{
  public:
    DREAM3D_SHARED_POINTERS(FilterWidgetManager)
    DREAM3D_TYPE_MACRO(FilterWidgetManager)

    virtual ~FilterWidgetManager();

    typedef QMap<QString, IFilterWidgetFactory::Pointer> Collection;

    /**
     * @brief Static instance to retrieve the global instance of this class
     * @return
     */
    static Pointer Instance();

    /**
     * @brief Registers a PipelineFilterWidgetFactory instance for a give name
     * @param name The name of the filter
     * @param factory An instance of the factory
     */
    static void RegisterFilterWidgetFactory(const QString& name, IFilterWidgetFactory::Pointer factory);

    static void RegisterKnownFilterWidgets();


    /**
     * @brief Returns the mapping of names to Factory instances for all the
     * factories that are registered.
     * @return
     */
    Collection getFactories();

    /**
       * @brief Adds a Factory that creates PipelineFilterWidgets
       * @param name
       * @param factory
       */
    void addFilterWidgetFactory(const QString& widgetType, IFilterWidgetFactory::Pointer factory);

    QWidget* createWidget(FilterParameter* parameter, AbstractFilter* filter);

  protected:
    FilterWidgetManager();

  private:

    Collection m_Factories;


    FilterWidgetManager(const FilterWidgetManager&); // Copy Constructor Not Implemented
    void operator=(const FilterWidgetManager&); // Operator '=' Not Implemented
};

#endif /* PipelineFilterWidgetMANAGER_H_ */

