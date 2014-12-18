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

#include "DREAM3DLib/Plugin/DREAM3DPluginInterface.h"
#include "DREAM3DLib/Plugin/DREAM3DPluginLoader.h"
#include "DREAM3DLib/Utilities/UnitTestSupport.hpp"
#include "DREAM3DLib/DataContainers/DataContainerArray.h"
#include "DREAM3DLib/DataContainers/DataContainer.h"
#include "DREAM3DLib/DataContainers/AttributeMatrix.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "Plugins/Sampling/SamplingFilters/CropVolume.h"

#include "TestFileLocations.h"

#include "GenerateFeatureIds.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::Pointer CreateDataContainerArrayTestStructure()
{
  DataContainerArray::Pointer dca = DataContainerArray::New();

  VolumeDataContainer::Pointer dc1 = VolumeDataContainer::New("Data Container");
  dc1->setDimensions(40, 30, 20);
  dc1->setOrigin(0, 0, 0);
  dc1->setResolution(0.25, 0.25, 0.25);

  QVector<size_t> amDims;
  amDims.push_back(40);
  amDims.push_back(30);
  amDims.push_back(20);
  AttributeMatrix::Pointer am1 = AttributeMatrix::New(amDims, "Attribute Matrix", DREAM3D::AttributeMatrixType::Cell);
  dc1->addAttributeMatrix(am1->getName(), am1);

  //IDataArray::Pointer da1 = am1->createAndAddAttributeArray()

  dca->addDataContainer(dc1);

  return dca;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestCropVolume()
{
  DataContainerArray::Pointer dca = CreateDataContainerArrayTestStructure();

//  CropVolume::Pointer cropFilter = CropVolume::New();
//  cropFilter->setDataContainerArray(dca);
//  cropFilter->setXMax(20);
//  cropFilter->setXMin(10);
//  cropFilter->setYMax(28);

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

  DREAM3D_REGISTER_TEST( TestCropVolume() )

  PRINT_TEST_SUMMARY();
  return err;
}





