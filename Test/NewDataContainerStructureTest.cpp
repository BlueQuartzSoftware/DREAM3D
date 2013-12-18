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


#include <stdlib.h>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QVector>

#include "DREAM3DLib/DREAM3DLib.h"

#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/DataContainers/DataContainerArray.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

#include "DREAM3DLib/SyntheticBuildingFilters/InitializeSyntheticVolume.h"
#include "DREAM3DLib/SyntheticBuildingFilters/PackPrimaryPhases.h"
#include "DREAM3DLib/StatisticsFilters/FindNeighbors.h"
#include "DREAM3DLib/StatisticsFilters/FindNumFeatures.h"
#include "DREAM3DLib/SyntheticBuildingFilters/MatchCrystallography.h"
#include "DREAM3DLib/GenericFilters/GenerateIPFColors.h"
#include "DREAM3DLib/IOFilters/DataContainerWriter.h"

#include "DREAM3DLib/IOFilters/DataContainerReader.h"
#include "DREAM3DLib/SurfaceMeshingFilters/QuickSurfaceMesh.h"

#include "DREAM3DLib/SurfaceMeshingFilters/LaplacianSmoothing.h"

#include "DREAM3DLib/IOFilters/VASPReader.h"

#include "DREAM3DLib/IOFilters/ParaDisReader.h"

#include"DREAM3DLib/SamplingFilters/RegularGridSampleSurfaceMesh.h"

#include"DREAM3DLib/GenericFilters/FindSurfaceFeatures.h"
#include"DREAM3DLib/GenericFilters/FindBoundingBoxFeatures.h"
#include"DREAM3DLib/GenericFilters/FindFeatureCentroids.h"
#include"DREAM3DLib/StatisticsFilters/FindSizes.h"
#include"DREAM3DLib/StatisticsFilters/FindNeighbors.h"
#include"DREAM3DLib/StatisticsFilters/FitFeatureData.h"
#include"DREAM3DLib/StatisticsFilters/FitCorrelatedFeatureData.h"

#include "UnitTestSupport.hpp"

#include "TestFileLocations.h"


void RunPipeline1()
{
  Observer obs;
  // Send progress messages from PipelineBuilder to this object for display
  qRegisterMetaType<PipelineMessage>();

  // Create our Pipeline object
  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  pipeline->addMessageReceiver(&obs);

  InitializeSyntheticVolume::Pointer isv = InitializeSyntheticVolume::New();
  isv->setInputFile(UnitTest::NewDataContainerStructureTest::SyntheticInputFile);
  isv->setXVoxels(64);
  isv->setYVoxels(64);
  isv->setZVoxels(64);
  isv->setXRes(0.1);
  isv->setYRes(0.1);
  isv->setZRes(0.1);
  QVector<uint> sTypes;
  sTypes.push_back(DREAM3D::ShapeType::UnknownShapeType);
  sTypes.push_back(DREAM3D::ShapeType::EllipsoidShape);
  isv->setShapeTypes(sTypes);
  pipeline->pushBack(isv);

  PackPrimaryPhases::Pointer ppp = PackPrimaryPhases::New();
  ppp->setPeriodicBoundaries(false);
  ppp->setWriteGoalAttributes(false);
  pipeline->pushBack(ppp);

  FindNeighbors::Pointer fn = FindNeighbors::New();
  pipeline->pushBack(fn);

  FindNumFeatures::Pointer fnf = FindNumFeatures::New();
  pipeline->pushBack(fnf);

  MatchCrystallography::Pointer mc = MatchCrystallography::New();
  mc->setMaxIterations(100000);
  pipeline->pushBack(mc);

  GenerateIPFColors::Pointer gipfc = GenerateIPFColors::New();
  pipeline->pushBack(gipfc);

  DataContainerWriter::Pointer dcw = DataContainerWriter::New();
  dcw->setOutputFile(UnitTest::NewDataContainerStructureTest::SyntheticOutputFile);
  dcw->setWriteXdmfFile(true);
  pipeline->pushBack(dcw);

  int err = pipeline->preflightPipeline();
  if(err < 0)
  {
    std::cout << "Failed Preflight" << std::endl;
  }
  pipeline->run();
}

