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
#include "RotateSampleRefFrame.h"

#if DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range3d.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif


#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/Common/MatrixMath.h"

const static float m_pi = static_cast<float>(M_PI);

typedef struct {
  size_t   xpNew;
  size_t   ypNew;
  size_t   zpNew;
  int      xStride;
  int      yStride;
  int      zStride;
  int      xStart;
  int      yStart;
  int      zStart;

} RotateSampleRefFrameImplArg_t;

/**
 * @brief The RotateSampleRefFrameImpl class does that actual computation of the rotation
 * applying the rotation to each euler angle
 */
class RotateSampleRefFrameImpl
{

    DataArray<size_t>::Pointer newIndicesPtr;
    RotateSampleRefFrameImplArg_t*  m_params;

  public:
    RotateSampleRefFrameImpl(DataArray<size_t>::Pointer newindices, RotateSampleRefFrameImplArg_t*  args) :
      newIndicesPtr(newindices),
      m_params(args)
    {}
    virtual ~RotateSampleRefFrameImpl(){}

    void convert(size_t pageStart, size_t pageEnd,
                  size_t rowStart, size_t rowEnd,
                  size_t colStrart, size_t colEnd) const
    {


      size_t* newindicies = newIndicesPtr->GetPointer(0);
      int64_t index_old = 0;
      int64_t index = 0;

      int x, y, z;
      for (size_t k = pageStart; k < pageEnd; k++)
      {
        index = (k * m_params->xpNew * m_params->ypNew);
        for (size_t j = rowStart; j < rowEnd; j++)
        {
          index = index + (j * m_params->xpNew);
          for (size_t i = colStrart; i < colEnd; i++)
          {
            x = abs(int(m_params->xStart-i));
            y = abs(int(m_params->yStart-j));
            z = abs(int(m_params->zStart-k));
            index_old = (m_params->xStart + (i*m_params->xStride)) + (m_params->yStart + (j*m_params->yStride)) + (m_params->zStart + (k*m_params->zStride));
            newindicies[index + i] = index_old;
          }
        }
      }
    }

#if DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range3d<size_t, size_t, size_t> &r) const
    {
      convert(r.pages().begin(), r.pages().end(), r.rows().begin(), r.rows().end(), r.cols().begin(), r.cols().end());
    }
#endif

