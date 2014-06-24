/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

#include <QtCore/QtDebug>
#include <QtCore/QCoreApplication>
#include <QtCore/QObject>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QMetaProperty>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Observer.h"
#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/FilterFactory.hpp"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Plugin/DREAM3DPluginInterface.h"
#include "DREAM3DLib/Plugin/DREAM3DPluginLoader.h"
#include "DREAM3DLib/Utilities/QMetaObjectUtilities.h"
#include "DREAM3DLib/DREAM3DFilters.h"

#include "DREAM3DLib/Utilities/UnitTestSupport.hpp"
#include "TestFileLocations.h"

#include "PreflightVerify.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString getH5StatsFile()
{
  QString s = UnitTest::DataDir + "/Equiaxed_Precip.h5stats";
  return s;
}

void setCurrentFilter(AbstractFilter::Pointer f)
{

}

void pipelineFinished()
{

}

void setErrorCondition(int err)
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES

#endif
}

#define PASS 0
#define FAIL_IS_PASS 1

#define MAKE_FILTER_TEST(name, condition)\
  void name##_PreFlightTest() {\
  int err = 0;\
  VoxelDataContainer::Pointer m = VoxelDataContainer::New();\
  std::vector<AbstractFilter::Pointer> pipeline;\
  name::Pointer filter = name::New();\
  pipeline.push_back(filter);\
  int preflightError = 0;\
  QStringstream ss;\
  ss << "------------------------------------------------" << std::endl;\
  ss << "Starting Preflight test for " << #name << std::endl;\
  for (std::vector<AbstractFilter::Pointer>::iterator filter = pipeline.begin(); filter != pipeline.end(); ++filter) {\
  (*filter)->setVoxelDataContainer(m.get());\
  setCurrentFilter(*filter);\
  (*filter)->preflight();\
  err = (*filter)->getErrorCondition();\
  if(err < 0) {\
  preflightError |= err;\
  ss << (*filter)->getNameOfClass() << " produced the following preflight errors:" << std::endl;\
  std::vector<PipelineMessage> ems = (*filter)->getPipelineMessages();\
  for (std::vector<PipelineMessage>::iterator iter = ems.begin(); iter != ems.end(); ++iter ) {\
  ss << (*iter).generateErrorString();\
  }\
  }\
  }\
  std::cout << ss.str() << std::endl;\
  if (condition) { DREAM3D_REQUIRE_NE(preflightError, 0);}\
  else { DREAM3D_REQUIRE_EQUAL(preflightError, 0);  }\
  }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateCopyCode()
{
  qDebug() << "-------------- GenerateCopyCode ------------------------------";

  FilterManager::Pointer fm = FilterManager::Instance();
  //QByteArray normType = ("updateFilterParameters(AbstractFilter*)");
  FilterManager::Collection factories = fm->getFactories();
  QMapIterator<QString, IFilterFactory::Pointer> iter(factories);
  while(iter.hasNext())
  {
    iter.next();
    IFilterFactory::Pointer factory = iter.value();
    AbstractFilter::Pointer filter = factory->create();
    const QMetaObject* meta = filter->metaObject();



    std::string cn = filter->getNameOfClass().toStdString();
    std::cout << cn << "::Pointer " << cn << "::newFilterInstance(bool copyFilterParameters)" << std::endl;
    std::cout << "{" << std::endl;

    QStringList properties;
    std::cout << "/*" << std::endl;
    for(int i = meta->propertyOffset(); i < meta->propertyCount(); ++i)
    {
      properties << QString::fromLatin1(meta->property(i).name());
      std::cout << QString::fromLatin1(meta->property(i).name()).toStdString() << std::endl;
    }
    std::cout << "*/" << std::endl;


    std::cout << "  " << cn << "::Pointer filter = " << cn << "::New();" << std::endl;
    std::cout << "  if(true == copyFilterParameters)\n  {" << std::endl;

    QVector<FilterParameter::Pointer> options = filter->getFilterParameters();
    for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
    {
      FilterParameter* option = (*iter).get();
      QByteArray normType = QString("%1").arg( option->getPropertyName()).toLatin1();
      int index = meta->indexOfProperty(normType);
      if (index < 0)
      {
        std::cout << "#error Filter: " << filter->getNameOfClass().toStdString() << "  Missing Property: " << option->getPropertyName().toStdString() << std::endl;
      }

      std::cout << "    filter->set" << option->getPropertyName().toStdString() << "( get" << option->getPropertyName().toStdString() << "() );" << std::endl;
    }
    if(options.size() != properties.count())
    {
      std::cout << "#error The number of Q_PROPERITES " << properties.count() <<
      " does not match the number of FilterParameters " << options.size() << " created in the setupFilterParameters() function." << std::endl;
    }
    std::cout << "  }" << std::endl;
    std::cout << "  return filter;" << std::endl;
    std::cout << "}" << std::endl;
  }
}


