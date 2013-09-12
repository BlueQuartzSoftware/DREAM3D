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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterWidgetManager::FilterWidgetManager()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterWidgetManager::~FilterWidgetManager()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterWidgetManager::Pointer FilterWidgetManager::Instance()
{
  static FilterWidgetManager::Pointer singleton;

  if (singleton.get() == NULL)
  {
   //   qDebug() << "FilterWidgetManager::Instance singleton was NULL" << "\n";
    singleton.reset (new FilterWidgetManager() );
   // qDebug() << "singleton.get(): " << singleton.get() << "\n";
  }
  return singleton;
}

// -----------------------------------------------------------------------------
//  Static Method
// -----------------------------------------------------------------------------
void FilterWidgetManager::RegisterFilterWidgetFactory(const QString &name, IFilterWidgetFactory::Pointer factory)
{
  if (NULL != factory.get() )
  {
    // Instantiate the Instance Manager for IFilterWidgetFactory
    FilterWidgetManager::Pointer idManager = FilterWidgetManager::Instance();
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
FilterWidgetManager::Collection FilterWidgetManager::getFactories(const QString &groupName)
{
  FilterWidgetManager::Collection groupFactories;


  for (FilterWidgetManager::Collection::iterator factory = m_Factories.begin(); factory != m_Factories.end(); ++factory)
  {
    IFilterWidgetFactory::Pointer filterFactory = (*factory).second;
    if ( NULL != filterFactory.get() && (*factory).second->getFilterGroup().compare(groupName) == 0)
    {
      groupFactories[(*factory).first] = (*factory).second;
    }
  }
  return groupFactories;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterWidgetManager::Collection FilterWidgetManager::getFactories(const QString &groupName, const QString &subGroupName)
{
  FilterWidgetManager::Collection groupFactories;


  for (FilterWidgetManager::Collection::iterator factory = m_Factories.begin(); factory != m_Factories.end(); ++factory)
  {
    IFilterWidgetFactory::Pointer filterFactory = (*factory).second;
  if ( NULL != filterFactory.get() && (*factory).second->getFilterGroup().compare(groupName) == 0 && (*factory).second->getFilterSubGroup().compare(subGroupName) == 0)
    {
      groupFactories[(*factory).first] = (*factory).second;
    }
  }
  return groupFactories;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterWidgetManager::addFilterWidgetFactory(const QString &name, IFilterWidgetFactory::Pointer factory)
{
  m_Factories[name] = factory;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSet<QString> FilterWidgetManager::getGroupNames()
{
 // qDebug() << "FilterWidgetManager::getGroupNames" << "\n";
  // Get all the Widget Factories and loop over each one we know about and instantiate a new one
  FilterWidgetManager::Pointer fm = FilterWidgetManager::Instance();
  FilterWidgetManager::Collection factories = fm->getFactories();
  QSet<QString> groupNames;
  for (FilterWidgetManager::Collection::iterator factory = factories.begin(); factory != factories.end(); ++factory)
  {
    IFilterWidgetFactory::Pointer filterFactory = (*factory).second;
    groupNames.insert((*factory).second->getFilterGroup());
  //  qDebug() << (*factory).second->getFilterGroup() << "\n";
  }
  return groupNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSet<QString> FilterWidgetManager::getSubGroupNames(const QString &groupName)
{
 // qDebug() << "FilterWidgetManager::getGroupNames" << "\n";
  // Get all the Widget Factories and loop over each one we know about and instantiate a new one
  FilterWidgetManager::Pointer fm = FilterWidgetManager::Instance();
  FilterWidgetManager::Collection factories = fm->getFactories();
  QSet<QString> subGroupNames;
  for (FilterWidgetManager::Collection::iterator factory = factories.begin(); factory != factories.end(); ++factory)
  {
    IFilterWidgetFactory::Pointer filterFactory = (*factory).second;
  if ( NULL != filterFactory.get() && (*factory).second->getFilterGroup().compare(groupName) == 0)
    {
      subGroupNames.insert((*factory).second->getFilterSubGroup());
  }
  }
  return subGroupNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IFilterWidgetFactory::Pointer FilterWidgetManager::getFactoryForFilter(const QString &filterName)
{
  return m_Factories[filterName];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IFilterWidgetFactory::Pointer FilterWidgetManager::getFactoryForFilterHumanName(const QString &humanName)
{
  IFilterWidgetFactory::Pointer widgetFactory;

  for (FilterWidgetManager::Collection::iterator factory = m_Factories.begin(); factory != m_Factories.end(); ++factory)
  {
    IFilterWidgetFactory::Pointer filterFactory = (*factory).second;
    if ( NULL != filterFactory.get() && filterFactory->getFilterHumanLabel().compare(humanName) == 0)
    {
      widgetFactory = filterFactory;
      break;
    }
  }
  return widgetFactory;
}

