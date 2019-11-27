#pragma once

#include <memory>

#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/SIMPLib.h"

#include "ImportExportTestFileLocations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class GenerateFeatureIds : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(GenerateFeatureIds SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(GenerateFeatureIds)
  PYB11_FILTER_NEW_MACRO(GenerateFeatureIds)
  PYB11_SHARED_POINTERS(CreateImageGeomDataContainer)
  PYB11_FILTER_NEW_MACRO(CreateImageGeomDataContainer)
  PYB11_FILTER_PARAMETER(size_t, XDim)
  PYB11_FILTER_PARAMETER(size_t, YDim)
  PYB11_FILTER_PARAMETER(size_t, ZDim)
#endif

public:
  using Self = GenerateFeatureIds;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer()
  {
    return Pointer(static_cast<Self*>(nullptr));
  }

  /**
   * @brief New
   * @return
   */
  static Pointer New()
  {
    Pointer sharedPtr(new(Self));
    return sharedPtr;
  }

  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  QString getNameOfClass() const override
  {
    return QString("GenerateFeatureIds");
  }
  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  static QString ClassName()
  {
    return QString("GenerateFeatureIds");
  }

  ~GenerateFeatureIds() override = default;

  /**
   * @brief Setter property for DataContainerName
   */
  void setDataContainerName(const QString& value)
  {
    m_DataContainerName = value;
  }
  /**
   * @brief Getter property for DataContainerName
   * @return Value of DataContainerName
   */
  QString getDataContainerName() const
  {
    return m_DataContainerName;
  }

  //------ Created Cell Data
  /**
   * @brief Setter property for CellAttributeMatrixName
   */
  void setCellAttributeMatrixName(const QString& value)
  {
    m_CellAttributeMatrixName = value;
  }
  /**
   * @brief Getter property for CellAttributeMatrixName
   * @return Value of CellAttributeMatrixName
   */
  QString getCellAttributeMatrixName() const
  {
    return m_CellAttributeMatrixName;
  }

  /**
   * @brief Setter property for CellFeatureAttributeMatrixName
   */
  void setCellFeatureAttributeMatrixName(const QString& value)
  {
    m_CellFeatureAttributeMatrixName = value;
  }
  /**
   * @brief Getter property for CellFeatureAttributeMatrixName
   * @return Value of CellFeatureAttributeMatrixName
   */
  QString getCellFeatureAttributeMatrixName() const
  {
    return m_CellFeatureAttributeMatrixName;
  }

  /**
   * @brief Setter property for CellEnsembleAttributeMatrixName
   */
  void setCellEnsembleAttributeMatrixName(const QString& value)
  {
    m_CellEnsembleAttributeMatrixName = value;
  }
  /**
   * @brief Getter property for CellEnsembleAttributeMatrixName
   * @return Value of CellEnsembleAttributeMatrixName
   */
  QString getCellEnsembleAttributeMatrixName() const
  {
    return m_CellEnsembleAttributeMatrixName;
  }

  QString getGroupName() const override
  {
    return "UnitTest";
  }
  QString getHumanLabel() const override
  {
    return "Generate Feature Ids";
  }
  void execute() override
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
    m->getAttributeMatrix(getCellAttributeMatrixName())->removeAttributeArray(m_FeatureIdsArrayName);

    dataCheck();
    // Set the default data into the FeatureIds
    for(int i = 0; i < size; ++i)
    {
      m_FeatureIds[i] = i + UnitTest::FeatureIdsTest::Offset;
    }
  }
  void preflight() override
  {
    dataCheck();
  }

  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override
  {
    reader->openFilterGroup(this, index);
    reader->closeFilterGroup();
  }

protected:
  GenerateFeatureIds()
  : m_DataContainerName(SIMPL::Defaults::DataContainerName)
  , m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
  , m_CellFeatureAttributeMatrixName(SIMPL::Defaults::CellFeatureAttributeMatrixName)
  , m_FeatureIdsArrayName(SIMPL::CellData::FeatureIds)
  {
  }

private:
  QString m_DataContainerName = {};
  QString m_CellAttributeMatrixName = {};
  QString m_CellFeatureAttributeMatrixName = {};
  QString m_CellEnsembleAttributeMatrixName = {};

  QString m_FeatureIdsArrayName;

  std::weak_ptr<DataArray<int32_t>> m_FeatureIdsPtr;
  int32_t* m_FeatureIds = nullptr;

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
  GenerateFeatureIds(const GenerateFeatureIds&) = delete;            // Copy Constructor Not Implemented
  GenerateFeatureIds(GenerateFeatureIds&&) = delete;                 // Move Constructor Not Implemented
  GenerateFeatureIds& operator=(const GenerateFeatureIds&) = delete; // Copy Assignment Not Implemented
  GenerateFeatureIds& operator=(GenerateFeatureIds&&) = delete;      // Move Assignment Not Implemented
};

