/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "FindBoundaryStrengths.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"

#include "MXA/Common/MXAEndian.h"
#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/MXADir.h"


const static float m_pi = static_cast<float>(M_PI);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBoundaryStrengths::FindBoundaryStrengths() :
AbstractFilter(),
m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
m_SchmidsArrayName(DREAM3D::FieldData::Schmids),
m_SlipSystemsArrayName(DREAM3D::FieldData::SlipSystems),
m_FieldPhases(NULL),
m_SlipSystems(NULL),
m_AvgQuats(NULL),
m_Schmids(NULL)
{
  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_HexOps.get()));

  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_CubicOps.get()));

  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_OrthoOps.get()));
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBoundaryStrengths::~FindBoundaryStrengths()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryStrengths::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("VTK Output File");
    option->setPropertyName("vtkOutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void FindBoundaryStrengths::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("vtkOutputFile", getvtkOutputFile() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryStrengths::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  int err = 0;

  GET_PREREQ_DATA(m, DREAM3D, FieldData, Schmids, ss, -305, float, FloatArrayType, fields, 1)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, SlipSystems, ss, -306, int32_t, Int32ArrayType, fields, 1)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -301, float, FloatArrayType, fields, 5)
  TEST_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, err, -302, int32_t, Int32ArrayType, fields, 1)
  if(err == -302)
  {
    setErrorCondition(0);
    FindGrainPhases::Pointer find_grainphases = FindGrainPhases::New();
    find_grainphases->setObservers(this->getObservers());
    find_grainphases->setVoxelDataContainer(getVoxelDataContainer());
    if(preflight == true) find_grainphases->preflight();
    if(preflight == false) find_grainphases->execute();
  }
  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -302, int32_t, Int32ArrayType, fields, 1)

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryStrengths::preflight()
{
  dataCheck(true, 1,1 ,1);
}


