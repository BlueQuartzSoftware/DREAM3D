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
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SurfaceMeshing/SurfaceMeshingFilters/SurfaceMeshFilter.h"

#include "SurfaceMeshing/SurfaceMeshingDLLExport.h"

/**
 * @brief The GenerateGeometryConnectivity class. See [Filter documentation](@ref generategeometryconnectivity) for details.
 */
class SurfaceMeshing_EXPORT GenerateGeometryConnectivity : public SurfaceMeshFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(GenerateGeometryConnectivity SUPERCLASS SurfaceMeshFilter)
  PYB11_SHARED_POINTERS(GenerateGeometryConnectivity)
  PYB11_FILTER_NEW_MACRO(GenerateGeometryConnectivity)
  PYB11_FILTER_PARAMETER(DataArrayPath, SurfaceDataContainerName)
  PYB11_FILTER_PARAMETER(bool, GenerateVertexTriangleLists)
  PYB11_FILTER_PARAMETER(bool, GenerateTriangleNeighbors)
  PYB11_PROPERTY(DataArrayPath SurfaceDataContainerName READ getSurfaceDataContainerName WRITE setSurfaceDataContainerName)
  PYB11_PROPERTY(bool GenerateVertexTriangleLists READ getGenerateVertexTriangleLists WRITE setGenerateVertexTriangleLists)
  PYB11_PROPERTY(bool GenerateTriangleNeighbors READ getGenerateTriangleNeighbors WRITE setGenerateTriangleNeighbors)
#endif

public:
  using Self = GenerateGeometryConnectivity;
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
   * @brief Returns the name of the class for GenerateGeometryConnectivity
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for GenerateGeometryConnectivity
   */
  static QString ClassName();

  ~GenerateGeometryConnectivity() override;

  /**
   * @brief Setter property for SurfaceDataContainerName
   */
  void setSurfaceDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for SurfaceDataContainerName
   * @return Value of SurfaceDataContainerName
   */
  DataArrayPath getSurfaceDataContainerName() const;

  Q_PROPERTY(DataArrayPath SurfaceDataContainerName READ getSurfaceDataContainerName WRITE setSurfaceDataContainerName)

  /**
   * @brief Setter property for GenerateVertexTriangleLists
   */
  void setGenerateVertexTriangleLists(bool value);
  /**
   * @brief Getter property for GenerateVertexTriangleLists
   * @return Value of GenerateVertexTriangleLists
   */
  bool getGenerateVertexTriangleLists() const;

  Q_PROPERTY(bool GenerateVertexTriangleLists READ getGenerateVertexTriangleLists WRITE setGenerateVertexTriangleLists)

  /**
   * @brief Setter property for GenerateTriangleNeighbors
   */
  void setGenerateTriangleNeighbors(bool value);
  /**
   * @brief Getter property for GenerateTriangleNeighbors
   * @return Value of GenerateTriangleNeighbors
   */
  bool getGenerateTriangleNeighbors() const;

  Q_PROPERTY(bool GenerateTriangleNeighbors READ getGenerateTriangleNeighbors WRITE setGenerateTriangleNeighbors)

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
  GenerateGeometryConnectivity();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

public:
  GenerateGeometryConnectivity(const GenerateGeometryConnectivity&) = delete;            // Copy Constructor Not Implemented
  GenerateGeometryConnectivity(GenerateGeometryConnectivity&&) = delete;                 // Move Constructor Not Implemented
  GenerateGeometryConnectivity& operator=(const GenerateGeometryConnectivity&) = delete; // Copy Assignment Not Implemented
  GenerateGeometryConnectivity& operator=(GenerateGeometryConnectivity&&) = delete;      // Move assignment Not Implemented

private:
  DataArrayPath m_SurfaceDataContainerName = {};
  bool m_GenerateVertexTriangleLists = {};
  bool m_GenerateTriangleNeighbors = {};
};

