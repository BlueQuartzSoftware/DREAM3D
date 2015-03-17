/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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

#include <QtCore/QCoreApplication>
#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/Common/ComparisonInputs.h"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/FilterFactory.hpp"
#include "DREAM3DLib/Plugin/IDREAM3DPlugin.h"
#include "DREAM3DLib/Plugin/DREAM3DPluginLoader.h"
#include "DREAM3DLib/Utilities/UnitTestSupport.hpp"
#include "DREAM3DLib/Utilities/QMetaObjectUtilities.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestFilterAvailability()
{
  // Now instantiate the CalculateArrayHistogram Filter from the FilterManager
  QString filtName = "CalculateArrayHistogram";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL == filterFactory.get() )
  {
    std::stringstream ss;
    ss << "The CalculateArrayHistogramTest Requires the use of the " << filtName.toStdString() << " filter which is found in the IO Plugin";
    DREAM3D_TEST_THROW_EXCEPTION(ss.str())
  }
  return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CalculateArrayHistogramTest()
{ 
  const size_t arraySize = 10;
  int32_t test[] = { 1, 5, 3, 2, 4, 9, 1, 1, 6, 7 };
  DataContainerArray::Pointer dca = DataContainerArray::New();
  DataContainer::Pointer vdc = DataContainer::New("dc1");

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(DREAM3D::Geometry::ImageGeometry);
  vdc->setGeometry(image);
  size_t dims[1] = { arraySize };
  image->setDimensions(dims);

  QVector<size_t> tDims(1, 0);
  tDims[0] = arraySize;
  QVector<size_t> cDims(1);
  cDims[0] = 1;
  int inum = 0;
  AttributeMatrix::Pointer am = AttributeMatrix::New(tDims, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::AttributeMatrixType::Cell);
  DataArray<int32_t>::Pointer data1 = DataArray<int32_t>::CreateArray(tDims, cDims, "TestArrayInt");

  for (size_t i = 0; i <arraySize; i++)
  {
    data1->setValue(i, test[i]);
  }

  am->addAttributeArray(data1->getName(), data1); // int array
  vdc->addAttributeMatrix(am->getName(), am);
  dca->addDataContainer(vdc);

  // Now instantiate the CalculateArrayHistogram Filter from the FilterManager
  QString filtName = "CalculateArrayHistogram";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL != filterFactory.get())
  {
    // If we get this far, the Factory is good so creating the filter should not fail unless something has
    // horribly gone wrong in which case the system is going to come down quickly after this.
    AbstractFilter::Pointer filter = filterFactory->create();
    filter->setDataContainerArray(dca);

    QVariant var;
    QString str;
    bool propWasSet;
    DataArrayPath path;

    path = DataArrayPath(vdc->getName(), am->getName(), data1->getName());
    var.setValue(path);
    propWasSet = filter->setProperty("SelectedArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)
	
	  var.setValue(3);
    propWasSet = filter->setProperty("NumberOfBins", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)
	
	  var.setValue(false);
    propWasSet = filter->setProperty("Normalize", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    str = "AttributeMatrix1";
    QVariant qv(str);
    propWasSet = filter->setProperty("NewAttributeMatrixName", qv);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    str = "Hist";
    QVariant qv1(str);
    propWasSet = filter->setProperty("NewDataArrayName", qv1);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)
	
	  var.setValue(false);
    propWasSet = filter->setProperty("NewDataContainer", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)
	
    filter->execute();
    DREAM3D_REQUIRED(filter->getErrorCondition(), >= , 0);

    path = DataArrayPath("dc1", "AttributeMatrix1", "Hist");
    IDataArray::Pointer histArray = vdc->getAttributeMatrix(path.getAttributeMatrixName())->getAttributeArray(path.getDataArrayName());
    DataArray<double>* inputArray = DataArray<double>::SafePointerDownCast(histArray.get());
    double* inputArrayPtr = inputArray->getPointer(0);
 
    for (size_t i = 0; i < 3; i++)
    {
      double d = inputArrayPtr[i];
//      DREAM3D_REQUIRE_EQUAL(0, 1)
    }
  }
  else
  {
    QString ss = QObject::tr("CalculateArrayHistogramTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
    DREAM3D_REQUIRE_EQUAL(0, 1)
  }
  return 1;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loadFilterPlugins()
{
  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  DREAM3DPluginLoader::LoadPluginFilters(fm);

  // Send progress messages from PipelineBuilder to this object for display
  QMetaObjectUtilities::RegisterMetaTypes();
}


// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("CalculateArrayHistogramTest");

  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( loadFilterPlugins() );
  DREAM3D_REGISTER_TEST( TestFilterAvailability() );
  DREAM3D_REGISTER_TEST( CalculateArrayHistogramTest() )
  PRINT_TEST_SUMMARY();
  return err;
}

