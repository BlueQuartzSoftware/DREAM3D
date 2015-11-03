/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                               *
 * Copyright (c) 2015 The Regents of the University of California, Author: William Lenthe        *
 *                                                                                               *
 * This program is free software: you can redistribute it and/or modify it under the terms of    *
 * the GNU Lesser General Public License as published by the Free Software Foundation, either    *
 * version 2 of the License, or (at your option) any later version.                              *
 *                                                                                               *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;     *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     *
 * See the GNU Lesser General Public License for more details.                                   *
 *                                                                                               *
 * You should have received a copy of the GNU Lesser General Public License                      *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.                         *
 *                                                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "ArrayCalculator.h"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "LGPLToolbox/LGPLToolboxConstants.h"

#include "ExpressionParser.hpp"



/**
 * @brief The ArrayCalculatorImpl class implements a threaded algorithm that 
 * evaluates an expression for each element in a geometry (templated on output type)
 */
template <typename T>
class ArrayCalculatorImpl
{
  public:
    ArrayCalculatorImpl(AttributeMatrix::Pointer p, QList<QString> names, std::string expr, T* out) :
      m_attrMat(p), m_arrayNames(names), m_expression(expr), m_output(out) {}
    virtual ~ArrayCalculatorImpl() {}

    class TPointer {
      public:
        TPointer(IDataArray::Pointer p) {
          if(CheckDataArrayType<Int8ArrayType>()(p)) {u.pInt8 = reinterpret_cast<int8_t*>(p->getVoidPointer(0)); type = DataArray<T>::Int8;}
          else if(CheckDataArrayType<UInt8ArrayType>()(p)) {u.pUInt8 = reinterpret_cast<uint8_t*>(p->getVoidPointer(0)); type = DataArray<T>::UInt8;}
          else if(CheckDataArrayType<Int16ArrayType>()(p)) {u.pInt16 = reinterpret_cast<int16_t*>(p->getVoidPointer(0)); type = DataArray<T>::Int16;}
          else if(CheckDataArrayType<UInt16ArrayType>()(p)) {u.pUInt16 = reinterpret_cast<uint16_t*>(p->getVoidPointer(0)); type = DataArray<T>::UInt16;}
          else if(CheckDataArrayType<Int32ArrayType>()(p)) {u.pInt32 = reinterpret_cast<int32_t*>(p->getVoidPointer(0)); type = DataArray<T>::Int32;}
          else if(CheckDataArrayType<UInt32ArrayType>()(p)) {u.pUInt32 = reinterpret_cast<uint32_t*>(p->getVoidPointer(0)); type = DataArray<T>::UInt32;}
          else if(CheckDataArrayType<Int64ArrayType>()(p)) {u.pInt64 = reinterpret_cast<int64_t*>(p->getVoidPointer(0)); type = DataArray<T>::Int64;}
          else if(CheckDataArrayType<UInt64ArrayType>()(p)) {u.pUInt64 = reinterpret_cast<uint64_t*>(p->getVoidPointer(0)); type = DataArray<T>::UInt64;}
          else if(CheckDataArrayType<FloatArrayType>()(p)) {u.pFloat = reinterpret_cast<float*>(p->getVoidPointer(0)); type = DataArray<T>::Float;}
          else if(CheckDataArrayType<DoubleArrayType>()(p)) {u.pDouble = reinterpret_cast<double*>(p->getVoidPointer(0)); type = DataArray<T>::Double;}
          else {throw std::runtime_error("unknown type");}
        }

        T getValue(size_t i) const {
          switch(type) {
            case DataArray<T>::Int8: return static_cast<T>(u.pInt8[i]);
            case DataArray<T>::UInt8: return static_cast<T>(u.pInt8[i]);
            case DataArray<T>::Int16: return static_cast<T>(u.pInt16[i]);
            case DataArray<T>::UInt16: return static_cast<T>(u.pInt16[i]);
            case DataArray<T>::Int32: return static_cast<T>(u.pInt32[i]);
            case DataArray<T>::UInt32: return static_cast<T>(u.pInt32[i]);
            case DataArray<T>::Int64: return static_cast<T>(u.pInt64[i]);
            case DataArray<T>::UInt64: return static_cast<T>(u.pInt64[i]);
            case DataArray<T>::Float: return static_cast<T>(u.pFloat[i]);
            case DataArray<T>::Double: return static_cast<T>(u.pDouble[i]);
            default: throw std::runtime_error("unknown type");
          }
        }