/**
 * @class CreateImageGeomDataContainer
 * @brief This class simply adds a DataContainer with default name to the DataContainerArray
 * @author
 * @date
 * @version 1.0
 */
class CreateImageGeomDataContainer : public AbstractFilter
{
  Q_OBJECT

public:
  using Self = CreateImageGeomDataContainer;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer()
  {
    return Pointer(static_cast<Self*>(nullptr));
  }

  /**
   * @brief New
   * @return
   */
  static Pointer New()
  {
    Pointer sharedPtr(new(Self));
    return sharedPtr;
  }

  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  QString getNameOfClass() const override
  {
    return QString("CreateImageGeomDataContainer");
  }
  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  static QString ClassName()
  {
    return QString("CreateImageGeomDataContainer");
  }

  /**
   * @brief Setter property for XDim
   */
  void setXDim(const size_t& value)
  {
    m_XDim = value;
  }
  /**
   * @brief Getter property for XDim
   * @return Value of XDim
   */
  size_t getXDim() const
  {
    return m_XDim;
  }

  /**
   * @brief Setter property for YDim
   */
  void setYDim(const size_t& value)
  {
    m_YDim = value;
  }
  /**
   * @brief Getter property for YDim
   * @return Value of YDim
   */
  size_t getYDim() const
  {
    return m_YDim;
  }

  /**
   * @brief Setter property for ZDim
   */
  void setZDim(const size_t& value)
  {
    m_ZDim = value;
  }
  /**
   * @brief Getter property for ZDim
   * @return Value of ZDim
   */
  size_t getZDim() const
  {
    return m_ZDim;
  }

  ~CreateImageGeomDataContainer() override = default;

  /**
   * @brief This returns the group that the filter belonds to. You can select
   * a different group if you want. The string returned here will be displayed
   * in the GUI for the filter
   */
  QString getGroupName() const override
  {
    return "UnitTest";
  }

  /**
   * @brief This returns a string that is displayed in the GUI. It should be readable
   * and understandable by humans.
   */
  QString getHumanLabel() const override
  {
    return "CreateImageGeomDataContainer Filter";
  }

  /**
   * @brief Reimplemented from @see AbstractFilter class
   */
  void execute() override
  {
    dataCheck();
  }

  /**
   * @brief This function runs some sanity checks on the DataContainer and inputs
   * in an attempt to ensure the filter can process the inputs.
   */
  void preflight() override
  {
    dataCheck();
  }

  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override
  {
    reader->openFilterGroup(this, index);
    reader->closeFilterGroup();
  }

protected:
  CreateImageGeomDataContainer() = default;

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
    DataContainer::Pointer m = DataContainer::New("DataContainer");
    ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
    m->setGeometry(image);

    m->setName(SIMPL::Defaults::DataContainerName);
    m->getGeometryAs<ImageGeom>()->setDimensions(m_XDim, m_YDim, m_ZDim);
    getDataContainerArray()->addOrReplaceDataContainer(m);
    std::vector<size_t> tDims(3, 0);
    tDims[0] = m_XDim;
    tDims[1] = m_YDim;
    tDims[2] = m_ZDim;
    AttributeMatrix::Pointer attrMat = AttributeMatrix::New(tDims, SIMPL::Defaults::CellAttributeMatrixName, AttributeMatrix::Type::Cell);
    m->addOrReplaceAttributeMatrix(attrMat);
  }

public:
  CreateImageGeomDataContainer(const CreateImageGeomDataContainer&) = delete;            // Copy Constructor Not Implemented
  CreateImageGeomDataContainer(CreateImageGeomDataContainer&&) = delete;                 // Move Constructor Not Implemented
  CreateImageGeomDataContainer& operator=(const CreateImageGeomDataContainer&) = delete; // Copy Assignment Not Implemented
  CreateImageGeomDataContainer& operator=(CreateImageGeomDataContainer&&) = delete;      // Move Assignment Not Implemented

private:
  size_t m_XDim = 0;
  size_t m_YDim = 0;
  size_t m_ZDim = 0;
};
