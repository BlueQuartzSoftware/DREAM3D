/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#pragma once

#include <memory>

#include <cstring> // needed for the ::memcpy function below

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/CoreFilters/FileReader.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"


class OrientationAnalysis_EXPORT YSChoiAbaqusReader : public FileReader
{
    Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
    PYB11_CREATE_BINDINGS(YSChoiAbaqusReader SUPERCLASS FileReader)
    PYB11_SHARED_POINTERS(YSChoiAbaqusReader)
    PYB11_FILTER_NEW_MACRO(YSChoiAbaqusReader)
    PYB11_FILTER_PARAMETER(DataArrayPath, DataContainerName)
    PYB11_FILTER_PARAMETER(QString, CellEnsembleAttributeMatrixName)
    PYB11_FILTER_PARAMETER(QString, CellFeatureAttributeMatrixName)
    PYB11_FILTER_PARAMETER(QString, CellAttributeMatrixName)
    PYB11_FILTER_PARAMETER(QString, InputFile)
    PYB11_FILTER_PARAMETER(QString, InputFeatureInfoFile)
    PYB11_FILTER_PARAMETER(QString, CellEulerAnglesArrayName)
    PYB11_FILTER_PARAMETER(QString, QuatsArrayName)
    PYB11_FILTER_PARAMETER(QString, AvgQuatsArrayName)
    PYB11_FILTER_PARAMETER(QString, CellPhasesArrayName)
    PYB11_FILTER_PARAMETER(QString, SurfaceFeaturesArrayName)
    PYB11_FILTER_PARAMETER(QString, FeatureIdsArrayName)
    PYB11_FILTER_PARAMETER(QString, CrystalStructuresArrayName)
    PYB11_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)
    PYB11_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)
    PYB11_PROPERTY(QString CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)
    PYB11_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)
    PYB11_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
    PYB11_PROPERTY(QString InputFeatureInfoFile READ getInputFeatureInfoFile WRITE setInputFeatureInfoFile)
    PYB11_PROPERTY(QString CellEulerAnglesArrayName READ getCellEulerAnglesArrayName WRITE setCellEulerAnglesArrayName)
    PYB11_PROPERTY(QString QuatsArrayName READ getQuatsArrayName WRITE setQuatsArrayName)
    PYB11_PROPERTY(QString AvgQuatsArrayName READ getAvgQuatsArrayName WRITE setAvgQuatsArrayName)
    PYB11_PROPERTY(QString CellPhasesArrayName READ getCellPhasesArrayName WRITE setCellPhasesArrayName)
    PYB11_PROPERTY(QString SurfaceFeaturesArrayName READ getSurfaceFeaturesArrayName WRITE setSurfaceFeaturesArrayName)
    PYB11_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)
    PYB11_PROPERTY(QString CrystalStructuresArrayName READ getCrystalStructuresArrayName WRITE setCrystalStructuresArrayName)