      private:
        union {
          int8_t* pInt8;
          uint8_t* pUInt8;
          int16_t* pInt16;
          uint16_t* pUInt16;
          int32_t* pInt32;
          uint32_t* pUInt32;
          int64_t* pInt64;
          uint64_t* pUInt64;
          float* pFloat;
          double* pDouble;
        } u;
        int type;

    };

    void compute(size_t start, size_t end) const
    {
      //check input arrays and get variables
      std::vector<T> variables;
      std::vector<TPointer> inputArrays;
      std::vector<size_t> offsets;
      std::vector<size_t> numComp;
      for(size_t i = 0; i < m_arrayNames.size(); i++) {
        //get data array + dimensions
        IDataArray::Pointer p = m_attrMat->getAttributeArray(m_arrayNames.at(i));
        QVector<size_t> dims = p->getComponentDimensions();
        if(1 != dims.size()) throw std::runtime_error("scalar or 1D vector arrays required");
        numComp.push_back(dims[0]);
        inputArrays.push_back(TPointer(p));
        offsets.push_back(variables.size());//begining of variables for this array

        //accumulate variables
        for(size_t i = 0; i < dims[0]; i++) {
          variables.push_back(0);
        }
      }

      //create a parser and populate variables
      Parser<T> parser;
      for(size_t i = 0; i < m_arrayNames.size(); i++) {
        if(1 == numComp[i]) { //add name for scalar arrays
          parser.addVariable(m_arrayNames.at(i).toStdString(), &variables[offsets[i]]);
        } else {//add name[x] for each components of vector arrays
          for(size_t j = 0; j < numComp[i]; j++) {
            std::stringstream ss;
            ss << m_arrayNames.at(i).toStdString() << "[" << j << "]";
            parser.addVariable(ss.str(), &variables[offsets[i] + j]);
          }
        }
      }

      std::vector<T*> varAddresses;
      for(size_t i = 0; i < variables.size(); i++) {
        varAddresses.push_back(&variables[i]);
      }

      parser.setExpression(m_expression);

      //loop over block
      for(size_t i = start; i < end; i++) {
        //populate variables
        for(size_t j = 0; j < inputArrays.size(); j++) {
          for(size_t k = 0; k < numComp[j]; k++) {
            variables[offsets[j] + k] = inputArrays[j].getValue(numComp[j] * i + k);
          }
        }

        //evaluate expression
        m_output[i] = parser.evaluate();
      }
    }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      compute(r.begin(), r.end());
    }
#endif
  private:
    AttributeMatrix::Pointer m_attrMat;
    QList<QString> m_arrayNames;
    std::string m_expression;
    T* m_output;

};

