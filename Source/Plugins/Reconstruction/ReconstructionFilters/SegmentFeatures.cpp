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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "SegmentFeatures.h"

#include <QtCore/QTextStream>

#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Reconstruction/ReconstructionConstants.h"
#include "Reconstruction/ReconstructionVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SegmentFeatures::SegmentFeatures() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SegmentFeatures::~SegmentFeatures() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SegmentFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SegmentFeatures::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SegmentFeatures::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  getDataContainerArray()->getPrereqGeometryFromDataContainer<IGeometryGrid>(this, getDataContainerName());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t SegmentFeatures::getSeed(int32_t gnum, int64_t nextSeed)
{
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SegmentFeatures::determineGrouping(int64_t referencepoint, int64_t neighborpoint, int32_t gnum)
{
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SegmentFeatures::execute()
{
  clearErrorCode();
  clearWarningCode();
  SegmentFeatures::dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  SizeVec3Type udims = m->getGeometryAs<IGeometryGrid>()->getDimensions();

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]),
      static_cast<int64_t>(udims[1]),
      static_cast<int64_t>(udims[2]),
  };

  int32_t gnum = 1;
  int64_t seed = 0;
  int64_t neighbor = 0;
  bool good = false;
  int64_t col = 0, row = 0, plane = 0;
  size_t size = 0;
  size_t initialVoxelsListSize = 100000;
  std::vector<int64_t> voxelslist(initialVoxelsListSize, -1);
  int64_t neighpoints[6] = {0, 0, 0, 0, 0, 0};
  neighpoints[0] = -(dims[0] * dims[1]);
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = (dims[0] * dims[1]);
  int64_t nextSeed = 0;

  while(seed >= 0)
  {
    seed = getSeed(gnum, nextSeed);
    nextSeed = seed + 1;
    if(seed >= 0)
    {
      size = 0;
      voxelslist[size] = seed;
      size++;
      while(size > 0)
      {
        int64_t currentpoint = voxelslist[size - 1];
        size -= 1;
        col = currentpoint % dims[0];
        row = (currentpoint / dims[0]) % dims[1];
        plane = currentpoint / (dims[0] * dims[1]);
        for(int32_t i = 0; i < 6; i++)
        {
          good = true;
          neighbor = currentpoint + neighpoints[i];
          if(i == 0 && plane == 0)
          {
            good = false;
          }
          if(i == 5 && plane == (dims[2] - 1))
          {
            good = false;
          }
          if(i == 1 && row == 0)
          {
            good = false;
          }
          if(i == 4 && row == (dims[1] - 1))
          {
            good = false;
          }
          if(i == 2 && col == 0)
          {
            good = false;
          }
          if(i == 3 && col == (dims[0] - 1))
          {
            good = false;
          }
          if(good)
          {
            if(determineGrouping(currentpoint, neighbor, gnum))
            {
              voxelslist[size] = neighbor;
              size++;
              if(neighbor == nextSeed)
              {
                nextSeed = neighbor + 1;
              }
              if(size >= voxelslist.size())
              {
                size = voxelslist.size();
                voxelslist.resize(size + initialVoxelsListSize);
                for(std::vector<int64_t>::size_type j = size; j < voxelslist.size(); ++j)
                {
                  voxelslist[j] = -1;
                }
              }
            }
          }
        }
      }

      voxelslist.assign(initialVoxelsListSize, -1);
      gnum++;
      QString ss = QObject::tr("Total Features: %1").arg(gnum);
      if(gnum % 100 == 0)
      {
        notifyStatusMessage(ss);
      }
    }
    if(getCancel())
    {
      break;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer SegmentFeatures::newFilterInstance(bool copyFilterParameters) const
{
  SegmentFeatures::Pointer filter = SegmentFeatures::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SegmentFeatures::getCompiledLibraryName() const
{
  return ReconstructionConstants::ReconstructionBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SegmentFeatures::getBrandingString() const
{
  return "Reconstruction";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SegmentFeatures::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Reconstruction::Version::Major() << "." << Reconstruction::Version::Minor() << "." << Reconstruction::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SegmentFeatures::getGroupName() const
{
  return SIMPL::FilterGroups::ReconstructionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid SegmentFeatures::getUuid() const
{
  return QUuid("{03fd1d06-a376-5b6e-b103-912dc624f867}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SegmentFeatures::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::SegmentationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SegmentFeatures::getHumanLabel() const
{
  return "Segment Features";
}

// -----------------------------------------------------------------------------
SegmentFeatures::Pointer SegmentFeatures::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<SegmentFeatures> SegmentFeatures::New()
{
  struct make_shared_enabler : public SegmentFeatures
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString SegmentFeatures::getNameOfClass() const
{
  return QString("SegmentFeatures");
}

// -----------------------------------------------------------------------------
QString SegmentFeatures::ClassName()
{
  return QString("SegmentFeatures");
}

// -----------------------------------------------------------------------------
void SegmentFeatures::setDataContainerName(const QString& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
QString SegmentFeatures::getDataContainerName() const
{
  return m_DataContainerName;
}
