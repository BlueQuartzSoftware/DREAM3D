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
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "Plugins/SurfaceMeshing/SurfaceMeshingFilters/SurfaceMeshFilter.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"
#include "SurfaceMeshing/SurfaceMeshingDLLExport.h"

/**
 * @brief The FeatureFaceCurvatureFilter class. See [Filter documentation](@ref featurefacecurvaturefilter) for details.
 */
class SurfaceMeshing_EXPORT FeatureFaceCurvatureFilter : public SurfaceMeshFilter
{
  Q_OBJECT
#ifdef SIMPL_ENABLE_PYTHON
  // PYB11_CREATE_BINDINGS(FeatureFaceCurvatureFilter SUPERCLASS SurfaceMeshFilter)
  // PYB11_PROPERTY(DataArrayPath FaceAttributeMatrixPath READ getFaceAttributeMatrixPath WRITE setFaceAttributeMatrixPath)
  // PYB11_PROPERTY(QString SurfaceMeshPrincipalCurvature1sArrayName READ getSurfaceMeshPrincipalCurvature1sArrayName WRITE setSurfaceMeshPrincipalCurvature1sArrayName)
  // PYB11_PROPERTY(QString SurfaceMeshPrincipalCurvature2sArrayName READ getSurfaceMeshPrincipalCurvature2sArrayName WRITE setSurfaceMeshPrincipalCurvature2sArrayName)
  // PYB11_PROPERTY(QString SurfaceMeshPrincipalDirection1sArrayName READ getSurfaceMeshPrincipalDirection1sArrayName WRITE setSurfaceMeshPrincipalDirection1sArrayName)
  // PYB11_PROPERTY(QString SurfaceMeshPrincipalDirection2sArrayName READ getSurfaceMeshPrincipalDirection2sArrayName WRITE setSurfaceMeshPrincipalDirection2sArrayName)
  // PYB11_PROPERTY(QString SurfaceMeshGaussianCurvaturesArrayName READ getSurfaceMeshGaussianCurvaturesArrayName WRITE setSurfaceMeshGaussianCurvaturesArrayName)
  // PYB11_PROPERTY(QString SurfaceMeshMeanCurvaturesArrayName READ getSurfaceMeshMeanCurvaturesArrayName WRITE setSurfaceMeshMeanCurvaturesArrayName)
  // PYB11_PROPERTY(int NRing READ getNRing WRITE setNRing)
  // PYB11_PROPERTY(bool ComputePrincipalDirectionVectors READ getComputePrincipalDirectionVectors WRITE setComputePrincipalDirectionVectors)
  // PYB11_PROPERTY(bool ComputeMeanCurvature READ getComputeMeanCurvature WRITE setComputeMeanCurvature)
  // PYB11_PROPERTY(bool ComputeGaussianCurvature READ getComputeGaussianCurvature WRITE setComputeGaussianCurvature)
  // PYB11_PROPERTY(bool UseNormalsForCurveFitting READ getUseNormalsForCurveFitting WRITE setUseNormalsForCurveFitting)
  // PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)
  // PYB11_PROPERTY(DataArrayPath SurfaceMeshFeatureFaceIdsArrayPath READ getSurfaceMeshFeatureFaceIdsArrayPath WRITE setSurfaceMeshFeatureFaceIdsArrayPath)
  // PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceNormalsArrayPath READ getSurfaceMeshFaceNormalsArrayPath WRITE setSurfaceMeshFaceNormalsArrayPath)
  // PYB11_PROPERTY(DataArrayPath SurfaceMeshTriangleCentroidsArrayPath READ getSurfaceMeshTriangleCentroidsArrayPath WRITE setSurfaceMeshTriangleCentroidsArrayPath)
#endif

public:
  using Self = FeatureFaceCurvatureFilter;
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
   * @brief Returns the name of the class for FeatureFaceCurvatureFilter
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FeatureFaceCurvatureFilter
   */
  static QString ClassName();

  ~FeatureFaceCurvatureFilter() override;

  typedef std::vector<int64_t> FaceIds_t;
  typedef std::map<int32_t, FaceIds_t> SharedFeatureFaces_t;

