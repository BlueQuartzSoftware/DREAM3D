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

#include <memory>

#include "EMsoftSO3Sampler.h"

#include <cmath>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "OrientationLib/Core/Orientation.hpp"
#include "OrientationLib/Core/OrientationTransformation.hpp"
#include "OrientationLib/Core/Quaternion.hpp"
#include "OrientationLib/OrientationLibConstants.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,
  AttributeMatrixID22 = 22,

  DataArrayID31 = 31,

  DataContainerID = 1
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMsoftSO3Sampler::EMsoftSO3Sampler()
: m_sampleModeSelector(0)
, m_PointGroup(32)
, m_Numsp(5)
, m_MisOr(3.0)
, m_MisOrFull(3.0)
, m_OffsetGrid(false)
, m_DataContainerName(SIMPL::Defaults::ImageDataContainerName)
, m_EMsoftAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
{
  m_RefOr[0] = 0.0;
  m_RefOr[1] = 0.0;
  m_RefOr[2] = 0.0;

  m_RefOrFull[0] = 0.0;
  m_RefOrFull[1] = 0.0;
  m_RefOrFull[2] = 0.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMsoftSO3Sampler::~EMsoftSO3Sampler() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMsoftSO3Sampler::setupFilterParameters()
{
  FilterParameterVectorType parameters;
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

  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Euler Angles", EulerAnglesArrayName, DataContainerName, EMsoftAttributeMatrixName, FilterParameter::CreatedArray, EMsoftSO3Sampler));

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
  clearErrorCode();
  clearWarningCode();

  // Try to get the DataContainer that the user has named.
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  // If that did not exist then create one with that name
  if(nullptr == m.get())
  {
    m = getDataContainerArray()->createNonPrereqDataContainer(this, getDataContainerName());
  }
  if(getErrorCode() < 0 || nullptr == m.get())
  {
    return;
  } // This truly is an error condition.

  // DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName(), DataContainerID);
  // if (getErrorCode() < 0) { return; }

  // Now try to either get or create the needed AttributeMatrix
  std::vector<size_t> tDims(1, 1);
  AttributeMatrix::Pointer emsoftAttrMat = m->getAttributeMatrix(getEMsoftAttributeMatrixName());
  if(nullptr == emsoftAttrMat.get())
  {
    emsoftAttrMat = m->createNonPrereqAttributeMatrix(this, getEMsoftAttributeMatrixName(), tDims, AttributeMatrix::Type::Generic, AttributeMatrixID21);
  }
  if(getErrorCode() < 0 || nullptr == emsoftAttrMat.get())
  {
    return;
  }

  //  std::vector<size_t> tDims(1, 1);
  //  AttributeMatrix::Pointer emsoftAttrMat = m->createNonPrereqAttributeMatrix(this, getEMsoftAttributeMatrixName(), tDims, AttributeMatrix::Type::Generic, AttributeMatrixID22);
  //  if (getErrorCode() < 0) { return; }
  //  m->addOrReplaceAttributeMatrix(emsoftAttrMat->getName(),emsoftAttrMat);

  // check on the point group index; must be between 1 and 32.
  if(getsampleModeSelector() == 0)
  {
    if((getPointGroup() < 1) || (getPointGroup() > 32))
    {
      QString ss = QObject::tr("Point group number must fall in interval [1,32]");
      setWarningCondition(-70001, ss);
    }
  }

  // make sure that the misorientation angle lies in [0,90], and the Euler angles in [0,360], [0,180], [0,360]
  if(getsampleModeSelector() == 1)
  {
    if((getMisOr() < 0.0) || (getMisOr() > 90.0))
    {
      QString ss = QObject::tr("Misorientation angle must fall in interval [0,90]");
      setWarningCondition(-70002, ss);
    }
    if((getRefOr()[0] < 0.0) || (getRefOr()[0] > 360.0f) || (getRefOr()[1] < 0.0f) || (getRefOr()[1] > 180.0f) || (getRefOr()[2] < 0.0f) || (getRefOr()[2] > 360.0f))
    {
      QString ss = QObject::tr("Euler angles must be positive and less than [360°,180°,360°]");
      setWarningCondition(-70003, ss);
    }
  }
  if(getsampleModeSelector() == 2)
  {
    if((getMisOrFull() < 0.0) || (getMisOrFull() > 90.0))
    {
      QString ss = QObject::tr("Misorientation angle must fall in interval [0,90]");
      setWarningCondition(-70004, ss);
    }
    if((getRefOrFull()[0] < 0.0f) || (getRefOrFull()[0] > 360.0f) || (getRefOrFull()[1] < 0.0f) || (getRefOrFull()[1] > 180.0f) || (getRefOrFull()[2] < 0.0f) || (getRefOrFull()[2] > 360.0f))
    {
      QString ss = QObject::tr("Euler angles must be positive and less than [360°,180°,360°]");
      setWarningCondition(-70005, ss);
    }
  }

  // check on the number of sampling intervals (>1)
  if(getNumsp() < 1)
  {
    QString ss = QObject::tr("Number of sampling intervals must be at least 1 ");
    setWarningCondition(-70002, ss);
  }

  QVector<DataArrayPath> dataArraypaths;

  // allocate space for the EulerAngles array, which will subsequently be filled by the SO(3) sampling code.
  std::vector<size_t> cDims(1);
  cDims[0] = 3;
  DataArrayPath tempPath;
  tempPath.update(getDataContainerName().getDataContainerName(), getEMsoftAttributeMatrixName(), getEulerAnglesArrayName());
  m_EulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0.0f, cDims, "", DataArrayID31);
  if(nullptr != m_EulerAnglesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
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
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  if(getCancel())
  {
    return;
  }

  OrientationListArrayType FZlist;

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
                OrientationD cu(x, y, z);
                OrientationD rod = OrientationTransformation::cu2ro<OrientationD, OrientationD>(cu);

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
        QString ss = QString("Euler Angles | Tested: %1 of %2 | Inside RFZ: %3 ").arg(QString::number(Di), QString::number(Totp), QString::number(Dg));
        notifyStatusMessage(ss);
        Dc += Dn;
      }
      if(getCancel())
      {
        break;
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
    OrientationD sigma(3), referenceOrientation(3);
    referenceOrientation[0] = static_cast<double>(getRefOr()[0] * SIMPLib::Constants::k_PiOver180);
    referenceOrientation[1] = static_cast<double>(getRefOr()[1] * SIMPLib::Constants::k_PiOver180);
    referenceOrientation[2] = static_cast<double>(getRefOr()[2] * SIMPLib::Constants::k_PiOver180);
    OrientationD sigm = OrientationTransformation::eu2ro<OrientationD, OrientationD>(referenceOrientation);
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
            OrientationD cu(-x, -y, -semi);
            OrientationD rod = OrientationTransformation::cu2ro<OrientationD, OrientationD>(cu);
            RodriguesComposition(sigma, rod);
            FZlist.push_back(rod);
            Dg += 1;
          }
          {
            OrientationD cu(-x, -y, semi);
            OrientationD rod = OrientationTransformation::cu2ro<OrientationD, OrientationD>(cu);
            RodriguesComposition(sigma, rod);
            FZlist.push_back(rod);
            Dg += 1;
          }
        }
        if(getCancel())
        {
          break;
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
            OrientationD cu(-semi, -y, -z);
            OrientationD rod = OrientationTransformation::cu2ro<OrientationD, OrientationD>(cu);
            RodriguesComposition(sigma, rod);
            FZlist.push_back(rod);
            Dg += 1;
          }
          {
            OrientationD cu(semi, -y, -z);
            OrientationD rod = OrientationTransformation::cu2ro<OrientationD, OrientationD>(cu);
            RodriguesComposition(sigma, rod);
            FZlist.push_back(rod);
            Dg += 1;
          }
        }
        if(getCancel())
        {
          break;
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
            OrientationD cu(-x, -semi, -z);
            OrientationD rod = OrientationTransformation::cu2ro<OrientationD, OrientationD>(cu);
            RodriguesComposition(sigma, rod);
            FZlist.push_back(rod);
            Dg += 1;
          }
          {
            OrientationD cu(-x, semi, -z);
            OrientationD rod = OrientationTransformation::cu2ro<OrientationD, OrientationD>(cu);
            RodriguesComposition(sigma, rod);
            FZlist.push_back(rod);
            Dg += 1;
          }
        }
        if(getCancel())
        {
          break;
        }
      }

      // report on status of computation
      if(Dg > Dc)
      {
        QString ss = QString("Euler Angles | Generated: %1 / %2").arg(QString::number(Dg), QString::number(Totp));
        notifyStatusMessage(ss);
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
              OrientationD cu(-x, -y, -z);
              OrientationD rod = OrientationTransformation::cu2ro<OrientationD, OrientationD>(cu);
              RodriguesComposition(sigma, rod);
              FZlist.push_back(rod);
              Dg += 1;
            }
          }
        }
        // report on status of computation
        if(Dg > Dc)
        {
          QString ss = QString("Euler Angles | Generated: %1 / %2").arg(QString::number(Dg), QString::number(Totp));
          notifyStatusMessage(ss);
          Dc += Dn;
        }
      }
    }
  }

  // resize the EulerAngles array to the number of items in FZlist; don't forget to redefine the hard pointer
  AttributeMatrix::Pointer am = getDataContainerArray()->getAttributeMatrix(DataArrayPath(getDataContainerName().getDataContainerName(), getEMsoftAttributeMatrixName(), ""));
  std::vector<size_t> tDims(1, FZlist.size());
  am->resizeAttributeArrays(tDims);
  m_EulerAngles = m_EulerAnglesPtr.lock()->getPointer(0);

  // copy the Rodrigues vectors as Euler angles into the m_EulerAngles array; convert doubles to floats along the way
  int j = -1;
  for(const OrientationD& rod : FZlist)
  {
    j += 1;
    OrientationD eu = OrientationTransformation::ro2eu<OrientationD, OrientationD>(rod);

    m_EulerAngles[j * 3 + 0] = static_cast<float>(eu[0]);
    m_EulerAngles[j * 3 + 1] = static_cast<float>(eu[1]);
    m_EulerAngles[j * 3 + 2] = static_cast<float>(eu[2]);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMsoftSO3Sampler::RodriguesComposition(OrientationD sigma, OrientationD& rod)
{
  OrientationD rho(3), rhomis(3);
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
    if(!std::isinf(rod[3]))
    {
      insideFZ = insideDihedralFZ(rod, FZorder);
    }
    break;
  case OrientationAnalysisConstants::TetrahedralType:
    if(!std::isinf(rod[3]))
    {
      insideFZ = insideCubicFZ(rod, OrientationAnalysisConstants::TetrahedralType);
    }
    break;
  case OrientationAnalysisConstants::OctahedralType:
    if(!std::isinf(rod[3]))
    {
      insideFZ = insideCubicFZ(rod, OrientationAnalysisConstants::OctahedralType);
    }
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

  if(! std::isinf(rod[3]))
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
  r[0] = std::fabs(rod[0] * rod[3]);
  r[1] = std::fabs(rod[1] * rod[3]);
  r[2] = std::fabs(rod[2] * rod[3]);
  const double r1 = 1.0;

  // primary cube planes (only needed for octahedral case)
  if(ot == OrientationAnalysisConstants::OctahedralType)
  {
    double maxValue = *(std::max_element(r.begin(), r.end()));
    c1 = (maxValue <= LPs::BP[3]);
  }
  else
  {
    c1 = true;
  }

  // octahedral truncation planes, both for tetrahedral and octahedral point groups
  c2 = ((r[0] + r[1] + r[2]) <= r1);

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
AbstractFilter::Pointer EMsoftSO3Sampler::newFilterInstance(bool copyFilterParameters) const
{
  EMsoftSO3Sampler::Pointer filter = EMsoftSO3Sampler::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMsoftSO3Sampler::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMsoftSO3Sampler::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMsoftSO3Sampler::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMsoftSO3Sampler::getGroupName() const
{
  return SIMPL::FilterGroups::SamplingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid EMsoftSO3Sampler::getUuid() const
{
  return QUuid("{b78d8825-d3ac-5351-be20-172f07fd2aec}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMsoftSO3Sampler::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CrystallographyFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMsoftSO3Sampler::getHumanLabel() const
{
  return "SO3 Angle Sampler";
}

// -----------------------------------------------------------------------------
EMsoftSO3Sampler::Pointer EMsoftSO3Sampler::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<EMsoftSO3Sampler> EMsoftSO3Sampler::New()
{
  struct make_shared_enabler : public EMsoftSO3Sampler
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString EMsoftSO3Sampler::getNameOfClass() const
{
  return QString("EMsoftSO3Sampler");
}

// -----------------------------------------------------------------------------
QString EMsoftSO3Sampler::ClassName()
{
  return QString("EMsoftSO3Sampler");
}

// -----------------------------------------------------------------------------
void EMsoftSO3Sampler::setsampleModeSelector(int value)
{
  m_sampleModeSelector = value;
}

// -----------------------------------------------------------------------------
int EMsoftSO3Sampler::getsampleModeSelector() const
{
  return m_sampleModeSelector;
}

// -----------------------------------------------------------------------------
void EMsoftSO3Sampler::setPointGroup(int value)
{
  m_PointGroup = value;
}

// -----------------------------------------------------------------------------
int EMsoftSO3Sampler::getPointGroup() const
{
  return m_PointGroup;
}

// -----------------------------------------------------------------------------
void EMsoftSO3Sampler::setNumsp(int value)
{
  m_Numsp = value;
}

// -----------------------------------------------------------------------------
int EMsoftSO3Sampler::getNumsp() const
{
  return m_Numsp;
}

// -----------------------------------------------------------------------------
void EMsoftSO3Sampler::setMisOr(double value)
{
  m_MisOr = value;
}

// -----------------------------------------------------------------------------
double EMsoftSO3Sampler::getMisOr() const
{
  return m_MisOr;
}

// -----------------------------------------------------------------------------
void EMsoftSO3Sampler::setRefOr(const FloatVec3Type& value)
{
  m_RefOr = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type EMsoftSO3Sampler::getRefOr() const
{
  return m_RefOr;
}

// -----------------------------------------------------------------------------
void EMsoftSO3Sampler::setMisOrFull(double value)
{
  m_MisOrFull = value;
}

// -----------------------------------------------------------------------------
double EMsoftSO3Sampler::getMisOrFull() const
{
  return m_MisOrFull;
}

// -----------------------------------------------------------------------------
void EMsoftSO3Sampler::setRefOrFull(const FloatVec3Type& value)
{
  m_RefOrFull = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type EMsoftSO3Sampler::getRefOrFull() const
{
  return m_RefOrFull;
}

// -----------------------------------------------------------------------------
void EMsoftSO3Sampler::setOffsetGrid(bool value)
{
  m_OffsetGrid = value;
}

// -----------------------------------------------------------------------------
bool EMsoftSO3Sampler::getOffsetGrid() const
{
  return m_OffsetGrid;
}

// -----------------------------------------------------------------------------
void EMsoftSO3Sampler::setEulerAnglesArrayName(const QString& value)
{
  m_EulerAnglesArrayName = value;
}

// -----------------------------------------------------------------------------
QString EMsoftSO3Sampler::getEulerAnglesArrayName() const
{
  return m_EulerAnglesArrayName;
}

// -----------------------------------------------------------------------------
void EMsoftSO3Sampler::setDataContainerName(const DataArrayPath& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath EMsoftSO3Sampler::getDataContainerName() const
{
  return m_DataContainerName;
}

// -----------------------------------------------------------------------------
void EMsoftSO3Sampler::setEMsoftAttributeMatrixName(const QString& value)
{
  m_EMsoftAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString EMsoftSO3Sampler::getEMsoftAttributeMatrixName() const
{
  return m_EMsoftAttributeMatrixName;
}
