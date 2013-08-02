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
#include "H5FilterParametersWriter.h"

#include "MXA/Utilities/StringUtils.h"

#include "H5Support/H5Utilities.h"
#include "H5Support/H5Lite.h"

#include "DREAM3DLib/HDF5/H5FilterParametersConstants.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5FilterParametersWriter::H5FilterParametersWriter()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5FilterParametersWriter::~H5FilterParametersWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::openFilterGroup(AbstractFilter* filter, int index)
{
  int err = 0;
  if (m_GroupId <= 0)
  {
    return -1;
  }
  std::string name = StringUtils::numToString(index);
  m_CurrentGroupId = H5Utilities::createGroup(m_GroupId, name);
  err = H5Lite::writeStringAttribute(m_GroupId, name, "ClassName", filter->getNameOfClass());
  err = H5Lite::writeStringAttribute(m_GroupId, name, "HumanLabel", filter->getHumanLabel());
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::closeFilterGroup()
{
  H5Gclose(m_CurrentGroupId);
  m_CurrentGroupId = -1;
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const std::string name, const std::string value)
{
  int err = 0;
  err = H5Lite::writeStringDataset(m_CurrentGroupId, name, value);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const std::string name, const std::vector<std::string> value)
{
  int vectorSize = value.size();
  int err = H5Lite::writeScalarDataset(m_CurrentGroupId, name, vectorSize);
  std::stringstream ss;
  for (int i=0; i<vectorSize; i++)
  {
    ss << i;
    err = H5Lite::writeStringAttribute(m_CurrentGroupId, name, ss.str(), value[i]);
    ss.clear();
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const std::string name, int8_t value)
{
  int err = 0;
  err = H5Lite::writeScalarDataset(m_CurrentGroupId, name, value);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const std::string name, int16_t value)
{
  int err = 0;
  err = H5Lite::writeScalarDataset(m_CurrentGroupId, name, value);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const std::string name, int32_t value)
{
  int err = 0;
  err = H5Lite::writeScalarDataset(m_CurrentGroupId, name, value);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const std::string name, int64_t value)
{
  int err = 0;
  err = H5Lite::writeScalarDataset(m_CurrentGroupId, name, value);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const std::string name, uint8_t value)
{
  int err = 0;
  err = H5Lite::writeScalarDataset(m_CurrentGroupId, name, value);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const std::string name, uint16_t value)
{
  int err = 0;
  err = H5Lite::writeScalarDataset(m_CurrentGroupId, name, value);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const std::string name, uint32_t value)
{
  int err = 0;
  err = H5Lite::writeScalarDataset(m_CurrentGroupId, name, value);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const std::string name, uint64_t value)
{
  int err = 0;
  err = H5Lite::writeScalarDataset(m_CurrentGroupId, name, value);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const std::string name, float value)
{
  int err = 0;
  err = H5Lite::writeScalarDataset(m_CurrentGroupId, name, value);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const std::string name, double value)
{
  int err = 0;
  err = H5Lite::writeScalarDataset(m_CurrentGroupId, name, value);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const std::string name, std::vector<unsigned int> value)
{
  int vectorSize = value.size();
  int err = H5Lite::writeScalarDataset(m_CurrentGroupId, name, vectorSize);
  std::stringstream ss;
  for (int i=0; i<vectorSize; i++)
  {
    ss << i;
    err = H5Lite::writeScalarAttribute(m_CurrentGroupId, name, ss.str(), value[i]);
    ss.clear();
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const std::string name, IntVec3Widget_t v)
{
  int err = 0;
  int32_t rank = 1;
  hsize_t dims[1] = { 3 };
  err = H5Lite::writePointerDataset<int32_t>(m_CurrentGroupId, name, rank, dims, reinterpret_cast<int32_t*>(&v) );
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const std::string name, FloatVec3Widget_t v)
{
  int err = 0;
  int32_t rank = 1;
  hsize_t dims[1] = { 3 };
  err = H5Lite::writePointerDataset<float>(m_CurrentGroupId, name, rank, dims, reinterpret_cast<float*>(&v) );
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const std::string name, ComparisonInput_t v, int vectorPos)
{
  int err = 0;

  std::ostringstream convert1;
  convert1 << vectorPos << H5FilterParameter::ArrayNameConstant;
  std::string strAttribute1 = convert1.str();

  std::ostringstream convert2;
  convert2 << vectorPos << H5FilterParameter::CompOperatorConstant;
  std::string strAttribute2 = convert2.str();

  std::ostringstream convert3;
  convert3 << vectorPos << H5FilterParameter::CompValueConstant;
  std::string strAttribute3 = convert3.str();

  err = H5Lite::writeStringAttribute(m_CurrentGroupId, name, strAttribute1, v.arrayName);
  err = H5Lite::writeScalarAttribute(m_CurrentGroupId, name, strAttribute2, v.compOperator);
  err = H5Lite::writeScalarAttribute(m_CurrentGroupId, name, strAttribute3, v.compValue);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const std::string name, std::vector<ComparisonInput_t> v)
{
  int numQFilters = static_cast<int>( v.size() );
  int err = writeValue(name,  numQFilters);
  for(int i = 0; i < numQFilters; i++)
  {
    err = writeValue(name, v[i], i);
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const std::string name, std::vector<AxisAngleInput_t> v)
{
  int numQFilters = static_cast<int>( v.size() );
  int err = writeValue(name,  numQFilters);
  for(int i = 0; i < numQFilters; i++)
  {
    err = writeValue(name, v[i], i);
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const std::string name, AxisAngleInput_t v, int vectorPos)
{
  int err = 0;
  int32_t rank = 1;
  hsize_t dims[1] = { 4 };

  std::ostringstream convert1;
  convert1 << vectorPos << H5FilterParameter::AngleConstant;
  std::string strAttribute1 = convert1.str();

  std::ostringstream convert2;
  convert2 << vectorPos << H5FilterParameter::HConstant;
  std::string strAttribute2 = convert2.str();

  std::ostringstream convert3;
  convert3 << vectorPos << H5FilterParameter::KConstant;
  std::string strAttribute3 = convert3.str();

  std::ostringstream convert4;
  convert4 << vectorPos << H5FilterParameter::LConstant;
  std::string strAttribute4 = convert4.str();

  err = H5Lite::writePointerDataset<float>(m_CurrentGroupId, name, rank, dims, reinterpret_cast<float*>(&v) );
  err = H5Lite::writeScalarAttribute(m_CurrentGroupId, name, strAttribute1, v.angle);
  err = H5Lite::writeScalarAttribute(m_CurrentGroupId, name, strAttribute2, v.h);
  err = H5Lite::writeScalarAttribute(m_CurrentGroupId, name, strAttribute3, v.k);
  err = H5Lite::writeScalarAttribute(m_CurrentGroupId, name, strAttribute4, v.l);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const std::string name, std::set<std::string> v)
{
  size_t size = v.size();
  herr_t err = 0;
  if (size > 0)
  {
    std::string setStr = "";
    std::set<std::string>::iterator iter = v.begin();
    for (; iter != v.end(); iter++)
    {
      setStr.append(*iter).append("\n");
    }

    err = H5Lite::writeStringDataset(m_CurrentGroupId, name, setStr);
    if (err < 0)
    {
      return err;
    }
  }
  err = H5Lite::writeScalarAttribute(m_CurrentGroupId, name, "NumArrays", size);
  if (err < 0)
  {
    return err;
  }
  return err;
}
