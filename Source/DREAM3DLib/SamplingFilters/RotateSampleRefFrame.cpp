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
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif


#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/Common/MatrixMath.h"

const static float m_pi = static_cast<float>(M_PI);


class RotateSampleRefFrameImpl
{
	DataContainer* m;
	uint32_t angle;
	uint32_t axis;
  public:
    RotateSampleRefFrameImpl(DataContainer* dc, uint32_t rotAngle, uint32_t rotAxis) :
	  m(dc),
	  angle(rotAngle),
	  axis(rotAxis)
    {}
    virtual ~RotateSampleRefFrameImpl(){}

    void convert() const
    {
	  size_t xp, yp, zp;
	  float xRes, yRes, zRes;
	  size_t xpNew, ypNew, zpNew;
	  float xResNew, yResNew, zResNew;
	  int xStart, yStart, zStart;
	  int xStride, yStride, zStride;

	  xp = m->getXPoints();
	  xRes = m->getXRes();
	  yp = m->getYPoints();
	  yRes = m->getYRes();
	  zp = m->getZPoints();
	  zRes = m->getZRes();

	  xpNew = xp;
	  xResNew = xRes;
	  ypNew = yp;
	  yResNew = yRes;
	  zpNew = zp;
	  zResNew = zRes;

	  xStart = 0;
	  yStart = 0;
	  zStart = 0;

	  xStride = 1;
	  yStride = xp;
	  zStride = (xp*yp);

	  if (axis == DREAM3D::SampleFrameRotationAxis::X)
	  {
		  if (angle == DREAM3D::RefFrameRotationAngle::Ninety)
		  {
			ypNew = zp;
			yResNew = zRes;
			zpNew = yp;
			zResNew = yRes;

			zStart = (yp-1)*xp;

			yStride = (xp*yp);
			zStride = -xp;
		  }
		  else if (angle == DREAM3D::RefFrameRotationAngle::oneEighty)
		  {
			yStart = (yp-1)*xp;
			zStart = (zp-1)*xp*yp;

			yStride = -xp;
			zStride = -(xp*yp);
		  }
		  else if (angle == DREAM3D::RefFrameRotationAngle::twoSeventy)
		  {
			ypNew = zp;
			yResNew = zRes;
			zpNew = yp;
			zResNew = yRes;

			yStart = (zp-1)*xp*yp;

			yStride = -(xp*yp);
			zStride = xp;
		  }
		  else if (angle == DREAM3D::RefFrameRotationAngle::Mirror)
		  {
			xStart = (xp-1);

			xStride = -1;
		  }
	  }
	  else if (axis == DREAM3D::SampleFrameRotationAxis::Y)
	  {
		  if (angle == DREAM3D::RefFrameRotationAngle::Ninety)
		  {
			xpNew = zp;
			xResNew = zRes;
			zpNew = xp;
			zResNew = xRes;

			xStart = (zp-1)*xp*yp;

			xStride = -(xp*yp);
			zStride = 1;
		  }
		  else if (angle == DREAM3D::RefFrameRotationAngle::oneEighty)
		  {
			xStart = (xp-1);
			zStart = (zp-1)*xp*yp;

			xStride = -1;
			zStride = -(xp*yp);
		  }
		  else if (angle == DREAM3D::RefFrameRotationAngle::twoSeventy)
		  {
			xpNew = zp;
			xResNew = zRes;
			zpNew = xp;
			zResNew = xRes;

			zStart = (xp-1);

			xStride = (xp*yp);
			zStride = -1;
		  }
		  else if (angle == DREAM3D::RefFrameRotationAngle::Mirror)
		  {
			yStart = (yp-1)*xp;

			yStride = -xp;
		  }
	  }
	  else if (axis == DREAM3D::SampleFrameRotationAxis::Z)
	  {
		  if (angle == DREAM3D::RefFrameRotationAngle::Ninety)
		  {
			xpNew = yp;
			xResNew = yRes;
			ypNew = xp;
			yResNew = xRes;

			yStart = (xp-1);

			xStride = xp;
			yStride = -1;
		  }
		  else if (angle == DREAM3D::RefFrameRotationAngle::oneEighty)
		  {
			xStart = (xp-1);
			yStart = (yp-1)*xp;

			xStride = -1;
			yStride = -xp;
		  }
		  else if (angle == DREAM3D::RefFrameRotationAngle::twoSeventy)
		  {
			xpNew = yp;
			xResNew = yRes;
			ypNew = xp;
			yResNew = xRes;

			xStart = (yp-1)*xp;

			xStride = -xp;
			yStride = 1;
		  }
		  else if (angle == DREAM3D::RefFrameRotationAngle::Mirror)
		  {
			zStart = (zp-1)*xp*yp;

			zStride = -(xp*yp);
		  }
	  }
	  int64_t totalPoints = m->getTotalPoints();
	  std::vector<size_t> newindicies;
	  newindicies.resize(totalPoints);
	  int64_t index_old = 0;
	  int64_t index = 0;
	  int x, y, z;
	  for (size_t k = 0; k < zpNew; k++)
      {
		  for (size_t j = 0; j < ypNew; j++)
	      {
			  for (size_t i = 0; i < xpNew; i++)
			  {
				index = (k*xpNew*ypNew)+(j*xpNew)+i;
				x = abs(int(xStart-i));
				y = abs(int(yStart-j));
				z = abs(int(zStart-k));
				index_old = (xStart + (i*xStride)) + (yStart + (j*yStride)) + (zStart + (k*zStride));
				newindicies[index] = index_old;
			  }
		  }
	  }
	  std::list<std::string> voxelArrayNames = m->getCellArrayNameList();
	  for (std::list<std::string>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
	  {
		std::string name = *iter;
		IDataArray::Pointer p = m->getCellData(*iter);
		// Make a copy of the 'p' array that has the same name. When placed into
		// the data container this will over write the current array with
		// the same name. At least in theory
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
	  m->setDimensions(xpNew, ypNew, zpNew);
	}

#if DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t> &r) const
    {
      convert(r.begin(), r.end());
    }
#endif


};



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RotateSampleRefFrame::RotateSampleRefFrame() :
AbstractFilter(),
m_RotationAngle(DREAM3D::RefFrameRotationAngle::Zero),
m_RotationAxis(DREAM3D::SampleFrameRotationAxis::None)
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
  DataContainer* m = getDataContainer();
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

//  std::cout << "RotateSampleRefFrame: " << m_ConversionFactor << std::endl;
#if DREAM3D_USE_PARALLEL_ALGORITHMS
//#if 0
  tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints),
                    RotateSampleRefFrameImpl(m_CellEulerAngles, conversionFactor), tbb::auto_partitioner());

#else
  RotateSampleRefFrameImpl serial(m, m_RotationAngle, m_RotationAxis);
  serial.convert();
#endif

 notifyStatusMessage("Complete");
}
