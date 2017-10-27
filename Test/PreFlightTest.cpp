/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QMetaProperty>
#include <QtCore/QObject>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QtDebug>

#include "SIMPLib/Common/Observer.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/SIMPLibFilters.h"
#include "UnitTestSupport.hpp"

#include "DREAM3DTestFileLocations.h"

#include "PreflightVerify.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString getH5StatsFile()
{
  QString s = UnitTest::DataDir + "/Equiaxed_Precip.h5stats";
  return s;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void writeAllFilters(QString filePath)
{
  FilterManager* fm = FilterManager::Instance();
  FilterManager::Collection factories = fm->getFactories();
  QMapIterator<QString, IFilterFactory::Pointer> iter(factories);
  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  while(iter.hasNext())
  {
    iter.next();
    IFilterFactory::Pointer factory = iter.value();
    AbstractFilter::Pointer filter = factory->create();
    pipeline->pushBack(filter);
  }

  JsonFilterParametersWriter::Pointer writer = JsonFilterParametersWriter::New();
  writer->writePipelineToFile(pipeline, filePath, "Pipeline");
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

#define MAKE_FILTER_TEST(name, condition)                                                                                                                                                              \
  void name##_PreFlightTest()                                                                                                                                                                          \
  {                                                                                                                                                                                                    \
    int err = 0;                                                                                                                                                                                       \
    VoxelDataContainer::Pointer m = VoxelDataContainer::New();                                                                                                                                         \
    std::vector<AbstractFilter::Pointer> pipeline;                                                                                                                                                     \
    name::Pointer filter = name::New();                                                                                                                                                                \
    pipeline.push_back(filter);                                                                                                                                                                        \
    int preflightError = 0;                                                                                                                                                                            \
    QStringstream ss;                                                                                                                                                                                  \
    ss << "------------------------------------------------" << std::endl;                                                                                                                             \
    ss << "Starting Preflight test for " << #name << std::endl;                                                                                                                                        \
    for(std::vector<AbstractFilter::Pointer>::iterator filter = pipeline.begin(); filter != pipeline.end(); ++filter)                                                                                  \
    {                                                                                                                                                                                                  \
      (*filter)->setVoxelDataContainer(m.get());                                                                                                                                                       \
      setCurrentFilter(*filter);                                                                                                                                                                       \
      (*filter)->preflight();                                                                                                                                                                          \
      err = (*filter)->getErrorCondition();                                                                                                                                                            \
      if(err < 0)                                                                                                                                                                                      \
      {                                                                                                                                                                                                \
        preflightError |= err;                                                                                                                                                                         \
        ss << (*filter)->getNameOfClass() << " produced the following preflight errors:" << std::endl;                                                                                                 \
        std::vector<PipelineMessage> ems = (*filter)->getPipelineMessages();                                                                                                                           \
        for(std::vector<PipelineMessage>::iterator iter = ems.begin(); iter != ems.end(); ++iter)                                                                                                      \
        {                                                                                                                                                                                              \
          ss << (*iter).generateErrorString();                                                                                                                                                         \
        }                                                                                                                                                                                              \
      }                                                                                                                                                                                                \
    }                                                                                                                                                                                                  \
    std::cout << ss.str() << std::endl;                                                                                                                                                                \
    if(condition)                                                                                                                                                                                      \
    {                                                                                                                                                                                                  \
      DREAM3D_REQUIRE_NE(preflightError, 0);                                                                                                                                                           \
    }                                                                                                                                                                                                  \
    else                                                                                                                                                                                               \
    {                                                                                                                                                                                                  \
      DREAM3D_REQUIRE_EQUAL(preflightError, 0);                                                                                                                                                        \
    }                                                                                                                                                                                                  \
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateCopyCode()
{
  qDebug() << "-------------- GenerateCopyCode ------------------------------";

  FilterManager* fm = FilterManager::Instance();
  // QByteArray normType = ("updateFilterParameters(AbstractFilter*)");
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
    for(QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter)
    {
      FilterParameter* option = (*iter).get();
      QByteArray normType = QString("%1").arg(option->getPropertyName()).toLatin1();
      int index = meta->indexOfProperty(normType);
      if(index < 0)
      {
        std::cout << "#error Filter: " << filter->getNameOfClass().toStdString() << "  Missing Property: " << option->getPropertyName().toStdString() << std::endl;
      }

      std::cout << "    filter->set" << option->getPropertyName().toStdString() << "( get" << option->getPropertyName().toStdString() << "() );" << std::endl;
    }
    if(options.size() != properties.count())
    {
      std::cout << "#error The number of Q_PROPERITES " << properties.count() << " does not match the number of FilterParameters " << options.size()
                << " created in the setupFilterParameters() function." << std::endl;
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

  FilterManager* fm = FilterManager::Instance();
  // QByteArray normType = ("updateFilterParameters(AbstractFilter*)");
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
    for(QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter)
    {
      FilterParameter* option = (*iter).get();
      if(option->getHumanLabel().compare("Required Information") == 0 || option->getHumanLabel().compare("Created Information") == 0 || option->getHumanLabel().compare("Optional Information") == 0)
      {
        continue;
      }

      if(nullptr != dynamic_cast<SeparatorFilterParameter*>(option))
      {
        continue;
      }
      QByteArray normType = QString("%1").arg(option->getPropertyName()).toLatin1();
      int index = meta->indexOfProperty(normType);
      if(index < 0)
      {
        qDebug() << "Filter: " << filter->getNameOfClass() << "  Missing Property: " << option->getHumanLabel() << "  " << option->getPropertyName();
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

  FilterManager* fm = FilterManager::Instance();
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
    if(index < 0)
    {
      qDebug() << "Filter: " << iter.key() << "  Method Count: " << count;
      qDebug() << "==>  Signal void updateFilterParameters(AbstractFilter* filter) does not exist";
    }
  }
}

#if 0
if ( (L) == (R) )
{
  \
  QString buf;
  \
  QTextStream ss(&buf);
  \
  ss << "Your test required the following\n            '";
  \
  ss << #L << " != " << #R << "'\n             but this condition was not met.\n";\
  ss << "             " << L << "==" << R;\
  DREAM3D_TEST_THROW_EXCEPTION( buf.toStdString() ) }
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void verifyPreflightEmitsProperly()
{
  qDebug() << "-------------- verifyPreflightEmitsProperly ------------------------------";
  FilterManager* fm = FilterManager::Instance();
  FilterManager::Collection factories = fm->getFactories();
  QMapIterator<QString, IFilterFactory::Pointer> iter(factories);

  while(iter.hasNext())
  {
    PreflightVerify p(nullptr);
    iter.next();
    IFilterFactory::Pointer factory = iter.value();
    AbstractFilter::Pointer filter = factory->create();
    filter->connect(filter.get(), SIGNAL(preflightAboutToExecute()), &p, SLOT(beforePreflight()));
    filter->connect(filter.get(), SIGNAL(preflightExecuted()), &p, SLOT(afterPreflight()));
    filter->connect(filter.get(), SIGNAL(updateFilterParameters(AbstractFilter*)), &p, SLOT(filterNeedsInputParameters(AbstractFilter*)));

    filter->preflight();

    QString buf;
    QTextStream ss(&buf);
    if(p.m_beforePreflight == false)
    {
      ss << filter->getNameOfClass() << " Missing emit preflightAboutToExecute()";
    }
    if(p.m_afterPreflight == false)
    {
      ss << filter->getNameOfClass() << " Missing emit preflightExecuted()";
    }
    if(p.m_filterNeedsInputParameters == false)
    {
      ss << filter->getNameOfClass() << " Missing emit updateFilterParameters()";
    }
    if(filter->getInPreflight() == true)
    {
      ss << filter->getNameOfClass() << " Bool 'InPreflight' was NOT set correctly at end of 'preflight()'";
    }
    if(buf.isEmpty() == false)
    {
      DREAM3D_TEST_THROW_EXCEPTION(buf.toStdString())
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestPreflight(bool dataContainer = false, bool attributeMatrix = false, bool dataArray = false)
{
  qDebug() << "-------------- TestPreflight ------------------------------";

  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  FilterManager* fm = FilterManager::Instance();

  QString includeStr = "----------- ";
  if(true == dataContainer)
  {
    IFilterFactory::Pointer ff = fm->getFactoryForFilter(QString("CreateDataContainer"));

    AbstractFilter::Pointer dcFilter = ff->create();
    pipeline->pushBack(dcFilter);

    includeStr += "With Data Container ";
    if(true == attributeMatrix)
    {
      IFilterFactory::Pointer ff = fm->getFactoryForFilter(QString("CreateAttributeMatrix"));

      AbstractFilter::Pointer amFilter = ff->create();
      pipeline->pushBack(amFilter);

      includeStr += "and Attribute Matrix ";
      if(true == dataArray)
      {
        IFilterFactory::Pointer ff = fm->getFactoryForFilter(QString("CreateDataArray"));

        AbstractFilter::Pointer daFilter = ff->create();
        pipeline->pushBack(daFilter);

        includeStr += "and Data Array ";
      }
    }

    includeStr += "-----------";
    qDebug() << includeStr.toStdString().c_str();
  }

  FilterManager::Collection factories = fm->getFactories();
  FilterManager::Collection::const_iterator factoryMapIter = factories.constBegin();
  int err = 0;
  while(factoryMapIter != factories.constEnd())
  {
    IFilterFactory::Pointer factory = fm->getFactoryForFilter(factoryMapIter.key());
    if(factory.get() != nullptr)
    {
      AbstractFilter::Pointer filter = factory->create();

      pipeline->pushBack(filter);
      pipeline->preflightPipeline();

      // DREAM3D_REQUIRE_EQUAL(filter->getInPreflight(), false);
      err = pipeline->getErrorCondition();
      // An error condition GREATER than ZERO is an anomoly and should be looked at.
      if(err > 0)
      {
        qDebug() << "Anomalous result for Preflight for " << filter->getGroupName() << "/" << filter->getNameOfClass() << " Error Condition = " << filter->getErrorCondition();
      }
      pipeline->popBack();
    }
    factoryMapIter++;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestUniqueHumanLabels()
{
  FilterManager* fm = FilterManager::Instance();

  FilterManager::Collection factories = fm->getFactories();
  FilterManager::Collection::const_iterator factoryMapIter = factories.constBegin();
  QMap<QString, AbstractFilter::Pointer> filterMap;
  while(factoryMapIter != factories.constEnd())
  {
    IFilterFactory::Pointer factory = fm->getFactoryForFilter(factoryMapIter.key());
    if(factory.get() != nullptr)
    {
      AbstractFilter::Pointer filter = factory->create();
      if(filter.get() != nullptr)
      {
        QString name = filter->getHumanLabel();
        if(filterMap.contains(name))
        {
          AbstractFilter::Pointer other = filterMap.value(name);
          qDebug() << "Filters in class names '" << filter->getNameOfClass() << "' and '" << other->getNameOfClass() << "' have the same human label, and this is not allowed.";
          DREAM3D_REQUIRE_EQUAL(0, 1)
        }
        else
        {
          filterMap.insert(name, filter);
        }
      }
    }
    factoryMapIter++;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestUncategorizedFilterParameters()
{
  FilterManager* fm = FilterManager::Instance();

  FilterManager::Collection factories = fm->getFactories();
  FilterManager::Collection::const_iterator factoryMapIter = factories.constBegin();
  QMap<QString, AbstractFilter::Pointer> filterMap;
  while(factoryMapIter != factories.constEnd())
  {
    IFilterFactory::Pointer factory = fm->getFactoryForFilter(factoryMapIter.key());
    if(factory.get() != nullptr)
    {
      AbstractFilter::Pointer filter = factory->create();
      if(filter.get() != nullptr)
      {
        QVector<FilterParameter::Pointer> parameters = filter->getFilterParameters();
        foreach(FilterParameter::Pointer fp, parameters)
        {
          if(fp->getCategory() == FilterParameter::Uncategorized)
          {
            qDebug() << "[]" << filter->getCompiledLibraryName() << "  Filter: " << filter->getNameOfClass() << " Filter Parameter: " << fp->getPropertyName() << " IS NOT Categorized.";
          }
        }
      }
    }
    factoryMapIter++;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestNewInstanceAvailable()
{
  FilterManager* fm = FilterManager::Instance();
  QByteArray normType = ("updateFilterParameters(AbstractFilter*)");
  FilterManager::Collection factories = fm->getFactories();
  QMapIterator<QString, IFilterFactory::Pointer> iter(factories);
  while(iter.hasNext())
  {
    iter.next();
    IFilterFactory::Pointer factory = iter.value();
    AbstractFilter::Pointer filter = factory->create();

    AbstractFilter::Pointer copy = filter->newFilterInstance(true);
    if(nullptr == copy.get())
    {
      std::cout << "Filter: '" << filter->getNameOfClass().toStdString() << "' from Library/Plugin '" << filter->getCompiledLibraryName().toStdString()
                << " has not implemented the newInstance() function" << std::endl;
    }
    DREAM3D_REQUIRED_PTR(copy.get(), !=, nullptr)
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrintFilterInfo()
{
  qDebug() << "-------------- PrintFilterInfo ------------------------------";
  FilterManager* fm = FilterManager::Instance();

  FilterManager::Collection factories = fm->getFactories();
  FilterManager::Collection::const_iterator factoryMapIter = factories.constBegin();
  while(factoryMapIter != factories.constEnd())
  {
    IFilterFactory::Pointer factory = fm->getFactoryForFilter(factoryMapIter.key());
    if(factory.get() != nullptr)
    {
      AbstractFilter::Pointer filter = factory->create();

      std::cout << filter->getNameOfClass().toStdString() << "\t" << filter->getCompiledLibraryName().toStdString() << "\t" << filter->getHumanLabel().toStdString() << std::endl;
    }
    factoryMapIter++;
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
  FilterManager* fm = FilterManager::Instance();
  SIMPLibPluginLoader::LoadPluginFilters(fm);
  // THIS IS A VERY IMPORTANT LINE: It will register all the known filters in the dream3d library. This
  // will NOT however get filters from plugins. We are going to have to figure out how to compile filters
  // into their own plugin and load the plugins from a command line.
  fm->RegisterKnownFilters(fm);

  QMetaObjectUtilities::RegisterMetaTypes();

  //// These functions are just to verify that the filters have certain signals and properties available.
  //  verifyPreflightEmitsProperly();
  verifySignals();
  verifyFilterParameters();

  int err = EXIT_SUCCESS;

  QString outputFilePath = QString("%1/AllFiltersPipelineNew.json").arg(UnitTest::TestTempDir);
  writeAllFilters(outputFilePath);
  DREAM3D_REGISTER_TEST(verifyPreflightEmitsProperly())
  DREAM3D_REGISTER_TEST(TestPreflight())
  DREAM3D_REGISTER_TEST(TestPreflight(true))
  DREAM3D_REGISTER_TEST(TestPreflight(true, true))
  DREAM3D_REGISTER_TEST(TestPreflight(true, true, true))
  DREAM3D_REGISTER_TEST(TestUniqueHumanLabels())
  DREAM3D_REGISTER_TEST(TestNewInstanceAvailable())
  DREAM3D_REGISTER_TEST(TestUncategorizedFilterParameters())
  PRINT_TEST_SUMMARY();

  //  GenerateCopyCode();
  return err;
}