  /**
   * @brief Setter property for FaceAttributeMatrixPath
   */
  void setFaceAttributeMatrixPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FaceAttributeMatrixPath
   * @return Value of FaceAttributeMatrixPath
   */
  DataArrayPath getFaceAttributeMatrixPath() const;

  Q_PROPERTY(DataArrayPath FaceAttributeMatrixPath READ getFaceAttributeMatrixPath WRITE setFaceAttributeMatrixPath)

  /**
   * @brief Setter property for SurfaceMeshPrincipalCurvature1sArrayName
   */
  void setSurfaceMeshPrincipalCurvature1sArrayName(const QString& value);
  /**
   * @brief Getter property for SurfaceMeshPrincipalCurvature1sArrayName
   * @return Value of SurfaceMeshPrincipalCurvature1sArrayName
   */
  QString getSurfaceMeshPrincipalCurvature1sArrayName() const;

  Q_PROPERTY(QString SurfaceMeshPrincipalCurvature1sArrayName READ getSurfaceMeshPrincipalCurvature1sArrayName WRITE setSurfaceMeshPrincipalCurvature1sArrayName)

  /**
   * @brief Setter property for SurfaceMeshPrincipalCurvature2sArrayName
   */
  void setSurfaceMeshPrincipalCurvature2sArrayName(const QString& value);
  /**
   * @brief Getter property for SurfaceMeshPrincipalCurvature2sArrayName
   * @return Value of SurfaceMeshPrincipalCurvature2sArrayName
   */
  QString getSurfaceMeshPrincipalCurvature2sArrayName() const;

  Q_PROPERTY(QString SurfaceMeshPrincipalCurvature2sArrayName READ getSurfaceMeshPrincipalCurvature2sArrayName WRITE setSurfaceMeshPrincipalCurvature2sArrayName)

  /**
   * @brief Setter property for SurfaceMeshPrincipalDirection1sArrayName
   */
  void setSurfaceMeshPrincipalDirection1sArrayName(const QString& value);
  /**
   * @brief Getter property for SurfaceMeshPrincipalDirection1sArrayName
   * @return Value of SurfaceMeshPrincipalDirection1sArrayName
   */
  QString getSurfaceMeshPrincipalDirection1sArrayName() const;

  Q_PROPERTY(QString SurfaceMeshPrincipalDirection1sArrayName READ getSurfaceMeshPrincipalDirection1sArrayName WRITE setSurfaceMeshPrincipalDirection1sArrayName)

  /**
   * @brief Setter property for SurfaceMeshPrincipalDirection2sArrayName
   */
  void setSurfaceMeshPrincipalDirection2sArrayName(const QString& value);
  /**
   * @brief Getter property for SurfaceMeshPrincipalDirection2sArrayName
   * @return Value of SurfaceMeshPrincipalDirection2sArrayName
   */
  QString getSurfaceMeshPrincipalDirection2sArrayName() const;

  Q_PROPERTY(QString SurfaceMeshPrincipalDirection2sArrayName READ getSurfaceMeshPrincipalDirection2sArrayName WRITE setSurfaceMeshPrincipalDirection2sArrayName)

  /**
   * @brief Setter property for SurfaceMeshGaussianCurvaturesArrayName
   */
  void setSurfaceMeshGaussianCurvaturesArrayName(const QString& value);
  /**
   * @brief Getter property for SurfaceMeshGaussianCurvaturesArrayName
   * @return Value of SurfaceMeshGaussianCurvaturesArrayName
   */
  QString getSurfaceMeshGaussianCurvaturesArrayName() const;

  Q_PROPERTY(QString SurfaceMeshGaussianCurvaturesArrayName READ getSurfaceMeshGaussianCurvaturesArrayName WRITE setSurfaceMeshGaussianCurvaturesArrayName)

  /**
   * @brief Setter property for SurfaceMeshMeanCurvaturesArrayName
   */
  void setSurfaceMeshMeanCurvaturesArrayName(const QString& value);
  /**
   * @brief Getter property for SurfaceMeshMeanCurvaturesArrayName
   * @return Value of SurfaceMeshMeanCurvaturesArrayName
   */
  QString getSurfaceMeshMeanCurvaturesArrayName() const;

  Q_PROPERTY(QString SurfaceMeshMeanCurvaturesArrayName READ getSurfaceMeshMeanCurvaturesArrayName WRITE setSurfaceMeshMeanCurvaturesArrayName)

