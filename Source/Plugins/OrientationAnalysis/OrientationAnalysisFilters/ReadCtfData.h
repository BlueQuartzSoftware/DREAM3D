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
#ifndef _ReadCtfData_H_
#define _ReadCtfData_H_

#include <QtCore/QString>
#include <QtCore/QScopedPointer>
#include <QtCore/QDateTime>

#include "EbsdLib/HKL/CtfReader.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/DataArrays/StringDataArray.hpp"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/DataContainer.h"




struct Ctf_Private_Data
{
  QVector<size_t> dims;
  QVector<float> resolution;
  QVector<float> origin;
  QVector<CtfPhase::Pointer> phases;
};

enum CTF_READ_FLAG
{
  CTF_FULL_FILE,
  CTF_HEADER_ONLY
};

// our PIMPL private class
class ReadCtfDataPrivate;

/**
* @class ReadCtfData ReadCtfData.h /FilterCategoryFilters/ReadCtfData.h
* @brief
* @author
* @date
* @version 1.0
*/
class ReadCtfData : public AbstractFilter
{
  Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
    Q_DECLARE_PRIVATE(ReadCtfData)

public:
  DREAM3D_SHARED_POINTERS(ReadCtfData)
    DREAM3D_STATIC_NEW_MACRO(ReadCtfData)
    DREAM3D_TYPE_MACRO_SUPER(ReadCtfData, AbstractFilter)

    virtual ~ReadCtfData();

