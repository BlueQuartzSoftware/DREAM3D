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
* This code was written under United States Air Force Contract number
* FA8650-07-D-5800
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "MergeArrays.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MergeArrays::MergeArrays() :
  AbstractFilter(),
  //m_ScalarType(),
  m_GoodValesArrayName(""),
  m_BadValesArrayName(""),
  m_FlagArrayName(""),
  m_Flag(NULL),
  m_NewCellArrayName("")
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MergeArrays::~MergeArrays()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeArrays::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("'Good' Values Source Array");
    parameter->setPropertyName("GoodValesArrayName");
    parameter->setWidgetType(FilterParameter::VoxelCellArrayNameSelectionWidget);
    parameter->setValueType("string");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("'Bad' Values Source Array");
    parameter->setPropertyName("BadValesArrayName");
    parameter->setWidgetType(FilterParameter::VoxelCellArrayNameSelectionWidget);
    parameter->setValueType("string");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("GoodVoxels (flag) Array");
    parameter->setPropertyName("FlagArrayName");
    parameter->setWidgetType(FilterParameter::VoxelCellArrayNameSelectionWidget);
    parameter->setValueType("string");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Created Array Name");
    parameter->setPropertyName("NewCellArrayName");
    parameter->setWidgetType(FilterParameter::StringWidget);
    parameter->setValueType("string");
    parameters.push_back(parameter);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeArrays::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setGoodValesArrayName( reader->readValue( "GoodValesArrayName", getGoodValesArrayName() ) );
  setBadValesArrayName( reader->readValue( "BadValesArrayName", getBadValesArrayName() ) );
  setFlagArrayName( reader->readValue( "FlagArrayName", getFlagArrayName() ) );
  setNewCellArrayName( reader->readValue( "NewCellArrayName", getNewCellArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MergeArrays::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("GoodValesArrayName", getGoodValesArrayName() );
  writer->writeValue("BadValesArrayName", getBadValesArrayName() );
  writer->writeValue("FlagArrayName", getFlagArrayName() );
  writer->writeValue("NewCellArrayName", getNewCellArrayName() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeArrays::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  if(m_GoodValesArrayName.empty() == true)
  {
    ss.str("");
    ss << "The 'Good' Values Source Array Name is blank (empty).";
    setErrorCondition(-397);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }
  if(m_BadValesArrayName.empty() == true)
  {
    ss.str("");
    ss << "The 'Bad' Values Source Array is blank (empty).";
    setErrorCondition(-397);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }
  if(m_FlagArrayName.empty() == true)
  {
    ss.str("");
    ss << "The 'Flag' (GoodVoxels) Array is blank (empty).";
    setErrorCondition(-397);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }
  else
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, Flag, ss, -300, bool, BoolArrayType, voxels, 1)
  }

  //get input arrays
  IDataArray::Pointer pGoodArray = m->getCellData(m_GoodValesArrayName);
  IDataArray::Pointer pBadArray = m->getCellData(m_BadValesArrayName);

  //get # components
  int goodNumComp = 0;
  int badNumComp = 0;
  int flagNumComp = 0;
  if(NULL!=pGoodArray)
  {
    goodNumComp=pGoodArray->GetNumberOfComponents();//3 for euler angles, 4 for quats etc
  }
  if(NULL!=pBadArray)
  {
    badNumComp=pBadArray->GetNumberOfComponents();//3 for euler angles, 4 for quats etc
  }

  if(goodNumComp!=badNumComp)
  {
    ss.str("");
    ss << "'Good' and 'Bad' Value Source Array component size mismatch (" << goodNumComp <<" vs. " << badNumComp <<")";
    setErrorCondition(-100);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }

  if (true == preflight)
  {
    IDataArray::Pointer p = IDataArray::NullPointer();
    p=UInt8ArrayType::CreateArray(voxels, goodNumComp, m_NewCellArrayName);
    m->addCellData(m_NewCellArrayName, p);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeArrays::preflight()
{
  dataCheck(true, 1, 1, 1);
}

template<typename T>
void MergeData(IDataArray::Pointer goodData, IDataArray::Pointer badData, VoxelDataContainer* m, bool* flagArray, std::string arrayName)
{
  typename T::Pointer pGood = boost::dynamic_pointer_cast<T>(goodData);
  typename T::Pointer pBad = boost::dynamic_pointer_cast<T>(badData);
  int numberOfComponents = pGood->GetNumberOfComponents();
  int voxels = pGood->GetNumberOfTuples();
  size_t size = pGood->GetSize();

  typename T::Pointer p = T::CreateArray(voxels, numberOfComponents, arrayName);
  m->addCellData(p->GetName(), p);

  //get number of components so we can stride flagArray correctly
  int numCom = size/voxels;
  for(size_t i=0; i<voxels; ++i)
  {
    int index=i*numCom;
    if(1==flagArray[i])
    {
      for(int j=0; j<numCom; ++j)
      {
        p->SetValue(index+j, pGood->GetValue(index+j) );
      }
    }
    else
    {
      for(int j=0; j<numCom; ++j)
      {
        p->SetValue(index+j, pBad->GetValue(index+j) );
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeArrays::execute()
{
  int32_t err =0;
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  int64_t totalPoints = m->getTotalPoints();
  size_t numgrains = m->getNumFieldTuples();
  size_t numensembles = m->getNumEnsembleTuples();
  dataCheck(false, totalPoints, numgrains, numensembles);
  if (getErrorCondition() < 0)
  {
    return;
  }


  IDataArray::Pointer pGoodArray = m->getCellData(m_GoodValesArrayName);
  IDataArray::Pointer pBadArray = m->getCellData(m_BadValesArrayName);

  std::string goodType = pGoodArray->getTypeAsString();
  std::string badType = pBadArray->getTypeAsString();
  if(0!=goodType.compare(badType))
  {
    std::stringstream ss;
    ss.str("");
    ss << "Type mismatch: "<<pGoodArray->GetName() <<" (" <<goodType<<") vs. "<<pBadArray->GetName()<<" ("<<badType<<")";
    setErrorCondition(-800);
    notifyErrorMessage(ss.str(), -800);
    return;
  }

  if(0==goodType.compare("bool"))
  {
    MergeData<BoolArrayType>(pGoodArray, pBadArray, m, m_Flag, m_NewCellArrayName);
  }
  else if (0==goodType.compare("int8_t"))
  {
    MergeData<Int8ArrayType>(pGoodArray, pBadArray, m, m_Flag, m_NewCellArrayName);
  }
  else if(0==goodType.compare("uint8_t"))
  {
    MergeData<UInt8ArrayType>(pGoodArray, pBadArray, m, m_Flag, m_NewCellArrayName);
  }
  else if(0==goodType.compare("int16_t"))
  {
    MergeData<Int16ArrayType>(pGoodArray, pBadArray, m, m_Flag, m_NewCellArrayName);
  }
  else if(0==goodType.compare("uint16_t"))
  {
    MergeData<UInt16ArrayType>(pGoodArray, pBadArray, m, m_Flag, m_NewCellArrayName);
  }
  else if(0==goodType.compare("int32_t"))
  {
    MergeData<Int32ArrayType>(pGoodArray, pBadArray, m, m_Flag, m_NewCellArrayName);
  }
  else if(0==goodType.compare("uint32_t"))
  {
    MergeData<UInt32ArrayType>(pGoodArray, pBadArray, m, m_Flag, m_NewCellArrayName);
  }
  else if(0==goodType.compare("int64_t"))
  {
    MergeData<Int64ArrayType>(pGoodArray, pBadArray, m, m_Flag, m_NewCellArrayName);
  }
  else if(0==goodType.compare("uint64_t"))
  {
    MergeData<UInt64ArrayType>(pGoodArray, pBadArray, m, m_Flag, m_NewCellArrayName);
  }
  else if(0==goodType.compare("float"))
  {
       MergeData<FloatArrayType>(pGoodArray, pBadArray, m, m_Flag, m_NewCellArrayName);
  }
  else if(0==goodType.compare("double"))
  {
    MergeData<DoubleArrayType>(pGoodArray, pBadArray, m, m_Flag, m_NewCellArrayName);
  }

  notifyStatusMessage("Complete");
}
