/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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


#include "H5BoundaryStatsDataDelegate.h"

#include "H5Support/QH5Lite.h"
#include "H5Support/QH5Utilities.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5BoundaryStatsDataDelegate::H5BoundaryStatsDataDelegate()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5BoundaryStatsDataDelegate::~H5BoundaryStatsDataDelegate()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5BoundaryStatsDataDelegate::readBoundaryStatsData(BoundaryStatsData* data, hid_t groupId)
{
  int err = 0;
  //Read the NumFeatures
  err = readBoundaryArea(data, groupId);

  //Read the PhaseFraction
  err = readPhaseFraction(data, groupId);

  // Read the Parent Phase
  err = readParentPhase(data, groupId);

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5BoundaryStatsDataDelegate::writeBoundaryStatsData(BoundaryStatsData* data, hid_t groupId)
{
  if (NULL == data)
  {
    return -1;
  }
  int err = 0;

  // Write the NumFeatures
  err = writeBoundaryArea(data, groupId);
  if (err < 0)
  {
    return err;
  }

  // Write the PhaseFraction
  err = writePhaseFraction(data, groupId);
  if (err < 0)
  {
    return err;
  }

  // Write the Parent Phase
  err = writeParentPhase(data, groupId);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5BoundaryStatsDataDelegate::writeBoundaryArea(BoundaryStatsData* data, hid_t pid)
{
  float BoundaryArea = data->getBoundaryArea();
  return QH5Lite::writeScalarDataset(pid, DREAM3D::StringConstants::BoundaryArea, BoundaryArea);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5BoundaryStatsDataDelegate::readBoundaryArea(BoundaryStatsData* data, hid_t pid)
{
  float BoundaryArea = 0.0f;
  int err = QH5Lite::readScalarDataset(pid, DREAM3D::StringConstants::BoundaryArea, BoundaryArea);
  data->setBoundaryArea(BoundaryArea);
  return err;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5BoundaryStatsDataDelegate::writePhaseFraction(BoundaryStatsData* data, hid_t pid)
{
  float phaseFraction = data->getPhaseFraction();
  return QH5Lite::writeScalarDataset(pid, DREAM3D::StringConstants::PhaseFraction, phaseFraction);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5BoundaryStatsDataDelegate::readPhaseFraction(BoundaryStatsData* data, hid_t pid)
{
  float phaseFraction = 0.0f;
  int err = QH5Lite::readScalarDataset(pid, DREAM3D::StringConstants::PhaseFraction, phaseFraction);
  data->setPhaseFraction(phaseFraction);
  return err;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5BoundaryStatsDataDelegate::writeParentPhase(BoundaryStatsData* data, hid_t pid)
{
  unsigned int var = static_cast<unsigned int>( data->getParentPhase() );
  return QH5Lite::writeScalarDataset(pid, DREAM3D::StringConstants::ParentPhase, var);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5BoundaryStatsDataDelegate::readParentPhase(BoundaryStatsData* data, hid_t pid)
{
  unsigned int parentPhase = static_cast<unsigned int>(0.0f);
  int err = QH5Lite::readScalarDataset(pid, DREAM3D::StringConstants::ParentPhase, parentPhase);
  data->setParentPhase( static_cast<float>(parentPhase) );
  return err;
}
