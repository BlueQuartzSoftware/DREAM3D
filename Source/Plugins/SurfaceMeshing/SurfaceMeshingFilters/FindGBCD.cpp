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


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif


#include <QtCore/QDateTime>

#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/Utilities/TimeUtilities.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"


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

    FloatArrayType::Pointer m_GbcdDeltasArray;
    FloatArrayType::Pointer m_GbcdLimitsArray;
    Int32ArrayType::Pointer m_GbcdSizesArray;
    Int32ArrayType::Pointer m_GbcdBinsArray;
    BoolArrayType::Pointer  m_GbcdHemiCheckArray;

    unsigned int* m_CrystalStructures;
    QVector<OrientationOps::Pointer> m_OrientationOps;

  public:
    CalculateGBCDImpl(size_t i, size_t numMisoReps, int32_t* Labels, double* Normals, float* Eulers,
                      int32_t* Phases, unsigned int* CrystalStructures,
                      Int32ArrayType::Pointer Bins, BoolArrayType::Pointer HemiCheck,
                      FloatArrayType::Pointer GBCDdeltas, Int32ArrayType::Pointer  GBCDsizes,
                      FloatArrayType::Pointer GBCDlimits) :
      startOffset(i),
      numEntriesPerTri(numMisoReps),
      m_Labels(Labels),
      m_Normals(Normals),
      m_Phases(Phases),
      m_Eulers(Eulers),
      m_GbcdDeltasArray(GBCDdeltas),
      m_GbcdLimitsArray(GBCDlimits),
      m_GbcdSizesArray(GBCDsizes),
      m_GbcdBinsArray(Bins),
      m_GbcdHemiCheckArray(HemiCheck),
      m_CrystalStructures(CrystalStructures)
    {
      m_OrientationOps = OrientationOps::getOrientationOpsVector();
    }
    virtual ~CalculateGBCDImpl() {}

    /**
     * @brief generate
     * @param start
     * @param end
     */
    void generate(size_t start, size_t end) const
    {

      // We want to work with the raw pointers for speed so get those pointers.
      float* m_GBCDdeltas = m_GbcdDeltasArray->getPointer(0);
      float* m_GBCDlimits = m_GbcdLimitsArray->getPointer(0);
      int* m_GBCDsizes = m_GbcdSizesArray->getPointer(0);
      int32_t* m_Bins = m_GbcdBinsArray->getPointer(0);
      bool* m_HemiCheck = m_GbcdHemiCheckArray->getPointer(0);



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
      float xstl1_norm1[3];
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
          unsigned int cryst = m_CrystalStructures[m_Phases[feature1]];
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

            int nsym = m_OrientationOps[cryst]->getNumSymOps();
            for (j = 0; j < nsym; j++)
            {
              //rotate g1 by symOp
              m_OrientationOps[cryst]->getMatSymOp(j, sym1);
              MatrixMath::Multiply3x3with3x3(sym1, g1, g1s);
              //get the crystal directions along the triangle normals
              MatrixMath::Multiply3x3with3x1(g1s, normal, xstl1_norm1);
              //get coordinates in square projection of crystal normal parallel to boundary normal
              nhCheck = getSquareCoord(xstl1_norm1, sqCoord);
              if(inversion == 1)
              {
                sqCoordInv[0] = -sqCoord[0];
                sqCoordInv[1] = -sqCoord[1];
                if(nhCheck == false) { nhCheckInv = true; }
                else { nhCheckInv = false; }
              }

              for (k = 0; k < nsym; k++)
              {
                //calculate the symmetric misorienation
                m_OrientationOps[cryst]->getMatSymOp(k, sym2);
                //rotate g2 by symOp
                MatrixMath::Multiply3x3with3x3(sym2, g2, g2s);
                //transpose rotated g2
                MatrixMath::Transpose3x3(g2s, g2t);
                //calculate delta g
                MatrixMath::Multiply3x3with3x3(g1s, g2t, dg);
                //translate matrix to euler angles
                OrientationMath::MattoEuler(dg, euler_mis[0], euler_mis[1], euler_mis[2]);

                if(euler_mis[0] < DREAM3D::Constants::k_PiOver2 && euler_mis[1] < DREAM3D::Constants::k_PiOver2 && euler_mis[2] < DREAM3D::Constants::k_PiOver2)
                {
                  //PHI euler angle is stored in GBCD as cos(PHI)
                  euler_mis[1] = cosf(euler_mis[1]);
                  //get the indexes that this point would be in the GBCD histogram
                  gbcd_index = GBCDIndex (m_GBCDdeltas, m_GBCDsizes, m_GBCDlimits, euler_mis, sqCoord);
                  if (gbcd_index != -1)
                  {
                    m_HemiCheck[TRIcounterShift + SYMcounter] = nhCheck;
                    m_Bins[TRIcounterShift + SYMcounter] = gbcd_index;
                  }
                  SYMcounter++;
                  if(inversion == 1)
                  {
                    gbcd_index = GBCDIndex (m_GBCDdeltas, m_GBCDsizes, m_GBCDlimits, euler_mis, sqCoordInv);
                    if (gbcd_index != -1)
                    {
                      m_HemiCheck[TRIcounterShift + SYMcounter] = nhCheckInv;
                      m_Bins[TRIcounterShift + SYMcounter] = gbcd_index;
                    }
                    SYMcounter++;
                  }
                }
                else { SYMcounter += 2; }
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
  m_FaceEnsembleAttributeMatrixName(DREAM3D::Defaults::FaceEnsembleAttributeMatrixName),
  m_GBCDRes(9.0),
  m_SurfaceMeshFaceLabelsArrayPath(DREAM3D::Defaults::SurfaceDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceNormalsArrayPath(DREAM3D::Defaults::SurfaceDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceNormals),
  m_SurfaceMeshFaceAreasArrayPath(DREAM3D::Defaults::SurfaceDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceAreas),
  m_FeatureEulerAnglesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::EulerAngles),
  m_FeaturePhasesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_GBCDArrayName(DREAM3D::EnsembleData::GBCD),
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
  m_GBCD(NULL),
  m_GbcdDeltas(NULL),
  m_GbcdSizes(NULL),
  m_GbcdLimits(NULL),
  m_GbcdBins(NULL),
  m_HemiCheck(NULL)

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
  parameters.push_back(FilterParameter::New("GBCD Resolution", "GBCDRes", FilterParameterWidgetType::DoubleWidget, getGBCDRes(), false, "Degrees"));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("SurfaceMeshFaceLabels", "SurfaceMeshFaceLabelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSurfaceMeshFaceLabelsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("SurfaceMeshFaceNormals", "SurfaceMeshFaceNormalsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSurfaceMeshFaceNormalsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("SurfaceMeshFaceAreas", "SurfaceMeshFaceAreasArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSurfaceMeshFaceAreasArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("FeatureEulerAngles", "FeatureEulerAnglesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureEulerAnglesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("FeaturePhases", "FeaturePhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeaturePhasesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCrystalStructuresArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Face Ensemble Attribute Matrix Name", "FaceEnsembleAttributeMatrixName", FilterParameterWidgetType::StringWidget, getFaceEnsembleAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("GBCD", "GBCDArrayName", FilterParameterWidgetType::StringWidget, getGBCDArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setGBCDArrayName(reader->readString("GBCDArrayName", getGBCDArrayName() ) );
  setFaceEnsembleAttributeMatrixName(reader->readString("FaceEnsembleAttributeMatrixName", getFaceEnsembleAttributeMatrixName() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setFeatureEulerAnglesArrayPath(reader->readDataArrayPath("FeatureEulerAnglesArrayPath", getFeatureEulerAnglesArrayPath() ) );
  setSurfaceMeshFaceAreasArrayPath(reader->readDataArrayPath("SurfaceMeshFaceAreasArrayPath", getSurfaceMeshFaceAreasArrayPath() ) );
  setSurfaceMeshFaceNormalsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceNormalsArrayPath", getSurfaceMeshFaceNormalsArrayPath() ) );
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath() ) );
  setGBCDRes( reader->readValue("GBCDRes", getGBCDRes()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindGBCD::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FaceEnsembleAttributeMatrixName)
      DREAM3D_FILTER_WRITE_PARAMETER(GBCDArrayName)
      DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
      DREAM3D_FILTER_WRITE_PARAMETER(FeaturePhasesArrayPath)
      DREAM3D_FILTER_WRITE_PARAMETER(FeatureEulerAnglesArrayPath)
      DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshFaceAreasArrayPath)
      DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshFaceNormalsArrayPath)
      DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshFaceLabelsArrayPath)
      DREAM3D_FILTER_WRITE_PARAMETER(GBCDRes)
      writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::dataCheckSurfaceMesh()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  SurfaceDataContainer* sm = getDataContainerArray()->getPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  QVector<size_t> tDims(1, m_CrystalStructuresPtr.lock()->getNumberOfTuples());
  AttributeMatrix::Pointer faceEnsembleAttrMat = sm->createNonPrereqAttributeMatrix<AbstractFilter>(this, getFaceEnsembleAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::FaceEnsemble);
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
    m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    dims[0] = 3;
    m_SurfaceMeshFaceNormalsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, getSurfaceMeshFaceNormalsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshFaceNormalsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshFaceNormals = m_SurfaceMeshFaceNormalsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    dims[0] = 1;
    m_SurfaceMeshFaceAreasPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, getSurfaceMeshFaceAreasArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshFaceAreasPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshFaceAreas = m_SurfaceMeshFaceAreasPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

    //call the sizeGBCD function to get the GBCD ranges, dimensions, etc.  Note that the input parameters do not affect the size and can be dummy values here;
    sizeGBCD(0, 0);
    dims.resize(6);
    dims[0] = m_GbcdSizes[0];
    dims[1] = m_GbcdSizes[1];
    dims[2] = m_GbcdSizes[2];
    dims[3] = m_GbcdSizes[3];
    dims[4] = m_GbcdSizes[4];
    dims[5] = 2;

    tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), getFaceEnsembleAttributeMatrixName(), getGBCDArrayName() );
    m_GBCDPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_GBCDPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_GBCD = m_GBCDPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::dataCheckVoxel()
{
  setErrorCondition(0);

  QVector<size_t> dims(1, 3);
  m_FeatureEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getFeatureEulerAnglesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheckVoxel();
  //order here matters...because we are going to use the size of the crystal structures out of the dataCheckVoxel to size the faceAttrMat in dataCheckSurfaceMesh
  if (getErrorCondition() >= 0) { dataCheckSurfaceMesh(); }
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::execute()
{
  int err = 0;
  setErrorCondition(err);

  dataCheckVoxel();
  if(getErrorCondition() < 0) { return; }
  //order here matters...because we are going to use the size of the crystal structures out of the dataCheckVoxel to size the faceAttrMat in dataCheckSurfaceMesh
  dataCheckSurfaceMesh();
  if(getErrorCondition() < 0) { return; }

  QString ss = QObject::tr("Starting GBCD Calculation");
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  size_t totalFaces = m_SurfaceMeshFaceLabelsPtr.lock()->getNumberOfTuples();

  size_t faceChunkSize = 50000;
  size_t numMisoReps = 576;
  if(totalFaces < faceChunkSize) { faceChunkSize = totalFaces; }
  //call the sizeGBCD function with proper chunkSize and numMisoReps to get Bins array set up properly
  sizeGBCD(faceChunkSize, numMisoReps);

  uint64_t millis = QDateTime::currentMSecsSinceEpoch();
  uint64_t currentMillis = millis;
  uint64_t startMillis = millis;
  uint64_t estimatedTime = 0;
  float timeDiff = 0.0f;
  startMillis =  QDateTime::currentMSecsSinceEpoch();
  int lastIteration = 0;
  int numIterationsPerTime = 0;
  int hemisphere = 0;

  double totalFaceArea = 0.0;
  ss = QObject::tr("Calculating GBCD: 0/%1 Completed").arg(totalFaces);
  for(size_t i = 0; i < totalFaces; i = i + faceChunkSize)
  {
    if(getCancel() == true) { return; }
    if(i + faceChunkSize >= totalFaces)
    {
      faceChunkSize = totalFaces - i;
    }
    m_GbcdBinsArray->initializeWithValue(-1);
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(i, i + faceChunkSize),
                        CalculateGBCDImpl(i, numMisoReps, m_SurfaceMeshFaceLabels, m_SurfaceMeshFaceNormals, m_FeatureEulerAngles, m_FeaturePhases, m_CrystalStructures, m_GbcdBinsArray, m_GbcdHemiCheckArray, m_GbcdDeltasArray, m_GbcdSizesArray, m_GbcdLimitsArray), tbb::auto_partitioner());

    }
    else
#endif
    {
      CalculateGBCDImpl serial(i, numMisoReps, m_SurfaceMeshFaceLabels, m_SurfaceMeshFaceNormals, m_FeatureEulerAngles, m_FeaturePhases, m_CrystalStructures, m_GbcdBinsArray, m_GbcdHemiCheckArray, m_GbcdDeltasArray, m_GbcdSizesArray, m_GbcdLimitsArray);
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
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

    for(int j = 0; j < faceChunkSize; j++)
    {
      for(int k = 0; k < numMisoReps; k++)
      {
        if(m_GbcdBins[(j * numMisoReps) + (k)] >= 0)
        {
          hemisphere = 0;
          if(m_HemiCheck[(j * numMisoReps) + k] == false) { hemisphere = 1; }
          m_GBCD[2 * m_GbcdBins[(j * numMisoReps) + (k)] + hemisphere] += (m_SurfaceMeshFaceAreas[i + j]);
          totalFaceArea += (m_SurfaceMeshFaceAreas[i + j]);
        }
      }
    }
  }

  ss = QObject::tr("Starting GBCD Normalization");
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

  int totalBins = m_GbcdSizes[0] * m_GbcdSizes[1] * m_GbcdSizes[2] * m_GbcdSizes[3] * m_GbcdSizes[4] * 2;
  double MRDfactor = double(totalBins) / totalFaceArea;
  for(int i = 0; i < totalBins; i++)
  {
    m_GBCD[2 * i + 0] *= MRDfactor;
    m_GBCD[2 * i + 1] *= MRDfactor;
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::sizeGBCD(int faceChunkSize, int numMisoReps)
{
  m_GbcdDeltasArray = FloatArrayType::CreateArray(5, "GBCDDeltas");
  m_GbcdDeltasArray->initializeWithZeros();
  m_GbcdLimitsArray = FloatArrayType::CreateArray(10, "GBCDLimits");
  m_GbcdLimitsArray->initializeWithZeros();
  m_GbcdSizesArray = Int32ArrayType::CreateArray(5, "GBCDSizes");
  m_GbcdSizesArray->initializeWithZeros();
  QVector<size_t> dims(1, numMisoReps);
  m_GbcdBinsArray = Int32ArrayType::CreateArray(faceChunkSize, dims, "GBCDBins");
  m_GbcdBinsArray->initializeWithZeros();
  m_GbcdHemiCheckArray = BoolArrayType::CreateArray(faceChunkSize, dims, "GBCDHemiCheck");
  m_GbcdHemiCheckArray->initializeWithValue(false);

  m_GbcdDeltas = m_GbcdDeltasArray->getPointer(0);
  m_GbcdSizes = m_GbcdSizesArray->getPointer(0);
  m_GbcdLimits = m_GbcdLimitsArray->getPointer(0);
  m_GbcdBins = m_GbcdBinsArray->getPointer(0);
  m_HemiCheck = m_GbcdHemiCheckArray->getPointer(0);

  //Original Ranges from Dave R.
  //m_GBCDlimits[0] = 0.0;
  //m_GBCDlimits[1] = cosf(1.0*m_pi);
  //m_GBCDlimits[2] = 0.0;
  //m_GBCDlimits[3] = 0.0;
  //m_GBCDlimits[4] = cosf(1.0*m_pi);
  //m_GBCDlimits[5] = 2.0*m_pi;
  //m_GBCDlimits[6] = cosf(0.0);
  //m_GBCDlimits[7] = 2.0*m_pi;
  //m_GBCDlimits[8] = 2.0*m_pi;
  //m_GBCDlimits[9] = cosf(0.0);

  //Greg's Ranges
  m_GbcdLimits[0] = 0.0;
  m_GbcdLimits[1] = 0.0;
  m_GbcdLimits[2] = 0.0;
  m_GbcdLimits[3] = 0.0;
  m_GbcdLimits[4] = 0.0;
  m_GbcdLimits[5] = DREAM3D::Constants::k_PiOver2;
  m_GbcdLimits[6] = 1.0;
  m_GbcdLimits[7] = DREAM3D::Constants::k_PiOver2;
  m_GbcdLimits[8] = 1.0;
  m_GbcdLimits[9] = DREAM3D::Constants::k_2Pi;

  float binsize = m_GBCDRes * DREAM3D::Constants::k_PiOver180;
  float binsize2 = binsize * (2.0 / DREAM3D::Constants::k_Pi);
  m_GbcdDeltas[0] = binsize;
  m_GbcdDeltas[1] = binsize2;
  m_GbcdDeltas[2] = binsize;
  m_GbcdDeltas[3] = binsize2;
  m_GbcdDeltas[4] = binsize;

  m_GbcdSizes[0] = int(0.5 + (m_GbcdLimits[5] - m_GbcdLimits[0]) / m_GbcdDeltas[0]);
  m_GbcdSizes[1] = int(0.5 + (m_GbcdLimits[6] - m_GbcdLimits[1]) / m_GbcdDeltas[1]);
  m_GbcdSizes[2] = int(0.5 + (m_GbcdLimits[7] - m_GbcdLimits[2]) / m_GbcdDeltas[2]);
  m_GbcdSizes[3] = int(0.5 + (m_GbcdLimits[8] - m_GbcdLimits[3]) / m_GbcdDeltas[3]);
  m_GbcdSizes[4] = int(0.5 + (m_GbcdLimits[9] - m_GbcdLimits[4]) / m_GbcdDeltas[4]);

  //reset the 3rd and 4th dimensions using the square grid approach
  float totalNormalBins = m_GbcdSizes[3] * m_GbcdSizes[4];
  m_GbcdSizes[3] = int(sqrt(totalNormalBins) + 0.5);
  m_GbcdSizes[4] = int(sqrt(totalNormalBins) + 0.5);
  m_GbcdLimits[3] = -sqrt(DREAM3D::Constants::k_PiOver2);
  m_GbcdLimits[4] = -sqrt(DREAM3D::Constants::k_PiOver2);
  m_GbcdLimits[8] = sqrt(DREAM3D::Constants::k_PiOver2);
  m_GbcdLimits[9] = sqrt(DREAM3D::Constants::k_PiOver2);
  m_GbcdDeltas[3] = (m_GbcdLimits[8] - m_GbcdLimits[3]) / float(m_GbcdSizes[3]);
  m_GbcdDeltas[4] = (m_GbcdLimits[9] - m_GbcdLimits[4]) / float(m_GbcdSizes[4]);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindGBCD::newFilterInstance(bool copyFilterParameters)
{
  FindGBCD::Pointer filter = FindGBCD::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindGBCD::getCompiledLibraryName()
{ return SurfaceMeshing::SurfaceMeshingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindGBCD::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindGBCD::getSubGroupName()
{ return DREAM3D::FilterSubGroups::CrystallographicFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindGBCD::getHumanLabel()
{ return "Find GBCD"; }