  /**
   * @brief Setter property for NRing
   */
  void setNRing(int value);
  /**
   * @brief Getter property for NRing
   * @return Value of NRing
   */
  int getNRing() const;

  Q_PROPERTY(int NRing READ getNRing WRITE setNRing)

  /**
   * @brief Setter property for ComputePrincipalDirectionVectors
   */
  void setComputePrincipalDirectionVectors(bool value);
  /**
   * @brief Getter property for ComputePrincipalDirectionVectors
   * @return Value of ComputePrincipalDirectionVectors
   */
  bool getComputePrincipalDirectionVectors() const;

  Q_PROPERTY(bool ComputePrincipalDirectionVectors READ getComputePrincipalDirectionVectors WRITE setComputePrincipalDirectionVectors)

  /**
   * @brief Setter property for ComputeMeanCurvature
   */
  void setComputeMeanCurvature(bool value);
  /**
   * @brief Getter property for ComputeMeanCurvature
   * @return Value of ComputeMeanCurvature
   */
  bool getComputeMeanCurvature() const;

  Q_PROPERTY(bool ComputeMeanCurvature READ getComputeMeanCurvature WRITE setComputeMeanCurvature)

  /**
   * @brief Setter property for ComputeGaussianCurvature
   */
  void setComputeGaussianCurvature(bool value);
  /**
   * @brief Getter property for ComputeGaussianCurvature
   * @return Value of ComputeGaussianCurvature
   */
  bool getComputeGaussianCurvature() const;

  Q_PROPERTY(bool ComputeGaussianCurvature READ getComputeGaussianCurvature WRITE setComputeGaussianCurvature)

  /**
   * @brief Setter property for UseNormalsForCurveFitting
   */
  void setUseNormalsForCurveFitting(bool value);
  /**
   * @brief Getter property for UseNormalsForCurveFitting
   * @return Value of UseNormalsForCurveFitting
   */
  bool getUseNormalsForCurveFitting() const;

  Q_PROPERTY(bool UseNormalsForCurveFitting READ getUseNormalsForCurveFitting WRITE setUseNormalsForCurveFitting)

  /**
   * @brief This returns the group that the filter belonds to. You can select
   * a different group if you want. The string returned here will be displayed
   * in the GUI for the filter
   */
  /**
   * @brief Setter property for SurfaceMeshFaceLabelsArrayPath
   */
  void setSurfaceMeshFaceLabelsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SurfaceMeshFaceLabelsArrayPath
   * @return Value of SurfaceMeshFaceLabelsArrayPath
   */
  DataArrayPath getSurfaceMeshFaceLabelsArrayPath() const;

  Q_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)

  /**
   * @brief Setter property for SurfaceMeshFeatureFaceIdsArrayPath
   */
  void setSurfaceMeshFeatureFaceIdsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SurfaceMeshFeatureFaceIdsArrayPath
   * @return Value of SurfaceMeshFeatureFaceIdsArrayPath
   */
  DataArrayPath getSurfaceMeshFeatureFaceIdsArrayPath() const;

  Q_PROPERTY(DataArrayPath SurfaceMeshFeatureFaceIdsArrayPath READ getSurfaceMeshFeatureFaceIdsArrayPath WRITE setSurfaceMeshFeatureFaceIdsArrayPath)

  /**
   * @brief Setter property for SurfaceMeshFaceNormalsArrayPath
   */
  void setSurfaceMeshFaceNormalsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SurfaceMeshFaceNormalsArrayPath
   * @return Value of SurfaceMeshFaceNormalsArrayPath
   */
  DataArrayPath getSurfaceMeshFaceNormalsArrayPath() const;

  Q_PROPERTY(DataArrayPath SurfaceMeshFaceNormalsArrayPath READ getSurfaceMeshFaceNormalsArrayPath WRITE setSurfaceMeshFaceNormalsArrayPath)

  /**
   * @brief Setter property for SurfaceMeshTriangleCentroidsArrayPath
   */
  void setSurfaceMeshTriangleCentroidsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SurfaceMeshTriangleCentroidsArrayPath
   * @return Value of SurfaceMeshTriangleCentroidsArrayPath
   */
  DataArrayPath getSurfaceMeshTriangleCentroidsArrayPath() const;

  Q_PROPERTY(DataArrayPath SurfaceMeshTriangleCentroidsArrayPath READ getSurfaceMeshTriangleCentroidsArrayPath WRITE setSurfaceMeshTriangleCentroidsArrayPath)

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

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
    virtual void tbbTaskProgress();
