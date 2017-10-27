/*
 * ###################################################################
 * Copyright (c) 2016, Marc De Graef/Carnegie Mellon University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *     - Redistributions of source code must retain the above copyright notice, this list
 *        of conditions and the following disclaimer.
 *     - Redistributions in binary form must reproduce the above copyright notice, this
 *        list of conditions and the following disclaimer in the documentation and/or
 *        other materials provided with the distribution.
 *     - Neither the names of Marc De Graef, Carnegie Mellon University nor the names
 *        of its contributors may be used to endorse or promote products derived from
 *        this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ###################################################################
 */

#include "EMsoftSO3Sampler.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/SIMPLibVersion.h"

#include "OrientationLib/OrientationLibConstants.h"
#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_EMsoftSO3Sampler.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMsoftSO3Sampler::EMsoftSO3Sampler()
: AbstractFilter()
, m_sampleModeSelector(0)
, m_PointGroup(32)
, m_Numsp(5)
, m_MisOr(3.0)
, m_MisOrFull(3.0)
, m_OffsetGrid(false)
, m_DataContainerName(SIMPL::Defaults::ImageDataContainerName)
, m_EMsoftAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
, m_EulerAngles(nullptr)
{
  m_RefOr.x = 0.0;
  m_RefOr.y = 0.0;
  m_RefOr.z = 0.0;

  m_RefOrFull.x = 0.0;
  m_RefOrFull.y = 0.0;
  m_RefOrFull.z = 0.0;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMsoftSO3Sampler::~EMsoftSO3Sampler()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMsoftSO3Sampler::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Select the desired SO(3) sampling mode");
    parameter->setPropertyName("sampleModeSelector");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(EMsoftSO3Sampler, this, sampleModeSelector));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(EMsoftSO3Sampler, this, sampleModeSelector));
    parameter->setDefaultValue(0); // Always start with the standard sampling mode

    QVector<QString> choices;
    choices.push_back("- Rodrigues fundamental zone    ");
    choices.push_back("- Constant misorientation       ");
    choices.push_back("- Less than given misorientation");
    parameter->setChoices(choices);
    parameter->setChoices(choices);
    QStringList linkedProps;
    linkedProps << "PointGroup"
                << "OffsetGrid"
                << "MisOr"
                << "RefOr"
                << "MisOrFull"
                << "RefOrFull";

    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);

    /* standard sampling method */
    parameters.push_back(SIMPL_NEW_INTEGER_FP("Point group number (see documentation for list)", PointGroup, FilterParameter::Parameter, EMsoftSO3Sampler, 0));
    parameters.push_back(BooleanFilterParameter::New("Offset sampling grid from origin?", "OffsetGrid", getOffsetGrid(), FilterParameter::Parameter,
                                                     SIMPL_BIND_SETTER(EMsoftSO3Sampler, this, OffsetGrid), SIMPL_BIND_GETTER(EMsoftSO3Sampler, this, OffsetGrid), 0));

    /* equal misorientation sampling method */
    parameters.push_back(SIMPL_NEW_DOUBLE_FP("Misorientation angle (degree)", MisOr, FilterParameter::Parameter, EMsoftSO3Sampler, 1));
    parameters.push_back(FloatVec3FilterParameter::New("Reference orientation (Euler, °)", "RefOr", getRefOr(), FilterParameter::Parameter, SIMPL_BIND_SETTER(EMsoftSO3Sampler, this, RefOr),
                                                       SIMPL_BIND_GETTER(EMsoftSO3Sampler, this, RefOr), 1));

    /* maximum misorientation sampling method */
    parameters.push_back(SIMPL_NEW_DOUBLE_FP("Misorientation angle (degree)", MisOrFull, FilterParameter::Parameter, EMsoftSO3Sampler, 2));
    parameters.push_back(FloatVec3FilterParameter::New("Reference orientation (Euler, °)", "RefOrFull", getRefOrFull(), FilterParameter::Parameter,
                                                       SIMPL_BIND_SETTER(EMsoftSO3Sampler, this, RefOrFull), SIMPL_BIND_GETTER(EMsoftSO3Sampler, this, RefOrFull), 2));
  }
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Number of sampling points along cube semi-axis", Numsp, FilterParameter::Parameter, EMsoftSO3Sampler));

  parameters.push_back(SeparatorFilterParameter::New("Output Data", FilterParameter::Parameter));

  parameters.push_back(SIMPL_NEW_STRING_FP("Euler Angles", EulerAnglesArrayName, FilterParameter::CreatedArray, EMsoftSO3Sampler));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMsoftSO3Sampler::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMsoftSO3Sampler::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  // Try to get the DataContainer that the user has named.
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  // If that did not exist then create one with that name
  if(nullptr == m.get())
  {
    m = getDataContainerArray()->createNonPrereqDataContainer(this, getDataContainerName());
  }
  if(getErrorCondition() < 0 || nullptr == m.get())
  {
    return;
  } // This truly is an error condition.

  // DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
  // if (getErrorCondition() < 0) { return; }

  // Now try to either get or create the needed AttributeMatrix
  QVector<size_t> tDims(1, 1);
  AttributeMatrix::Pointer emsoftAttrMat = m->getAttributeMatrix(getEMsoftAttributeMatrixName());
  if(nullptr == emsoftAttrMat.get())
  {
    emsoftAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getEMsoftAttributeMatrixName(), tDims, AttributeMatrix::Type::Generic);
  }
  if(getErrorCondition() < 0 || nullptr == emsoftAttrMat.get())
  {
    return;
  }

  //  QVector<size_t> tDims(1, 1);
  //  AttributeMatrix::Pointer emsoftAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getEMsoftAttributeMatrixName(), tDims, AttributeMatrix::Type::Generic);
  //  if (getErrorCondition() < 0) { return; }
  //  m->addAttributeMatrix(emsoftAttrMat->getName(),emsoftAttrMat);

  // check on the point group index; must be between 1 and 32.
  if(getsampleModeSelector() == 0)
  {
    if((getPointGroup() < 1) || (getPointGroup() > 32))
    {
      setWarningCondition(-70001);
      QString ss = QObject::tr("Point group number must fall in interval [1,32]");
      notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
    }
  }

  // make sure that the misorientation angle lies in [0,90], and the Euler angles in [0,360], [0,180], [0,360]
  if(getsampleModeSelector() == 1)
  {
    if((getMisOr() < 0.0) || (getMisOr() > 90.0))
    {
      setWarningCondition(-70002);
      QString ss = QObject::tr("Misorientation angle must fall in interval [0,90]");
      notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
    }
    if((getRefOr().x < 0.0) || (getRefOr().x > 360.0f) || (getRefOr().y < 0.0f) || (getRefOr().y > 180.0f) || (getRefOr().z < 0.0f) || (getRefOr().z > 360.0f))
    {
      setWarningCondition(-70003);
      QString ss = QObject::tr("Euler angles must be positive and less than [360°,180°,360°]");
      notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
    }
  }
  if(getsampleModeSelector() == 2)
  {
    if((getMisOrFull() < 0.0) || (getMisOrFull() > 90.0))
    {
      setWarningCondition(-70004);
      QString ss = QObject::tr("Misorientation angle must fall in interval [0,90]");
      notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
    }
    if((getRefOrFull().x < 0.0f) || (getRefOrFull().x > 360.0f) || (getRefOrFull().y < 0.0f) || (getRefOrFull().y > 180.0f) || (getRefOrFull().z < 0.0f) || (getRefOrFull().z > 360.0f))
    {
      setWarningCondition(-70005);
      QString ss = QObject::tr("Euler angles must be positive and less than [360°,180°,360°]");
      notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
    }
  }

  // check on the number of sampling intervals (>1)
  if(getNumsp() < 1)
  {
    setWarningCondition(-70002);
    QString ss = QObject::tr("Number of sampling intervals must be at least 1 ");
    notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
  }

  QVector<DataArrayPath> dataArraypaths;

  // allocate space for the EulerAngles array, which will subsequently be filled by the SO(3) sampling code.
  QVector<size_t> cDims(1);
  cDims[0] = 3;
  DataArrayPath tempPath;
  tempPath.update(getDataContainerName(), getEMsoftAttributeMatrixName(), getEulerAnglesArrayName());
  m_EulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0.0f,
                                                                                                                    cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_EulerAnglesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_EulerAngles = m_EulerAnglesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArraypaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMsoftSO3Sampler::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMsoftSO3Sampler::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  if(getCancel() == true)
  {
    return;
  }

  OrientationListArrayType FZlist;
  typedef OrientationTransforms<DOrientArrayType, double> OrientationTransformsType;

  if(getsampleModeSelector() == 0)
  {
    // here we perform the actual calculation; once we have the FZlist,
    // we can allocate the data array and copy all entries
    double x, y, z, delta;
    int32_t FZtype, FZorder;

    // step size for sampling of grid; maximum total number of samples = pow(2*getNumsp()+1,3)
    delta = (0.50 * LPs::ap) / static_cast<double>(getNumsp());

    // do we need to shift this array away from the origin?
    double gridShift = 0.0;
    if(getOffsetGrid())
    {
      gridShift = 0.5;
    }

    // determine which function we should call for this point group symmetry
    FZtype = OrientationAnalysisConstants::FZtarray[getPointGroup() - 1];
    FZorder = OrientationAnalysisConstants::FZoarray[getPointGroup() - 1];

    // loop over the cube of volume pi^2; note that we do not want to include
    // the opposite edges/facets of the cube, to avoid double counting rotations
    // with a rotation angle of 180 degrees.  This only affects the cyclic groups.
    int Np = getNumsp();
    int Totp = (2 * Np + 1) * (2 * Np + 1) * (2 * Np + 1);
    int Dn = Totp / 10;
    int Dc = Dn;
    int Di = 0;
    int Dg = 0;

    // eliminate points for which any of the coordinates lies outside the cube with semi-edge length "edge"
    double edge = 0.5 * LPs::ap;

    for(int i = -Np + 1; i < Np + 1; i++)
    {
      x = (static_cast<double>(i) + gridShift) * delta;

      if(fabs(x) <= edge)
      {

        for(int j = -Np + 1; j < Np + 1; j++)
        {
          y = (static_cast<double>(j) + gridShift) * delta;

          if(fabs(y) <= edge)
          {

            for(int k = -Np + 1; k < Np + 1; k++)
            {
              z = (static_cast<double>(k) + gridShift) * delta;

              if(fabs(z) <= edge)
              {

                // convert to Rodrigues representation
                DOrientArrayType cu(x, y, z);
                DOrientArrayType rod(4);
                OrientationTransformsType::cu2ro(cu, rod);

                // If insideFZ=true, then add this point to FZlist
                bool b = IsinsideFZ(rod.data(), FZtype, FZorder);
                if(b)
                {
                  FZlist.push_back(rod);
                  Dg += 1;
                }
                Di += 1;
              }
            }
          }
        }
      }

      // report on status of computation
      if(Di > Dc)
      {
        QString ss = QString("Euler angle triplets tested: %1 of %2; triplets inside Rodrigues fundamental zone: %3 ").arg(QString::number(Di), QString::number(Totp), QString::number(Dg));
        notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
        Dc += Dn;
      }
    }
  }

  // here are the misorientation sampling cases:
  if(getsampleModeSelector() != 0)
  {
    // here we perform the actual calculation; once we have the FZlist,
    // we can allocate the data array and copy all entries
    double x, y, z, delta, omega, semi;

    // step size for sampling of grid; the edge length of the cube is (pi ( w - sin(w) ))^1/3 with w the misorientation angle
    omega = getMisOr() * SIMPLib::Constants::k_PiOver180;
    semi = pow(SIMPLib::Constants::k_Pi * (omega - sin(omega)), 1.0 / 3.0) * 0.5;
    delta = semi / static_cast<double>(getNumsp());

    // convert the reference orientation to a 3-component Rodrigues vector sigma
    DOrientArrayType sigm(4), sigma(3), referenceOrientation(3);
    referenceOrientation[0] = static_cast<double>(getRefOr().x * SIMPLib::Constants::k_PiOver180);
    referenceOrientation[1] = static_cast<double>(getRefOr().y * SIMPLib::Constants::k_PiOver180);
    referenceOrientation[2] = static_cast<double>(getRefOr().z * SIMPLib::Constants::k_PiOver180);
    OrientationTransformsType::eu2ro(referenceOrientation, sigm);
    sigma[0] = sigm[0] * sigm[3];
    sigma[1] = sigm[1] * sigm[3];
    sigma[2] = sigm[2] * sigm[3];

    if(getsampleModeSelector() == 1)
    {
      // set counter parameters for the loop over the sub-cube surface
      int Np = getNumsp();
      int Totp = 24 * Np * Np + 2;
      int Dn = Totp / 20;
      int Dc = Dn;
      int Dg = 0;

      // x-y bottom and top planes
      for(int i = -Np; i <= Np; i++)
      {
        x = static_cast<double>(i) * delta;
        for(int j = -Np; j <= Np; j++)
        {
          y = static_cast<double>(j) * delta;
          // convert to Rodrigues representation and apply Rodrigues composition formula
          {
            DOrientArrayType cu(-x, -y, -semi);
            DOrientArrayType rod(4);
            OrientationTransformsType::cu2ro(cu, rod);
            RodriguesComposition(sigma, rod);
            FZlist.push_back(rod);
            Dg += 1;
          }
          {
            DOrientArrayType cu(-x, -y, semi);
            DOrientArrayType rod(4);
            OrientationTransformsType::cu2ro(cu, rod);
            RodriguesComposition(sigma, rod);
            FZlist.push_back(rod);
            Dg += 1;
          }
        }
      }
      // y-z  planes
      for(int j = -Np; j <= Np; j++)
      {
        y = static_cast<double>(j) * delta;
        for(int k = -Np + 1; k <= Np - 1; k++)
        {
          z = static_cast<double>(k) * delta;
          // convert to Rodrigues representation and apply Rodrigues composition formula
          {
            DOrientArrayType cu(-semi, -y, -z);
            DOrientArrayType rod(4);
            OrientationTransformsType::cu2ro(cu, rod);
            RodriguesComposition(sigma, rod);
            FZlist.push_back(rod);
            Dg += 1;
          }
          {
            DOrientArrayType cu(semi, -y, -z);
            DOrientArrayType rod(4);
            OrientationTransformsType::cu2ro(cu, rod);
            RodriguesComposition(sigma, rod);
            FZlist.push_back(rod);
            Dg += 1;
          }
        }
      }
      // finally the x-z  planes
      for(int i = -Np + 1; i <= Np - 1; i++)
      {
        x = static_cast<double>(i) * delta;
        for(int k = -Np + 1; k <= Np - 1; k++)
        {
          z = static_cast<double>(k) * delta;
          // convert to Rodrigues representation and apply Rodrigues composition formula
          {
            DOrientArrayType cu(-x, -semi, -z);
            DOrientArrayType rod(4);
            OrientationTransformsType::cu2ro(cu, rod);
            RodriguesComposition(sigma, rod);
            FZlist.push_back(rod);
            Dg += 1;
          }
          {
            DOrientArrayType cu(-x, semi, -z);
            DOrientArrayType rod(4);
            OrientationTransformsType::cu2ro(cu, rod);
            RodriguesComposition(sigma, rod);
            FZlist.push_back(rod);
            Dg += 1;
          }
        }
      }

      // report on status of computation
      if(Dg > Dc)
      {
        QString ss = QString("Euler angle triplets generated: %1 out of %2").arg(QString::number(Dg), QString::number(Totp));
        notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
        Dc += Dn;
      }
    }
    else
    {
      // set counter parameters for the loop over the sub-cube surface
      int Np = getNumsp();
      int Totp = (2 * Np + 1) * (2 * Np + 1) * (2 * Np + 1); // see misorientation sampling paper for this expression
      int Dn = Totp / 20;
      int Dc = Dn;
      int Dg = 0;

      for(int i = -Np; i <= Np; i++)
      {
        x = static_cast<double>(i) * delta;
        for(int j = -Np; j <= Np; j++)
        {
          y = static_cast<double>(j) * delta;
          for(int k = -Np; k <= Np; k++)
          {
            z = static_cast<double>(k) * delta;
            // convert to Rodrigues representation and apply Rodrigues composition formula
            {
              DOrientArrayType cu(-x, -y, -z);
              DOrientArrayType rod(4);
              OrientationTransformsType::cu2ro(cu, rod);
              RodriguesComposition(sigma, rod);
              FZlist.push_back(rod);
              Dg += 1;
            }
          }
        }
        // report on status of computation
        if(Dg > Dc)
        {
          QString ss = QString("Euler angle triplets generated: %1 out of %2").arg(QString::number(Dg), QString::number(Totp));
          notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
          Dc += Dn;
        }
      }
    }
  }

  // resize the EulerAngles array to the number of items in FZlist; don't forget to redefine the hard pointer
  AttributeMatrix::Pointer am = getDataContainerArray()->getAttributeMatrix(DataArrayPath(getDataContainerName(), getEMsoftAttributeMatrixName(), ""));
  QVector<size_t> tDims(1, FZlist.size());
  am->resizeAttributeArrays(tDims);
  m_EulerAngles = m_EulerAnglesPtr.lock()->getPointer(0);

  // copy the Rodrigues vectors as Euler angles into the m_EulerAngles array; convert doubles to floats along the way
  int j = -1;
  for(DOrientArrayType rod : FZlist)
  {
    j += 1;
    DOrientArrayType eu(3, 0.0);
    OrientationTransformsType::ro2eu(rod, eu);

    m_EulerAngles[j * 3 + 0] = static_cast<float>(eu[0]);
    m_EulerAngles[j * 3 + 1] = static_cast<float>(eu[1]);
    m_EulerAngles[j * 3 + 2] = static_cast<float>(eu[2]);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMsoftSO3Sampler::RodriguesComposition(DOrientArrayType sigma, DOrientArrayType& rod)
{
  DOrientArrayType rho(3), rhomis(3);
  rho[0] = -rod[0] * rod[3];
  rho[1] = -rod[1] * rod[3];
  rho[2] = -rod[2] * rod[3];

  // perform the Rodrigues rotation composition with sigma to get rhomis
  double denom = 1.0 + (sigma[0] * rho[0] + sigma[1] * rho[1] + sigma[2] * rho[2]);
  if(denom == 0.0)
  {
    double len;
    len = sqrt(sigma[0] * sigma[0] + sigma[1] * sigma[1] + sigma[2] * sigma[2]);
    rod[0] = sigma[0] / len;
    rod[1] = sigma[1] / len;
    rod[2] = sigma[2] / len;
    rod[3] = std::numeric_limits<double>::infinity(); // set this to infinity
  }
  else
  {
    rhomis[0] = (rho[0] - sigma[0] + (rho[1] * sigma[2] - rho[2] * sigma[1])) / denom;
    rhomis[1] = (rho[1] - sigma[1] + (rho[2] * sigma[0] - rho[0] * sigma[2])) / denom;
    rhomis[2] = (rho[2] - sigma[2] + (rho[0] * sigma[1] - rho[1] * sigma[0])) / denom;
    // revert rhomis to a four-component Rodrigues vector
    double len;
    len = sqrt(rhomis[0] * rhomis[0] + rhomis[1] * rhomis[1] + rhomis[2] * rhomis[2]);
    if(len != 0.0)
    {
      rod[0] = -rhomis[0] / len;
      rod[1] = -rhomis[1] / len;
      rod[2] = -rhomis[2] / len;
      rod[3] = len;
    }
    else
    {
      rod[0] = 0.0;
      rod[1] = 0.0;
      rod[2] = 0.0;
      rod[3] = 0.0;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool EMsoftSO3Sampler::IsinsideFZ(double* rod, int FZtype, int FZorder)
{
  bool insideFZ = false;
  // dealing with 180 rotations is needed only for
  // FZtypes 0 and 1; the other FZs are always finite.
  switch(FZtype)
  {
  case OrientationAnalysisConstants::AnorthicType:
    insideFZ = true; // all points are inside the FZ
    break;
  case OrientationAnalysisConstants::CyclicType:
    insideFZ = insideCyclicFZ(rod, FZorder); // infinity is checked inside this function
    break;
  case OrientationAnalysisConstants::DihedralType:
    if(!isinf(rod[3]))
      insideFZ = insideDihedralFZ(rod, FZorder);
    break;
  case OrientationAnalysisConstants::TetrahedralType:
    if(!isinf(rod[3]))
      insideFZ = insideCubicFZ(rod, OrientationAnalysisConstants::TetrahedralType);
    break;
  case OrientationAnalysisConstants::OctahedralType:
    if(!isinf(rod[3]))
      insideFZ = insideCubicFZ(rod, OrientationAnalysisConstants::OctahedralType);
    break;
  default:
    insideFZ = false;
    break;
  }
  return insideFZ;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool EMsoftSO3Sampler::insideCyclicFZ(double* rod, int order)
{

  bool insideFZ = false;

  if(!isinf(rod[3]))
  {
    // check the z-component vs. tan(pi/2n)
    insideFZ = fabs(rod[2] * rod[3]) <= LPs::BP[order - 1];
  }
  else if(rod[2] == 0.0)
  {
    insideFZ = true;
  }
  return insideFZ;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool EMsoftSO3Sampler::insideDihedralFZ(double* rod, int order)
{

  bool res = false, c1 = false, c2 = false;
  double r[3] = {rod[0] * rod[3], rod[1] * rod[3], rod[2] * rod[3]};
  const double r1 = 1.0;

  // first, check the z-component vs. tan(pi/2n)  (same as insideCyclicFZ)
  c1 = fabs(r[2]) <= LPs::BP[order - 1];
  res = false;

  // check the square boundary planes if c1=true
  if(c1)
  {
    switch(order)
    {
    case OrientationAnalysisConstants::TwoFoldAxisOrder:
      c2 = (fabs(r[0]) <= r1) && (fabs(r[1]) <= r1);
      break;
    case OrientationAnalysisConstants::ThreeFoldAxisOrder:
      c2 = fabs(LPs::srt * r[0] + 0.50 * r[1]) <= r1;
      c2 = c2 && (fabs(LPs::srt * r[0] - 0.50 * r[1]) <= r1);
      c2 = c2 && (fabs(r[1]) <= r1);
      break;
    case OrientationAnalysisConstants::FourFoldAxisOrder:
      c2 = (fabs(r[0]) <= r1) && (fabs(r[1]) <= r1);
      c2 = c2 && ((LPs::r22 * fabs(r[0] + r[1]) <= r1) && (LPs::r22 * fabs(r[0] - r[1]) <= r1));
      break;
    case OrientationAnalysisConstants::SixFoldAxisOrder:
      c2 = fabs(0.50 * r[0] + LPs::srt * r[1]) <= r1;
      c2 = c2 && (fabs(LPs::srt * r[0] + 0.50 * r[1]) <= r1);
      c2 = c2 && (fabs(LPs::srt * r[0] - 0.50 * r[1]) <= r1);
      c2 = c2 && (fabs(0.50 * r[0] - LPs::srt * r[1]) <= r1);
      c2 = c2 && (fabs(r[1]) <= r1);
      c2 = c2 && (fabs(r[0]) <= r1);
      break;
    default:
      res = false;
      break;
    }
    res = c2;
  }
  return res;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool EMsoftSO3Sampler::insideCubicFZ(double* rod, int ot)
{
  bool res = false, c1 = false, c2 = false;
  std::vector<double> r(3);
  r[0] = rod[0] * rod[3];
  r[1] = rod[1] * rod[3];
  r[2] = rod[2] * rod[3];
  const double r1 = 1.0;

  // primary cube planes (only needed for octahedral case)
  if(ot == OrientationAnalysisConstants::OctahedralType)
  {

    typedef OrientationTransforms<std::vector<double>, double> OrientationTransformsType;

    c1 = OrientationTransformsType::OMHelperType::maxval(OrientationTransformsType::OMHelperType::absValue(r)) <= LPs::BP[3];
  }
  else
  {
    c1 = true;
  }

  // octahedral truncation planes, both for tetrahedral and octahedral point groups
  c2 = ((fabs(r[0]) + fabs(r[1]) + fabs(r[2])) <= r1);

  // if both c1 and c2, then the point is inside
  if(c1 && c2)
  {
    res = true;
  }

  return res;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer EMsoftSO3Sampler::newFilterInstance(bool copyFilterParameters)
{
  EMsoftSO3Sampler::Pointer filter = EMsoftSO3Sampler::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EMsoftSO3Sampler::getCompiledLibraryName()
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EMsoftSO3Sampler::getBrandingString()
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EMsoftSO3Sampler::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EMsoftSO3Sampler::getGroupName()
{
  return SIMPL::FilterGroups::SamplingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EMsoftSO3Sampler::getSubGroupName()
{
  return SIMPL::FilterSubGroups::CrystallographyFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EMsoftSO3Sampler::getHumanLabel()
{
  return "SO3 Angle Sampler";
}
