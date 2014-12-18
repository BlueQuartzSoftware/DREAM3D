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

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/FilterFactory.hpp"
#include "DREAM3DLib/Plugin/DREAM3DPluginInterface.h"
#include "DREAM3DLib/Plugin/DREAM3DPluginLoader.h"
#include "DREAM3DLib/Utilities/UnitTestSupport.hpp"
#include "DREAM3DLib/Utilities/QMetaObjectUtilities.h"

#include "GenerateFeatureIds.h"

static const int originalXMax = 40;
static const int originalYMax = 30;
static const int originalZMax = 20;

static const double originalXRes = 0.25;
static const double originalYRes = 0.25;
static const double originalZRes = 0.25;

static const int originalXOrigin = 0;
static const int originalYOrigin = 0;
static const int originalZOrigin = 0;

static const int XMax = 25;
static const int YMax = 18;
static const int ZMax = 7;

static const int XMin = 9;
static const int YMin = 5;
static const int ZMin = 2;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::Pointer CreateDataContainerArrayTestStructure()
{
  int err = 0;
  DataContainerArray::Pointer dca = DataContainerArray::New();

  VolumeDataContainer::Pointer dc1 = VolumeDataContainer::New("Data Container");
  dc1->setDimensions(originalXMax, originalYMax, originalZMax);
  dc1->setOrigin(originalXOrigin, originalYOrigin, originalZOrigin);
  dc1->setResolution(originalXRes, originalYRes, originalZRes);

  QVector<size_t> amDims;
  amDims.push_back(originalXMax);
  amDims.push_back(originalYMax);
  amDims.push_back(originalZMax);
  AttributeMatrix::Pointer am1 = AttributeMatrix::New(amDims, "Attribute Matrix", DREAM3D::AttributeMatrixType::Cell);
  am1->getNumTuples();

  QVector<size_t> tDims;
  tDims.push_back(originalXMax);
  tDims.push_back(originalYMax);
  tDims.push_back(originalZMax);
  QVector<size_t> cDims(1, 1);

  DataArray<float>::Pointer ConfidenceIndexArray = DataArray<float>::CreateArray(tDims, cDims, "Confidence Index", true);
  err = am1->addAttributeArray("Confidence Index", ConfidenceIndexArray);
  DREAM3D_REQUIRE(err >= 0);

  DataArray<float>::Pointer FeatureIdsArray = DataArray<float>::CreateArray(tDims, cDims, "FeatureIds", true);
  err = am1->addAttributeArray("FeatureIds", FeatureIdsArray);
  DREAM3D_REQUIRE(err >= 0);

  int64_t XP = tDims[0];
  int64_t YP = tDims[1];
  int64_t ZP = tDims[2];

  int64_t col, row, plane;
  int64_t index;
  QList<QString> voxelArrayNames = am1->getAttributeArrayNameList();
  for (int64_t i = 0; i < ZP; i++)
  {
    plane = i * XP * YP;
    for (int64_t j = 0; j < YP; j++)
    {
      row = j * XP;
      for (int64_t k = 0; k < XP; k++)
      {
        col = k;
        index = plane + row + col;
        for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
        {
          IDataArray::Pointer p = am1->getAttributeArray(*iter);
          p->initializeTuple(index, i+j+k);
        }
      }
    }
  }

  foreach (QString da, am1->getAttributeArrayNameList())
  {
    DREAM3D_REQUIRE_EQUAL(am1->getAttributeArray(da)->getSize(), XP*YP*ZP);
  }

  dc1->addAttributeMatrix(am1->getName(), am1);

  dca->addDataContainer(dc1);

  return dca;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestCropVolume()
{
  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  int err = 0;

  // Now instantiate the CropVolume Filter from the FilterManager
  QString filtName = "CropVolume";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL != filterFactory.get() )
  {
    // If we get this far, the Factory is good so creating the filter should not fail unless something has
    // horribly gone wrong in which case the system is going to come down quickly after this.
    DataContainerArray::Pointer dca = CreateDataContainerArrayTestStructure();

    AbstractFilter::Pointer cropVolume = filterFactory->create();
    cropVolume->setDataContainerArray(dca);

    QVariant var;
    var.setValue(XMax);
    bool propWasSet = cropVolume->setProperty("XMax", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(YMax);
    propWasSet = cropVolume->setProperty("YMax", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(ZMax);
    propWasSet = cropVolume->setProperty("ZMax", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(XMin);
    propWasSet = cropVolume->setProperty("XMin", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(YMin);
    propWasSet = cropVolume->setProperty("YMin", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(ZMin);
    propWasSet = cropVolume->setProperty("ZMin", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(false);
    propWasSet = cropVolume->setProperty("SaveAsNewDataContainer", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    pipeline->pushBack(cropVolume);
  }
  else
  {
    QString ss = QObject::tr("CropVolumeTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
    DREAM3D_REQUIRE_EQUAL(0, 1)
  }

  return 0;
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
  QCoreApplication::setApplicationName("CropVolumeTest");

  int err = EXIT_SUCCESS;

  DREAM3D_REGISTER_TEST( loadFilterPlugins() );

  DREAM3D_REGISTER_TEST( TestCropVolume() )

  PRINT_TEST_SUMMARY();
  return err;
}





