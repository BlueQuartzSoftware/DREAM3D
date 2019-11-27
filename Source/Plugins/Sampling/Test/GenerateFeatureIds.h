#pragma once

#include <memory>

#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/AttributeMatrix.h"

#include "TestFileLocations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class GenerateFeatureIds : public AbstractFilter
{
  Q_OBJECT
public:
  using Self = GenerateFeatureIds;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  
  /**
   * @brief Returns a NullPointer wrapped by a shared_ptr<>
   * @return
   */
  static Pointer NullPointer();

  /**
   * @brief Creates a new object wrapped in a shared_ptr<>
   * @return
   */
  static Pointer New();

  /**
   * @brief Returns the name of the class for GenerateFeatureIds
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for GenerateFeatureIds
   */
  static QString ClassName();

  ~GenerateFeatureIds()
  {
  }
  /**
   * @brief Setter property for DataContainerName
   */
  void setDataContainerName(const QString& value);
  /**
   * @brief Getter property for DataContainerName
   * @return Value of DataContainerName
   */
  QString getDataContainerName() const;

  //------ Created Cell Data
  /**
   * @brief Setter property for CellAttributeMatrixName
   */
  void setCellAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for CellAttributeMatrixName
   * @return Value of CellAttributeMatrixName
   */
  QString getCellAttributeMatrixName() const;

  /**
   * @brief Setter property for CellFeatureAttributeMatrixName
   */
  void setCellFeatureAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for CellFeatureAttributeMatrixName
   * @return Value of CellFeatureAttributeMatrixName
   */
  QString getCellFeatureAttributeMatrixName() const;

  /**
   * @brief Setter property for CellEnsembleAttributeMatrixName
   */
  void setCellEnsembleAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for CellEnsembleAttributeMatrixName
   * @return Value of CellEnsembleAttributeMatrixName
   */
  QString getCellEnsembleAttributeMatrixName() const;

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
    clearErrorCode();
    clearWarningCode();
    dataCheck();
    if(getErrorCode() < 0)
    {
      return;
    }

    DataContainer::Pointer m = getDataContainerArray()->getDataContainer(SIMPL::Defaults::DataContainerName);

    int size = UnitTest::FeatureIdsTest::XSize * UnitTest::FeatureIdsTest::YSize * UnitTest::FeatureIdsTest::ZSize;
    std::vector<size_t> tDims(3, 0);
    tDims[0] = UnitTest::FeatureIdsTest::XSize;
    tDims[1] = UnitTest::FeatureIdsTest::YSize;
    tDims[2] = UnitTest::FeatureIdsTest::ZSize;
    m->getAttributeMatrix(getCellAttributeMatrixName())->resizeAttributeArrays(tDims);

    dataCheck();
    // Set the default data into the FeatureIds
    for(int i = 0; i < size; ++i)
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
  GenerateFeatureIds()
  : m_DataContainerName(SIMPL::Defaults::DataContainerName)
  , m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
  , m_CellFeatureAttributeMatrixName(SIMPL::Defaults::CellFeatureAttributeMatrixName)
  , m_FeatureIdsArrayName(SIMPL::CellData::FeatureIds)
  , m_FeatureIds(nullptr)
  {
  }

private:
  std::weak_ptr<DataArray<int32_t>> m_FeatureIdsPtr;
  int32_t* m_FeatureIds = nullptr;

  QString m_DataContainerName = {};
  QString m_CellAttributeMatrixName = {};
  QString m_CellFeatureAttributeMatrixName = {};
  QString m_CellEnsembleAttributeMatrixName = {};

  void dataCheck()
  {
    clearErrorCode();
    clearWarningCode();
    DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer(this, getDataContainerName());
    if(getErrorCode() < 0)
    {
      return;
    }
    AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix(this, getCellAttributeMatrixName(), -301);
    if(getErrorCode() < 0)
    {
      return;
    }

    std::vector<size_t> dims(1, 1);
    m_FeatureIdsPtr =
        cellAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeatureIdsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_FeatureIdsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

public:
  GenerateFeatureIds(const GenerateFeatureIds&) = delete; // Copy Constructor Not Implemented
  GenerateFeatureIds(GenerateFeatureIds&&) = delete;      // Move Constructor Not Implemented
  GenerateFeatureIds& operator=(const GenerateFeatureIds&) = delete; // Copy Assignment Not Implemented
  GenerateFeatureIds& operator=(GenerateFeatureIds&&) = delete;      // Move Assignment Not Implemented
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
  Q_OBJECT
public:
  using Self = CreateDataContainer;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  
  /**
   * @brief Returns a NullPointer wrapped by a shared_ptr<>
   * @return
   */
  static Pointer NullPointer();

  /**
   * @brief Creates a new object wrapped in a shared_ptr<>
   * @return
   */
  static Pointer New();

  /**
   * @brief Returns the name of the class for CreateDataContainer
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for CreateDataContainer
   */
  static QString ClassName();

  ~CreateDataContainer()
  {
  }

  /**
  * @brief This returns the group that the filter belonds to. You can select
  * a different group if you want. The string returned here will be displayed
  * in the GUI for the filter
  */
  virtual const QString getGroupName()
  {
    return "UnitTest";
  }

  /**
  * @brief This returns a string that is displayed in the GUI. It should be readable
  * and understandable by humans.
  */
  virtual const QString getHumanLabel()
  {
    return "CreateDataContainer Filter";
  }

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
  CreateDataContainer()
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
    DataContainer::Pointer m = DataContainer::New(); /* FIXME: What Geometry do we need? */
    m->setName(SIMPL::Defaults::DataContainerName);
    int64_t nx = UnitTest::FeatureIdsTest::XSize;
    int64_t ny = UnitTest::FeatureIdsTest::YSize;
    int64_t nz = UnitTest::FeatureIdsTest::ZSize;
    /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->setDimensions(nx, ny, nz);
    getDataContainerArray()->addDataContainer(m);
    std::vector<size_t> tDims(3, 0);
    tDims[0] = nx;
    tDims[1] = ny;
    tDims[2] = nz;
    AttributeMatrix::Pointer attrMat = AttributeMatrix::New(tDims, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::AttributeMatrixType::Cell);
    m->addAttributeMatrix(attrMat);
  }

public:
  CreateDataContainer(const CreateDataContainer&) = delete;            // Copy Constructor Not Implemented
  CreateDataContainer(CreateDataContainer&&) = delete;                 // Move Constructor Not Implemented
  CreateDataContainer& operator=(const CreateDataContainer&) = delete; // Copy Assignment Not Implemented
  CreateDataContainer& operator=(CreateDataContainer&&) = delete;      // Move Assignment Not Implemented
};

