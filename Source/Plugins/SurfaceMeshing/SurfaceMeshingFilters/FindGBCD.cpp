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
#include "FindGBCD.h"


#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/Utilities/TimeUtilities.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

/**
 * @brief The CalculateAreasImpl class
 */
class CalculateGBCDImpl
{
    size_t startOffset;
    size_t numEntriesPerTri;
    int32_t* m_Labels;
    double* m_Normals;
    int32_t* m_Phases;
    float* m_Eulers;
    int32_t* m_Bins;
    bool* m_HemiCheck;
    float* m_GBCDdeltas;
    int* m_GBCDsizes;
    float* m_GBCDlimits;
    unsigned int* m_CrystalStructures;
    QVector<OrientationOps::Pointer> m_OrientationOps;

  public:
    CalculateGBCDImpl(size_t i, size_t numMisoReps, int32_t* Labels, double* Normals, float* Eulers, int32_t* Phases, unsigned int* CrystalStructures,
                      int32_t* Bins, bool* HemiCheck, float* GBCDdeltas, int* GBCDsizes, float* GBCDlimits) :
      startOffset(i),
      numEntriesPerTri(numMisoReps),
      m_Labels(Labels),
      m_Normals(Normals),
      m_Phases(Phases),
      m_Eulers(Eulers),
      m_Bins(Bins),
      m_HemiCheck(HemiCheck),
      m_GBCDdeltas(GBCDdeltas),
      m_GBCDsizes(GBCDsizes),
      m_GBCDlimits(GBCDlimits),
      m_CrystalStructures(CrystalStructures)
    {
      m_OrientationOps = OrientationOps::getOrientationOpsVector();
    }
    virtual ~CalculateGBCDImpl() {}

