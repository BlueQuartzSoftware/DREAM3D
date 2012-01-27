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

#include <limits>
#include "LoadVolume.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/GenericFilters/DataContainerReader.h"

const static float m_pi = static_cast<float>(M_PI);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LoadVolume::LoadVolume() :
AbstractFilter(),
m_GrainIds(NULL),
m_PhasesC(NULL),
m_PhasesF(NULL),
m_NumCells(NULL),
m_Active(NULL),
m_EulerAngles(NULL)
{
  setupFilterOptions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LoadVolume::~LoadVolume()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadVolume::setupFilterOptions()
{
  std::vector<FilterOption::Pointer> options;
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Input File");
    option->setPropertyName("InputFile");
    option->setWidgetType(FilterOption::InputFileWidget);
    option->setValueType("string");
    options.push_back(option);
  }
  setFilterOptions(options);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadVolume::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
//  DataContainer* m = getDataContainer();


  setErrorMessage(ss.str());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadVolume::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadVolume::execute()
{
  DataContainer* m = getDataContainer();
  if (NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }
  setErrorCondition(0);

  int err = 0;

  DataContainerReader::Pointer h5Reader = DataContainerReader::New();
  h5Reader->setInputFile(m_InputFile);
  h5Reader->setDataContainer(m);
  size_t dcDims[3];
  float spacing[3];
  float origin[3];
  h5Reader->execute();
  err = h5Reader->getErrorCondition();
  if (err < 0)
  {
    setErrorCondition(err);
    setErrorMessage(h5Reader->getErrorMessage());
    return;
  }
  m->getDimensions(dcDims);
  m->getResolution(spacing);
  m->getOrigin(origin);

  int64_t dims[3] = {dcDims[0], dcDims[1], dcDims[2]};

  /* Sanity check what we are trying to load to make sure it can fit in our address space.
   * Note that this does not guarantee the user has enough left, just that the
   * size of the volume can fit in the address space of the program
   */
#if   (CMP_SIZEOF_SSIZE_T==4)
    int64_t max = std::numeric_limits<size_t>::max();
#else
    int64_t max = std::numeric_limits<int64_t>::max();
#endif
  if (dims[0] * dims[1] * dims[2] > max )
  {
    err = -1;
    std::stringstream s;
    s << "The total number of elements '" << (dims[0] * dims[1] * dims[2])
                << "' is greater than this program can hold. Try the 64 bit version.";
    setErrorCondition(err);
    setErrorMessage(s.str());
    return;
  }

  if (dims[0] > max || dims[1] > max || dims[2] > max)
  {
    err = -1;
    std::stringstream s;
    s << "One of the dimensions is greater than the max index for this sysem. Try the 64 bit version.";
    s << " dim[0]="<< dims[0] << "  dim[1]="<<dims[1] << "  dim[2]=" << dims[2];
    setErrorCondition(err);
    setErrorMessage(s.str());
    return;
  }
  /* ************ End Sanity Check *************************** */

  int64_t totalPoints = m->totalPoints();

  initializeAttributes();
  if (getErrorCondition() < 0)
  {
    return;
  }


  if (err < 0)
  {
    setErrorCondition(err);
    setErrorMessage("Error Reading Voxel Data from input file");
    notify(getErrorMessage().c_str(), 0, Observable::UpdateProgressMessage);
    return;
  }

  typedef DataArray<Ebsd::CrystalStructure> XTalType;
  XTalType* crystructPtr
      = XTalType::SafeObjectDownCast<IDataArray*, XTalType*>(m->getEnsembleData(DREAM3D::EnsembleData::CrystalStructure).get());
  Ebsd::CrystalStructure* crystruct = crystructPtr->GetPointer(0);
  size_t numXTals = crystructPtr->GetNumberOfTuples();

  FloatArrayType::Pointer phaseFractions = FloatArrayType::CreateArray(numXTals);
  phaseFractions->SetName(DREAM3D::EnsembleData::PhaseFractions);
  m->addEnsembleData(DREAM3D::EnsembleData::PhaseFractions, phaseFractions);
 // m_PhaseFractions = phaseFractions->GetPointer(0);


  FloatArrayType::Pointer pptFractions = FloatArrayType::CreateArray(numXTals);
  pptFractions->SetName(DREAM3D::EnsembleData::PrecipitateFractions);
  m->addEnsembleData(DREAM3D::EnsembleData::PrecipitateFractions, pptFractions);
 // m_PrecipitateFractions = pptFractions->GetPointer(0);


  initializeGrains();
  if (getErrorCondition() < 0)
  {
    return;
  }
  notify("LoadVolume Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadVolume::initializeGrains()
{
  /*
   * This assumes a dense packed grain list which probably isn't that safe based
   * on some of the data that is floating around. For our data though this is a
   * reasonable assumption.
   */
  // Put at least 1 Grain in the Vector
  DataContainer* m = getDataContainer();
  m->resizeFieldDataArrays(1);
  dataCheck(false, m->totalPoints(), m->getTotalFields(), m->getNumEnsembleTuples());
  if(getErrorCondition() < 0)
  {
    return;
  }

//  size_t curGrainSize = 1;
  size_t grainIndex = 0;
  int64_t totalPoints = m->totalPoints();
  size_t totalGrains = 0;
  for (int i = 0; i < totalPoints; ++i)
  {
    grainIndex = m_GrainIds[i];
    //  curGrainSize = m->getTotalFields();
    if(grainIndex >= totalGrains)
    {
      ++totalGrains;
    }
  }
  // Now we know how many grains there are, lets do some initilization
  m->resizeFieldDataArrays(totalGrains);
  std::stringstream ss;
  CREATE_NON_PREREQ_DATA_SUFFIX(m, DREAM3D, FieldData, Phases, F, ss, int32_t, Int32ArrayType, totalGrains, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, NumCells, ss, int32_t, Int32ArrayType, totalGrains, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, bool, BoolArrayType, totalGrains, 1);

  for (int i = 0; i < totalPoints; ++i)
  {
    grainIndex = m_GrainIds[i];

    m_PhasesF[grainIndex] = m_PhasesC[i];
    m_NumCells[grainIndex]++;
    m_Active[grainIndex] = true;
  }

  // Loop over the Grains and initialize them as necessary
  size_t gSize = m->getTotalFields();
  for (size_t g = 0; g < gSize; ++g)
  {
    //FIXME: Initialize any new grains that were added
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadVolume::initializeAttributes()
{
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->totalPoints();

  m_GrainIds = m->createCellData<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::CellData::GrainIds, totalPoints, 1, this);
  if (m_GrainIds == NULL) { return; }
  m_PhasesC = m->createCellData<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::CellData::Phases, totalPoints, 1, this);
  if (m_PhasesC == NULL) { return; }
  m_EulerAngles = m->createCellData<float, FloatArrayType, AbstractFilter>(DREAM3D::CellData::EulerAngles, 3*totalPoints, 1, this);
  if (NULL == m_EulerAngles) {return;}

  for (int i = 0; i < totalPoints; ++i)
  {
    m_GrainIds[i] = 0;
	  m_PhasesC[i] = 0;
    m_EulerAngles[3*i] = -1.0f;
    m_EulerAngles[3*i + 1] = -1.0f;
    m_EulerAngles[3*i + 2] = -1.0f;
  }

}


