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
int H5FilterParametersWriter::openOptionsGroup(AbstractFilter* filter)
{
  int err = 0;
  if (m_GroupId <= 0)
  {
    return -1;
  }
  std::string name = StringUtils::numToString(filter->getPipelineIndex());
  m_CurrentGroupId = H5Utilities::createGroup(m_GroupId, name);
  err = H5Lite::writeStringAttribute(m_GroupId, name, "ClassName", filter->getNameOfClass());
  err = H5Lite::writeStringAttribute(m_GroupId, name, "HumanLabel", filter->getHumanLabel());
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::closeOptionsGroup()
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
int H5FilterParametersWriter::writeValue(const std::string name, ComparisonInput_t v)
{
  int err = 0;

  float value = v.compValue;
  err = H5Lite::writeScalarDataset(m_CurrentGroupId, name, value);
  err = H5Lite::writeStringAttribute(m_CurrentGroupId, name, "ArrayName", v.arrayName);
  err = H5Lite::writeScalarAttribute(m_CurrentGroupId, name, "CompOperator", v.compOperator);
  err = H5Lite::writeScalarAttribute(m_CurrentGroupId, name, "CompValue", v.compValue);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const std::string name, std::vector<ComparisonInput_t> v)
{
    int numQFilters = static_cast<int>( v.size() );
   int err = writeValue(name + "_NumComparisons",  numQFilters);
	std::stringstream ss;
	for(int i = 0; i < numQFilters; i++)
    {
      ss << "Comparison-" << i;
      err = writeValue(ss.str(), v[i]);
      ss.str("");
    }
	return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const std::string name, std::vector<AxisAngleInput_t> v)
{
	int numQFilters = static_cast<int>( v.size() );
	int err = writeValue(name + "_NumAxisAngleInputs",  numQFilters);
	std::stringstream ss;
	for(int i = 0; i < numQFilters; i++)
	{
		ss << "AxisAngleInput-" << i;
		err = writeValue(ss.str(), v[i]);
		ss.str("");
	}
	return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5FilterParametersWriter::writeValue(const std::string name, AxisAngleInput_t v)
{
  int err = 0;
  int32_t rank = 1;
  hsize_t dims[1] = { 4 };
  err = H5Lite::writePointerDataset<float>(m_CurrentGroupId, name, rank, dims, reinterpret_cast<float*>(&v) );
  return err;
}