#endif

  public:
    using Self = YSChoiAbaqusReader;
    using Pointer = std::shared_ptr<Self>;
    using ConstPointer = std::shared_ptr<const Self>;
    using WeakPointer = std::weak_ptr<Self>;
    using ConstWeakPointer = std::weak_ptr<const Self>;
    static Pointer NullPointer();

    static std::shared_ptr<YSChoiAbaqusReader> New();

    /**
     * @brief Returns the name of the class for YSChoiAbaqusReader
     */
    QString getNameOfClass() const override;
    /**
     * @brief Returns the name of the class for YSChoiAbaqusReader
     */
    static QString ClassName();

    ~YSChoiAbaqusReader() override;

    /**
     * @brief Setter property for DataContainerName
     */
    void setDataContainerName(const DataArrayPath& value);
    /**
     * @brief Getter property for DataContainerName
     * @return Value of DataContainerName
     */
    DataArrayPath getDataContainerName() const;

    Q_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)

    /**
     * @brief Setter property for CellEnsembleAttributeMatrixName
     */
    void setCellEnsembleAttributeMatrixName(const QString& value);
    /**
     * @brief Getter property for CellEnsembleAttributeMatrixName
     * @return Value of CellEnsembleAttributeMatrixName
     */
    QString getCellEnsembleAttributeMatrixName() const;

    Q_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)

    /**
     * @brief Setter property for CellFeatureAttributeMatrixName
     */
    void setCellFeatureAttributeMatrixName(const QString& value);
    /**
     * @brief Getter property for CellFeatureAttributeMatrixName
     * @return Value of CellFeatureAttributeMatrixName
     */
    QString getCellFeatureAttributeMatrixName() const;

    Q_PROPERTY(QString CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)

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
     * @brief Setter property for InputFeatureInfoFile
     */
    void setInputFeatureInfoFile(const QString& value);
    /**
     * @brief Getter property for InputFeatureInfoFile
     * @return Value of InputFeatureInfoFile
     */
    QString getInputFeatureInfoFile() const;

    Q_PROPERTY(QString InputFeatureInfoFile READ getInputFeatureInfoFile WRITE setInputFeatureInfoFile)

    /**
     * @brief Setter property for CellEulerAnglesArrayName
     */
    void setCellEulerAnglesArrayName(const QString& value);
    /**
     * @brief Getter property for CellEulerAnglesArrayName
     * @return Value of CellEulerAnglesArrayName
     */
    QString getCellEulerAnglesArrayName() const;

    Q_PROPERTY(QString CellEulerAnglesArrayName READ getCellEulerAnglesArrayName WRITE setCellEulerAnglesArrayName)

    /**
     * @brief Setter property for QuatsArrayName
     */
    void setQuatsArrayName(const QString& value);
    /**
     * @brief Getter property for QuatsArrayName
     * @return Value of QuatsArrayName
     */
    QString getQuatsArrayName() const;

    Q_PROPERTY(QString QuatsArrayName READ getQuatsArrayName WRITE setQuatsArrayName)

    /**
     * @brief Setter property for AvgQuatsArrayName
     */
    void setAvgQuatsArrayName(const QString& value);
    /**
     * @brief Getter property for AvgQuatsArrayName
     * @return Value of AvgQuatsArrayName
     */
    QString getAvgQuatsArrayName() const;

    Q_PROPERTY(QString AvgQuatsArrayName READ getAvgQuatsArrayName WRITE setAvgQuatsArrayName)

    /**
     * @brief Setter property for CellPhasesArrayName
     */
    void setCellPhasesArrayName(const QString& value);
    /**
     * @brief Getter property for CellPhasesArrayName
     * @return Value of CellPhasesArrayName
     */
    QString getCellPhasesArrayName() const;

    Q_PROPERTY(QString CellPhasesArrayName READ getCellPhasesArrayName WRITE setCellPhasesArrayName)

    /**
     * @brief Setter property for SurfaceFeaturesArrayName
     */
    void setSurfaceFeaturesArrayName(const QString& value);
    /**
     * @brief Getter property for SurfaceFeaturesArrayName
     * @return Value of SurfaceFeaturesArrayName
     */
    QString getSurfaceFeaturesArrayName() const;

    Q_PROPERTY(QString SurfaceFeaturesArrayName READ getSurfaceFeaturesArrayName WRITE setSurfaceFeaturesArrayName)

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
     * @brief Setter property for CrystalStructuresArrayName
     */
    void setCrystalStructuresArrayName(const QString& value);
    /**
     * @brief Getter property for CrystalStructuresArrayName
     * @return Value of CrystalStructuresArrayName
     */
    QString getCrystalStructuresArrayName() const;

    Q_PROPERTY(QString CrystalStructuresArrayName READ getCrystalStructuresArrayName WRITE setCrystalStructuresArrayName)

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
     * @brief readFilterParameters Reimplemented from @see AbstractFilter class
     */
    void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

    /**
     * @brief execute Reimplemented from @see AbstractFilter class
     */
    void execute() override;

    /**
    * @brief preflight Reimplemented from @see AbstractFilter class
    */
    void preflight() override;

  protected:
    YSChoiAbaqusReader();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck() override;

    /**
     * @brief Initializes all the private instance variables.
     */
    void initialize();

  private:
    std::weak_ptr<DataArray<int32_t>> m_FeatureIdsPtr;
    int32_t* m_FeatureIds = nullptr;
    std::weak_ptr<DataArray<int32_t>> m_CellPhasesPtr;
    int32_t* m_CellPhases = nullptr;
    std::weak_ptr<DataArray<bool>> m_SurfaceFeaturesPtr;
    bool* m_SurfaceFeatures = nullptr;
    std::weak_ptr<DataArray<float>> m_QuatsPtr;
    float* m_Quats = nullptr;
    std::weak_ptr<DataArray<float>> m_AvgQuatsPtr;
    float* m_AvgQuats = nullptr;
    std::weak_ptr<DataArray<float>> m_CellEulerAnglesPtr;
    float* m_CellEulerAngles = nullptr;
    std::weak_ptr<DataArray<unsigned int>> m_CrystalStructuresPtr;
    unsigned int* m_CrystalStructures = nullptr;

    DataArrayPath m_DataContainerName = {};
    QString m_CellEnsembleAttributeMatrixName = {};
    QString m_CellFeatureAttributeMatrixName = {};
    QString m_CellAttributeMatrixName = {};
    QString m_InputFile = {};
    QString m_InputFeatureInfoFile = {};
    QString m_CellEulerAnglesArrayName = {};
    QString m_QuatsArrayName = {};
    QString m_AvgQuatsArrayName = {};
    QString m_CellPhasesArrayName = {};
    QString m_SurfaceFeaturesArrayName = {};
    QString m_FeatureIdsArrayName = {};
    QString m_CrystalStructuresArrayName = {};

    void updateCellInstancePointers();

    void updateFeatureInstancePointers();

    void updateEnsembleInstancePointers();

  public:
    YSChoiAbaqusReader(const YSChoiAbaqusReader&) = delete; // Copy Constructor Not Implemented
    YSChoiAbaqusReader(YSChoiAbaqusReader&&) = delete;      // Move Constructor Not Implemented
    YSChoiAbaqusReader& operator=(const YSChoiAbaqusReader&) = delete; // Copy Assignment Not Implemented
    YSChoiAbaqusReader& operator=(YSChoiAbaqusReader&&) = delete;      // Move Assignment Not Implemented
};

