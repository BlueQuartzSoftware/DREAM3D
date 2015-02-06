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


#include "EnsembleInfoReader.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/Qsettings.h>

#include "DREAM3DLib/DataArrays/DataArray.hpp"

#include "IO/IOConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnsembleInfoReader::EnsembleInfoReader() :
  FileReader(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_InputFile(""),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_PhaseTypesArrayName(DREAM3D::EnsembleData::PhaseTypes),
  m_CrystalStructures(NULL),
  m_PhaseTypes(NULL),
	m_crystruct(999),
	m_ptype(999)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnsembleInfoReader::~EnsembleInfoReader()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoReader::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FileSystemFilterParameter::New("Input Ensemble Info File", "InputFile", FilterParameterWidgetType::InputFileWidget, getInputFile(), false, "", "*.ini *.txt"));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Data Container Name", "DataContainerName", FilterParameterWidgetType::DataContainerSelectionWidget, getDataContainerName(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Cell Ensemble Attribute Matrix", "CellEnsembleAttributeMatrixName", FilterParameterWidgetType::StringWidget, getCellEnsembleAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Crystal Structures", "CrystalStructuresArrayName", FilterParameterWidgetType::StringWidget, getCrystalStructuresArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("PhaseTypes", "PhaseTypesArrayName", FilterParameterWidgetType::StringWidget, getPhaseTypesArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName() ) );
  setCellEnsembleAttributeMatrixName(reader->readString("CellEnsembleAttributeMatrixName", getCellEnsembleAttributeMatrixName() ) );
  setPhaseTypesArrayName(reader->readString("PhaseTypesArrayName", getPhaseTypesArrayName() ) );
  setCrystalStructuresArrayName(reader->readString("CrystalStructuresArrayName", getCrystalStructuresArrayName() ) );
  setInputFile( reader->readString( "InputFile", getInputFile() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EnsembleInfoReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(DataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellEnsembleAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(PhaseTypesArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(InputFile)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoReader::updateEnsembleInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_PhaseTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoReader::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0) { return; }
  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellEnsemble);
  if(getErrorCondition() < 0) { return; }

  QFileInfo fi(getInputFile());
  if (getInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("%1 needs the Input File Set and it was not.").arg(ClassName());
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if (fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist.");
    setErrorCondition(-388);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
	QString ext = fi.suffix();
	if (ext != "ini" && ext != "txt")
	{
		QString ss = QObject::tr("Incorrect file extension in '%1'. The file extension must be .ini or .txt").arg(getInputFile());
		setErrorCondition(-10018);
		notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
	}

  QVector<size_t> dims(1, 1);
  tempPath.update(getDataContainerName(), getCellEnsembleAttributeMatrixName(), getCrystalStructuresArrayName() );
  m_CrystalStructuresPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(this,  tempPath, Ebsd::CrystalStructure::UnknownCrystalStructure, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getDataContainerName(), getCellEnsembleAttributeMatrixName(), getPhaseTypesArrayName() );
  m_PhaseTypesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(this,  tempPath, DREAM3D::PhaseType::UnknownPhaseType, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_PhaseTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoReader::preflight()
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
int EnsembleInfoReader::readHeader()
{
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EnsembleInfoReader::readFile()
{
	dataCheck();

	VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
	AttributeMatrix::Pointer cellensembleAttrMat = m->getAttributeMatrix(getCellEnsembleAttributeMatrixName());

	QFileInfo fi(getInputFile());
	QDir parentPath = fi.path();
	if (!parentPath.mkpath("."))
	{
		QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
		setErrorCondition(-8005);
		notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
		return -1;
	}

	int numphases = 0;
  int pnum = 0;

	QSettings settings(getInputFile(), QSettings::IniFormat); // The .ini or .txt input file
	settings.beginGroup("EnsembleInfo");
	numphases = settings.value("Number_Phases").toInt(); // read number of phases from input file
	settings.endGroup();

	if (0 == numphases) // Either the group name "EnsembleInfo" is incorrect or 0 was entered as the Number_Phases 
	{
		QString ss = QObject::tr("Check the group name EnsembleInfo or Number_Phases");
		setErrorCondition(-10003);
		notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
		return -1;
	}

	QVector<size_t> tDims(1, numphases + 1);
	cellensembleAttrMat->resizeAttributeArrays(tDims);
	updateEnsembleInstancePointers();

	// Read the crystal structure and the phase type for each group phase number
	Q_FOREACH(QString group, settings.childGroups())
	{
		if (group == "EnsembleInfo") // Ensemble Info group only has the phase number information, ignore
			continue;
		settings.beginGroup(group);
		const QStringList childKeys = settings.childKeys();
		QStringList values;
		foreach(const QString &childKey, childKeys)
		{
			values << settings.value(childKey).toString(); // Values should contain the crystal structure and the phase type
		}

		if (values.size() != 2) // There are not two values for that group read from the file
		{
			QString ss = QObject::tr("The Crystal Structure and the Phase Type must be entered for each Phase Number in the .ini or .txt file");
			setErrorCondition(-10004);
			notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
			return -1;				
		}

		if (group.toInt() != pnum + 1) // The group number must match the output array index
		{
			QString ss = QObject::tr("One of the phase group numbers is incorrect in the .ini or .txt file");
			setErrorCondition(-10005);
			notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
			return -1;
		}

		ensembleLookup(values); // Lookup number for the crystal number string and the phase type string read from the file

		if (m_crystruct == -1) // The crystal structure name read from the file was not found in the lookup table
		{
			QString ss = QObject::tr("Incorrect crystal structure name '%1'").arg(values.at(0));
			setErrorCondition(-10006);
			notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
			return -1;
		}
		else
		{
			m_CrystalStructures[pnum] = m_crystruct;
		}
		if (m_ptype == -1)
		{
			QString ss = QObject::tr("Incorrect phase type name '%1'").arg(values.at(1)); // The phase type name read from the file was not found in the lookup table
			setErrorCondition(-10007);
			notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
			return -1;
		}
		else
		{
			m_PhaseTypes[pnum] = m_ptype; 
		}
		pnum++;
		settings.endGroup();
	}	
	if (pnum != numphases)
	{
		QString ss = QObject::tr("The Number_Phases '%1' is incorrect in the .ini or .txt file").arg(numphases); // numphases does not match group phases enter in the file
		setErrorCondition(-10008);
		notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
		return -1;
	}
  notifyStatusMessage(getHumanLabel(), "Complete");
	return 0;
}

		// -----------------------------------------------------------------------------
		//
		// -----------------------------------------------------------------------------
		void EnsembleInfoReader::ensembleLookup(QStringList list)
		{
			// assign the corresponding number to the crystal structure string read from the input file
			if (QString::compare(list.at(0), "Hexagonal", Qt::CaseInsensitive) == 0) {
				m_crystruct = 0;
			}
			else if (QString::compare(list.at(0), "Cubic", Qt::CaseInsensitive) == 0) {
				m_crystruct = 1;
			}
			else if (QString::compare(list.at(0), "OrthoRhombic", Qt::CaseInsensitive) == 0) {
				m_crystruct = 2;
			}
			else if (QString::compare(list.at(0), "AxisOrthoRhombic", Qt::CaseInsensitive) == 0) {
				m_crystruct = 3;
			}
			else if (QString::compare(list.at(0), "UnknownCrystalStructure", Qt::CaseInsensitive) == 0) {
				m_crystruct = 999;
			}
			else{
				m_crystruct = -1; // no match for crystal structure name read from file
			}

			// assign the corresponding number to the phase type string read from the input file
			if (QString::compare(list.at(1), "PrimaryPhase", Qt::CaseInsensitive) == 0) {
				m_ptype = 0;
			}
			else if (QString::compare(list.at(1), "PrecipitatePhase", Qt::CaseInsensitive) == 0) {
				m_ptype = 1;
			}
			else if (QString::compare(list.at(1), "TransformationPhase", Qt::CaseInsensitive) == 0) {
				m_ptype = 2;
			}
			else if (QString::compare(list.at(1), "MatrixPhase", Qt::CaseInsensitive) == 0) {
				m_ptype = 3;
			}
			else if (QString::compare(list.at(1), "BoundaryPhase", Qt::CaseInsensitive) == 0) {
				m_ptype = 4;
			}
			else if (QString::compare(list.at(1), "UnknownPhaseType", Qt::CaseInsensitive) == 0) {
				m_ptype = 999;
			}
			else{
				m_ptype = -1; // no match for phase type name read from file
			}
		}


		// -----------------------------------------------------------------------------
		//
		// -----------------------------------------------------------------------------
		AbstractFilter::Pointer EnsembleInfoReader::newFilterInstance(bool copyFilterParameters)
		{
			EnsembleInfoReader::Pointer filter = EnsembleInfoReader::New();
			if (true == copyFilterParameters)
			{
				copyFilterParameterInstanceVariables(filter.get());
			}
			return filter;
		}

		// -----------------------------------------------------------------------------
		//
		// -----------------------------------------------------------------------------
		const QString EnsembleInfoReader::getCompiledLibraryName()
		{
			return IO::IOBaseName;
		}


		// -----------------------------------------------------------------------------
		//
		// -----------------------------------------------------------------------------
		const QString EnsembleInfoReader::getGroupName()
		{
			return DREAM3D::FilterGroups::IOFilters;
		}


		// -----------------------------------------------------------------------------
		//
		// -----------------------------------------------------------------------------
		const QString EnsembleInfoReader::getSubGroupName()
		{
			return DREAM3D::FilterSubGroups::InputFilters;
		}

		// -----------------------------------------------------------------------------
		//
		// -----------------------------------------------------------------------------
		const QString EnsembleInfoReader::getHumanLabel()
		{
			return "Read Ensemble Info File";
		}
