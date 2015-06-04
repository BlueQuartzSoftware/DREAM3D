/*
 * Your License or Copyright Information can go here
 */

#include "SampleSurfaceMeshSpecifiedPoints.h"

#include <fstream>
#include <sstream>

#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "DREAM3DLib/FilterParameters/FileSystemFilterParameter.h"

#include "Sampling/SamplingConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SampleSurfaceMeshSpecifiedPoints::SampleSurfaceMeshSpecifiedPoints() :
  m_InputFilePath(""),
  m_OutputFilePath(""),
  m_FeatureIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SampleSurfaceMeshSpecifiedPoints::~SampleSurfaceMeshSpecifiedPoints()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMeshSpecifiedPoints::setupFilterParameters()
{
  FilterParameterVector parameters = getFilterParameters();
  parameters.push_back(FileSystemFilterParameter::New("Specified Points Filte Path", "InputFilePath", FilterParameterWidgetType::InputFileWidget, getInputFilePath(), false, "", "*.raw *.bin"));
  parameters.push_back(FileSystemFilterParameter::New("Sampled Values File Path", "OutputFilePath", FilterParameterWidgetType::OutputFileWidget, getOutputFilePath(), false, ""));

  // Set the filter parameters after creating them
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMeshSpecifiedPoints::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  SampleSurfaceMesh::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setInputFilePath(reader->readString("InputFilePath", getInputFilePath()));
  setOutputFilePath(reader->readString("OutputFilePath", getOutputFilePath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SampleSurfaceMeshSpecifiedPoints::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  SampleSurfaceMesh::writeFilterParameters(writer, index);
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(InputFilePath)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputFilePath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMeshSpecifiedPoints::updateVertexInstancePointers()
{
	setErrorCondition(0);
	if (NULL != m_FeatureIdsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
	{
		m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
	} /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMeshSpecifiedPoints::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  if (true == m_InputFilePath.isEmpty())
  {
	  QString ss = QObject::tr("The Input file name must be set before executing this filter.");
	  setErrorCondition(-1);
	  notifyErrorMessage(getHumanLabel(), ss, -1);
  }
  if (true == m_OutputFilePath.isEmpty())
  {
	  QString ss = QObject::tr("The Output file name must be set before executing this filter.");
	  setErrorCondition(-1);
	  notifyErrorMessage(getHumanLabel(), ss, -1);
  }

  DataContainer::Pointer v = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, "SpecifiedPoints");
  if (getErrorCondition() < 0) { return; }

  VertexGeom::Pointer vertices = VertexGeom::CreateGeometry(1, DREAM3D::Geometry::VertexGeometry);
  v->setGeometry(vertices);

  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer vertexAttrMat = v->createNonPrereqAttributeMatrix<AbstractFilter>(this, "SpecifiedPointsData", tDims, DREAM3D::AttributeMatrixType::Vertex);
  if (getErrorCondition() < 0 || NULL == vertexAttrMat.get()) { return; }

  QVector<size_t> cDims(1, 1);
  tempPath.update("SpecifiedPoints", "SpecifiedPointsData", "FeatureIds");
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<>(this, tempPath, -301, dims);  Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (NULL != m_FeatureIdsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
	  m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMeshSpecifiedPoints::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  SampleSurfaceMesh::preflight();
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexGeom::Pointer SampleSurfaceMeshSpecifiedPoints::generate_points()
{
	m_NumPoints = 0;

	std::ifstream inFile;
	inFile.open(m_InputFilePath.toLatin1().data());

	//get the number of points in the specified points file
	inFile >> m_NumPoints;

	DataContainer::Pointer v = getDataContainerArray()->getDataContainer("SpecifiedPoints");
	VertexGeom::Pointer points = VertexGeom::CreateGeometry(m_NumPoints, "Points");

	float coords[3] = { 0.0f, 0.0f, 0.0f };

	for (int64_t i = 0; i < m_NumPoints; i++)
	{
		inFile >> coords[0] >> coords[1] >> coords[2];
		points->setCoords(i, coords);
	}

	v->setGeometry(points);

	return points;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMeshSpecifiedPoints::assign_points(Int32ArrayType::Pointer iArray)
{
	QVector<size_t> tDims(1, m_NumPoints);
	getDataContainerArray()->getDataContainer("SpecifiedPoints")->getAttributeMatrix("SpecifiedPointsData")->resizeAttributeArrays(tDims);
	updateVertexInstancePointers();

	int32_t* ids = iArray->getPointer(0);
	for (int64_t i = 0; i < m_NumPoints; i++)
	{
		m_FeatureIds[i] = ids[i];
	}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMeshSpecifiedPoints::execute()
{

  dataCheck();
  // Check to make sure you made it through the data check. Errors would have been reported already so if something
  // happens to fail in the dataCheck() then we simply return
  if(getErrorCondition() < 0) { return; }
  setErrorCondition(0);

  /* Place all your code to execute your filter here. */
  SampleSurfaceMesh::execute();

  std::ofstream outFile;
  outFile.open(m_OutputFilePath.toLatin1().data());
  for (int64_t i = 0; i < m_NumPoints; i++)
  {
	  outFile << m_FeatureIds[i] << std::endl;
  }


  if (getCancel() == true)
  {
    return;
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer SampleSurfaceMeshSpecifiedPoints::newFilterInstance(bool copyFilterParameters)
{
  /*
  * write code to optionally copy the filter parameters from the current filter into the new instance
  */
  SampleSurfaceMeshSpecifiedPoints::Pointer filter = SampleSurfaceMeshSpecifiedPoints::New();
  if(true == copyFilterParameters)
  {
    /* If the filter uses all the standard Filter Parameter Widgets you can probabaly get
     * away with using this method to copy the filter parameters from the current instance
     * into the new instance
     */
    copyFilterParameterInstanceVariables(filter.get());
    /* If your filter is using a lot of custom FilterParameterWidgets @see ReadH5Ebsd then you
     * may need to copy each filter parameter explicitly plus any other instance variables that
     * are needed into the new instance. Here is some example code from ReadH5Ebsd
     */
    //    DREAM3D_COPY_INSTANCEVAR(OutputFile)
    //    DREAM3D_COPY_INSTANCEVAR(ZStartIndex)
    //    DREAM3D_COPY_INSTANCEVAR(ZEndIndex)
    //    DREAM3D_COPY_INSTANCEVAR(ZResolution)
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SampleSurfaceMeshSpecifiedPoints::getCompiledLibraryName()
{
  return SamplingConstants::SamplingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SampleSurfaceMeshSpecifiedPoints::getGroupName()
{
	return DREAM3D::FilterGroups::SamplingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SampleSurfaceMeshSpecifiedPoints::getHumanLabel()
{
  return "Sample Surface Mesh at Specified Points";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SampleSurfaceMeshSpecifiedPoints::getSubGroupName()
{
  return "Misc";
}

