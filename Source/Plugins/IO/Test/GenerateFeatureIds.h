#ifndef _GenerateFeatureIds_H_
#define _GenerateFeatureIds_H_




#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "IOTestFileLocations.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class GenerateFeatureIds : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    SIMPL_SHARED_POINTERS(GenerateFeatureIds)
    SIMPL_STATIC_NEW_MACRO(GenerateFeatureIds)
    SIMPL_TYPE_MACRO_SUPER(GenerateFeatureIds, AbstractFilter)

    virtual ~GenerateFeatureIds() {}
    SIMPL_INSTANCE_STRING_PROPERTY(DataContainerName)
    //------ Created Cell Data
    SIMPL_INSTANCE_STRING_PROPERTY(CellAttributeMatrixName)
    SIMPL_INSTANCE_STRING_PROPERTY(CellFeatureAttributeMatrixName)
    SIMPL_INSTANCE_STRING_PROPERTY(CellEnsembleAttributeMatrixName)

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
      dataCheck();
      if(getErrorCondition() < 0) { return; }

      DataContainer::Pointer m = getDataContainerArray()->getDataContainer(DREAM3D::Defaults::DataContainerName);

      int size = UnitTest::FeatureIdsTest::XSize * UnitTest::FeatureIdsTest::YSize * UnitTest::FeatureIdsTest::ZSize;
      QVector<size_t> tDims(3, 0);
      tDims[0] = UnitTest::FeatureIdsTest::XSize;
      tDims[1] = UnitTest::FeatureIdsTest::YSize;
      tDims[2] = UnitTest::FeatureIdsTest::ZSize;
      m->getAttributeMatrix(getCellAttributeMatrixName())->resizeAttributeArrays(tDims);
      m->getAttributeMatrix(getCellAttributeMatrixName())->removeAttributeArray(m_FeatureIdsArrayName);

      dataCheck();
      // Set the default data into the FeatureIds
      for (int i = 0; i < size; ++i)
      {
        m_FeatureIds[i] = i + UnitTest::FeatureIdsTest::Offset;
      }

    }
    virtual void preflight()
    {
      dataCheck();
    }

    void readFilterParameters(AbstractFilterParametersReader* reader, int index)
    {
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
      m_DataContainerName(DREAM3D::Defaults::DataContainerName),
      m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
      m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
      m_FeatureIds(NULL),
      m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds)
    {
    }

  private:
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)
    QString m_FeatureIdsArrayName;

    void dataCheck()
    {
      setErrorCondition(0);
      DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
      if(getErrorCondition() < 0) { return; }
      AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
      if(getErrorCondition() < 0) { return; }

      QVector<size_t> dims(1, 1);
      m_FeatureIdsPtr = cellAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeatureIdsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
      if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
      { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    }

    GenerateFeatureIds(const GenerateFeatureIds&); // Copy Constructor Not Implemented
    void operator=(const GenerateFeatureIds&); // Operator '=' Not Implemented
};




/**
 * @class CreateDataContainer
 * @brief This class simply adds a DataContainer with default name to the DataContainerArray
 * @author
 * @date
 * @version 1.0
 */
class CreateDataContainer : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    SIMPL_SHARED_POINTERS(CreateDataContainer)
    SIMPL_STATIC_NEW_MACRO(CreateDataContainer)
    SIMPL_TYPE_MACRO_SUPER(CreateDataContainer, AbstractFilter)

    SIMPL_FILTER_PARAMETER(int64_t, XDim)
    SIMPL_FILTER_PARAMETER(int64_t, YDim)
    SIMPL_FILTER_PARAMETER(int64_t, ZDim)


    virtual ~CreateDataContainer() {}

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
    virtual const QString getHumanLabel() { return "CreateDataContainer Filter"; }

    /**
    * @brief Reimplemented from @see AbstractFilter class
    */
    virtual void execute()
    {
      dataCheck();
    }

    /**
    * @brief This function runs some sanity checks on the DataContainer and inputs
    * in an attempt to ensure the filter can process the inputs.
    */
    virtual void preflight()
    {
      dataCheck();
    }

    void readFilterParameters(AbstractFilterParametersReader* reader, int index)
    {
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
    CreateDataContainer() :
      AbstractFilter(),
      m_XDim(0),
      m_YDim(0),
      m_ZDim(0)
    {

    }

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param features The number of features
    * @param ensembles The number of ensembles
    */
    void dataCheck()
    {
      DataContainer::Pointer m = DataContainer::New();
      ImageGeom::Pointer image = ImageGeom::CreateGeometry(DREAM3D::Geometry::ImageGeometry);
      m->setGeometry(image);

      m->setName(DREAM3D::Defaults::DataContainerName);
      int64_t nx = m_XDim;
      int64_t ny = m_YDim;
      int64_t nz = m_ZDim;
      m->getGeometryAs<ImageGeom>()->setDimensions(nx, ny, nz);
      getDataContainerArray()->addDataContainer(m);
      QVector<size_t> tDims(3, 0);
      tDims[0] = nx;
      tDims[1] = ny;
      tDims[2] = nz;
      AttributeMatrix::Pointer attrMat = AttributeMatrix::New(tDims, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::AttributeMatrixType::Cell);
      m->addAttributeMatrix(attrMat->getName(), attrMat);
    }

  private:

    CreateDataContainer(const CreateDataContainer&); // Copy Constructor Not Implemented
    void operator=(const CreateDataContainer&); // Operator '=' Not Implemented
};



#endif /* _GenerateFeatureIds_H_ */
