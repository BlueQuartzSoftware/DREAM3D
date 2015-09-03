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



#include <stdlib.h>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QVector>

#include "SIMPLib/SIMPLib.h"
#include "EbsdLib/EbsdConstants.h"

// DREAM3D Lib
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataArrays/NeighborList.hpp"
#include "SIMPLib/DataContainers/DataContainerArray.h"
// DREAM3DLib Filters
#include "SIMPLib/GenericFilters/FindBoundingBoxFeatures.h"
#include "SIMPLib/GenericFilters/FindFeatureCentroids.h"
#include "SIMPLib/GenericFilters/FindSurfaceFeatures.h"
#include "SIMPLib/GenericFilters/GenerateIPFColors.h"
#include "SIMPLib/IOFilters/DataContainerReader.h"
#include "SIMPLib/IOFilters/DataContainerWriter.h"
#include "SIMPLib/IOFilters/ParaDisReader.h"
#include "SIMPLib/IOFilters/VASPReader.h"

#include "SIMPLib/SyntheticBuildingFilters/InitializeSyntheticVolume.h"
#include "SIMPLib/SyntheticBuildingFilters/MatchCrystallography.h"
#include "SIMPLib/SyntheticBuildingFilters/PackPrimaryPhases.h"

// Sampling Plugin
//#include "Sampling/SamplingFilters/ChangeResolution.h"
//#include "Sampling/SamplingFilters/RegularGridSampleSurfaceMesh.h"

// Statistics Plugin
//#include "Statistics/StatisticsFilters/FindNeighbors.h"
//#include "Statistics/StatisticsFilters/FindNeighbors.h"
//#include "Statistics/StatisticsFilters/FindNumFeatures.h"
//#include "Statistics/StatisticsFilters/FindSizes.h"
//#include "Statistics/StatisticsFilters/FitCorrelatedFeatureData.h"
//#include "Statistics/StatisticsFilters/FitFeatureData.h"

// SurfaceMeshing Plugin
//#include "SurfaceMeshing/SurfaceMeshingFilters/LaplacianSmoothing.h"
//#include "SurfaceMeshing/SurfaceMeshingFilters/QuickSurfaceMesh.h"


#include "UnitTestSupport.hpp"

#include "TestFileLocations.h"



