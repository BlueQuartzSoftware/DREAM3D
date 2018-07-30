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
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "SurfaceMeshing/SurfaceMeshingFilters/SurfaceMeshFilter.h"

#include "SurfaceMeshing/SurfaceMeshingDLLExport.h"

/**
 * @brief The SharedFeatureFaceFilter class. See [Filter documentation](@ref sharedfeaturefacefilter) for details.
 */
class SurfaceMeshing_EXPORT SharedFeatureFaceFilter : public SurfaceMeshFilter
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(SharedFeatureFaceFilter SUPERCLASS SurfaceMeshFilter)
    PYB11_PROPERTY(QString FaceFeatureAttributeMatrixName READ getFaceFeatureAttributeMatrixName WRITE setFaceFeatureAttributeMatrixName)
    PYB11_PROPERTY(QString SurfaceMeshFeatureFaceIdsArrayName READ getSurfaceMeshFeatureFaceIdsArrayName WRITE setSurfaceMeshFeatureFaceIdsArrayName)
    PYB11_PROPERTY(QString SurfaceMeshFeatureFaceLabelsArrayName READ getSurfaceMeshFeatureFaceLabelsArrayName WRITE setSurfaceMeshFeatureFaceLabelsArrayName)
    PYB11_PROPERTY(QString SurfaceMeshFeatureFaceNumTrianglesArrayName READ getSurfaceMeshFeatureFaceNumTrianglesArrayName WRITE setSurfaceMeshFeatureFaceNumTrianglesArrayName)
    PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)
public:
  SIMPL_SHARED_POINTERS(SharedFeatureFaceFilter)
  SIMPL_FILTER_NEW_MACRO(SharedFeatureFaceFilter)
   SIMPL_TYPE_MACRO_SUPER_OVERRIDE(SharedFeatureFaceFilter, SurfaceMeshFilter)

   ~SharedFeatureFaceFilter() override;

   typedef QVector<int32_t> FaceIds_t;
   typedef QMap<int32_t, FaceIds_t> SharedFeatureFaces_t;

   SIMPL_FILTER_PARAMETER(QString, FaceFeatureAttributeMatrixName)
   Q_PROPERTY(QString FaceFeatureAttributeMatrixName READ getFaceFeatureAttributeMatrixName WRITE setFaceFeatureAttributeMatrixName)

   SIMPL_FILTER_PARAMETER(QString, SurfaceMeshFeatureFaceIdsArrayName)
   Q_PROPERTY(QString SurfaceMeshFeatureFaceIdsArrayName READ getSurfaceMeshFeatureFaceIdsArrayName WRITE setSurfaceMeshFeatureFaceIdsArrayName)

   SIMPL_FILTER_PARAMETER(QString, SurfaceMeshFeatureFaceLabelsArrayName)
   Q_PROPERTY(QString SurfaceMeshFeatureFaceLabelsArrayName READ getSurfaceMeshFeatureFaceLabelsArrayName WRITE setSurfaceMeshFeatureFaceLabelsArrayName)

   SIMPL_FILTER_PARAMETER(QString, SurfaceMeshFeatureFaceNumTrianglesArrayName)
   Q_PROPERTY(QString SurfaceMeshFeatureFaceNumTrianglesArrayName READ getSurfaceMeshFeatureFaceNumTrianglesArrayName WRITE setSurfaceMeshFeatureFaceNumTrianglesArrayName)

   SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceLabelsArrayPath)
   Q_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)

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

 protected:
   SharedFeatureFaceFilter();
   /**
    * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
    */
   void dataCheck();

   /**
    * @brief Initializes all the private instance variables.
    */
   void initialize();

 private:
   DEFINE_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFaceLabels)

   DEFINE_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFeatureFaceIds)
   DEFINE_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFeatureFaceLabels)
   DEFINE_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFeatureFaceNumTriangles)

 public:
   SharedFeatureFaceFilter(const SharedFeatureFaceFilter&) = delete;            // Copy Constructor Not Implemented
   SharedFeatureFaceFilter(SharedFeatureFaceFilter&&) = delete;                 // Move Constructor Not Implemented
   SharedFeatureFaceFilter& operator=(const SharedFeatureFaceFilter&) = delete; // Copy Assignment Not Implemented
   SharedFeatureFaceFilter& operator=(SharedFeatureFaceFilter&&) = delete;      //           // Move assignment Not Implemented
};

