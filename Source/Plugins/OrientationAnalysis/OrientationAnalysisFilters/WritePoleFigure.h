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

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @brief The WritePoleFigure class. See [Filter documentation](@ref writepolefigure) for details.
 */
class OrientationAnalysis_EXPORT WritePoleFigure : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(WritePoleFigure SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(WritePoleFigure)
  PYB11_FILTER_NEW_MACRO(WritePoleFigure)
  PYB11_FILTER_PARAMETER(QString, ImagePrefix)
  PYB11_FILTER_PARAMETER(QString, OutputPath)
  PYB11_FILTER_PARAMETER(int, ImageFormat)
  PYB11_FILTER_PARAMETER(int, ImageSize)
  PYB11_FILTER_PARAMETER(int, LambertSize)
  PYB11_FILTER_PARAMETER(int, NumColors)
  PYB11_FILTER_PARAMETER(int, ImageLayout)
  PYB11_FILTER_PARAMETER(DataArrayPath, CellEulerAnglesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, GoodVoxelsArrayPath)
  PYB11_FILTER_PARAMETER(bool, UseGoodVoxels)
  PYB11_FILTER_PARAMETER(int, GenerationAlgorithm)
  PYB11_FILTER_PARAMETER(QString, Title)
  PYB11_FILTER_PARAMETER(DataArrayPath, MaterialNameArrayPath)
  PYB11_PROPERTY(QString ImagePrefix READ getImagePrefix WRITE setImagePrefix)
  PYB11_PROPERTY(QString OutputPath READ getOutputPath WRITE setOutputPath)
  PYB11_PROPERTY(int ImageFormat READ getImageFormat WRITE setImageFormat)
  PYB11_PROPERTY(int ImageSize READ getImageSize WRITE setImageSize)
  PYB11_PROPERTY(int LambertSize READ getLambertSize WRITE setLambertSize)
  PYB11_PROPERTY(int NumColors READ getNumColors WRITE setNumColors)
  PYB11_PROPERTY(int ImageLayout READ getImageLayout WRITE setImageLayout)
  PYB11_PROPERTY(DataArrayPath CellEulerAnglesArrayPath READ getCellEulerAnglesArrayPath WRITE setCellEulerAnglesArrayPath)
  PYB11_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
  PYB11_PROPERTY(DataArrayPath MaterialNameArrayPath READ getMaterialNameArrayPath WRITE setMaterialNameArrayPath)
  PYB11_PROPERTY(DataArrayPath GoodVoxelsArrayPath READ getGoodVoxelsArrayPath WRITE setGoodVoxelsArrayPath)
  PYB11_PROPERTY(bool UseGoodVoxels READ getUseGoodVoxels WRITE setUseGoodVoxels)
  PYB11_PROPERTY(int GenerationAlgorithm READ getGenerationAlgorithm WRITE setGenerationAlgorithm)
#endif

  //  PYB11_PROPERTY(bool UseDiscreteHeatMap READ getUseDiscreteHeatMap WRITE setUseDiscreteHeatMap)
public:
  using Self = WritePoleFigure;
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
   * @brief Returns the name of the class for WritePoleFigure
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for WritePoleFigure
   */
  static QString ClassName();

  ~WritePoleFigure() override;

    enum ImageFormatType
    {
      TifImageType = 0,
      BmpImageType = 1,
      PngImageType = 2,
      JpgImageType = 3,
      PdfImageType = 4,
    };

    using EnumType = int;

    enum class Algorithm : EnumType
    {
      LambertProjection = 0, //!<
      Discrete = 1,          //!<
      Unknown = 2,           //!
    };

    /**
     * @brief Setter property for ImagePrefix
     */
    void setImagePrefix(const QString& value);
    /**
     * @brief Getter property for ImagePrefix
     * @return Value of ImagePrefix
     */
    QString getImagePrefix() const;

    Q_PROPERTY(QString ImagePrefix READ getImagePrefix WRITE setImagePrefix)

    /**
     * @brief Setter property for OutputPath
     */
    void setOutputPath(const QString& value);
    /**
     * @brief Getter property for OutputPath
     * @return Value of OutputPath
     */
    QString getOutputPath() const;

    Q_PROPERTY(QString OutputPath READ getOutputPath WRITE setOutputPath)

    /**
     * @brief Setter property for ImageFormat
     */
    void setImageFormat(int value);
    /**
     * @brief Getter property for ImageFormat
     * @return Value of ImageFormat
     */
    int getImageFormat() const;

    Q_PROPERTY(int ImageFormat READ getImageFormat WRITE setImageFormat)

    /**
     * @brief Setter property for ImageSize
     */
    void setImageSize(int value);
    /**
     * @brief Getter property for ImageSize
     * @return Value of ImageSize
     */
    int getImageSize() const;

    Q_PROPERTY(int ImageSize READ getImageSize WRITE setImageSize)

    /**
     * @brief Setter property for LambertSize
     */
    void setLambertSize(int value);
    /**
     * @brief Getter property for LambertSize
     * @return Value of LambertSize
     */
    int getLambertSize() const;

    Q_PROPERTY(int LambertSize READ getLambertSize WRITE setLambertSize)

    /**
     * @brief Setter property for NumColors
     */
    void setNumColors(int value);
    /**
     * @brief Getter property for NumColors
     * @return Value of NumColors
     */
    int getNumColors() const;

    Q_PROPERTY(int NumColors READ getNumColors WRITE setNumColors)

    /**
     * @brief Setter property for ImageLayout
     */
    void setImageLayout(int value);
    /**
     * @brief Getter property for ImageLayout
     * @return Value of ImageLayout
     */
    int getImageLayout() const;

    Q_PROPERTY(int ImageLayout READ getImageLayout WRITE setImageLayout)

    /**
     * @brief Setter property for CellEulerAnglesArrayPath
     */
    void setCellEulerAnglesArrayPath(const DataArrayPath& value);
    /**
     * @brief Getter property for CellEulerAnglesArrayPath
     * @return Value of CellEulerAnglesArrayPath
     */
    DataArrayPath getCellEulerAnglesArrayPath() const;

    Q_PROPERTY(DataArrayPath CellEulerAnglesArrayPath READ getCellEulerAnglesArrayPath WRITE setCellEulerAnglesArrayPath)

    /**
     * @brief Setter property for CellPhasesArrayPath
     */
    void setCellPhasesArrayPath(const DataArrayPath& value);
    /**
     * @brief Getter property for CellPhasesArrayPath
     * @return Value of CellPhasesArrayPath
     */
    DataArrayPath getCellPhasesArrayPath() const;

    Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)

    /**
     * @brief Setter property for CrystalStructuresArrayPath
     */
    void setCrystalStructuresArrayPath(const DataArrayPath& value);
    /**
     * @brief Getter property for CrystalStructuresArrayPath
     * @return Value of CrystalStructuresArrayPath
     */
    DataArrayPath getCrystalStructuresArrayPath() const;

    Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

    /**
     * @brief Setter property for GoodVoxelsArrayPath
     */
    void setGoodVoxelsArrayPath(const DataArrayPath& value);
    /**
     * @brief Getter property for GoodVoxelsArrayPath
     * @return Value of GoodVoxelsArrayPath
     */
    DataArrayPath getGoodVoxelsArrayPath() const;

    Q_PROPERTY(DataArrayPath GoodVoxelsArrayPath READ getGoodVoxelsArrayPath WRITE setGoodVoxelsArrayPath)

    /**
     * @brief Setter property for UseGoodVoxels
     */
    void setUseGoodVoxels(bool value);
    /**
     * @brief Getter property for UseGoodVoxels
     * @return Value of UseGoodVoxels
     */
    bool getUseGoodVoxels() const;

    Q_PROPERTY(bool UseGoodVoxels READ getUseGoodVoxels WRITE setUseGoodVoxels)

    /**
     * @brief Setter property for GenerationAlgorithm
     */
    void setGenerationAlgorithm(int value);
    /**
     * @brief Getter property for GenerationAlgorithm
     * @return Value of GenerationAlgorithm
     */
    int getGenerationAlgorithm() const;

    Q_PROPERTY(int GenerationAlgorithm READ getGenerationAlgorithm WRITE setGenerationAlgorithm)

    /**
     * @brief Setter property for Title
     */
    void setTitle(const QString& value);
    /**
     * @brief Getter property for Title
     * @return Value of Title
     */
    QString getTitle() const;

    Q_PROPERTY(QString Title READ getTitle WRITE setTitle)

    /**
     * @brief Setter property for MaterialNameArrayPath
     */
    void setMaterialNameArrayPath(const DataArrayPath& value);
    /**
     * @brief Getter property for MaterialNameArrayPath
     * @return Value of MaterialNameArrayPath
     */
    DataArrayPath getMaterialNameArrayPath() const;

    Q_PROPERTY(DataArrayPath MaterialNameArrayPath READ getMaterialNameArrayPath WRITE setMaterialNameArrayPath)
        
    //  SIMPL_FILTER_PARAMETER(bool, UseDiscreteHeatMap)
    //  Q_PROPERTY(bool UseDiscreteHeatMap READ getUseDiscreteHeatMap WRITE setUseDiscreteHeatMap)

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

  signals:
    /**
     * @brief updateFilterParameters Emitted when the Filter requests all the latest Filter parameters
     * be pushed from a user-facing control (such as a widget)
     * @param filter Filter instance pointer
     */
    void updateFilterParameters(AbstractFilter* filter);

    /**
     * @brief parametersChanged Emitted when any Filter parameter is changed internally
     */
    void parametersChanged();

    /**
     * @brief preflightAboutToExecute Emitted just before calling dataCheck()
     */
    void preflightAboutToExecute();

    /**
     * @brief preflightExecuted Emitted just after calling dataCheck()
     */
    void preflightExecuted();

  protected:
    WritePoleFigure();
    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief Initializes all the private instance variables.
     */
    void initialize();

    /**
     * @brief generateImagePath Generates the path to write the image
     * @param label Name of file
     * @return Generated output path
     */
    QString generateImagePath(const QString &label);

    /**
     * @brief writeImage Writes the pole figure image to a file
     * @param image Image to write
     * @param label Name of file
     */
    //void writeImage(QImage image, QString label);

  private:
    std::weak_ptr<DataArray<float>> m_CellEulerAnglesPtr;
    float* m_CellEulerAngles = nullptr;
    std::weak_ptr<DataArray<int32_t>> m_CellPhasesPtr;
    int32_t* m_CellPhases = nullptr;
    std::weak_ptr<DataArray<uint32_t>> m_CrystalStructuresPtr;
    uint32_t* m_CrystalStructures = nullptr;
    std::weak_ptr<DataArray<bool>> m_GoodVoxelsPtr;
    bool* m_GoodVoxels = nullptr;

    QString m_ImagePrefix = {};
    QString m_OutputPath = {};
    int m_ImageFormat = {};
    int m_ImageSize = {};
    int m_LambertSize = {};
    int m_NumColors = {};
    int m_ImageLayout = {};
    DataArrayPath m_CellEulerAnglesArrayPath = {};
    DataArrayPath m_CellPhasesArrayPath = {};
    DataArrayPath m_CrystalStructuresArrayPath = {};
    DataArrayPath m_GoodVoxelsArrayPath = {};
    bool m_UseGoodVoxels = {};
    int m_GenerationAlgorithm = {};
    QString m_Title = {};
    DataArrayPath m_MaterialNameArrayPath = {};

    bool m_UseDiscreteHeatMap = false;

    StringDataArray::Pointer m_MaterialNames;

public:
  WritePoleFigure(const WritePoleFigure&) = delete; // Copy Constructor Not Implemented
  WritePoleFigure(WritePoleFigure&&) = delete;      // Move Constructor Not Implemented
  WritePoleFigure& operator=(const WritePoleFigure&) = delete; // Copy Assignment Not Implemented
  WritePoleFigure& operator=(WritePoleFigure&&) = delete;      // Move Assignment Not Implemented
};