#define PROPERTY_NAME_AS_CHAR option->getPropertyName().toLatin1().constData()



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void verifyFilterParameters()
{
  qDebug() << "-------------- verifyFilterParameters ------------------------------";

  FilterManager::Pointer fm = FilterManager::Instance();
  //QByteArray normType = ("updateFilterParameters(AbstractFilter*)");
  FilterManager::Collection factories = fm->getFactories();
  QMapIterator<QString, IFilterFactory::Pointer> iter(factories);
  while(iter.hasNext())
  {
    iter.next();
    IFilterFactory::Pointer factory = iter.value();
    AbstractFilter::Pointer filter = factory->create();
    const QMetaObject* meta = filter->metaObject();
//    qDebug() << filter->getNameOfClass() << "Default Values";
    QVector<FilterParameter::Pointer> options = filter->getFilterParameters();
    for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
    {
      FilterParameter* option = (*iter).get();
      if (option->getHumanLabel().compare("Required Information") == 0
          || option->getHumanLabel().compare("Created Information") == 0
          || option->getHumanLabel().compare("Optional Information") == 0)
          { continue; }
      QByteArray normType = QString("%1").arg( option->getPropertyName()).toLatin1();
      int index = meta->indexOfProperty(normType);
      if (index < 0)
      {
        qDebug() << "Filter: " << filter->getNameOfClass() << "  Missing Property: " << option->getHumanLabel() << "  "  << option->getPropertyName();
      }
//      qDebug() << "    " << option->getHumanLabel() << "::" << filter->property(PROPERTY_NAME_AS_CHAR);
    }

    // If something is wrong with the newFilterInstance then the next line will assert
    AbstractFilter::Pointer nFilter = filter->newFilterInstance(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void verifySignals()
{
  qDebug() << "-------------- verifySignals ------------------------------";

  FilterManager::Pointer fm = FilterManager::Instance();
  QByteArray normType = ("updateFilterParameters(AbstractFilter*)");
  FilterManager::Collection factories = fm->getFactories();
  QMapIterator<QString, IFilterFactory::Pointer> iter(factories);
  while(iter.hasNext())
  {
    iter.next();
    IFilterFactory::Pointer factory = iter.value();
    AbstractFilter::Pointer filter = factory->create();
    const QMetaObject* meta = filter->metaObject();
    int count = meta->methodCount();
    int index = meta->indexOfSignal(normType);
    if (index < 0)
    {
      qDebug() << "Filter: " << iter.key() << "  Method Count: " << count;
      qDebug() << "==>  Signal void updateFilterParameters(AbstractFilter* filter) does not exist";
    }
  }
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void verifyPreflightEmitsProperly()
{
  qDebug() << "-------------- verifyPreflightEmitsProperly ------------------------------";
  FilterManager::Pointer fm = FilterManager::Instance();
  FilterManager::Collection factories = fm->getFactories();
  QMapIterator<QString, IFilterFactory::Pointer> iter(factories);

  while(iter.hasNext())
  {
    PreflightVerify p(NULL);
    iter.next();
    IFilterFactory::Pointer factory = iter.value();
    AbstractFilter::Pointer filter = factory->create();
    filter->connect(filter.get(), SIGNAL(preflightAboutToExecute()),
                    &p, SLOT(beforePreflight()) );
    filter->connect(filter.get(), SIGNAL(preflightExecuted()),
                    &p, SLOT(afterPreflight()) );
    filter->connect(filter.get(), SIGNAL(updateFilterParameters(AbstractFilter*) ),
                    &p, SLOT(filterNeedsInputParameters(AbstractFilter*)) );

    filter->preflight();

    if(p.m_beforePreflight == false)
    {
      qDebug() << filter->getNameOfClass() << " Missing emit preflightAboutToExecute()";
    }
    if(p.m_afterPreflight == false)
    {
      qDebug() << filter->getNameOfClass() << " Missing emit preflightExecuted()";
    }
    if(p.m_filterNeedsInputParameters == false)
    {
      qDebug() << filter->getNameOfClass() << " Missing emit updateFilterParameters()";
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestPreflight()
{
  qDebug() << "-------------- TestPreflight ------------------------------";
  FilterManager::Pointer fm = FilterManager::Instance();

  FilterManager::Collection factories = fm->getFactories();
  FilterManager::Collection::const_iterator factoryMapIter = factories.constBegin();
  int err = 0;
  while (factoryMapIter != factories.constEnd())
  {
    IFilterFactory::Pointer factory = fm->getFactoryForFilter(factoryMapIter.key());
    if(factory.get() != NULL)
    {
      AbstractFilter::Pointer filter = factory->create();

      filter->preflight();
      err = filter->getErrorCondition();
      // An error condition GREATER than ZERO is an anomoly and should be looked at.
      if (err >= 0) { qDebug() << "Testing Preflight for " << filter->getGroupName() << "/" << filter->getNameOfClass(); }
    }
    factoryMapIter++;
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestNewInstanceAvailable()
{
  FilterManager::Pointer fm = FilterManager::Instance();
  QByteArray normType = ("updateFilterParameters(AbstractFilter*)");
  FilterManager::Collection factories = fm->getFactories();
  QMapIterator<QString, IFilterFactory::Pointer> iter(factories);
  while(iter.hasNext())
  {
    iter.next();
    IFilterFactory::Pointer factory = iter.value();
    AbstractFilter::Pointer filter = factory->create();

    AbstractFilter::Pointer copy = filter->newFilterInstance(true);
    if(NULL == copy.get())
    {
      std::cout << "Filter: '" << filter->getNameOfClass().toStdString() << "' from Library/Plugin '" << filter->getCompiledLibraryName().toStdString() << " has not implemented the newInstance() function" << std::endl;
    }
    DREAM3D_REQUIRE_NE(NULL, copy.get());
  }
}

// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{

  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("PreflightTest");


  // Load all the plugins and
  // Register all the filters including trying to load those from Plugins
  FilterManager::Pointer fm = FilterManager::Instance();
  DREAM3DPluginLoader::LoadPluginFilters(fm.get());
  // THIS IS A VERY IMPORTANT LINE: It will register all the known filters in the dream3d library. This
  // will NOT however get filters from plugins. We are going to have to figure out how to compile filters
  // into their own plugin and load the plugins from a command line.
  fm->RegisterKnownFilters(fm.get());

  QMetaObjectUtilities::RegisterMetaTypes();

  //// These functions are just to verify that the filters have certain signals and properties available.
  verifyPreflightEmitsProperly();
  verifySignals();
  verifyFilterParameters();

  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( TestPreflight() )
  DREAM3D_REGISTER_TEST( TestNewInstanceAvailable() )
  PRINT_TEST_SUMMARY();

//  GenerateCopyCode();
  return err;
}

