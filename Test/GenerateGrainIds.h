#ifndef _GenerateGrainIds_H_
#define _GenerateGrainIds_H_




#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"

#include "TestFileLocations.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class GenerateGrainIds : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(GenerateGrainIds)
    DREAM3D_STATIC_NEW_MACRO(GenerateGrainIds)
    DREAM3D_TYPE_MACRO_SUPER(GenerateGrainIds, AbstractFilter)

    virtual ~GenerateGrainIds(){}
    DREAM3D_INSTANCE_STRING_PROPERTY(DataContainerName)
    //------ Created Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdsArrayName)

    virtual const QString getGroupName()
    {
      return "UnitTest";
    }
    virtual const QString getHumanLabel()
    {
      return "Generate Grain Ids";
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
      int size = UnitTest::GrainIdsTest::XSize * UnitTest::GrainIdsTest::YSize * UnitTest::GrainIdsTest::ZSize;

      int64_t nx = UnitTest::GrainIdsTest::XSize;
      int64_t ny = UnitTest::GrainIdsTest::YSize;
      int64_t nz = UnitTest::GrainIdsTest::ZSize;
      m->setDimensions(nx, ny, nz);

      int64_t totalPoints = m->getTotalPoints();
      dataCheck(false, totalPoints, m->getNumCellFieldTuples(), m->getNumCellEnsembleTuples());
      // Set the default data into the GrainIds
      for (int i = 0; i < size; ++i)
      {
        m_GrainIds[i] = i + UnitTest::GrainIdsTest::Offset;
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
    GenerateGrainIds() :
      AbstractFilter(),
      m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
      m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
      m_GrainIds(NULL)
    {
    }

  private:
    int32_t* m_GrainIds;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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
      CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, int32_t, Int32ArrayType, 0, voxels, dims)
    }

    GenerateGrainIds(const GenerateGrainIds&); // Copy Constructor Not Implemented
    void operator=(const GenerateGrainIds&); // Operator '=' Not Implemented
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
    * @param fields The number of fields
    * @param ensembles The number of ensembles
    */
    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
    {
      VolumeDataContainer::Pointer m = VolumeDataContainer::New();
      m->setName(DREAM3D::HDF5::VolumeDataContainerName);
      getDataContainerArray()->pushBack(m);
    }

  private:

    CreateVolumeDataContainer(const CreateVolumeDataContainer&); // Copy Constructor Not Implemented
    void operator=(const CreateVolumeDataContainer&); // Operator '=' Not Implemented
};

#endif /* _GenerateGrainIds_H_ */
