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

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @brief The WritePoleFigure class. See [Filter documentation](@ref writepolefigure) for details.
 */
class OrientationAnalysis_EXPORT WritePoleFigure : public AbstractFilter
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(WritePoleFigure SUPERCLASS AbstractFilter)
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
    //  PYB11_PROPERTY(bool UseDiscreteHeatMap READ getUseDiscreteHeatMap WRITE setUseDiscreteHeatMap)
public:
  SIMPL_SHARED_POINTERS(WritePoleFigure)
  SIMPL_FILTER_NEW_MACRO(WritePoleFigure)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(WritePoleFigure, AbstractFilter)

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

    SIMPL_FILTER_PARAMETER(QString, ImagePrefix)
    Q_PROPERTY(QString ImagePrefix READ getImagePrefix WRITE setImagePrefix)

    SIMPL_FILTER_PARAMETER(QString, OutputPath)
    Q_PROPERTY(QString OutputPath READ getOutputPath WRITE setOutputPath)

    SIMPL_FILTER_PARAMETER(int, ImageFormat)
    Q_PROPERTY(int ImageFormat READ getImageFormat WRITE setImageFormat)

    SIMPL_FILTER_PARAMETER(int, ImageSize)
    Q_PROPERTY(int ImageSize READ getImageSize WRITE setImageSize)

    SIMPL_FILTER_PARAMETER(int, LambertSize)
    Q_PROPERTY(int LambertSize READ getLambertSize WRITE setLambertSize)

    SIMPL_FILTER_PARAMETER(int, NumColors)
    Q_PROPERTY(int NumColors READ getNumColors WRITE setNumColors)

    SIMPL_FILTER_PARAMETER(int, ImageLayout)
    Q_PROPERTY(int ImageLayout READ getImageLayout WRITE setImageLayout)

    SIMPL_FILTER_PARAMETER(DataArrayPath, CellEulerAnglesArrayPath)
    Q_PROPERTY(DataArrayPath CellEulerAnglesArrayPath READ getCellEulerAnglesArrayPath WRITE setCellEulerAnglesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
    Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
    Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, GoodVoxelsArrayPath)
    Q_PROPERTY(DataArrayPath GoodVoxelsArrayPath READ getGoodVoxelsArrayPath WRITE setGoodVoxelsArrayPath)

    SIMPL_FILTER_PARAMETER(bool, UseGoodVoxels)
    Q_PROPERTY(bool UseGoodVoxels READ getUseGoodVoxels WRITE setUseGoodVoxels)

    SIMPL_FILTER_PARAMETER(int, GenerationAlgorithm)
    Q_PROPERTY(int GenerationAlgorithm READ getGenerationAlgorithm WRITE setGenerationAlgorithm)

    SIMPL_FILTER_PARAMETER(QString, Title)
    Q_PROPERTY(QString Title READ getTitle WRITE setTitle)
        

    SIMPL_FILTER_PARAMETER(DataArrayPath, MaterialNameArrayPath)
    Q_PROPERTY(DataArrayPath MaterialNameArrayPath READ getMaterialNameArrayPath WRITE setMaterialNameArrayPath)
        
    //  SIMPL_FILTER_PARAMETER(bool, UseDiscreteHeatMap)
    //  Q_PROPERTY(bool UseDiscreteHeatMap READ getUseDiscreteHeatMap WRITE setUseDiscreteHeatMap)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  const QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
  */
  const QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  const QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  const QUuid getUuid() override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  const QString getHumanLabel() const override;

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
    bool m_UseDiscreteHeatMap = false;
    DEFINE_DATAARRAY_VARIABLE(float, CellEulerAngles)
    DEFINE_DATAARRAY_VARIABLE(int32_t, CellPhases)
    DEFINE_DATAARRAY_VARIABLE(uint32_t, CrystalStructures)
    DEFINE_DATAARRAY_VARIABLE(bool, GoodVoxels)
    
    StringDataArray::Pointer m_MaterialNames;

public:
  WritePoleFigure(const WritePoleFigure&) = delete; // Copy Constructor Not Implemented
  WritePoleFigure(WritePoleFigure&&) = delete;      // Move Constructor Not Implemented
  WritePoleFigure& operator=(const WritePoleFigure&) = delete; // Copy Assignment Not Implemented
  WritePoleFigure& operator=(WritePoleFigure&&) = delete;      // Move Assignment Not Implemented
};