    void generate(size_t start, size_t end) const
    {
      int j;//, j4;
      int k;//, k4;
      int m;
      int temp;
      //bool gbcd_indices_good;
      int feature1, feature2;
      int inversion = 1;
      float g1ea[3], g2ea[3];
      float g1[3][3], g2[3][3];
      float g1s[3][3], g2s[3][3];
      float sym1[3][3], sym2[3][3];
      float g2t[3][3], dg[3][3];
      float euler_mis[3];
      float normal[3];
      float xstl1_norm0[3], xstl1_norm1[3];
      int gbcd_index;
      float sqCoord[2], sqCoordInv[2];
      bool nhCheck, nhCheckInv;
      int SYMcounter = 0;
      int TRIcounter = start - startOffset;
      int TRIcounterShift;

      for (size_t i = start; i < end; i++)
      {
        SYMcounter = 0;
        feature1 = m_Labels[2 * i];
        feature2 = m_Labels[2 * i + 1];
        normal[0] = m_Normals[3 * i];
        normal[1] = m_Normals[3 * i + 1];
        normal[2] = m_Normals[3 * i + 2];
        if(m_Phases[feature1] == m_Phases[feature2])
        {
          TRIcounterShift = (TRIcounter * numEntriesPerTri);
          for(int q = 0; q < 2; q++)
          {
            if(q == 1)
            {
              temp = feature1;
              feature1 = feature2;
              feature2 = temp;
              normal[0] = -normal[0];
              normal[1] = -normal[1];
              normal[2] = -normal[2];
            }
            for(m = 0; m < 3; m++)
            {
              g1ea[m] = m_Eulers[3 * feature1 + m];
              g2ea[m] = m_Eulers[3 * feature2 + m];
            }

            OrientationMath::EulertoMat(g1ea[0], g1ea[1], g1ea[2], g1);
            OrientationMath::EulertoMat(g2ea[0], g2ea[1], g2ea[2], g2);

            //get the crystal directions along the triangle normals
            MatrixMath::Multiply3x3with3x1(g1, normal, xstl1_norm0);
            //get the misorientation between feature1 and feature2
            int nsym = m_OrientationOps[m_CrystalStructures[m_Phases[feature1]]]->getNumSymOps();
            for (j = 0; j < nsym; j++)
            {
              //rotate g1 by symOp
              m_OrientationOps[m_CrystalStructures[m_Phases[feature1]]]->getMatSymOp(j, sym1);
              MatrixMath::Multiply3x3with3x3(sym1, g1, g1s);
              //find symmetric crystal directions
              MatrixMath::Multiply3x3with3x1(sym1, xstl1_norm0, xstl1_norm1);

              //get coordinates in square projection of crystal normal parallel to boundary normal
              nhCheck = getSquareCoord(xstl1_norm1, sqCoord);
              if(inversion == 1)
              {
                sqCoordInv[0] = -sqCoord[0];
                sqCoordInv[1] = -sqCoord[1];
                if(nhCheck == false) { nhCheckInv = true; }
                else { nhCheckInv = false; }
              }
              else
              {
                sqCoordInv[0] = sqCoord[0];
                sqCoordInv[1] = sqCoord[1];
                nhCheckInv = nhCheck;
              }

              for (k = 0; k < nsym; k++)
              {
                //calculate the symmetric misorienation
                m_OrientationOps[m_CrystalStructures[m_Phases[feature1]]]->getMatSymOp(k, sym2);
                //rotate g2 by symOp
                MatrixMath::Multiply3x3with3x3(sym2, g2, g2s);
                //transpose rotated g2
                MatrixMath::Transpose3x3(g2s, g2t);
                //calculate delta g
                MatrixMath::Multiply3x3with3x3(g1s, g2t, dg);
                //translate matrix to euler angles
                OrientationMath::MattoEuler(dg, euler_mis[0], euler_mis[1], euler_mis[2]);
                euler_mis[1] = cosf(euler_mis[1]);

                //get the indexes that this point would be in the GBCD histogram
                gbcd_index = GBCDIndex (m_GBCDdeltas, m_GBCDsizes, m_GBCDlimits, euler_mis, sqCoord);
                if (gbcd_index != -1)
                {
                  m_HemiCheck[TRIcounterShift + SYMcounter] = nhCheck;
                  m_Bins[TRIcounterShift + SYMcounter] = gbcd_index;
                }
                SYMcounter++;
                gbcd_index = GBCDIndex (m_GBCDdeltas, m_GBCDsizes, m_GBCDlimits, euler_mis, sqCoordInv);
                if (gbcd_index != -1)
                {
                  m_HemiCheck[TRIcounterShift + SYMcounter] = nhCheckInv;
                  m_Bins[TRIcounterShift + SYMcounter] = gbcd_index;
                }
                SYMcounter++;
              }
            }
          }
        }
        TRIcounter++;
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      generate(r.begin(), r.end());
    }
#endif

    int GBCDIndex (float* gbcddelta, int* gbcdsz, float* gbcdlimits, float* eulerN, float* sqCoord) const
    {
      int32_t gbcd_index;
      int i, index[5];
      int n1, n1n2, n1n2n3, n1n2n3n4;
      int flag_good = 1;
      float mis_eulerNorm[5];

      //concatonate the normalized euler angles and normalized spherical corrdinate normal
      for (i = 0; i < 3; i++) { mis_eulerNorm[i] = eulerN[i]; }
      for (i = 0; i < 2; i++) { mis_eulerNorm[i + 3] = sqCoord[i]; }
      //Check for a valid point in the GBCD space
      for (i = 0; i < 5; i++)
      {
        if (mis_eulerNorm[i] < gbcdlimits[i]) { flag_good = 0; }
        if (mis_eulerNorm[i] > gbcdlimits[i + 5]) { flag_good = 0; }
      }

      if (flag_good == 0) { return -1; } //does not fit in the gbcd space

      n1 =  gbcdsz[0];
      n1n2 = n1 * ( gbcdsz[1]);
      n1n2n3 = n1n2 * ( gbcdsz[2]);
      n1n2n3n4 = n1n2n3 * ( gbcdsz[3]);

      //determine the bin that the point should go into.
      for (i = 0; i < 5; i++)
      {
        index[i] = (int) ((mis_eulerNorm[i] - gbcdlimits[i]) / gbcddelta[i]);
        if (index[i] >  (gbcdsz[i] - 1))
        {
          index[i] =  (gbcdsz[i] - 1);
        }
        if (index[i] <  0)
        {
          index[i] =  0;
        }
      }

      gbcd_index = index[0] + n1 * index[1] + n1n2 * index[2] + n1n2n3 * index[3] + n1n2n3n4 * index[4];

      return gbcd_index;
    }

    bool getSquareCoord(float* xstl1_norm1, float* sqCoord) const
    {
      bool nhCheck = false;
      float adjust = 1.0;
      if(xstl1_norm1[2] >= 0.0)
      {
        adjust = -1.0;
        nhCheck = true;
      }
      if(fabs(xstl1_norm1[0]) >= fabs(xstl1_norm1[1]))
      {
        sqCoord[0] = (xstl1_norm1[0] / fabs(xstl1_norm1[0])) * sqrt(2.0 * 1.0 * (1.0 + (xstl1_norm1[2] * adjust))) * (DREAM3D::Constants::k_SqrtPi / 2.0);
        sqCoord[1] = (xstl1_norm1[0] / fabs(xstl1_norm1[0])) * sqrt(2.0 * 1.0 * (1.0 + (xstl1_norm1[2] * adjust))) * ((2.0 / DREAM3D::Constants::k_SqrtPi) * atan(xstl1_norm1[1] / xstl1_norm1[0]));
      }
      else
      {
        sqCoord[0] = (xstl1_norm1[1] / fabs(xstl1_norm1[1])) * sqrt(2.0 * 1.0 * (1.0 + (xstl1_norm1[2] * adjust))) * ((2.0 / DREAM3D::Constants::k_SqrtPi) * atan(xstl1_norm1[0] / xstl1_norm1[1]));
        sqCoord[1] = (xstl1_norm1[1] / fabs(xstl1_norm1[1])) * sqrt(2.0 * 1.0 * (1.0 + (xstl1_norm1[2] * adjust))) * (DREAM3D::Constants::k_SqrtPi / 2.0);
      }
      return nhCheck;
    }
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindGBCD::FindGBCD() :
  SurfaceMeshFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_SurfaceDataContainerName(DREAM3D::Defaults::SurfaceDataContainerName),
  m_FaceAttributeMatrixName(DREAM3D::Defaults::FaceAttributeMatrixName),
  m_FaceEnsembleAttributeMatrixName(DREAM3D::Defaults::FaceEnsembleAttributeMatrixName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_GBCDRes(9.0),
//  m_GBCDArrayNames(DREAM3D::EnsembleData::GBCD),
  m_SurfaceMeshFaceAreasArrayName(DREAM3D::FaceData::SurfaceMeshFaceAreas),
  m_SurfaceMeshFaceAreas(NULL),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshFaceNormalsArrayName(DREAM3D::FaceData::SurfaceMeshFaceNormals),
  m_SurfaceMeshFaceNormals(NULL),
  m_FeatureEulerAnglesArrayName(DREAM3D::FeatureData::EulerAngles),
  m_FeatureEulerAngles(NULL),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_FeaturePhases(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL),
  m_GBCDArrayName(DREAM3D::EnsembleData::GBCD),
  m_GBCD(NULL),
  m_GBCDdimensionsArrayName(DREAM3D::EnsembleData::GBCDdimensions),
  m_GBCDdimensions(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindGBCD::~FindGBCD()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setPropertyName("GBCDRes");
    parameter->setHumanLabel("GBCD Resolution");
    parameter->setWidgetType(FilterParameterWidgetType::DoubleWidget);
    parameter->setValueType("float");
    parameter->setCastableValueType("double");
    parameter->setUnits("Degrees");
    parameters.push_back(parameter);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setGBCDRes( reader->readValue("GBCDRes", getGBCDRes()));
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindGBCD::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("GBCDRes", getGBCDRes() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::dataCheckSurfaceMesh()
{
  setErrorCondition(0);

  SurfaceDataContainer* sm = getDataContainerArray()->getPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, getSurfaceDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer faceAttrMat = sm->getPrereqAttributeMatrix<AbstractFilter>(this, getFaceAttributeMatrixName(), -302);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer faceEnsembleAttrMat = sm->getPrereqAttributeMatrix<AbstractFilter>(this, getFaceEnsembleAttributeMatrixName(), -302);
  if(getErrorCondition() < 0) { return; }

  // We MUST have Nodes
  if(sm->getVertices().get() == NULL)
  {
    setErrorCondition(-384);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", getErrorCondition());
  }

  // We MUST have Triangles defined also.
  if(sm->getFaces().get() == NULL)
  {
    setErrorCondition(-384);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", getErrorCondition());
  }
  else
  {
    QVector<size_t> dims(1, 2);
    m_SurfaceMeshFaceLabelsPtr = faceAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_SurfaceMeshFaceLabelsArrayName, -386, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    dims[0] = 3;
    m_SurfaceMeshFaceNormalsPtr = faceAttrMat->getPrereqArray<DataArray<double>, AbstractFilter>(this, m_SurfaceMeshFaceNormalsArrayName, -387, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshFaceNormalsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshFaceNormals = m_SurfaceMeshFaceNormalsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    dims[0] = 1;
    m_SurfaceMeshFaceAreasPtr = faceAttrMat->getPrereqArray<DataArray<double>, AbstractFilter>(this, m_SurfaceMeshFaceAreasArrayName, -388, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshFaceAreasPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshFaceAreas = m_SurfaceMeshFaceAreasPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    m_GBCDPtr = faceEnsembleAttrMat->createNonPrereqArray<DataArray<double>, AbstractFilter, double>(this, m_GBCDArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_GBCDPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_GBCD = m_GBCDPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    dims[0] = 5;
    m_GBCDdimensionsPtr = faceEnsembleAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, m_GBCDdimensionsArrayName, 1, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_GBCDdimensionsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_GBCDdimensions = m_GBCDdimensionsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::dataCheckVoxel()
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -302);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), -303);
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> dims(1, 3);
  m_FeatureEulerAnglesPtr = cellFeatureAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_FeatureEulerAnglesArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  m_FeaturePhasesPtr = cellFeatureAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeaturePhasesArrayName, -302, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = cellEnsembleAttrMat->getPrereqArray<DataArray<unsigned int>, AbstractFilter>(this, m_CrystalStructuresArrayName, -304, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::preflight()
{
  dataCheckSurfaceMesh();
  dataCheckVoxel();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::execute()
{
  int err = 0;
  setErrorCondition(err);

  // Run the data check to allocate the memory for the centroid array
  // Note the use of the voxel datacontainer num ensembles to set the gbcd size
  dataCheckSurfaceMesh();
  if(getErrorCondition() < 0) { return; }

  dataCheckVoxel();
  if(getErrorCondition() < 0) { return; }

  notifyStatusMessage(getHumanLabel(), "Starting");

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  VertexArray::Pointer nodesPtr = sm->getVertices();

  FaceArray::Pointer trianglesPtr = sm->getFaces();
  size_t totalFaces = sm->getAttributeMatrix(getFaceAttributeMatrixName())->getNumTuples();

  size_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  size_t totalEnsembles = m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->getNumTuples();

  size_t faceChunkSize = 50000;
  size_t numMisoReps = 2304;
  if(totalFaces < faceChunkSize) { faceChunkSize = totalFaces; }

  FloatArrayType::Pointer gbcdDeltasArray = FloatArrayType::NullPointer();
  FloatArrayType::Pointer gbcdLimitsArray = FloatArrayType::NullPointer();
  Int32ArrayType::Pointer gbcdSizesArray = Int32ArrayType::NullPointer();
  Int32ArrayType::Pointer gbcdBinsArray = Int32ArrayType::NullPointer();
  BoolArrayType::Pointer gbcdHemiCheckArray = BoolArrayType::NullPointer();
  gbcdDeltasArray = FloatArrayType::CreateArray(5, "GBCDDeltas");
  gbcdDeltasArray->initializeWithZeros();
  gbcdLimitsArray = FloatArrayType::CreateArray(10, "GBCDLimits");
  gbcdLimitsArray->initializeWithZeros();
  gbcdSizesArray = Int32ArrayType::CreateArray(5, "GBCDSizes");
  gbcdSizesArray->initializeWithZeros();
  QVector<size_t> dims(1, numMisoReps);
  gbcdBinsArray = Int32ArrayType::CreateArray(faceChunkSize, dims, "GBCDBins");
  gbcdBinsArray->initializeWithZeros();
  gbcdHemiCheckArray = BoolArrayType::CreateArray(faceChunkSize, dims, "GBCDHemiCheck");
  gbcdHemiCheckArray->initializeWithValue(false);
  float* m_GBCDdeltas = gbcdDeltasArray->getPointer(0);
  int32_t* m_GBCDsizes = gbcdSizesArray->getPointer(0);
  float* m_GBCDlimits = gbcdLimitsArray->getPointer(0);
  int32_t* m_Bins = gbcdBinsArray->getPointer(0);
  bool* m_HemiCheck = gbcdHemiCheckArray->getPointer(0);

  m_GBCDlimits[0] = 0.0;
  m_GBCDlimits[1] = cosf(1.0 * DREAM3D::Constants::k_Pi);
  m_GBCDlimits[2] = 0.0;
  m_GBCDlimits[3] = 0.0;
  m_GBCDlimits[4] = cosf(1.0 * DREAM3D::Constants::k_Pi);
  m_GBCDlimits[5] = 2.0 * DREAM3D::Constants::k_Pi;
  m_GBCDlimits[6] = cosf(0.0);
  m_GBCDlimits[7] = 2.0 * DREAM3D::Constants::k_Pi;
  m_GBCDlimits[8] = 2.0 * DREAM3D::Constants::k_Pi;
  m_GBCDlimits[9] = cosf(0.0);

  float binsize = m_GBCDRes * DREAM3D::Constants::k_Pi / 180.0;
  float binsize2 = binsize * (2.0 / DREAM3D::Constants::k_Pi);
  m_GBCDdeltas[0] = binsize;
  m_GBCDdeltas[1] = binsize2;
  m_GBCDdeltas[2] = binsize;
  m_GBCDdeltas[3] = binsize;
  m_GBCDdeltas[4] = binsize2;

  m_GBCDsizes[0] = int(0.5 + (m_GBCDlimits[5] - m_GBCDlimits[0]) / m_GBCDdeltas[0]);
  m_GBCDsizes[1] = int(0.5 + (m_GBCDlimits[6] - m_GBCDlimits[1]) / m_GBCDdeltas[1]);
  m_GBCDsizes[2] = int(0.5 + (m_GBCDlimits[7] - m_GBCDlimits[2]) / m_GBCDdeltas[2]);
  m_GBCDsizes[3] = int(0.5 + (m_GBCDlimits[8] - m_GBCDlimits[3]) / m_GBCDdeltas[3]);
  m_GBCDsizes[4] = int(0.5 + (m_GBCDlimits[9] - m_GBCDlimits[4]) / m_GBCDdeltas[4]);

  //reset the 3rd and 4th dimensions using the square grid approach
  float totalNormalBins = m_GBCDsizes[3] * m_GBCDsizes[4];
  m_GBCDsizes[3] = int(sqrt(totalNormalBins / 2.0));
  m_GBCDsizes[4] = int(sqrt(totalNormalBins / 2.0));
  m_GBCDlimits[3] = -sqrt(DREAM3D::Constants::k_Pi / 2.0);
  m_GBCDlimits[4] = -sqrt(DREAM3D::Constants::k_Pi / 2.0);
  m_GBCDlimits[8] = sqrt(DREAM3D::Constants::k_Pi / 2.0);
  m_GBCDlimits[9] = sqrt(DREAM3D::Constants::k_Pi / 2.0);
  m_GBCDdeltas[3] = (m_GBCDlimits[8] - m_GBCDlimits[3]) / float(m_GBCDsizes[3]);
  m_GBCDdeltas[4] = (m_GBCDlimits[9] - m_GBCDlimits[4]) / float(m_GBCDsizes[4]);


  dims.resize(6);
  dims[0] = m_GBCDsizes[0];
  dims[1] = m_GBCDsizes[1];
  dims[2] = m_GBCDsizes[2];
  dims[3] = m_GBCDsizes[3];
  dims[4] = m_GBCDsizes[4];
  dims[5] = 2;

  AttributeMatrix::Pointer faceEnsembleAttrMat = sm->getPrereqAttributeMatrix<AbstractFilter>(this, getFaceEnsembleAttributeMatrixName(), -302);
  m_GBCDPtr = faceEnsembleAttrMat->createNonPrereqArray<DataArray<double>, AbstractFilter, double>(this,  m_GBCDArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_GBCDPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_GBCD = m_GBCDPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  for(int i = 0; i < faceEnsembleAttrMat->getNumTuples(); i++)
  {
    m_GBCDdimensions[5 * i + 0] = m_GBCDsizes[0];
    m_GBCDdimensions[5 * i + 1] = m_GBCDsizes[1];
    m_GBCDdimensions[5 * i + 2] = m_GBCDsizes[2];
    m_GBCDdimensions[5 * i + 3] = m_GBCDsizes[3];
    m_GBCDdimensions[5 * i + 4] = m_GBCDsizes[4];
  }

  uint64_t millis = QDateTime::currentMSecsSinceEpoch();
  uint64_t currentMillis = millis;
  uint64_t startMillis = millis;
  uint64_t estimatedTime = 0;
  float timeDiff = 0.0f;
  millis =  QDateTime::currentMSecsSinceEpoch();
  startMillis = millis;
  int lastIteration = 0;
  int numIterationsPerTime = 0;
  int hemisphere = 0;

  double totalFaceArea[2] = {0.0, 0.0};
  QString ss = QObject::tr("Calculating GBCD: 0/%1 Completed").arg(totalFaces);
  for(size_t i = 0; i < totalFaces; i = i + faceChunkSize)
  {

    if(getCancel() == true) { return; }
    if(i + faceChunkSize >= totalFaces)
    {
      faceChunkSize = totalFaces - i;
    }
    gbcdBinsArray->initializeWithZeros();
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(i, i + faceChunkSize),
                        CalculateGBCDImpl(i, numMisoReps, m_SurfaceMeshFaceLabels, m_SurfaceMeshFaceNormals, m_FeatureEulerAngles, m_FeaturePhases, m_CrystalStructures, m_Bins, m_HemiCheck, m_GBCDdeltas, m_GBCDsizes, m_GBCDlimits), tbb::auto_partitioner());

    }
    else
#endif
    {
      CalculateGBCDImpl serial(i, numMisoReps, m_SurfaceMeshFaceLabels, m_SurfaceMeshFaceNormals, m_FeatureEulerAngles, m_FeaturePhases, m_CrystalStructures, m_Bins, m_HemiCheck, m_GBCDdeltas, m_GBCDsizes, m_GBCDlimits);
      serial.generate(i, i + faceChunkSize);
    }

    QString ss = QObject::tr("Calculating GBCD: Triangles %1/%2 Completed").arg(i).arg(totalFaces);
    currentMillis = QDateTime::currentMSecsSinceEpoch();
    if (currentMillis - millis > 1000)
    {
      timeDiff = ((float)i / (float)(currentMillis - startMillis));
      estimatedTime = (float)(totalFaces - i) / timeDiff;
      ss = ss + QObject::tr(" || Est. Time Remain: %1").arg(DREAM3D::convertMillisToHrsMinSecs(estimatedTime));
      millis = QDateTime::currentMSecsSinceEpoch();
      numIterationsPerTime = i - lastIteration;
      lastIteration = i;
    }
    notifyStatusMessage(getHumanLabel(), ss);

    for(int j = 0; j < faceChunkSize; j++)
    {
      for(int k = 0; k < numMisoReps; k++)
      {
        hemisphere = 0;
        if(m_HemiCheck[(j * numMisoReps) + k] == false) { hemisphere = 1; }
        m_GBCD[2 * m_Bins[(j * numMisoReps) + (k)] + hemisphere] += (m_SurfaceMeshFaceAreas[i + j]);
        totalFaceArea[hemisphere] += (m_SurfaceMeshFaceAreas[i + j]);
      }
    }
  }

  ss = QObject::tr("Starting GBCD Normalization");
  notifyStatusMessage(getHumanLabel(), ss);

  int totalBins = m_GBCDsizes[0] * m_GBCDsizes[1] * m_GBCDsizes[2] * m_GBCDsizes[3] * m_GBCDsizes[4];
  double MRDfactor[2] = {double(totalBins) / totalFaceArea[0], double(totalBins) / totalFaceArea[1]};
  for(int i = 0; i < totalBins; i++)
  {
    m_GBCD[2 * i + 0] *= MRDfactor[0];
    m_GBCD[2 * i + 1] *= MRDfactor[1];
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}


