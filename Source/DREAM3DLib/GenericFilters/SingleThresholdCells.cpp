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
#include "SingleThresholdCells.h"

#include "DREAM3DLib/Common/Constants.h"


/**
 * @brief The ThresholdFilter class
 */
class ThresholdFilter : public IDataArrayFilter
{
  public:
    ThresholdFilter(DREAM3D::Comparison::Enumeration compType, double compValue, BoolArrayType* output) :
      comparisonOperator(compType),
      comparisonValue(compValue),
      m_Output(output)
    {}

    virtual ~ThresholdFilter()
    {}

    template<typename T>
    void filterDataLessThan(IDataArray* m_Input)
    {
      size_t m_NumValues = m_Input->GetNumberOfTuples();
      T v = static_cast<T>(comparisonValue);
      T* data = IDataArray::SafeReinterpretCast<IDataArray*, DataArray<T>*, T*>(m_Input);
      for (size_t i = 0; i < m_NumValues; ++i)
      {
        bool b = (data[i] < v);
        m_Output->SetValue(i, b);
      }
    }

    template<typename T>
    void filterDataGreaterThan(IDataArray* m_Input)
    {
      size_t m_NumValues = m_Input->GetNumberOfTuples();
      T v = static_cast<T>(comparisonValue);
      T* data = IDataArray::SafeReinterpretCast<IDataArray*, DataArray<T>*, T*>(m_Input);
      for (size_t i = 0; i < m_NumValues; ++i)
      {
        bool b = (data[i] > v);
        m_Output->SetValue(i, b);
      }
    }

