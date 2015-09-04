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

#ifndef _FilterManager_H_
#define _FilterManager_H_


#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QMapIterator>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/IFilterFactory.hpp"

/**
 * @brief The FilterManager class manages instances of filters and is mainly used to instantiate
 * an instance of a filter given its human label or class name. This class uses the Factory design
 * pattern.
 */
class SIMPLib_EXPORT FilterManager
{
  public:
//    SIMPL_SHARED_POINTERS(FilterManager)
    SIMPL_TYPE_MACRO(FilterManager)

    virtual ~FilterManager();

    typedef QMap<QString, IFilterFactory::Pointer> Collection;
    typedef QMapIterator<QString, IFilterFactory::Pointer> CollectionIterator;

    /**
     * @brief Static instance to retrieve the global instance of this class
     * @return
     */
    static FilterManager* Instance();

    /**
     * @brief Registers a QFilterFactory instance for a give name
     * @param name The name of the filter
     * @param factory An instance of the factory
     */
    static void RegisterFilterFactory(const QString& name, IFilterFactory::Pointer factory);

    /**
     * @brief RegisterKnownFilters This filter registers a factory for each filter that is included
     * in the core of DREAM3DLib. This method is called when the singleton instance of this class is
     * first created.
     */
    static void RegisterKnownFilters(FilterManager* fm);

    /**
     * @brief FilterManager::printFactoryNames
     */
    void printFactoryNames();

    /**
     * @brief Returns the mapping of names to Factory instances for all the
     * factories that are registered.
     * @return
     */
    Collection getFactories();

    /**
     * @brief Returns the mapping of names to the Factory instances for a given filter group
     * @param groupName The name of the group.
     * @return
     */
    Collection getFactories(const QString& groupName);

    /**
     * @brief Returns the mapping of names to the Factory instances for a given filter subgroup
     * @param subGroupName The name of the subgroup.
     * @return
     */
    Collection getFactories(const QString& groupName, const QString& subGroupName);

    /**
     * @brief Adds a Factory that creates QFilters
     * @param name
     * @param factory
     */
    void addFilterFactory(const QString& name, IFilterFactory::Pointer factory);

    /**
     * @brief getGroupNames Returns the uniqe set of group names for all the filters
     * @return
     */
    QSet<QString> getGroupNames();

    /**
     * @brief getSubGroupNames For a given group, returns all the subgroups
     * @param groupName The name of the Filter group
     * @return
     */
    QSet<QString> getSubGroupNames(const QString& groupName);


    /**
     * @brief getFactoryForFilter Returns a FilterFactory for a given filter
     * @param filterName
     * @return
     */
    IFilterFactory::Pointer getFactoryForFilter(const QString& filterName);

    /**
     * @brief getFactoryForFilterHumanName For a given human label, the FilterFactory is given
     * @param humanName
     * @return
     */
    IFilterFactory::Pointer getFactoryForFilterHumanName(const QString& humanName);


  protected:
    FilterManager();

  private:

    Collection m_Factories;
    static FilterManager* self;

    FilterManager(const FilterManager&); // Copy Constructor Not Implemented
    void operator=(const FilterManager&); // Operator '=' Not Implemented
};

#endif /* _FilterManager_H_ */

