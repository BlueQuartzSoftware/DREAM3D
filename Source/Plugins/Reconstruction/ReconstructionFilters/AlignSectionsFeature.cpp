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

#include "AlignSectionsFeature.h"

#include <QtCore/QtDebug>
#include <fstream>
#include <sstream>

#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

using namespace std;



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSectionsFeature::AlignSectionsFeature() :
  AlignSections(),
  m_GoodVoxelsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::GoodVoxels),
  m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
  m_GoodVoxels(NULL)
{
  //only setting up the child parameters because the parent constructor has already been called
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSectionsFeature::~AlignSectionsFeature()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsFeature::setupFilterParameters()
{
  //getting the current parameters that were set by the parent and adding to it before resetting it
  FilterParameterVector parameters = getFilterParameters();
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("GoodVoxels", "GoodVoxelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getGoodVoxelsArrayPath(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsFeature::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  AlignSections::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AlignSectionsFeature::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  AlignSections::writeFilterParameters(writer, index);
  writer->openFilterGroup(this, index);
  writer->writeValue("GoodVoxelsArrayPath", getGoodVoxelsArrayPath() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsFeature::dataCheck()
{
  setErrorCondition(0);

  //Set the DataContainerName and AttributematrixName for the Parent Class (AlignSections) to Use.
  setDataContainerName(m_GoodVoxelsArrayPath.getDataContainerName());
  setCellAttributeMatrixName(m_GoodVoxelsArrayPath.getAttributeMatrixName());

  if(true == getWriteAlignmentShifts() && getAlignmentShiftFileName().isEmpty() == true)
  {
    QString ss = QObject::tr("The Alignment Shift file name must be set before executing this filter.");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  QVector<size_t> dims(1, 1);
  m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsFeature::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsFeature::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  AlignSections::execute();

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Aligning Sections Complete");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsFeature::find_shifts(QVector<int>& xshifts, QVector<int>& yshifts)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  ofstream outFile;
  if (getWriteAlignmentShifts() == true)
  {
    outFile.open(getAlignmentShiftFileName().toLatin1().data());
  }
  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  float disorientation = 0;
  float mindisorientation = 100000000;
  int newxshift = 0;
  int newyshift = 0;
  int oldxshift = 0;
  int oldyshift = 0;
  float count = 0;
  int slice = 0;
//  int xspot, yspot;
  int refposition = 0;
  int curposition = 0;

  QVector<QVector<float> >  misorients(dims[0]);
  for (DimType a = 0; a < dims[0]; a++)
  {
    misorients[a].fill(0.0, dims[1]);
  }
  for (DimType iter = 1; iter < dims[2]; iter++)
  {

    QString ss = QObject::tr("Aligning Sections - Determining Shifts - %1 Percent Complete").arg(((float)iter / dims[2]) * 100);
    //  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    mindisorientation = 100000000;
    slice = static_cast<int>( (dims[2] - 1) - iter );
    oldxshift = -1;
    oldyshift = -1;
    newxshift = 0;
    newyshift = 0;
    for (DimType a = 0; a < dims[0]; a++)
    {
      for (DimType b = 0; b < dims[1]; b++)
      {
        misorients[a][b] = 0;
      }
    }
    while (newxshift != oldxshift || newyshift != oldyshift)
    {
      oldxshift = newxshift;
      oldyshift = newyshift;
      for (int j = -3; j < 4; j++)
      {
        for (int k = -3; k < 4; k++)
        {
          disorientation = 0;
          count = 0;
          if(misorients[k + oldxshift + size_t(dims[0] / 2)][j + oldyshift + (size_t)(dims[1] / 2)] == 0 && abs(k + oldxshift) < (dims[0] / 2)
              && (j + oldyshift) < (dims[1] / 2))
          {
            for (DimType l = 0; l < dims[1]; l = l + 4)
            {
              for (DimType n = 0; n < dims[0]; n = n + 4)
              {
                if((l + j + oldyshift) >= 0 && (l + j + oldyshift) < dims[1] && (n + k + oldxshift) >= 0 && (n + k + oldxshift) < dims[0])
                {
                  refposition = static_cast<int>( ((slice + 1) * dims[0] * dims[1]) + (l * dims[0]) + n );
                  curposition = static_cast<int>( (slice * dims[0] * dims[1]) + ((l + j + oldyshift) * dims[0]) + (n + k + oldxshift) );
                  if(m_GoodVoxels[refposition] != m_GoodVoxels[curposition]) { disorientation++; }
                  count++;
                }
                else
                {

                }
              }
            }
            disorientation = disorientation / count;
            misorients[k + oldxshift + int(dims[0] / 2)][j + oldyshift + int(dims[1] / 2)] = disorientation;
            if(disorientation < mindisorientation)
            {
              newxshift = k + oldxshift;
              newyshift = j + oldyshift;
              mindisorientation = disorientation;
            }
          }
        }
      }
    }
    xshifts[iter] = xshifts[iter - 1] + newxshift;
    yshifts[iter] = yshifts[iter - 1] + newyshift;
    if (getWriteAlignmentShifts() == true)
    {
      outFile << slice << "	" << slice + 1 << "	" << newxshift << "	" << newyshift << "	" << xshifts[iter] << "	" << yshifts[iter] << endl;
    }
  }
  if (getWriteAlignmentShifts() == true)
  {
    outFile.close();
  }
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AlignSectionsFeature::newFilterInstance(bool copyFilterParameters)
{
  AlignSectionsFeature::Pointer filter = AlignSectionsFeature::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsFeature::getCompiledLibraryName()
{ return Reconstruction::ReconstructionBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsFeature::getGroupName()
{ return DREAM3D::FilterGroups::ReconstructionFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsFeature::getSubGroupName()
{return DREAM3D::FilterSubGroups::AlignmentFilters;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsFeature::getHumanLabel()
{ return "Align Sections (Feature)"; }