    template<typename T>
    void filterDataEqualTo(IDataArray* m_Input)
    {
      size_t m_NumValues = m_Input->GetNumberOfTuples();
      T v = static_cast<T>(comparisonValue);
      T* data = IDataArray::SafeReinterpretCast<IDataArray*, DataArray<T>*, T*>(m_Input);
      for (size_t i = 0; i < m_NumValues; ++i)
      {
        bool b = (data[i] == v);
        m_Output->SetValue(i, b);
      }
    }


#define FILTER_DATA_HELPER(dType, ops, Type) \
  if (dType.compare(#Type) == 0) {\
  if (ops == DREAM3D::Comparison::Operator_LessThan) filterDataLessThan<Type>(input);\
  else if (ops == DREAM3D::Comparison::Operator_GreaterThan) filterDataGreaterThan<Type>(input);\
  else if (ops == DREAM3D::Comparison::Operator_Equal) filterDataEqualTo<Type>(input);\
  return 1;\
  }


    int execute(IDataArray *input, IDataArray *output)
    {
      m_Output->initializeWithZeros();
      std::string dType = input->getTypeAsString();

      FILTER_DATA_HELPER(dType, comparisonOperator, float);
      FILTER_DATA_HELPER(dType, comparisonOperator, double);

      FILTER_DATA_HELPER(dType, comparisonOperator, int8_t);
      FILTER_DATA_HELPER(dType, comparisonOperator, uint8_t);
# if CMP_TYPE_CHAR_IS_SIGNED
      FILTER_DATA_HELPER(dType, comparisonOperator, char);
#else
      FILTER_DATA_HELPER(dType, comparisonOperator, char);
#endif
      FILTER_DATA_HELPER(dType, comparisonOperator, signed char);
      FILTER_DATA_HELPER(dType, comparisonOperator, unsigned char);


      FILTER_DATA_HELPER(dType, comparisonOperator, int16_t);
      FILTER_DATA_HELPER(dType, comparisonOperator, short);
      FILTER_DATA_HELPER(dType, comparisonOperator, signed short);
      FILTER_DATA_HELPER(dType, comparisonOperator, uint16_t);
      FILTER_DATA_HELPER(dType, comparisonOperator, unsigned short);


      FILTER_DATA_HELPER(dType, comparisonOperator, int32_t);
      FILTER_DATA_HELPER(dType, comparisonOperator, uint32_t);
#if (CMP_SIZEOF_INT == 4)
      FILTER_DATA_HELPER(dType, comparisonOperator, int);
      FILTER_DATA_HELPER(dType, comparisonOperator, signed int);
      FILTER_DATA_HELPER(dType, comparisonOperator, unsigned int);
#endif


#if (CMP_SIZEOF_LONG == 4)
      FILTER_DATA_HELPER(dType, comparisonOperator, long int);
      FILTER_DATA_HELPER(dType, comparisonOperator, signed long int);
      FILTER_DATA_HELPER(dType, comparisonOperator, unsigned long int);
#elif (CMP_SIZEOF_LONG == 8)
      FILTER_DATA_HELPER(dType, comparisonOperator, long int);
      FILTER_DATA_HELPER(dType, comparisonOperator, signed long int);
      FILTER_DATA_HELPER(dType, comparisonOperator, unsigned long int);
#endif


#if (CMP_SIZEOF_LONG_LONG == 8)
      FILTER_DATA_HELPER(dType, comparisonOperator, long long int);
      FILTER_DATA_HELPER(dType, comparisonOperator, signed long long int);
      FILTER_DATA_HELPER(dType, comparisonOperator, unsigned long long int);
#endif
      FILTER_DATA_HELPER(dType, comparisonOperator, int64_t);
      FILTER_DATA_HELPER(dType, comparisonOperator, uint64_t);

      FILTER_DATA_HELPER(dType, comparisonOperator, bool);

      return -1;
    }

  private:
    DREAM3D::Comparison::Enumeration comparisonOperator;
    double comparisonValue;
    BoolArrayType* m_Output;


    ThresholdFilter(const ThresholdFilter&); // Copy Constructor Not Implemented
    void operator=(const ThresholdFilter&); // Operator '=' Not Implemented
};




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SingleThresholdCells::SingleThresholdCells():
  AbstractFilter(),
  m_SelectedCellArrayName(""),
  m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
  m_GoodVoxels(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SingleThresholdCells::~SingleThresholdCells()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SingleThresholdCells::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;

  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input Cell Array Name");
    option->setPropertyName("SelectedCellArrayName");
    option->setWidgetType(FilterParameter::VoxelCellArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    parameters.push_back(option);
  }
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Comparison Operator");
    option->setPropertyName("ComparisonOperator");
    option->setWidgetType(FilterParameter::ChoiceWidget);
    option->setValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back(DREAM3D::Comparison::Strings::LessThan);
    choices.push_back(DREAM3D::Comparison::Strings::GreaterThan);
    choices.push_back(DREAM3D::Comparison::Strings::Equal);
    option->setChoices(choices);
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Value");
    option->setPropertyName("ComparisonValue");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("double");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SingleThresholdCells::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("SelectedCellArrayName", getSelectedCellArrayName() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SingleThresholdCells::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GoodVoxels, ss, bool, BoolArrayType, true, voxels, 1)

      if(m_SelectedCellArrayName.empty() == true)
  {
    setErrorCondition(-11000);
    notifyErrorMessage("An array from the Voxel Data Container must be selected.", getErrorCondition());
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SingleThresholdCells::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SingleThresholdCells::execute()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }
  //int err = 0;
  std::stringstream ss;

  IDataArray::Pointer inputData = m->getCellData(m_SelectedCellArrayName);
  if (NULL == inputData.get())
  {
    ss.str("");
    ss << "Selected array '" << m_SelectedCellArrayName << "' does not exist in the Voxel Data Container. Was it spelled correctly?";
    setErrorCondition(-11001);
    notifyErrorMessage(ss.str(), getErrorCondition());
    return;
  }

  IDataArray::Pointer goodVoxelsPtr = m->getCellData(m_GoodVoxelsArrayName);
  BoolArrayType* goodVoxels = BoolArrayType::SafeObjectDownCast<IDataArray*, BoolArrayType*>(goodVoxelsPtr.get());
  if (NULL == goodVoxels)
  {
    setErrorCondition(-11002);
    notifyErrorMessage("Could not properly cast the output array to a BoolArrayType", getErrorCondition());
    return;
  }


  ThresholdFilter filter(static_cast<DREAM3D::Comparison::Enumeration>(m_ComparisonOperator), m_ComparisonValue, goodVoxels);

  filter.execute(inputData.get(), goodVoxelsPtr.get());


  m->addCellData(goodVoxelsPtr->GetName(), goodVoxelsPtr);
  notifyStatusMessage("Determine Good Voxels Complete");
}

