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
#include "FilterManager.h"

#include "DREAM3DLib/Common/FilterFactory.hpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterManager::FilterManager()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterManager::~FilterManager()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterManager::Pointer FilterManager::Instance()
{
  static FilterManager::Pointer singleton;

  if (singleton.get() == NULL)
  {
    singleton.reset ( new FilterManager() );
    singleton->RegisterKnownFilters();
  }
  return singleton;
}

// -----------------------------------------------------------------------------
//  Static Method
// -----------------------------------------------------------------------------
void FilterManager::RegisterFilterFactory(const std::string &name, IFilterFactory::Pointer factory)
{
  if (NULL != factory.get() )
  {
    // Instantiate the Instance Manager for IFilterFactory
    FilterManager::Pointer idManager = FilterManager::Instance();
    idManager->addFilterFactory( name, factory );
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterManager::Collection FilterManager::getFactories()
{
  return m_Factories;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterManager::Collection FilterManager::getFactories(const std::string &groupName)
{
  FilterManager::Collection groupFactories;


  for (FilterManager::Collection::iterator factory = m_Factories.begin(); factory != m_Factories.end(); ++factory)
  {
    IFilterFactory::Pointer filterFactory = (*factory).second;
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
FilterManager::Collection FilterManager::getFactories(const std::string &groupName, const std::string &subGroupName)
{
  FilterManager::Collection groupFactories;


  for (FilterManager::Collection::iterator factory = m_Factories.begin(); factory != m_Factories.end(); ++factory)
  {
    IFilterFactory::Pointer filterFactory = (*factory).second;
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
void FilterManager::addFilterFactory(const std::string &name, IFilterFactory::Pointer factory)
{
  m_Factories[name] = factory;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::set<std::string> FilterManager::getGroupNames()
{
 // std::cout << "FilterManager::getGroupNames" << std::endl;
  // Get all the  Factories and loop over each one we know about and instantiate a new one
  FilterManager::Pointer fm = FilterManager::Instance();
  FilterManager::Collection factories = fm->getFactories();
  std::set<std::string> groupNames;
  for (FilterManager::Collection::iterator factory = factories.begin(); factory != factories.end(); ++factory)
  {
    IFilterFactory::Pointer filterFactory = (*factory).second;
    groupNames.insert((*factory).second->getFilterGroup());
  //  std::cout << (*factory).second->getFilterGroup() << std::endl;
  }
  return groupNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::set<std::string> FilterManager::getSubGroupNames(const std::string &groupName)
{
 // std::cout << "FilterManager::getGroupNames" << std::endl;
  // Get all the  Factories and loop over each one we know about and instantiate a new one
  FilterManager::Pointer fm = FilterManager::Instance();
  FilterManager::Collection factories = fm->getFactories();
  std::set<std::string> subGroupNames;
  for (FilterManager::Collection::iterator factory = factories.begin(); factory != factories.end(); ++factory)
  {
    IFilterFactory::Pointer filterFactory = (*factory).second;
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
IFilterFactory::Pointer FilterManager::getFactoryForFilter(const std::string &filterName)
{
  return m_Factories[filterName];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IFilterFactory::Pointer FilterManager::getFactoryForFilterHumanName(const std::string &humanName)
{
  IFilterFactory::Pointer Factory;

  for (FilterManager::Collection::iterator factory = m_Factories.begin(); factory != m_Factories.end(); ++factory)
  {
    IFilterFactory::Pointer filterFactory = (*factory).second;
    if ( NULL != filterFactory.get() && filterFactory->getFilterHumanLabel().compare(humanName) == 0)
    {
      Factory = filterFactory;
      break;
    }
  }
  return Factory;
}


/* This next line includes a file that is generated at CMake time and includes all the filter headers
 * and code to register a factory instance for each filter.
 */
#include "DREAM3DLib/RegisterKnownFilters.cpp"
