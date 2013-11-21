#ifndef _GenerateFeatureIds_H_
#define _GenerateFeatureIds_H_




#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"

#include "TestFileLocations.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class GenerateFeatureIds : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(GenerateFeatureIds)
    DREAM3D_STATIC_NEW_MACRO(GenerateFeatureIds)
    DREAM3D_TYPE_MACRO_SUPER(GenerateFeatureIds, AbstractFilter)

    virtual ~GenerateFeatureIds(){}
    DREAM3D_INSTANCE_STRING_PROPERTY(DataContainerName)
    //------ Created Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(FeatureIdsArrayName)

    virtual const QString getGroupName()
    {
      return "UnitTest";
    }
    virtual const QString getHumanLabel()
    {
      return "Generate Feature Ids";
    }
    virtual void execute()
    {
      setErrorCondition(0);
      VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(DREAM3D::HDF5::VolumeDataContainerName);
      if(NULL == m)
      {
        setErrorCondition(-1);
        addErrorMessage(getHumanLabel(), " DataContainer was NULL", -1);
        return;
      }
      int size = UnitTest::FeatureIdsTest::XSize * UnitTest::FeatureIdsTest::YSize * UnitTest::FeatureIdsTest::ZSize;

      int64_t nx = UnitTest::FeatureIdsTest::XSize;
      int64_t ny = UnitTest::FeatureIdsTest::YSize;
      int64_t nz = UnitTest::FeatureIdsTest::ZSize;
      m->setDimensions(nx, ny, nz);

      int64_t totalPoints = m->getTotalPoints();
  size_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  size_t totalEnsembles = m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->getNumTuples();
  dataCheck(false, totalPoints, totalFeatures, totalEnsembles);
      // Set the default data into the FeatureIds
      for (int i = 0; i < size; ++i)
      {
        m_FeatureIds[i] = i + UnitTest::FeatureIdsTest::Offset;
      }

    }
    virtual void preflight()
    {
      dataCheck(true, 1, 1, 1);
    }

    void readFilterParameters(AbstractFilterParametersReader* reader, int index) {
      reader->openFilterGroup(this, index);
      reader->closeFilterGroup();
    }

    int writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
    {
      writer->openFilterGroup(this, index);
      writer->closeFilterGroup();
      return ++index; // we want to return the next index that was just written to
    }


  protected:
    GenerateFeatureIds() :
      AbstractFilter(),
      m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
      m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
      m_FeatureIds(NULL)
    {
    }

  private:
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, FeatureIds)

    void dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
    {
      setErrorCondition(0);
      VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(DREAM3D::HDF5::VolumeDataContainerName);
      if (NULL == m)
      {
        QString ss = QObject::tr("The Volume Data Container with name '%1'' was not found in the Data Container Array.").arg(getDataContainerName());
        setErrorCondition(-1001);
        addErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
      QVector<int> dims(1, 1);
      m_FeatureIdsPtr = m->createNonPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_FeatureIdsArrayName, 0, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  	  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    }

    GenerateFeatureIds(const GenerateFeatureIds&); // Copy Constructor Not Implemented
    void operator=(const GenerateFeatureIds&); // Operator '=' Not Implemented
};




/**
 * @class CreateVolumeDataContainer
 * @brief This class simply adds a VolumeDataContainer with default name to the DataContainerArray
 * @author
 * @date
 * @version 1.0
 */
class CreateVolumeDataContainer : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(CreateVolumeDataContainer)
    DREAM3D_STATIC_NEW_MACRO(CreateVolumeDataContainer)
    DREAM3D_TYPE_MACRO_SUPER(CreateVolumeDataContainer, AbstractFilter)

    virtual ~CreateVolumeDataContainer() {}

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const QString getGroupName() { return "UnitTest"; }

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const QString getHumanLabel() { return "CreateVolumeDataContainer Filter"; }

    /**
    * @brief Reimplemented from @see AbstractFilter class
    */
    virtual void execute()
    {
      dataCheck(true, 1, 1, 1);
    }

    /**
    * @brief This function runs some sanity checks on the DataContainer and inputs
    * in an attempt to ensure the filter can process the inputs.
    */
    virtual void preflight()
    {
      dataCheck(true, 1, 1, 1);
    }

    void readFilterParameters(AbstractFilterParametersReader* reader, int index) {
      reader->openFilterGroup(this, index);
      reader->closeFilterGroup();
    }

    int writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
    {
      writer->openFilterGroup(this, index);
      writer->closeFilterGroup();
      return ++index; // we want to return the next index that was just written to
    }

  protected:
    CreateVolumeDataContainer() : AbstractFilter() {}

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param features The number of features
    * @param ensembles The number of ensembles
    */
    void dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
    {
      VolumeDataContainer::Pointer m = VolumeDataContainer::New();
      m->setName(DREAM3D::HDF5::VolumeDataContainerName);
      getDataContainerArray()->pushBack(m);
    }

  private:

    CreateVolumeDataContainer(const CreateVolumeDataContainer&); // Copy Constructor Not Implemented
    void operator=(const CreateVolumeDataContainer&); // Operator '=' Not Implemented
};

#endif /* _GenerateFeatureIds_H_ */
