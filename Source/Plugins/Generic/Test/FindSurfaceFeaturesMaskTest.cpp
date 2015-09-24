/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                   *
 * Copyright (c) 2015, William Lenthe                                                *
 * All right reserved.                                                               *
 *                                                                                   *
 * Redistribution and use in source and binary forms, with or without modification,  *
 * are permitted provided that the following conditions are met:                     *
 *                                                                                   *
 * 1. Redistributions of source code must retain the above copyright notice,         *
 *    this list of conditions and the following disclaimer.                          *
 *                                                                                   *
 * 2. Redistributions in binary form must reproduce the above copyright notice,      *
 *    this list of conditions and the following disclaimer in the documentation      *
 *    and/or other materials provided with the distribution.                         *
 *                                                                                   *
 * 3. Neither the name of the copyright holder nor the names of its contributors     *
 *    may be used to endorse or promote products derived from this software          *
 *    without specific prior written permission.                                     *
 *                                                                                   *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND   *
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED     *
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.*
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,  *
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,    *
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     *
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF   *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE   *
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED *
 * OF THE POSSIBILITY OF SUCH DAMAGE.                                                *
 *                                                                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestFilterAvailability()
{
	// Now instantiate the FindSurfaceFeaturesMaskTest Filter from the FilterManager
	QString filtName = "FindSurfaceFeaturesMask";
	FilterManager* fm = FilterManager::Instance();
	IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
	if (NULL == filterFactory.get())
	{
		std::stringstream ss;
		ss << "The FindSurfaceFeaturesMaskTest Requires the use of the " << filtName.toStdString() << " filter which is found in the DataFusion Plugin";
		DREAM3D_TEST_THROW_EXCEPTION(ss.str())
	}
	return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindSurfaceFeaturesMaskTest()
{
  //test procedure:
  // -create grid of rectangular grains with an outer shell of 0 padding
  // -set mask to be true everywhere except for z = 0
  // -check that grains touching bottom layer are flagged

  static const size_t fX = 6, fY = 5, fZ = 4;//synthetic dataset dimensions in features (excluding padding)
  static const size_t sX = 3, sY = 2, sZ = 1;//size of features in voxels

  //voxel dimensions excluding 0 pad
  static const size_t vX = fX * sX;
  static const size_t vY = fY * sY;
  static const size_t vZ = fZ * sZ;

  //compute correct output
  QVector<size_t> fDims(1, fX * fY * fZ + 1);
  bool surface[fX * fY * fZ + 1] = {false};
  for(size_t i = 0; i <= fX * fY; i++) {
    surface[i] = true;
  }

  //create array to hold feature ids
  QVector<size_t> tDims(3);
  tDims[0] = vX + 2;
  tDims[1] = vY + 2;
  tDims[2] = vZ + 2;
  QVector<size_t> cDims(1, 1);
  DataArray<int32_t>::Pointer pIds = DataArray<int32_t>::CreateArray(tDims, cDims, "FeatureIds");
  pIds->initializeWithValue(0);
  int32_t* ids = pIds->getPointer(0);

  //fill feature ids
  int32_t index = 0;
  for(size_t k = 0; k < vZ; k += sZ) {
    size_t zOffset = (k + 1) * (vX + 2) * (vY + 2);
    for(size_t j = 0; j < vY; j += sY) {
      size_t yOffset = (j + 1) * (vX + 2);
      for(size_t i = 0; i < vX; i += sX) {
        //seed each feature in bottom corner and expand across x
        size_t offset = zOffset + yOffset + + i + 1;
        ids[offset] = ++index;
        for(size_t m = 0; m < sX; m++) {
          ids[offset + m] = ids[offset];
        }
      }
      //expand features across y by copying rows
      size_t offset = zOffset + yOffset;
      for(size_t m = 1; m < sY; m++) {
        std::memcpy(&ids[offset + m * (vX + 2)], &ids[offset], (vX + 2) * sizeof(int32_t));
      }
    }
    //expand features across z by copying slices
    for(size_t m = 0; m < sZ; m++) {
      std::memcpy(&ids[zOffset + m * (vX + 2) * (vY + 2)], &ids[zOffset], (vX + 2) * (vY + 2) * sizeof(int32_t));
    }
  }

  //create array to hold mask
  DataArray<bool>::Pointer pMask = DataArray<bool>::CreateArray(tDims, cDims, "Mask");
  pMask->initializeWithValue(true);
  bool* mask = pMask->getPointer(0);
  for(size_t i = 0; i < (vX + 2) * (vY + 2); i++) {
    mask[i] = false;
  }

  //create attribute matrix to hold feature ids and fill
  AttributeMatrix::Pointer cellAm = AttributeMatrix::New(tDims, "CellData", DREAM3D::AttributeMatrixType::Cell);
  cellAm->addAttributeArray(pIds->getName(), pIds);
  cellAm->addAttributeArray(pMask->getName(), pMask);

  //create cell feature attribute matrix
  AttributeMatrix::Pointer featAm = AttributeMatrix::New(fDims, "CellFeatureData", DREAM3D::AttributeMatrixType::Cell);

  //create data container and fill
  ImageGeom::Pointer image = ImageGeom::CreateGeometry(DREAM3D::Geometry::ImageGeometry);
  image->setDimensions(tDims.data());
  DataContainer::Pointer dc = DataContainer::New("dc");
  dc->setGeometry(image);
  dc->addAttributeMatrix(cellAm->getName(), cellAm);
  dc->addAttributeMatrix(featAm->getName(), featAm);

  DataContainerArray::Pointer dca = DataContainerArray::New();
  dca->addDataContainer(dc);

  //create filter, execute, and check output
  QString filtName = "FindSurfaceFeaturesMask";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if(NULL != filterFactory.get())
  {
    //create filter and set parameters
    AbstractFilter::Pointer filter = filterFactory->create();
    filter->setDataContainerArray(dca);

    QVariant var;
    bool propWasSet;
    DataArrayPath path;
    QString outputName("SurfaceFeature");

    path.update(dc->getName(), cellAm->getName(), pIds->getName());
    var.setValue(path);
    propWasSet = filter->setProperty("FeatureIdsArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    path.update(dc->getName(), cellAm->getName(), pMask->getName());
    var.setValue(path);
    propWasSet = filter->setProperty("MaskArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    path.update(dc->getName(), featAm->getName(), outputName);
    var.setValue(path);
    propWasSet = filter->setProperty("SurfaceFeaturesArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    //execute filter and check output
    filter->execute();
    DREAM3D_REQUIRED(filter->getErrorCondition(), >= , 0);

    //get computed transformations and compare to applied transformations
    IDataArray::Pointer iSurfaceFeatures = dc->getAttributeMatrix(featAm->getName())->getAttributeArray(outputName);
    DataArray<bool>* pSurfaceFeatures = DataArray<bool>::SafePointerDownCast(iSurfaceFeatures.get());
    DREAM3D_REQUIRE_VALID_POINTER(pSurfaceFeatures)
    bool* surfaceFeatures = pSurfaceFeatures->getPointer(0);

    for(size_t i = 0; i < pSurfaceFeatures->getNumberOfTuples(); i++) {
      DREAM3D_REQUIRE_EQUAL(surfaceFeatures[i], surface[i])
    }
  }
  else
  {
    QString ss = QObject::tr("FindSurfaceFeaturesMaskTest Error creating filter '%1'. Filter was not created/executed. Please notivY the developers.").arg(filtName);
    DREAM3D_TEST_THROW_EXCEPTION(ss.toStdString())
  }

  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loadFilterPlugins()
{
  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  SIMPLibPluginLoader::LoadPluginFilters(fm);

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
  QCoreApplication::setOrganizationName("");
  QCoreApplication::setOrganizationDomain("");
  QCoreApplication::setApplicationName("FindSurfaceFeaturesMaskTest");

  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( loadFilterPlugins() );
  DREAM3D_REGISTER_TEST( TestFilterAvailability() );

  DREAM3D_REGISTER_TEST( FindSurfaceFeaturesMaskTest() )

  PRINT_TEST_SUMMARY();
  return err;
}