void BuildNewDream3dFile()
{
  DataContainerArray::Pointer dca = DataContainerArray::New();
  VolumeDataContainer::Pointer vdc = VolumeDataContainer::New();
  dca->pushBack(vdc);
  QVector<size_t> tDims(3, 0);
  tDims[0] = 128;
  tDims[1] = 128;
  tDims[2] = 128;
  AttributeMatrix::Pointer cellAttrMat = AttributeMatrix::New(tDims, "CellData", DREAM3D::AttributeMatrixType::Cell);
  vdc->addAttributeMatrix("CellData", cellAttrMat);
  QVector<size_t> dims(1, 1);
  cellAttrMat->createAndAddAttributeArray<DataArray<int64_t>, int64_t>("Ids", 1, dims);

  DataArray<int64_t>::WeakPointer m_FeatureIdsPtr;
  int64_t* m_FeatureIds;
  AbstractFilter::Pointer filt = AbstractFilter::New();
  m_FeatureIdsPtr = cellAttrMat->getPrereqArray<DataArray<int64_t>, AbstractFilter>(filt.get(), "Ids", -300, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  for(int i = 0; i < (128 * 128 * 128); i++)
  {
    m_FeatureIds[i] = i;
  }
  AttributeMatrix::Pointer cellFeatureAttrMat = AttributeMatrix::New(tDims, "CellFeatureData", DREAM3D::AttributeMatrixType::CellFeature);
  vdc->addAttributeMatrix("CellFeatureData", cellFeatureAttrMat);
  tDims.resize(1);
  tDims[0] = 2;
  AttributeMatrix::Pointer cellEnsembleAttrMat = AttributeMatrix::New(tDims, "CellEnsembleData", DREAM3D::AttributeMatrixType::CellEnsemble);
  vdc->addAttributeMatrix("CellEnsembleData", cellEnsembleAttrMat);

  cellEnsembleAttrMat->createAndAddAttributeArray<DataArray<unsigned int>, unsigned int>("PhaseTypes", DREAM3D::PhaseType::PrimaryPhase, dims);
  DataArray<unsigned int>::WeakPointer m_PhaseTypesPtr;
  unsigned int* m_PhaseTypes;
  m_PhaseTypesPtr = cellEnsembleAttrMat->getPrereqArray<DataArray<unsigned int>, AbstractFilter>(filt.get(), "PhaseTypes", -300, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_PhaseTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
    m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0);  /* Now assign the raw pointer to data from the DataArray<T> object */
    m_PhaseTypes[0] = DREAM3D::PhaseType::UnknownPhaseType;
  }

  cellEnsembleAttrMat->createAndAddAttributeArray<DataArray<unsigned int>, unsigned int>("ShapeTypes", DREAM3D::ShapeType::EllipsoidShape, dims);
  DataArray<unsigned int>::WeakPointer m_ShapeTypesPtr;
  unsigned int* m_ShapeTypes;
  m_ShapeTypesPtr = cellEnsembleAttrMat->getPrereqArray<DataArray<unsigned int>, AbstractFilter>(filt.get(), "ShapeTypes", -300, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ShapeTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
    m_ShapeTypes = m_ShapeTypesPtr.lock()->getPointer(0);  /* Now assign the raw pointer to data from the DataArray<T> object */
    m_ShapeTypes[0] = DREAM3D::ShapeType::UnknownShapeType;
  }

  cellEnsembleAttrMat->createAndAddAttributeArray<DataArray<unsigned int>, unsigned int>("CrystalStructures", Ebsd::CrystalStructure::Cubic_High, dims);
  DataArray<unsigned int>::WeakPointer m_CrystalStructuresPtr;
  unsigned int* m_CrystalStructures;
  m_CrystalStructuresPtr = cellEnsembleAttrMat->getPrereqArray<DataArray<unsigned int>, AbstractFilter>(filt.get(), "CrystalStructures", -300, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); /* Now assign the raw pointer to data from the DataArray<T> object */
    m_CrystalStructures[0] = Ebsd::CrystalStructure::UnknownCrystalStructure;
  }

  StatsDataArray* m_StatsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(cellEnsembleAttrMat->getAttributeArray(DREAM3D::EnsembleData::Statistics).get());
  if(m_StatsDataArray == NULL)
  {
    StatsDataArray::Pointer p = StatsDataArray::New();
    m_StatsDataArray = p.get();
    m_StatsDataArray->fillArrayWithNewStatsData(cellEnsembleAttrMat->getNumTuples(), m_PhaseTypes);
    cellEnsembleAttrMat->addAttributeArray(DREAM3D::EnsembleData::Statistics, p);
  }

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[1].get());
  pp->setPhaseFraction(1);
  VectorOfFloatArray sizedist = statsDataArray[1]->CreateCorrelatedDistributionArrays(DREAM3D::DistributionType::LogNormal, 1);
  sizedist[0]->setValue(0, 1);
  sizedist[1]->setValue(0, 0.1);
  pp->setFeatureSizeDistribution(sizedist);
  float maxdiam = 4.48169;
  float mindiam = 1.64872;
  float binSize = 0.5;
  int numbins = int(((maxdiam - mindiam) / binSize) + 1);
  pp->setFeatureDiameterInfo(binSize, maxdiam, mindiam);
  FloatArrayType::Pointer binnumbers = FloatArrayType::CreateArray(numbins, DREAM3D::StringConstants::BinNumber);
  DistributionAnalysisOps::determinebinnumbers(maxdiam, mindiam, binSize, binnumbers);
  pp->setBinNumbers(binnumbers);
  VectorOfFloatArray boveras = statsDataArray[1]->CreateCorrelatedDistributionArrays(DREAM3D::DistributionType::Beta, numbins);
  VectorOfFloatArray coveras = statsDataArray[1]->CreateCorrelatedDistributionArrays(DREAM3D::DistributionType::Beta, numbins);
  boveras[0]->setValue(0, 15.0425);
  boveras[1]->setValue(0, 1.26719);
  boveras[0]->setValue(1, 15.9438);
  boveras[1]->setValue(1, 1.44586);
  boveras[0]->setValue(2, 15.4206);
  boveras[1]->setValue(2, 1.6027);
  boveras[0]->setValue(3, 15.3367);
  boveras[1]->setValue(3, 1.47998);
  boveras[0]->setValue(4, 15.7276);
  boveras[1]->setValue(4, 1.64522);
  boveras[0]->setValue(5, 15.9084);
  boveras[1]->setValue(5, 1.71922);
  pp->setFeatureSize_BOverA(boveras);
  coveras[0]->setValue(0, 15.0425);
  coveras[1]->setValue(0, 1.26719);
  coveras[0]->setValue(1, 15.9438);
  coveras[1]->setValue(1, 1.44586);
  coveras[0]->setValue(2, 15.4206);
  coveras[1]->setValue(2, 1.6027);
  coveras[0]->setValue(3, 15.3367);
  coveras[1]->setValue(3, 1.47998);
  coveras[0]->setValue(4, 15.7276);
  coveras[1]->setValue(4, 1.64522);
  coveras[0]->setValue(5, 15.9084);
  coveras[1]->setValue(5, 1.71922);
  pp->setFeatureSize_COverA(coveras);
  VectorOfFloatArray omega3s = statsDataArray[1]->CreateCorrelatedDistributionArrays(DREAM3D::DistributionType::Beta, numbins);
  omega3s[0]->setValue(0, 15.0425);
  omega3s[1]->setValue(0, 1.26719);
  omega3s[0]->setValue(1, 15.9438);
  omega3s[1]->setValue(1, 1.44586);
  omega3s[0]->setValue(2, 15.4206);
  omega3s[1]->setValue(2, 1.6027);
  omega3s[0]->setValue(3, 15.3367);
  omega3s[1]->setValue(3, 1.47998);
  omega3s[0]->setValue(4, 15.7276);
  omega3s[1]->setValue(4, 1.64522);
  omega3s[0]->setValue(5, 15.9084);
  omega3s[1]->setValue(5, 1.71922);
  pp->setFeatureSize_Omegas(omega3s);
  VectorOfFloatArray neighborhoods = statsDataArray[1]->CreateCorrelatedDistributionArrays(DREAM3D::DistributionType::LogNormal, numbins);
  neighborhoods[0]->setValue(0, 2.07944);
  neighborhoods[1]->setValue(0, 0.4);
  neighborhoods[0]->setValue(1, 2.30259);
  neighborhoods[1]->setValue(1, 0.36666);
  neighborhoods[0]->setValue(2, 2.48491);
  neighborhoods[1]->setValue(2, 0.33333);
  neighborhoods[0]->setValue(3, 2.63906);
  neighborhoods[1]->setValue(3, 0.3);
  neighborhoods[0]->setValue(4, 2.77259);
  neighborhoods[1]->setValue(4, 0.26666);
  neighborhoods[0]->setValue(5, 2.89037);
  neighborhoods[1]->setValue(5, 0.23333);
  pp->setFeatureSize_Neighbors(neighborhoods);
  FloatArrayType::Pointer axisodf;
  axisodf = FloatArrayType::CreateArray((36 * 36 * 36), DREAM3D::StringConstants::AxisOrientation);
  float val = 1.0 / (36.0 * 36.0 * 36.0);
  for (int j = 0; j < (36 * 36 * 36); j++)
  {
    axisodf->setValue(j, val);
  }
  pp->setAxisOrientation(axisodf);


  DataContainerWriter::Pointer dcw = DataContainerWriter::New();
  dcw->setOutputFile(UnitTest::NewDataContainerStructureTest::SyntheticInputFile);
  dcw->setWriteXdmfFile(false);
  dcw->setDataContainerArray(dca);
  dcw->setPreviousFilter(AbstractFilter::NullPointer());
  dcw->execute();
}

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
  isv->setXVoxels(128);
  isv->setYVoxels(128);
  isv->setZVoxels(128);
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
//  pipeline->pushBack(fn);

  FindNumFeatures::Pointer fnf = FindNumFeatures::New();
//  pipeline->pushBack(fnf);

  MatchCrystallography::Pointer mc = MatchCrystallography::New();
  mc->setMaxIterations(100000);
//  pipeline->pushBack(mc);

  GenerateIPFColors::Pointer gipfc = GenerateIPFColors::New();
//  pipeline->pushBack(gipfc);

//  ChangeResolution::Pointer cr = ChangeResolution::New();
//  FloatVec3_t res = {0.2, 0.2, 0.2};
//  cr->setResolution(res);
//  pipeline->pushBack(cr);

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
//  FloatVec3_t res = {0.05,0.05,0.05};
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

//  BuildNewDream3dFile();
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