#endif

  protected:
    FeatureFaceCurvatureFilter();
    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief Initializes all the private instance variables.
     */
    void initialize();


  private:
    std::weak_ptr<DataArray<int32_t>> m_SurfaceMeshFaceLabelsPtr;
    int32_t* m_SurfaceMeshFaceLabels = nullptr;
    std::weak_ptr<DataArray<double>> m_SurfaceMeshTriangleCentroidsPtr;
    double* m_SurfaceMeshTriangleCentroids = nullptr;
    std::weak_ptr<DataArray<double>> m_SurfaceMeshFaceNormalsPtr;
    double* m_SurfaceMeshFaceNormals = nullptr;
    std::weak_ptr<DataArray<int32_t>> m_SurfaceMeshFeatureFaceIdsPtr;
    int32_t* m_SurfaceMeshFeatureFaceIds = nullptr;
    std::weak_ptr<DataArray<double>> m_SurfaceMeshPrincipalCurvature1sPtr;
    double* m_SurfaceMeshPrincipalCurvature1s = nullptr;
    std::weak_ptr<DataArray<double>> m_SurfaceMeshPrincipalCurvature2sPtr;
    double* m_SurfaceMeshPrincipalCurvature2s = nullptr;
    std::weak_ptr<DataArray<double>> m_SurfaceMeshPrincipalDirection1sPtr;
    double* m_SurfaceMeshPrincipalDirection1s = nullptr;
    std::weak_ptr<DataArray<double>> m_SurfaceMeshPrincipalDirection2sPtr;
    double* m_SurfaceMeshPrincipalDirection2s = nullptr;
    std::weak_ptr<DataArray<double>> m_SurfaceMeshGaussianCurvaturesPtr;
    double* m_SurfaceMeshGaussianCurvatures = nullptr;
    std::weak_ptr<DataArray<double>> m_SurfaceMeshMeanCurvaturesPtr;
    double* m_SurfaceMeshMeanCurvatures = nullptr;

    DataArrayPath m_FaceAttributeMatrixPath = {};
    QString m_SurfaceMeshPrincipalCurvature1sArrayName = {};
    QString m_SurfaceMeshPrincipalCurvature2sArrayName = {};
    QString m_SurfaceMeshPrincipalDirection1sArrayName = {};
    QString m_SurfaceMeshPrincipalDirection2sArrayName = {};
    QString m_SurfaceMeshGaussianCurvaturesArrayName = {};
    QString m_SurfaceMeshMeanCurvaturesArrayName = {};
    int m_NRing = {};
    bool m_ComputePrincipalDirectionVectors = {};
    bool m_ComputeMeanCurvature = {};
    bool m_ComputeGaussianCurvature = {};
    bool m_UseNormalsForCurveFitting = {};
    DataArrayPath m_SurfaceMeshFaceLabelsArrayPath = {};
    DataArrayPath m_SurfaceMeshFeatureFaceIdsArrayPath = {};
    DataArrayPath m_SurfaceMeshFaceNormalsArrayPath = {};
    DataArrayPath m_SurfaceMeshTriangleCentroidsArrayPath = {};

    DataArray<int32_t>::WeakPointer m_SurfaceMeshUniqueEdgesPtr;

    int32_t* m_SurfaceMeshFaceEdges;
    int32_t  m_TotalFeatureFaces;
    int32_t  m_CompletedFeatureFaces;

  public:
    FeatureFaceCurvatureFilter(const FeatureFaceCurvatureFilter&) = delete; // Copy Constructor Not Implemented
    FeatureFaceCurvatureFilter(FeatureFaceCurvatureFilter&&) = delete;      // Move Constructor Not Implemented
    FeatureFaceCurvatureFilter& operator=(const FeatureFaceCurvatureFilter&) = delete; // Copy Assignment Not Implemented
    FeatureFaceCurvatureFilter& operator=(FeatureFaceCurvatureFilter&&) = delete;      // Move Assignment Not Implemented
};
