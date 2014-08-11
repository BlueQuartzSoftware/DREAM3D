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
#include "FilterWidgetManager.h"


#include "PipelineFilterWidgetFactory.hpp"

#include "DREAM3DWidgetsLib/FilterWidgetHeaders.h"

FilterWidgetManager* FilterWidgetManager::self = 0;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterWidgetManager::FilterWidgetManager()
{
// qDebug() << "FilterWidgetManager()" << this;

  Q_ASSERT_X(!self, "FilterWidgetManager", "there should be only one FilterWidgetManager object");
  FilterWidgetManager::self = this;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterWidgetManager::~FilterWidgetManager()
{
// qDebug() << "~FilterWidgetManager()" << this;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterWidgetManager* FilterWidgetManager::Instance()
{
  if (self == NULL)
  {
//   qDebug() << "FilterWidgetManager::Instance self was NULL" << "\n";
    self = new FilterWidgetManager();
  }
//  qDebug() << "self.get(): " << self << "\n";
  return self;
}

// -----------------------------------------------------------------------------
//  Static Method
// -----------------------------------------------------------------------------
void FilterWidgetManager::RegisterFilterWidgetFactory(const QString& name, IFilterWidgetFactory::Pointer factory)
{
  if (NULL != factory.get() )
  {
    // Instantiate the Instance Manager for IFilterWidgetFactory
    FilterWidgetManager* idManager = FilterWidgetManager::Instance();
    idManager->addFilterWidgetFactory( name, factory );
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterWidgetManager::Collection FilterWidgetManager::getFactories()
{
  return m_Factories;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterWidgetManager::addFilterWidgetFactory(const QString& name, IFilterWidgetFactory::Pointer factory)
{
  m_Factories[name] = factory;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterWidgetManager::RegisterKnownFilterWidgets()
{
  FilterWidgetManager* idManager = FilterWidgetManager::Instance();
  // This next file is generated with CMake
#include "DREAM3DWidgetsLib/FilterWidgetManager_RegisterWidgets.cpp"

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* FilterWidgetManager::createWidget(FilterParameter* parameter, AbstractFilter* filter)
{
  IFilterWidgetFactory::Pointer factory = m_Factories.value(parameter->getWidgetType());
  if(NULL != factory.get())
  {
    return factory->createWidget(parameter, filter, NULL);
  }

  factory = m_Factories.value(FilterParameterWidgetType::UnknownWidget);
  QWidget* w = factory->createWidget(parameter, filter, NULL);
  return w;
}