// Include the MOC generated file for this class
#include "moc_ArrayCalculator.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ArrayCalculator::ArrayCalculator() :
  AbstractFilter(),
  m_InputArrayNames(),
  m_OutputArrayName("Scalars"),
  m_Expression("0"),
  m_OutputArray(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ArrayCalculator::~ArrayCalculator()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArrayCalculator::setupFilterParameters()
{
  FilterParameterVector parameters;
  QVector<DataArrayPath> paths = getInputArrayNames();
  MultiDataArraySelectionFilterParameter::RequirementType req;
  parameters.push_back(MultiDataArraySelectionFilterParameter::New("Input Arrays", "InputArrayNames", getInputArrayNames(), FilterParameter::RequiredArray, req, 0));
  parameters.push_back(StringFilterParameter::New("Expression", "Expression", getExpression(), FilterParameter::Parameter));
  parameters.push_back(StringFilterParameter::New("Created Array", "OutputArrayName", getOutputArrayName(), FilterParameter::CreatedArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArrayCalculator::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setInputArrayNames(reader->readDataArrayPathVector("InputArrayNames", getInputArrayNames()));
  setOutputArrayName(reader->readString("OutputArrayName", getOutputArrayName()));
  setExpression(reader->readString("Expression", getExpression()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ArrayCalculator::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(InputArrayNames)
  SIMPL_FILTER_WRITE_PARAMETER(OutputArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(Expression)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArrayCalculator::dataCheck()
{
  setErrorCondition(0);

  //get input attribute matrix
  DataArrayPath amPath = DataArrayPath::GetAttributeMatrixPath(getInputArrayNames());
  if(getInputArrayNames().empty()) {
    QString ss = QObject::tr("At least one input array must be selected");
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  AttributeMatrix::Pointer attrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, amPath, -101);
  if(getErrorCondition() < 0 || NULL == attrMat.get()) {return;}

  //create output array
  DataArrayPath tempPath = amPath;
  QVector<size_t> cDims(1, 1);
  tempPath.setDataArrayName(getOutputArrayName());
  m_OutputArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(this, tempPath, 0, cDims);
  if(NULL != m_OutputArrayPtr.lock().get()) {m_OutputArray = m_OutputArrayPtr.lock()->getPointer(0);}

  try {
    //create parser and dummy variable
    Parser<double> parser;
    double dummy = 1;

    //add variables for each selected array
    QList<QString> names = DataArrayPath::GetDataArrayNames(getInputArrayNames());
    for(size_t i = 0; i < names.size(); i++) {
      //get data array
      DataArrayPath inputPath = amPath;
      inputPath.setDataArrayName(names.at(i));
      IDataArray::Pointer p = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, inputPath);
      if (getErrorCondition() < 0) {return;}

      //get dimensions
      QVector<size_t> dims = p->getComponentDimensions();
      if(1 != dims.size()) {//technically higher rank arrays are possible (e.g. data[a][b][c]) but messy
        QString ss = QObject::tr("The input array has too many dimensions (%1), only scalars and vectors are allowed").arg(dims.size());
        setErrorCondition(-102);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }

      if(1 == dims[0]) { //add name for scalar arrays 
        parser.addVariable(names.at(i).toStdString(), &dummy);
      } else {//add name[x] for each components of vector arrays
        for(size_t j = 0; j < dims[0]; j++) {
          std::stringstream ss;
          ss << names.at(i).toStdString() << "[" << j << "]";
          parser.addVariable(ss.str(), &dummy);
        }
      }
    }

    //check if expression is valid
    parser.setExpression(getExpression().toStdString());
  } catch(std::exception& e) {
    setErrorCondition(-103);
    notifyErrorMessage(getHumanLabel(), e.what(), getErrorCondition());
    return;
  } catch(...) {
    setErrorCondition(-104);
    notifyErrorMessage(getHumanLabel(), "unknown error", getErrorCondition());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArrayCalculator::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArrayCalculator::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  if (getCancel() == true) { return; }

  //get input attribute matrix
  DataArrayPath amPath = DataArrayPath::GetAttributeMatrixPath(getInputArrayNames());
  AttributeMatrix::Pointer attrMat = getDataContainerArray()->getAttributeMatrix(amPath);

  //get selected array names
  QList<QString> names = DataArrayPath::GetDataArrayNames(getInputArrayNames());

  //get the first array to determine number of components
  int64_t totalPoints = attrMat->getAttributeArray(names.at(0))->getNumberOfTuples();
  try{
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    tbb::task_scheduler_init init;
    bool doParallel = true;
#endif

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
   if (doParallel == true) {
     tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), ArrayCalculatorImpl<double>(attrMat, names, getExpression().toStdString(), m_OutputArray), tbb::auto_partitioner());
   } else
#endif
    {
      ArrayCalculatorImpl<double> serial(attrMat, names, getExpression().toStdString(), m_OutputArray);
      serial.compute(0, totalPoints);
    }
  } catch(std::exception& e) {
    setErrorCondition(-201);
    notifyErrorMessage(getHumanLabel(), e.what(), getErrorCondition());
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ArrayCalculator::newFilterInstance(bool copyFilterParameters)
{
  ArrayCalculator::Pointer filter = ArrayCalculator::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ArrayCalculator::getCompiledLibraryName()
{ return LGPLToolboxConstants::LGPLToolboxBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ArrayCalculator::getGroupName()
{ return DREAM3D::FilterGroups::Unsupported; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ArrayCalculator::getSubGroupName()
{ return "LGPLToolbox"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ArrayCalculator::getHumanLabel()
{ return "Array Calculator"; }