class ScopedFileMonitor
{
  public:
    ScopedFileMonitor(FILE* f) : m_File(f) {}
    virtual ~ScopedFileMonitor() { fclose(m_File);}
  private:
    FILE* m_File;
    ScopedFileMonitor(const ScopedFileMonitor&); // Copy Constructor Not Implemented
    void operator=(const ScopedFileMonitor&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryStrengths::execute()
{
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == m || NULL == sm)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  int64_t totalPoints = m->getTotalPoints();
  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  typedef DataArray<unsigned int> XTalType;
  XTalType* crystruct
      = XTalType::SafeObjectDownCast<IDataArray*, XTalType*>(m->getEnsembleData(DREAM3D::EnsembleData::CrystalStructures).get());

//  size_t numXTals = crystruct->GetNumberOfTuples();


  StructArray<Node>::Pointer nodesPtr = sm->getNodes();
  StructArray<Node>& nodes = *(nodesPtr);
  int nNodes = nodes.GetNumberOfTuples();

  // Write the triangle indices into the vtk File
  StructArray<Triangle>& triangles = *(sm->getTriangles());
  int nTriangles = triangles.GetNumberOfTuples();

  float w, n1, n2, n3;
  float sf1, sf2, ssap;
  int gname1, gname2, ss1, ss2;
  float q1[5], q2[5];

  std::vector<float> mPrimes(nTriangles,0.0);

  for (int i = 0; i < nTriangles; i++)
  {
      Triangle& t = triangles[i]; // Get the current Node
	  gname1 = t.nSpin[0];
      gname2 = t.nSpin[1];
      ss1 = m_SlipSystems[gname1];
      ss2 = m_SlipSystems[gname2];
      for (int j = 0; j < 5; j++)
      {
        q1[j] = m_AvgQuats[5 * gname1 + j];
        q2[j] = m_AvgQuats[5 * gname2 + j];
      }
//      OrientationMath::getSlipMisalignment(ss1, q1, q2, ssap);
      if(crystruct->GetValue(m_FieldPhases[gname1]) == crystruct->GetValue(m_FieldPhases[gname2])
          && m_FieldPhases[gname1] > 0)
      {
		m_OrientationOps[crystruct->GetValue(m_FieldPhases[gname1])]->getSlipMisalignment(ss1, ss2, q1, q2, ssap);
      }
      else
      {
		ssap = 0;
      }
	  mPrimes[i] = ssap;
  }

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  std::string parentPath = MXAFileInfo::parentPath(getvtkOutputFile());
  if(!MXADir::mkdir(parentPath, true))
  {
      ss.str("");
      ss << "Error creating parent path '" << parentPath << "'";
      notifyErrorMessage(ss.str(), -1);
      setErrorCondition(-1);
      return;
  }

// Open the output VTK File for writing
  FILE* vtkFile = NULL;
  vtkFile = fopen(getvtkOutputFile().c_str(), "wb");
  if (NULL == vtkFile)
  {
      ss.str("");
      ss << "Error creating file '" << getvtkOutputFile() << "'";
      notifyErrorMessage(ss.str(), -18542);
      setErrorCondition(-18542);
      return;
  }
  ScopedFileMonitor vtkFileMonitor(vtkFile);

  fprintf(vtkFile, "# vtk DataFile Version 2.0\n");
  fprintf(vtkFile, "Data set from DREAM.3D Surface Meshing Module\n");
  fprintf(vtkFile, "BINARY\n");
  fprintf(vtkFile, "DATASET POLYDATA\n");

  fprintf(vtkFile, "POINTS %d float\n", nNodes);

  float pos[3] = {0.0f, 0.0f, 0.0f};

  size_t totalWritten = 0;
  // Write the POINTS data (Vertex)
  for (int i = 0; i < nNodes; i++)
  {
    Node& n = nodes[i]; // Get the current Node
    pos[0] = static_cast<float>(n.coord[0]);
    pos[1] = static_cast<float>(n.coord[1]);
    pos[2] = static_cast<float>(n.coord[2]);
    MXA::Endian::FromSystemToBig::convert<float>(pos[0]);
    MXA::Endian::FromSystemToBig::convert<float>(pos[1]);
    MXA::Endian::FromSystemToBig::convert<float>(pos[2]);
    totalWritten = fwrite(pos, sizeof(float), 3, vtkFile);
    if (totalWritten != sizeof(float) * 3)
    {

    }
  }

  int tData[4];
  int nT = triangles.GetNumberOfTuples();
  int triangleCount = nT;
//  int tn1, tn2, tn3;
  triangleCount = nT * 2;
  // Write the CELLS Data
  fprintf(vtkFile, "POLYGONS %d %d\n", triangleCount, (triangleCount * 4));
  for (int j = 0; j < nT; j++)
  {
  //  Triangle& t = triangles[j];
    tData[1] = triangles[j].node_id[0];
    tData[2] = triangles[j].node_id[1];
    tData[3] = triangles[j].node_id[2];

    tData[0] = 3; // Push on the total number of entries for this entry
    MXA::Endian::FromSystemToBig::convert<int>(tData[0]);
    MXA::Endian::FromSystemToBig::convert<int>(tData[1]); // Index of Vertex 0
    MXA::Endian::FromSystemToBig::convert<int>(tData[2]); // Index of Vertex 1
    MXA::Endian::FromSystemToBig::convert<int>(tData[3]); // Index of Vertex 2
    fwrite(tData, sizeof(int), 4, vtkFile);
    tData[0] = tData[1];
    tData[1] = tData[3];
    tData[3] = tData[0];
    tData[0] = 3;
    MXA::Endian::FromSystemToBig::convert<int>(tData[0]);
    fwrite(tData, sizeof(int), 4, vtkFile);
  }

  // Write the Data to the file
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "CELL_DATA %d\n", triangleCount);
  fprintf(vtkFile, "SCALARS GrainIds int 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");

  for(int i = 0; i < nT; ++i)
  {
    Triangle& t = triangles[i]; // Get the current Node

    int Gid = t.nSpin[0];
    MXA::Endian::FromSystemToBig::convert<int>(Gid);
    fwrite(&Gid, sizeof(int), 1, vtkFile);
    Gid = t.nSpin[1];
    MXA::Endian::FromSystemToBig::convert<int>(Gid);
    fwrite(&Gid, sizeof(int), 1, vtkFile);
    }

  fprintf(vtkFile, "\n");

  // Write the Data to the file
//  fprintf(vtkFile, "\n");
//  fprintf(vtkFile, "CELL_DATA %d\n", triangleCount);
  fprintf(vtkFile, "SCALARS mPrime float\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");

  for(int i = 0; i < nT; ++i)
  {
    Triangle& t = triangles[i]; // Get the current Node

    float mPrime = mPrimes[i];
    MXA::Endian::FromSystemToBig::convert<float>(mPrime);
    fwrite(&mPrime, sizeof(float), 1, vtkFile);
    mPrime = mPrimes[i];
    MXA::Endian::FromSystemToBig::convert<float>(mPrime);
    fwrite(&mPrime, sizeof(float), 1, vtkFile);
    }

  fprintf(vtkFile, "\n");

  notifyStatusMessage("Completed");
}
