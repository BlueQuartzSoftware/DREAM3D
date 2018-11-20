/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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


#include "H5EbsdVolumeReader.h"



#if defined (H5Support_NAMESPACE)
using namespace H5Support_NAMESPACE;
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5EbsdVolumeReader::H5EbsdVolumeReader() :
  m_Cancel(false),
  m_SliceStart(0),
  m_SliceEnd(0),
  m_ManageMemory(true),
  m_NumberOfElements(0),
  m_ReadAllArrays(true)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5EbsdVolumeReader::~H5EbsdVolumeReader() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5EbsdVolumeReader::initPointers(size_t numElements)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5EbsdVolumeReader::deletePointers()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void* H5EbsdVolumeReader::getPointerByName(const QString& featureName)
{
  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Ebsd::NumType H5EbsdVolumeReader::getPointerType(const QString& featureName)
{
  return Ebsd::UnknownNumType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5EbsdVolumeReader::loadData(int64_t xpoints, int64_t ypoints, int64_t zpoints, uint32_t ZDir)
{
  // This class should be subclassed and this method implemented.
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5EbsdVolumeReader::setArraysToRead(QSet<QString> names)
{
  m_ArrayNames = names;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSet<QString> H5EbsdVolumeReader::getArraysToRead()
{
  return m_ArrayNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool H5EbsdVolumeReader::getReadAllArrays()
{
  return m_ReadAllArrays;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5EbsdVolumeReader::readAllArrays(bool b)
{
  m_ReadAllArrays = b;
}