  private:
    VoxelDataContainer* m;
    uint32_t angle;
    uint32_t axis;

};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RotateSampleRefFrame::RotateSampleRefFrame() :
AbstractFilter(),
m_RotationAxis(DREAM3D::SampleFrameRotationAxis::None),
m_RotationAngle(DREAM3D::RefFrameRotationAngle::Zero)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RotateSampleRefFrame::~RotateSampleRefFrame()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Rotation Axis");
    option->setPropertyName("RotationAxis");
    option->setWidgetType(FilterParameter::ChoiceWidget);
    option->setValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back("X");
    choices.push_back("Y");
    choices.push_back("Z");
    option->setChoices(choices);
    parameters.push_back(option);
  }
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Rotation Angle");
    option->setPropertyName("RotationAngle");
    option->setWidgetType(FilterParameter::ChoiceWidget);
    option->setValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back("90 Degrees");
    choices.push_back("180 Degrees");
    choices.push_back("270 Degrees");
    choices.push_back("Mirror (Invert)");
    option->setChoices(choices);
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("RotationAxis", getRotationAxis() );
  writer->writeValue("RotationAngle", getRotationAngle() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::execute()
{
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

  int32_t xp, yp, zp;
  float xRes, yRes, zRes;
  float xResNew, yResNew, zResNew;
  RotateSampleRefFrameImplArg_t params;


  xp = static_cast<int32_t>(m->getXPoints());
  xRes = m->getXRes();
  yp = static_cast<int32_t>(m->getYPoints());
  yRes = m->getYRes();
  zp = static_cast<int32_t>(m->getZPoints());
  zRes = m->getZRes();

  params.xpNew = xp;
  xResNew = xRes;
  params.ypNew = yp;
  yResNew = yRes;
  params.zpNew = zp;
  zResNew = zRes;

  params.xStart = 0;
  params.yStart = 0;
  params.zStart = 0;

  params.xStride = 1;
  params.yStride = xp;
  params.zStride = (xp*yp);

  if (m_RotationAxis == DREAM3D::SampleFrameRotationAxis::X)
  {
    if (m_RotationAngle == DREAM3D::RefFrameRotationAngle::Ninety)
    {
      params.ypNew = zp;
      yResNew = zRes;
      params.zpNew = yp;
      zResNew = yRes;
      params.zStart = (yp-1)*xp;
      params.yStride = (xp*yp);
      params.zStride = -xp;
    }
    else if (m_RotationAngle == DREAM3D::RefFrameRotationAngle::oneEighty)
    {
      params.yStart = (yp-1)*xp;
      params.zStart = (zp-1)*xp*yp;
      params.yStride = -xp;
      params.zStride = -(xp*yp);
    }
    else if (m_RotationAngle == DREAM3D::RefFrameRotationAngle::twoSeventy)
    {
      params.ypNew = zp;
      yResNew = zRes;
      params.zpNew = yp;
      zResNew = yRes;
      params.yStart = (zp-1)*xp*yp;
      params.yStride = -(xp*yp);
      params.zStride = xp;
    }
    else if (m_RotationAngle == DREAM3D::RefFrameRotationAngle::Mirror)
    {
      params.xStart = (xp-1);
      params.xStride = -1;
    }
  }
  else if (m_RotationAxis == DREAM3D::SampleFrameRotationAxis::Y)
  {
    if (m_RotationAngle == DREAM3D::RefFrameRotationAngle::Ninety)
    {
      params.xpNew = zp;
      xResNew = zRes;
      params.zpNew = xp;
      zResNew = xRes;
      params.xStart = (zp-1)*xp*yp;
      params.xStride = -(xp*yp);
      params.zStride = 1;
    }
    else if (m_RotationAngle == DREAM3D::RefFrameRotationAngle::oneEighty)
    {
      params.xStart = (xp-1);
      params.zStart = (zp-1)*xp*yp;
      params.xStride = -1;
      params.zStride = -(xp*yp);
    }
    else if (m_RotationAngle == DREAM3D::RefFrameRotationAngle::twoSeventy)
    {
      params.xpNew = zp;
      xResNew = zRes;
      params.zpNew = xp;
      zResNew = xRes;
      params.zStart = (xp-1);
      params.xStride = (xp*yp);
      params.zStride = -1;
    }
    else if (m_RotationAngle == DREAM3D::RefFrameRotationAngle::Mirror)
    {
      params.yStart = (yp-1)*xp;
      params.yStride = -xp;
    }
  }
  else if (m_RotationAxis == DREAM3D::SampleFrameRotationAxis::Z)
  {
    if (m_RotationAngle == DREAM3D::RefFrameRotationAngle::Ninety)
    {
      params.xpNew = yp;
      xResNew = yRes;
      params.ypNew = xp;
      yResNew = xRes;
      params.yStart = (xp-1);
      params.xStride = xp;
      params.yStride = -1;
    }
    else if (m_RotationAngle == DREAM3D::RefFrameRotationAngle::oneEighty)
    {
      params.xStart = (xp-1);
      params.yStart = (yp-1)*xp;
      params.xStride = -1;
      params.yStride = -xp;
    }
    else if (m_RotationAngle == DREAM3D::RefFrameRotationAngle::twoSeventy)
    {
      params.xpNew = yp;
      xResNew = yRes;
      params.ypNew = xp;
      yResNew = xRes;
      params.xStart = (yp-1)*xp;
      params.xStride = -xp;
      params.yStride = 1;
    }
    else if (m_RotationAngle == DREAM3D::RefFrameRotationAngle::Mirror)
    {
      params.zStart = (zp-1)*xp*yp;
      params.zStride = -(xp*yp);
    }
  }

  DataArray<size_t>::Pointer newIndiciesPtr = DataArray<size_t>::CreateArray(totalPoints, 1, "RotateSampleRef_NewIndicies");
  size_t* newindicies = newIndiciesPtr->GetPointer(0);

#if DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::parallel_for(tbb::blocked_range3d<size_t, size_t, size_t>(0, params.zpNew, 0, params.ypNew, 0, params.xpNew),
                    RotateSampleRefFrameImpl(newIndiciesPtr, &params), tbb::auto_partitioner());

#else
  RotateSampleRefFrameImpl serial(newIndiciesPtr, &params);
  serial.convert(0, params.zpNew, 0, params.ypNew, 0, params.xpNew);
#endif


  // This could technically be parallelized also where each thred takes an array to adjust. Except
  // that the DataContainer is NOT thread safe or re-entrant so that would actually be a BAD idea.
  std::list<std::string> voxelArrayNames = m->getCellArrayNameList();
  for (std::list<std::string>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    //std::string name = *iter;
    IDataArray::Pointer p = m->getCellData(*iter);
    // Make a copy of the 'p' array that has the same name. When placed into
    // the data container this will over write the current array with
    // the same name.
    IDataArray::Pointer data = p->createNewArray(p->GetNumberOfTuples(), p->GetNumberOfComponents(), p->GetName());
    data->Resize(totalPoints);
    void* source = NULL;
    void* destination = NULL;
    size_t newIndicies_I = 0;
    int nComp = data->GetNumberOfComponents();
    for (size_t i = 0; i < static_cast<size_t>(totalPoints); i++)
    {
      newIndicies_I = newindicies[i];

      source = p->GetVoidPointer((nComp * newIndicies_I));
      destination = data->GetVoidPointer((data->GetNumberOfComponents() * i));
      ::memcpy(destination, source, p->GetTypeSize() * data->GetNumberOfComponents());
    }
    m->addCellData(*iter, data);
  }
  m->setResolution(xResNew, yResNew, zResNew);
  m->setDimensions(params.xpNew, params.ypNew, params.zpNew);


  notifyStatusMessage("Complete");
}
