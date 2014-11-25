/*
 * Your License or Copyright Information can go here
 */

#include "RecrystalizeVolume.h"
#include "CellularAutomataHelpers.hpp"

#include <QtCore/QString>

#include "CellularAutomata/CellularAutomataConstants.h"

#define INIT_SYNTH_VOLUME_CHECK(var, errCond) \
  if (m_##var <= 0) { QString ss = QObject::tr(":%1 must be a value > 0\n").arg( #var); notifyErrorMessage(getHumanLabel(), ss, errCond);}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RecrystalizeVolume::RecrystalizeVolume() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::SyntheticVolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName)
{
  m_Dimensions.x = 128;
  m_Dimensions.y = 128;
  m_Dimensions.z = 128;

  m_Resolution.x = 0.25;
  m_Resolution.y = 0.25;
  m_Resolution.z = 0.25;

  m_Origin.x = 0.0f;
  m_Origin.y = 0.0f;
  m_Origin.z = 0.0f;
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RecrystalizeVolume::~RecrystalizeVolume()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RecrystalizeVolume::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Nucleation Rate", "NucleationRate", FilterParameterWidgetType::DoubleWidget, getNucleationRate(), false, "per cubic micron per time step"));
  parameters.push_back(FilterParameter::New("New DataContainer Name", "DataContainerName", FilterParameterWidgetType::StringWidget, getDataContainerName(), true, ""));
  parameters.push_back(FilterParameter::New("New Cell Attribute Matrix Name", "CellAttributeMatrixName", FilterParameterWidgetType::StringWidget, getCellAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("New Cell Feature Attribute Matrix Name", "CellFeatureAttributeMatrixName", FilterParameterWidgetType::StringWidget, getCellFeatureAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Dimensions", "Dimensions", FilterParameterWidgetType::IntVec3Widget, getDimensions(), false, "Voxels"));
  parameters.push_back(FilterParameter::New("Resolution", "Resolution", FilterParameterWidgetType::FloatVec3Widget, getResolution(), false, "Microns"));
  parameters.push_back(FilterParameter::New("Origin", "Origin", FilterParameterWidgetType::FloatVec3Widget, getOrigin(), false, "Microns"));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RecrystalizeVolume::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNucleationRate(reader->readValue("NucleationRate", getNucleationRate() ) );
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName() ) );
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName() ) );
  setCellFeatureAttributeMatrixName(reader->readString("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName() ) );
  setDimensions( reader->readIntVec3("Dimensions", getDimensions() ) );
  setResolution( reader->readFloatVec3("Resolution", getResolution() ) );
  setOrigin( reader->readFloatVec3("Origin", getOrigin() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RecrystalizeVolume::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(NucleationRate)
  DREAM3D_FILTER_WRITE_PARAMETER(DataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(Dimensions)
  DREAM3D_FILTER_WRITE_PARAMETER(Resolution)
  DREAM3D_FILTER_WRITE_PARAMETER(Origin)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RecrystalizeVolume::dataCheck()
{
  setErrorCondition(0);

  // Create the output Data Container
  VolumeDataContainer* m = getDataContainerArray()->createNonPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0) { return; }

  // Sanity Check the Dimensions and Resolution
  INIT_SYNTH_VOLUME_CHECK(Dimensions.x, -5000);
  INIT_SYNTH_VOLUME_CHECK(Dimensions.y, -5001);
  INIT_SYNTH_VOLUME_CHECK(Dimensions.z, -5002);
  INIT_SYNTH_VOLUME_CHECK(Resolution.x, -5003);
  INIT_SYNTH_VOLUME_CHECK(Resolution.y, -5004);
  INIT_SYNTH_VOLUME_CHECK(Resolution.z, -5005);

  // Set teh Dimensions, Resolution and Origin of the output data container
  m->setDimensions(m_Dimensions.x, m_Dimensions.y, m_Dimensions.z);
  m->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  m->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);

  // Create our output Cell and Cell Feature Attribute Matrix objects
  QVector<size_t> tDims(3, 0);
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> cDims(1, 0);
  AttributeMatrix::Pointer cellFeatureAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), cDims, DREAM3D::AttributeMatrixType::CellFeature);
  if(getErrorCondition() < 0) { return; }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RecrystalizeVolume::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RecrystalizeVolume::getCompiledLibraryName()
{
  return CellularAutomata::CellularAutomataBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RecrystalizeVolume::getGroupName()
{
  return "CellularAutomata";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RecrystalizeVolume::getHumanLabel()
{
  return "RecrystalizeVolume";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RecrystalizeVolume::getSubGroupName()
{
  return "Misc";
}


//cell class (contains member variables + rules)
class Cell
{
private:
public:
  size_t id;//0 if unrecrstallized, grainID if recrystllized
  size_t updateTime;

public:
  Cell(size_t grainID, size_t time) : id(grainID), updateTime(time) {}

  //function to get state of cell in next time step
  Cell updateCell(std::vector<Cell*>& neighbors, size_t& maxID, size_t t, bool seed)
  {
    Cell retCell(*this);

    //dont change cells that are already recrystallized
    if(0 != id)
      return retCell;

    //otherwise check if any neighbors are recrystallized
    std::vector<size_t> goodNeighbors;
    for(size_t i = 0; i < neighbors.size(); i++)
    {
      if(0 != neighbors[i]->id)
        goodNeighbors.push_back(neighbors[i]->id);
    }

    //if no neighbors are recrystalized, allow random chance to recrystallization
    if(0 == goodNeighbors.size())
    {
      //random chance to create nucluie
      // float seed = static_cast<float>(rand()) / RAND_MAX;
      // Generator test = Generator(*gen);
      // float seed = test();
      // if(seed <= pNuc)
      if(seed)
      {
        maxID++;
        retCell.updateTime = t;
        retCell.id = maxID;
      }
      return retCell;
    }

    //if some neighbors are recrystallized, choose one at random to join
    size_t index = rand() % goodNeighbors.size();
    retCell.id = goodNeighbors[index];
    retCell.updateTime = t;
    return retCell;
  }
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RecrystalizeVolume::execute()
{
  int err = 0;
  // typically run your dataCheck function to make sure you can get that far and all your variables are initialized
  dataCheck();
  // Check to make sure you made it through the data check. Errors would have been reported already so if something
  // happens to fail in the dataCheck() then we simply return
  if(getErrorCondition() < 0) { return; }
  setErrorCondition(0);

  //initialize random number generator
  m_Distribution = boost::shared_ptr<NumberDistribution>(new NumberDistribution(0, 1));
  m_RandomNumberGenerator = boost::shared_ptr<RandomNumberGenerator>(new RandomNumberGenerator);
  m_NumberGenerator = boost::shared_ptr<Generator>(new Generator(*m_RandomNumberGenerator, *m_Distribution));
  m_RandomNumberGenerator->seed(static_cast<size_t>( QDateTime::currentMSecsSinceEpoch() )); // seed with the current time
  Generator& numberGenerator = *m_NumberGenerator;

  //convert nucleation rate to probabilty / voxel / timestep
  float pNuc = m_NucleationRate * m_Resolution.x * m_Resolution.y * m_Resolution.z;

  //determine number of cells and create helper object for indicies
  size_t numCells = m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
  CellularAutomata::Lattice lattice(m_Dimensions.x, m_Dimensions.y, m_Dimensions.z);

  //create 2 arrays of cells (current state + working copy)
  typedef StructArray<Cell> CellArray;
  CellArray::Pointer currentState = CellArray::CreateArray(numCells, "current state", true);
  CellArray::Pointer workingCopy = CellArray::CreateArray(numCells, "working copy", true);

  //make sure allocation was sucessful
  if(CellArray::NullPointer() == currentState || CellArray::NullPointer() == workingCopy)
  {

    QString ss = QObject::tr("Unable to allocate memory for cell arrays");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  //create a default state and fill array
  Cell defaultState(0, 0);
  currentState->initializeWithValue(defaultState);

  //get pointers to both arrays
  Cell* pCurrent = currentState->getPointer(0);
  Cell* pWorking = workingCopy->getPointer(0);

  //continue time stepping until all cells are recrystallized
  size_t unrecrstallizedCount = 1;
  size_t grainCount = 0;
  size_t timeStep = 0; 

  while(0 != unrecrstallizedCount)
  {
    //reset count of remaining cells to recrystallize
    unrecrstallizedCount = 0;

    //loop over all cells
    for(size_t i = 0; i < numCells; i++)
    {
      //generate list of neighbors
      std::vector<size_t> neighborsIndex = lattice.VonNeumann(i);
      std::vector<Cell*> neighbors;
      for(size_t j = 0; j < neighborsIndex.size(); j++)
      {

        neighbors.push_back(currentState->getPointer(neighborsIndex[j]));
      }

      //randomly determine if this cell can nucleate
      bool seed = false;
      if(numberGenerator() <= pNuc)
        seed = true;

      //compute new state
      pWorking[i] = pCurrent[i].updateCell(neighbors, grainCount, timeStep, seed);
      if(0 == pWorking[i].id)
        unrecrstallizedCount++;
    }

    //increment time step and copy arrays
    timeStep++;
    // currentState = workingCopy;
    for(size_t i = 0; i < numCells; i++)
    {
      pCurrent[i] = pWorking[i];
    }

    //update progress
    float percent = 1.0 - static_cast<float>(unrecrstallizedCount) / numCells;
    QString ss = QObject::tr("%1% recrystallized").arg(100 * percent);
    notifyStatusMessage(getHumanLabel(), ss);
  }

  //clean up working copy
  workingCopy = CellArray::NullPointer();

  //get cretaed data container
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_DataContainerName);

  //get created cell attribute matricies
  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(m_CellAttributeMatrixName);
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(m_CellFeatureAttributeMatrixName);

  // Resize the Cell AttributeMatricies to have the correct Tuple Dimensions.
  QVector<size_t> cellDims(3, 0);
  cellDims[0] = m->getXPoints();
  cellDims[1] = m->getYPoints();
  cellDims[2] = m->getZPoints();
  cellAttrMat->resizeAttributeArrays(cellDims);

  QVector<size_t> featureDims(1, grainCount + 1);
  cellFeatureAttrMat->resizeAttributeArrays(featureDims);

  //Add cell data
    QVector<size_t> cDims(1, 1);
    Int32ArrayType::Pointer gid = Int32ArrayType::CreateArray(numCells, cDims, DREAM3D::CellData::FeatureIds);
    UInt32ArrayType::Pointer tid = UInt32ArrayType::CreateArray(numCells, cDims, "RecrystallizationTime");

    //copy cell states into grain ids
    int32_t* pGid = gid->getPointer(0);
    uint32_t* pTid = tid->getPointer(0);
    for(size_t i = 0; i < numCells; i++)
    {
      pGid[i] = pCurrent[i].id;
      pTid[i] = pCurrent[i].updateTime;
    }
    
    //add created cell arrays to attribute matrix
    cellAttrMat->addAttributeArray(gid->getName(), gid);
    cellAttrMat->addAttributeArray(tid->getName(), tid);

  //add feature data
    BoolArrayType::Pointer aid = BoolArrayType::CreateArray(grainCount + 1, cDims, DREAM3D::FeatureData::Active);
    aid->initializeWithValue(true);
    aid->getPointer(0)[0] = false;
    cellFeatureAttrMat->addAttributeArray(aid->getName(), aid);


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RecrystalizeVolume::newFilterInstance(bool copyFilterParameters)
{
  RecrystalizeVolume::Pointer filter = RecrystalizeVolume::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