  DREAM3D_FILTER_PARAMETER(QString, DataContainerName)
    Q_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)

    DREAM3D_FILTER_PARAMETER(QString, CellEnsembleAttributeMatrixName)
    Q_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(QString, CellAttributeMatrixName)
    Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(bool, FileWasRead)
    Q_PROPERTY(bool FileWasRead READ getFileWasRead)

    DREAM3D_INSTANCE_STRING_PROPERTY(PhaseNameArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(MaterialNameArrayName)

    DREAM3D_FILTER_PARAMETER(QString, InputFile)
    Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

  void populateCtfData(CtfReader* reader, DataContainer::Pointer m, QVector<size_t> dims, CTF_READ_FLAG = CTF_FULL_FILE);


  /**
  * @brief This returns the group that the filter belonds to. You can select
  * a different group if you want. The string returned here will be displayed
  * in the GUI for the filter
  */
  virtual const QString getCompiledLibraryName();
  virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
  virtual const QString getGroupName();

  /**
  * @brief This returns the group that the filter belonds to. You can select
  * a different group if you want. The string returned here will be displayed
  * in the GUI for the filter
  */
  virtual const QString getSubGroupName();

  /**
  * @brief This returns a string that is displayed in the GUI. It should be readable
  * and understandable by humans.
  */
  virtual const QString getHumanLabel();

  /**
  * @brief This method will instantiate all the end user settable options/parameters
  * for this filter
  */
  virtual void setupFilterParameters();

  /**
  * @brief This method will write the options to a file
  * @param writer The writer that is used to write the options to a file
  */
  virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

  /**
  * @brief This method will read the options from a file
  * @param reader The reader that is used to read the options from a file
  */
  virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

  /**
  * @brief Reimplemented from @see AbstractFilter class
  */
  virtual void execute();

  /**
  * @brief This function runs some sanity checks on the DataContainer and inputs
  * in an attempt to ensure the filter can process the inputs.
  */
  virtual void preflight();

  /* These are non-exposed to the user through the GUI. Manual Pipelines are OK to set them */
  DREAM3D_INSTANCE_PROPERTY(uint32_t, RefFrameZDir)
    DREAM3D_INSTANCE_PROPERTY(int, Manufacturer)

    DREAM3D_PIMPL_PROPERTY_DECL(QString, InputFile_Cache)
    DREAM3D_PIMPL_PROPERTY_DECL(QDateTime, TimeStamp_Cache)
    DREAM3D_PIMPL_PROPERTY_DECL(Ctf_Private_Data, Data)
    Q_PROPERTY(Ctf_Private_Data Data READ getData WRITE setData)

public slots:
  void flushCache();


signals:
  void updateFilterParameters(AbstractFilter* filter);
  void parametersChanged();
  void preflightAboutToExecute();
  void preflightExecuted();

protected:
  ReadCtfData();

  /**
  * @brief Checks for the appropriate parameter values and availability of
  * arrays in the data container
  * @param preflight
  * @param voxels The number of voxels
  * @param features The number of features
  * @param ensembles The number of ensembles
  */
  void dataCheck();

  /**
  * @brief readCtfFile This reads the Ctf file and puts the data into the Voxel Data container
  */
  void readCtfFile();

  /**
  * @brief This method reads the values for the phase type, crystal structure
  * and precipitate fractions from the EBSD file.
  * @param reader The EbsdReader instance
  * @param precipFractions Container to hold the precipitate fractions (out)
  * @param crystalStructures Container to hold the crystal structures (out)
  * @return Zero/Positive on Success - Negative on error.
  */
  int loadInfo(CtfReader* reader)
  {
    QVector<CtfPhase::Pointer> phases = getData().phases;
    if (phases.size() == 0)
    {
      setErrorCondition(reader->getErrorCode());
      notifyErrorMessage(getHumanLabel(), reader->getErrorMessage(), getErrorCondition());
      return getErrorCondition();
    }

    DataArray<unsigned int>::Pointer crystalStructures = DataArray<unsigned int>::CreateArray(phases.size() + 1, getCrystalStructuresArrayName());
    StringDataArray::Pointer materialNames = StringDataArray::CreateArray(phases.size() + 1, getMaterialNameArrayName());
    QVector<size_t> dims(1, 6);
    FloatArrayType::Pointer latticeConstants = FloatArrayType::CreateArray(phases.size() + 1, dims, getLatticeConstantsArrayName());

    // Initialize the zero'th element to unknowns. The other elements will
    // be filled in based on values from the data file
    crystalStructures->setValue(0, Ebsd::CrystalStructure::UnknownCrystalStructure);
    materialNames->setValue(0, "Invalid Phase");
    latticeConstants->setComponent(0, 0, 0.0f);
    latticeConstants->setComponent(0, 1, 0.0f);
    latticeConstants->setComponent(0, 2, 0.0f);
    latticeConstants->setComponent(0, 3, 0.0f);
    latticeConstants->setComponent(0, 4, 0.0f);
    latticeConstants->setComponent(0, 5, 0.0f);

    for (size_t i = 0; i < phases.size(); i++)
    {
      int phaseID = phases[i]->getPhaseIndex();
      crystalStructures->setValue(phaseID, phases[i]->determineCrystalStructure());
      materialNames->setValue(phaseID, phases[i]->getMaterialName());
      QVector<float> lc = phases[i]->getLatticeConstants();

      latticeConstants->setComponent(phaseID, 0, lc[0]);
      latticeConstants->setComponent(phaseID, 1, lc[1]);
      latticeConstants->setComponent(phaseID, 2, lc[2]);
      latticeConstants->setComponent(phaseID, 3, lc[3]);
      latticeConstants->setComponent(phaseID, 4, lc[4]);
      latticeConstants->setComponent(phaseID, 5, lc[5]);

    }
    DataContainer::Pointer vdc = getDataContainerArray()->getDataContainer(getDataContainerName());
    if (NULL == vdc) { return -1; }
    AttributeMatrix::Pointer attrMatrix = vdc->getAttributeMatrix(getCellEnsembleAttributeMatrixName());
    if (NULL == attrMatrix.get()) { return -2; }

    // Resize the AttributeMatrix based on the size of the crystal structures array
    QVector<size_t> tDims(1, crystalStructures->getNumberOfTuples());
    attrMatrix->resizeAttributeArrays(tDims);
    // Now add the attributeArray to the AttributeMatrix
    attrMatrix->addAttributeArray(DREAM3D::EnsembleData::CrystalStructures, crystalStructures);
    attrMatrix->addAttributeArray(DREAM3D::EnsembleData::MaterialName, materialNames);
    attrMatrix->addAttributeArray(DREAM3D::EnsembleData::LatticeConstants, latticeConstants);

    // Now reset the internal ensemble array references to these new arrays
    m_CrystalStructuresPtr = crystalStructures;
    if (NULL != m_CrystalStructuresPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    {
      m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */

    m_LatticeConstantsPtr = latticeConstants;
    if (NULL != m_LatticeConstantsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    {
      m_LatticeConstants = m_LatticeConstantsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    return 0;
  }

private:
  QScopedPointer<ReadCtfDataPrivate> const d_ptr;

  DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, CellPhases)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, CellEulerAngles)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(uint32_t, CrystalStructures)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, LatticeConstants)

    ReadCtfData(const ReadCtfData&); // Copy Constructor Not Implemented
  void operator=(const ReadCtfData&); // Operator '=' Not Implemented
};

Q_DECLARE_METATYPE(Ctf_Private_Data)

#endif /* _ReadCtfData_H_ */





