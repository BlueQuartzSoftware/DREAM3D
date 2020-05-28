/*
 * Your License or Copyright can go here
 */

#pragma once

#include <memory>

#include <QtCore/QFile>
#include <QtCore/QScopedPointer>
#include <QtGui/QAbstractUndoItem>
#include <QtWidgets/QWidget>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "ImportExport/ImportExportDLLExport.h"

// our PIMPL private class
class ImportOnscaleTableFilePrivate;

/**
 * @brief The ImportOnscaleTableFile class. See [Filter documentation](@ref importonscaletablefile) for details.
 */
class ImportExport_EXPORT ImportOnscaleTableFile : public AbstractFilter
{
  Q_OBJECT

  // clang-format off
  PYB11_BEGIN_BINDINGS(ImportOnscaleTableFile SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(ImportOnscaleTableFile)
  PYB11_FILTER_NEW_MACRO(ImportOnscaleTableFile)
  
  PYB11_PROPERTY(DataArrayPath VolumeDataContainerName READ getVolumeDataContainerName WRITE setVolumeDataContainerName)
  PYB11_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)
  PYB11_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
  PYB11_PROPERTY(FloatVec3Type Origin READ getOrigin WRITE setOrigin)
  PYB11_PROPERTY(FloatVec3Type Spacing READ getSpacing WRITE setSpacing)
  PYB11_PROPERTY(bool FileWasRead READ getFileWasRead WRITE setFileWasRead)
  PYB11_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)

  PYB11_END_BINDINGS()
  // clang-format on

  Q_DECLARE_PRIVATE(ImportOnscaleTableFile)
public:
  using Self = ImportOnscaleTableFile;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for ImportOnscaleTableFile
   */
  QString getNameOfClass() const override;

  /**
   * @brief Returns the name of the class for ImportOnscaleTableFile
   */
  static QString ClassName();

  ~ImportOnscaleTableFile() override;

  /**
   * @brief Setter property for VolumeDataContainerName
   */
  void setVolumeDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for VolumeDataContainerName
   * @return Value of VolumeDataContainerName
   */
  DataArrayPath getVolumeDataContainerName() const;
  Q_PROPERTY(DataArrayPath VolumeDataContainerName READ getVolumeDataContainerName WRITE setVolumeDataContainerName)

  /**
   * @brief Setter property for CellAttributeMatrixName
   */
  void setCellAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for CellAttributeMatrixName
   * @return Value of CellAttributeMatrixName
   */
  QString getCellAttributeMatrixName() const;
  Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

  /**
   * @brief Setter property for InputFile
   */
  void setInputFile(const QString& value);
  /**
   * @brief Getter property for InputFile
   * @return Value of InputFile
   */
  QString getInputFile() const;
  Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

  /**
   * @brief Setter property for Origin
   */
  void setOrigin(const FloatVec3Type& value);
  /**
   * @brief Getter property for Origin
   * @return Value of Origin
   */
  FloatVec3Type getOrigin() const;
  Q_PROPERTY(FloatVec3Type Origin READ getOrigin WRITE setOrigin)

  /**
   * @brief Setter property for Spacing
   */
  void setSpacing(const FloatVec3Type& value);
  /**
   * @brief Getter property for Spacing
   * @return Value of Spacing
   */
  FloatVec3Type getSpacing() const;
  Q_PROPERTY(FloatVec3Type Spacing READ getSpacing WRITE setSpacing)

  /**
   * @brief Setter property for FileWasRead
   */
  void setFileWasRead(bool value);
  /**
   * @brief Getter property for FileWasRead
   * @return Value of FileWasRead
   */
  bool getFileWasRead() const;
  Q_PROPERTY(bool FileWasRead READ getFileWasRead)

  /**
   * @brief Setter property for FeatureIdsArrayName
   */
  void setFeatureIdsArrayName(const QString& value);
  /**
   * @brief Getter property for FeatureIdsArrayName
   * @return Value of FeatureIdsArrayName
   */
  QString getFeatureIdsArrayName() const;
  Q_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)

  /**
   * @brief Setter property for Dims
   */
  void setDims(const std::vector<size_t>& value);
  /**
   * @brief Getter property for Dims
   * @return Value of Dims
   */
  std::vector<size_t> getDims() const;

  /**
   * @brief Setter property for InputFile_Cache
   */
  void setInputFile_Cache(const QString& value);
  /**
   * @brief Getter property for InputFile_Cache
   * @return Value of InputFile_Cache
   */
  QString getInputFile_Cache() const;

  /**
   * @brief Setter property for LastRead
   */
  void setLastRead(const QDateTime& value);
  /**
   * @brief Getter property for LastRead
   * @return Value of LastRead
   */
  QDateTime getLastRead() const;

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
   */
  QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  QUuid getUuid() const override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  QString getHumanLabel() const override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  void setupFilterParameters() override;

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

public slots:
  /**
   * @brief flushCache Clears the input file cache
   */
  void flushCache();

protected:
  ImportOnscaleTableFile();

  /**
   * @brief readHeader Reimplemented from @see FileReader class
   */
  int32_t readHeader();

  /**
   * @brief readFile Reimplemented from @see FileReader class
   */
  int32_t readFile();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  std::weak_ptr<Int32ArrayType> m_FeatureIdsPtr;
  int32_t* m_FeatureIds = nullptr;

  DataArrayPath m_VolumeDataContainerName = {"DataContainer", "", ""};
  QString m_CellAttributeMatrixName = {"Table Data"};
  QString m_InputFile = {""};
  FloatVec3Type m_Origin = {0.0f, 0.0f, 0.0f};
  FloatVec3Type m_Spacing = {1.0f, 1.0f, 1.0f};
  bool m_FileWasRead = {false};
  QString m_FeatureIdsArrayName = {"Material"};

  QScopedPointer<ImportOnscaleTableFilePrivate> const d_ptr;

  SizeVec3Type m_Dims = {0, 0, 0};
  QFile m_InStream;

public:
  ImportOnscaleTableFile(const ImportOnscaleTableFile&) = delete;            // Copy Constructor Not Implemented
  ImportOnscaleTableFile& operator=(const ImportOnscaleTableFile&) = delete; // Copy Assignment Not Implemented
  ImportOnscaleTableFile(ImportOnscaleTableFile&&) = delete;                 // Move Constructor Not Implemented
  ImportOnscaleTableFile& operator=(ImportOnscaleTableFile&&) = delete;      // Move Assignment Not Implemented
};
