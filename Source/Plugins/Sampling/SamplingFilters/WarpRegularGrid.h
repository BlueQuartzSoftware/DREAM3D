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
#include "SIMPLib/FilterParameters/FourthOrderPolynomialFilterParameter.h"
#include "SIMPLib/FilterParameters/SecondOrderPolynomialFilterParameter.h"
#include "SIMPLib/FilterParameters/ThirdOrderPolynomialFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "Sampling/SamplingDLLExport.h"

/**
 * @brief The WarpRegularGrid class. See [Filter documentation](@ref warpregulargrid) for details.
 */
class Sampling_EXPORT WarpRegularGrid : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(WarpRegularGrid SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(WarpRegularGrid)
  PYB11_FILTER_NEW_MACRO(WarpRegularGrid)
  PYB11_FILTER_PARAMETER(DataArrayPath, NewDataContainerName)
  PYB11_FILTER_PARAMETER(DataArrayPath, CellAttributeMatrixPath)
  PYB11_FILTER_PARAMETER(int, PolyOrder)
  PYB11_FILTER_PARAMETER(Float2ndOrderPoly_t, SecondOrderACoeff)
  PYB11_FILTER_PARAMETER(Float2ndOrderPoly_t, SecondOrderBCoeff)
  PYB11_FILTER_PARAMETER(Float3rdOrderPoly_t, ThirdOrderACoeff)
  PYB11_FILTER_PARAMETER(Float3rdOrderPoly_t, ThirdOrderBCoeff)
  PYB11_FILTER_PARAMETER(Float4thOrderPoly_t, FourthOrderACoeff)
  PYB11_FILTER_PARAMETER(Float4thOrderPoly_t, FourthOrderBCoeff)
  PYB11_FILTER_PARAMETER(bool, SaveAsNewDataContainer)
  PYB11_PROPERTY(DataArrayPath NewDataContainerName READ getNewDataContainerName WRITE setNewDataContainerName)
  PYB11_PROPERTY(DataArrayPath CellAttributeMatrixPath READ getCellAttributeMatrixPath WRITE setCellAttributeMatrixPath)
  PYB11_PROPERTY(int PolyOrder READ getPolyOrder WRITE setPolyOrder)
  PYB11_PROPERTY(Float2ndOrderPoly_t SecondOrderACoeff READ getSecondOrderACoeff WRITE setSecondOrderACoeff)
  PYB11_PROPERTY(Float2ndOrderPoly_t SecondOrderBCoeff READ getSecondOrderBCoeff WRITE setSecondOrderBCoeff)
  PYB11_PROPERTY(Float3rdOrderPoly_t ThirdOrderACoeff READ getThirdOrderACoeff WRITE setThirdOrderACoeff)
  PYB11_PROPERTY(Float3rdOrderPoly_t ThirdOrderBCoeff READ getThirdOrderBCoeff WRITE setThirdOrderBCoeff)
  PYB11_PROPERTY(Float4thOrderPoly_t FourthOrderACoeff READ getFourthOrderACoeff WRITE setFourthOrderACoeff)
  PYB11_PROPERTY(Float4thOrderPoly_t FourthOrderBCoeff READ getFourthOrderBCoeff WRITE setFourthOrderBCoeff)
  PYB11_PROPERTY(bool SaveAsNewDataContainer READ getSaveAsNewDataContainer WRITE setSaveAsNewDataContainer)
#endif

public:
  using Self = WarpRegularGrid;
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
   * @brief Returns the name of the class for WarpRegularGrid
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for WarpRegularGrid
   */
  static QString ClassName();

  ~WarpRegularGrid() override;

  /**
   * @brief Setter property for NewDataContainerName
   */
  void setNewDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for NewDataContainerName
   * @return Value of NewDataContainerName
   */
  DataArrayPath getNewDataContainerName() const;

  Q_PROPERTY(DataArrayPath NewDataContainerName READ getNewDataContainerName WRITE setNewDataContainerName)

  /**
   * @brief Setter property for CellAttributeMatrixPath
   */
  void setCellAttributeMatrixPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CellAttributeMatrixPath
   * @return Value of CellAttributeMatrixPath
   */
  DataArrayPath getCellAttributeMatrixPath() const;

  Q_PROPERTY(DataArrayPath CellAttributeMatrixPath READ getCellAttributeMatrixPath WRITE setCellAttributeMatrixPath)

  /**
   * @brief Setter property for PolyOrder
   */
  void setPolyOrder(int value);
  /**
   * @brief Getter property for PolyOrder
   * @return Value of PolyOrder
   */
  int getPolyOrder() const;

  Q_PROPERTY(int PolyOrder READ getPolyOrder WRITE setPolyOrder)

  /**
   * @brief Setter property for SecondOrderACoeff
   */
  void setSecondOrderACoeff(const Float2ndOrderPoly_t& value);
  /**
   * @brief Getter property for SecondOrderACoeff
   * @return Value of SecondOrderACoeff
   */
  Float2ndOrderPoly_t getSecondOrderACoeff() const;

  Q_PROPERTY(Float2ndOrderPoly_t SecondOrderACoeff READ getSecondOrderACoeff WRITE setSecondOrderACoeff)

  /**
   * @brief Setter property for SecondOrderBCoeff
   */
  void setSecondOrderBCoeff(const Float2ndOrderPoly_t& value);
  /**
   * @brief Getter property for SecondOrderBCoeff
   * @return Value of SecondOrderBCoeff
   */
  Float2ndOrderPoly_t getSecondOrderBCoeff() const;

  Q_PROPERTY(Float2ndOrderPoly_t SecondOrderBCoeff READ getSecondOrderBCoeff WRITE setSecondOrderBCoeff)

  /**
   * @brief Setter property for ThirdOrderACoeff
   */
  void setThirdOrderACoeff(const Float3rdOrderPoly_t& value);
  /**
   * @brief Getter property for ThirdOrderACoeff
   * @return Value of ThirdOrderACoeff
   */
  Float3rdOrderPoly_t getThirdOrderACoeff() const;

  Q_PROPERTY(Float3rdOrderPoly_t ThirdOrderACoeff READ getThirdOrderACoeff WRITE setThirdOrderACoeff)

  /**
   * @brief Setter property for ThirdOrderBCoeff
   */
  void setThirdOrderBCoeff(const Float3rdOrderPoly_t& value);
  /**
   * @brief Getter property for ThirdOrderBCoeff
   * @return Value of ThirdOrderBCoeff
   */
  Float3rdOrderPoly_t getThirdOrderBCoeff() const;

  Q_PROPERTY(Float3rdOrderPoly_t ThirdOrderBCoeff READ getThirdOrderBCoeff WRITE setThirdOrderBCoeff)

  /**
   * @brief Setter property for FourthOrderACoeff
   */
  void setFourthOrderACoeff(const Float4thOrderPoly_t& value);
  /**
   * @brief Getter property for FourthOrderACoeff
   * @return Value of FourthOrderACoeff
   */
  Float4thOrderPoly_t getFourthOrderACoeff() const;

  Q_PROPERTY(Float4thOrderPoly_t FourthOrderACoeff READ getFourthOrderACoeff WRITE setFourthOrderACoeff)

  /**
   * @brief Setter property for FourthOrderBCoeff
   */
  void setFourthOrderBCoeff(const Float4thOrderPoly_t& value);
  /**
   * @brief Getter property for FourthOrderBCoeff
   * @return Value of FourthOrderBCoeff
   */
  Float4thOrderPoly_t getFourthOrderBCoeff() const;

  Q_PROPERTY(Float4thOrderPoly_t FourthOrderBCoeff READ getFourthOrderBCoeff WRITE setFourthOrderBCoeff)

  /**
   * @brief Setter property for SaveAsNewDataContainer
   */
  void setSaveAsNewDataContainer(bool value);
  /**
   * @brief Getter property for SaveAsNewDataContainer
   * @return Value of SaveAsNewDataContainer
   */
  bool getSaveAsNewDataContainer() const;

  Q_PROPERTY(bool SaveAsNewDataContainer READ getSaveAsNewDataContainer WRITE setSaveAsNewDataContainer)

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
  WarpRegularGrid();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief determine_warped_coordinates Warps the supplied x and y coordinates by the supplied polynomial coefficients.
   * @param x Input x coordinate to be warped.
   * @param y Input y coordinate to be warped.
   * @param newX Output warped x coordinate.
   * @param newY Output warped y coordiante.
   */
  void determine_warped_coordinates(float x, float y, float& newX, float& newY);

public:
  WarpRegularGrid(const WarpRegularGrid&) = delete; // Copy Constructor Not Implemented
  WarpRegularGrid(WarpRegularGrid&&) = delete;      // Move Constructor Not Implemented
  WarpRegularGrid& operator=(const WarpRegularGrid&) = delete; // Copy Assignment Not Implemented
  WarpRegularGrid& operator=(WarpRegularGrid&&) = delete;      // Move Assignment Not Implemented

private:
  DataArrayPath m_NewDataContainerName = {};
  DataArrayPath m_CellAttributeMatrixPath = {};
  int m_PolyOrder = {};
  Float2ndOrderPoly_t m_SecondOrderACoeff = {};
  Float2ndOrderPoly_t m_SecondOrderBCoeff = {};
  Float3rdOrderPoly_t m_ThirdOrderACoeff = {};
  Float3rdOrderPoly_t m_ThirdOrderBCoeff = {};
  Float4thOrderPoly_t m_FourthOrderACoeff = {};
  Float4thOrderPoly_t m_FourthOrderBCoeff = {};
  bool m_SaveAsNewDataContainer = {};
};