//void RunPipeline2()
//{
//  FilterPipeline::Pointer pipeline = FilterPipeline::New();
//
//  DataContainerReader::Pointer dcr = DataContainerReader::New();
//  dcr->setInputFile(UnitTest::NewDataContainerStructureTest::SyntheticOutputFile);
//  dcr->setReadVertexData(false);
//  dcr->setReadEdgeData(false);
//  dcr->setReadSurfaceData(false);
//  dcr->setReadVolumeData(true);
//  dcr->setReadAllVertexArrays(true);
//  dcr->setReadAllCellArrays(true);
//  dcr->setReadAllCellFeatureArrays(true);
//  dcr->setReadAllCellEnsembleArrays(true);
//  pipeline->pushBack(dcr);
//
//  QuickSurfaceMesh::Pointer qsm = QuickSurfaceMesh::New();
//  pipeline->pushBack(qsm);
//
//  DataContainerWriter::Pointer dcw = DataContainerWriter::New();
//  dcw->setOutputFile(UnitTest::NewDataContainerStructureTest::SyntheticOutputFile2);
//  dcw->setWriteVolumeData(true);
//  dcw->setWriteSurfaceData(true);
//  dcw->setWriteEdgeData(false);
//  dcw->setWriteVertexData(false);
//  dcw->setWriteXdmfFile(true);
//  pipeline->pushBack(dcw);
//
//  int err = pipeline->preflightPipeline();
//  if(err < 0)
//  {
//    std::cout << "Failed Preflight" << std::endl;
//  }
//  pipeline->run();
//}
//
//void RunPipeline3()
//{
//  FilterPipeline::Pointer pipeline = FilterPipeline::New();
//
//  DataContainerReader::Pointer dcr = DataContainerReader::New();
//  dcr->setInputFile(UnitTest::NewDataContainerStructureTest::SyntheticOutputFile2);
//  dcr->setReadVertexData(false);
//  dcr->setReadEdgeData(false);
//  dcr->setReadSurfaceData(true);
//  dcr->setReadAllVertexArrays(true);
//  dcr->setReadAllFaceArrays(true);
//  dcr->setReadVolumeData(true);
//  dcr->setReadAllCellArrays(true);
//  dcr->setReadAllCellFeatureArrays(true);
//  dcr->setReadAllCellEnsembleArrays(true);
//  pipeline->pushBack(dcr);
//
//  LaplacianSmoothing::Pointer ls = LaplacianSmoothing::New();
//  ls->setIterationSteps(50);
//  ls->setLambda(0.25);
//  ls->setTripleLineLambda(0.2);
//  ls->setQuadPointLambda(0.15);
//  ls->setSurfacePointLambda(0.0);
//  ls->setSurfaceTripleLineLambda(0.0);
//  ls->setSurfaceQuadPointLambda(0.0);
//  pipeline->pushBack(ls);
//
//  DataContainerWriter::Pointer dcw = DataContainerWriter::New();
//  dcw->setOutputFile(UnitTest::NewDataContainerStructureTest::SyntheticOutputFile3);
//  dcw->setWriteVolumeData(true);
//  dcw->setWriteSurfaceData(true);
//  dcw->setWriteEdgeData(false);
//  dcw->setWriteVertexData(false);
//  dcw->setWriteXdmfFile(true);
//  pipeline->pushBack(dcw);
//
//  int err = pipeline->preflightPipeline();
//  if(err < 0)
//  {
//    std::cout << "Failed Preflight" << std::endl;
//  }
//  pipeline->run();
//}
//
//void RunPipeline4()
//{
//  FilterPipeline::Pointer pipeline = FilterPipeline::New();
//
//  VASPReader::Pointer vr = VASPReader::New();
//  vr->setInputFile(UnitTest::NewDataContainerStructureTest::VASPInputFile);
//  pipeline->pushBack(vr);
//
//  DataContainerWriter::Pointer dcw = DataContainerWriter::New();
//  dcw->setOutputFile(UnitTest::NewDataContainerStructureTest::VASPOutputFile);
//  dcw->setWriteVolumeData(false);
//  dcw->setWriteSurfaceData(false);
//  dcw->setWriteEdgeData(false);
//  dcw->setWriteVertexData(true);
//  dcw->setWriteXdmfFile(true);
//  pipeline->pushBack(dcw);
//
//  int err = pipeline->preflightPipeline();
//  if(err < 0)
//  {
//    std::cout << "Failed Preflight" << std::endl;
//  }
//  pipeline->run();
//}
//
//void RunPipeline5()
//{
//  FilterPipeline::Pointer pipeline = FilterPipeline::New();
//
//  ParaDisReader::Pointer pr = ParaDisReader::New();
//  pr->setInputFile(UnitTest::NewDataContainerStructureTest::ParaDisInputFile);
//  pipeline->pushBack(pr);
//
//  DataContainerWriter::Pointer dcw = DataContainerWriter::New();
//  dcw->setOutputFile(UnitTest::NewDataContainerStructureTest::ParaDisOutputFile);
//  dcw->setWriteVolumeData(false);
//  dcw->setWriteSurfaceData(false);
//  dcw->setWriteEdgeData(true);
//  dcw->setWriteVertexData(false);
//  dcw->setWriteXdmfFile(true);
//  pipeline->pushBack(dcw);
//
//  int err = pipeline->preflightPipeline();
//  if(err < 0)
//  {
//    std::cout << "Failed Preflight" << std::endl;
//  }
//  pipeline->run();
//}
//
//void RunPipeline6()
//{
//  FilterPipeline::Pointer pipeline = FilterPipeline::New();
//
//  DataContainerReader::Pointer dcr = DataContainerReader::New();
//  dcr->setInputFile(UnitTest::NewDataContainerStructureTest::ParaDisInputFile2);
//  dcr->setReadVertexData(false);
//  dcr->setReadEdgeData(true);
//  dcr->setReadSurfaceData(false);
//  dcr->setReadVolumeData(false);
//  dcr->setReadAllVertexArrays(true);
//  dcr->setReadAllEdgeArrays(true);
//  pipeline->pushBack(dcr);
//
//  DataContainerWriter::Pointer dcw = DataContainerWriter::New();
//  dcw->setOutputFile(UnitTest::NewDataContainerStructureTest::ParaDisOutputFile2);
//  dcw->setWriteVolumeData(false);
//  dcw->setWriteSurfaceData(false);
//  dcw->setWriteEdgeData(true);
//  dcw->setWriteVertexData(false);
//  dcw->setWriteXdmfFile(true);
//  pipeline->pushBack(dcw);
//
//  int err = pipeline->preflightPipeline();
//  if(err < 0)
//  {
//    std::cout << "Failed Preflight" << std::endl;
//  }
//  pipeline->run();
//}
//
//void RunPipeline7()
//{
//  FilterPipeline::Pointer pipeline = FilterPipeline::New();
//
//  DataContainerReader::Pointer dcr = DataContainerReader::New();
//  dcr->setInputFile(UnitTest::NewDataContainerStructureTest::SyntheticOutputFile3);
//  dcr->setReadVertexData(false);
//  dcr->setReadEdgeData(false);
//  dcr->setReadSurfaceData(true);
//  dcr->setReadAllVertexArrays(true);
//  dcr->setReadAllFaceArrays(true);
//  dcr->setReadVolumeData(false);
//  dcr->setReadAllCellArrays(false);
//  dcr->setReadAllCellFeatureArrays(false);
//  dcr->setReadAllCellEnsembleArrays(false);
//  pipeline->pushBack(dcr);
//
//  RegularGridSampleSurfaceMesh::Pointer rgssm = RegularGridSampleSurfaceMesh::New();
//  rgssm->setXPoints(256);
//  rgssm->setYPoints(256);
//  rgssm->setZPoints(256);
//  FloatVec3Widget_t res = {0.05,0.05,0.05};
//  rgssm->setResolution(res);
//  pipeline->pushBack(rgssm);
//
//  DataContainerWriter::Pointer dcw = DataContainerWriter::New();
//  dcw->setOutputFile(UnitTest::NewDataContainerStructureTest::SyntheticOutputFile4);
//  dcw->setWriteVolumeData(true);
//  dcw->setWriteSurfaceData(true);
//  dcw->setWriteEdgeData(false);
//  dcw->setWriteVertexData(false);
//  dcw->setWriteXdmfFile(true);
//  pipeline->pushBack(dcw);
//
//  int err = pipeline->preflightPipeline();
//  if(err < 0)
//  {
//    std::cout << "Failed Preflight" << std::endl;
//  }
//  pipeline->run();
//}
//
//void RunPipeline8()
//{
//  FilterPipeline::Pointer pipeline = FilterPipeline::New();
//
//  DataContainerReader::Pointer dcr = DataContainerReader::New();
//  dcr->setInputFile(UnitTest::NewDataContainerStructureTest::SmallIN100InputFile);
//  dcr->setReadVertexData(false);
//  dcr->setReadEdgeData(false);
//  dcr->setReadSurfaceData(false);
//  dcr->setReadVolumeData(true);
//  dcr->setReadAllCellArrays(true);
//  dcr->setReadAllCellFeatureArrays(true);
//  dcr->setReadAllCellEnsembleArrays(true);
//  pipeline->pushBack(dcr);
//
//  FindSurfaceFeatures::Pointer fsg = FindSurfaceFeatures::New();
//  pipeline->pushBack(fsg);
//
//  FindFeatureCentroids::Pointer fgc = FindFeatureCentroids::New();
//  pipeline->pushBack(fgc);
//
//  FindBoundingBoxFeatures::Pointer fbbg = FindBoundingBoxFeatures::New();
//  pipeline->pushBack(fbbg);
//
//  FindSizes::Pointer fs = FindSizes::New();
//  pipeline->pushBack(fs);
//
//  FindNeighbors::Pointer fn = FindNeighbors::New();
//  pipeline->pushBack(fn);
//
//  FitCorrelatedFeatureData::Pointer fcfd = FitCorrelatedFeatureData::New();
//  fcfd->setSelectedFeatureArrayName(DREAM3D::FeatureData::NumNeighbors);
//  fcfd->setCorrelatedFeatureArrayName(DREAM3D::FeatureData::EquivalentDiameters);
//  fcfd->setDistributionType(DREAM3D::DistributionType::LogNormal);
//  fcfd->setNumberOfCorrelatedBins(10);
//  fcfd->setRemoveBiasedFeatures(true);
//  pipeline->pushBack(fcfd);
//
//  DataContainerWriter::Pointer dcw = DataContainerWriter::New();
//  dcw->setOutputFile(UnitTest::NewDataContainerStructureTest::SmallIN100OutputFile);
//  dcw->setWriteVolumeData(true);
//  dcw->setWriteSurfaceData(false);
//  dcw->setWriteEdgeData(false);
//  dcw->setWriteVertexData(false);
//  dcw->setWriteXdmfFile(true);
//  pipeline->pushBack(dcw);
//
//  int err = pipeline->preflightPipeline();
//  if(err < 0)
//  {
//    std::cout << "Failed Preflight" << std::endl;
//  }
//  pipeline->run();
//
//  err = pipeline->getErrorCondition();
//  if(err < 0)
//  {
//    std::cout << "Pipeline Execute Failed" << std::endl;
//  }
//  DREAM3D_REQUIRE(err >= 0)
//
//}

// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  int err = EXIT_SUCCESS;

  RunPipeline1();
//  RunPipeline2();
//  RunPipeline3();
//  RunPipeline4();
//  RunPipeline5();
//  RunPipeline6();
//  RunPipeline7();
//  RunPipeline8();

  return err;
}

