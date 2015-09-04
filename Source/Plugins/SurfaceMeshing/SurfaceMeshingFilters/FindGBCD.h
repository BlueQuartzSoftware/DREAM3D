/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#ifndef _FindGBCD_H_
#define _FindGBCD_H_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "SurfaceMeshing/SurfaceMeshingFilters/SurfaceMeshFilter.h"

/**
 * @brief The FindGBCD class. See [Filter documentation](@ref findgbcd) for details.
 */
class FindGBCD : public SurfaceMeshFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    SIMPL_SHARED_POINTERS(FindGBCD)
    SIMPL_STATIC_NEW_MACRO(FindGBCD)
    SIMPL_TYPE_MACRO_SUPER(FindGBCD, SurfaceMeshFilter)

    virtual ~FindGBCD();

    SIMPL_FILTER_PARAMETER(QString, FaceEnsembleAttributeMatrixName)
    Q_PROPERTY(QString FaceEnsembleAttributeMatrixName READ getFaceEnsembleAttributeMatrixName WRITE setFaceEnsembleAttributeMatrixName)

    SIMPL_FILTER_PARAMETER(float, GBCDRes)
    Q_PROPERTY(float GBCDRes READ getGBCDRes WRITE setGBCDRes)

    SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceLabelsArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceNormalsArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshFaceNormalsArrayPath READ getSurfaceMeshFaceNormalsArrayPath WRITE setSurfaceMeshFaceNormalsArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceAreasArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshFaceAreasArrayPath READ getSurfaceMeshFaceAreasArrayPath WRITE setSurfaceMeshFaceAreasArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureEulerAnglesArrayPath)
    Q_PROPERTY(DataArrayPath FeatureEulerAnglesArrayPath READ getFeatureEulerAnglesArrayPath WRITE setFeatureEulerAnglesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
    Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
    Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

    SIMPL_FILTER_PARAMETER(QString, GBCDArrayName)
    Q_PROPERTY(QString GBCDArrayName READ getGBCDArrayName WRITE setGBCDArrayName)

    SIMPL_INSTANCE_PROPERTY(QVector<ComparisonInput_t>, GBCDArrayNames)

    /**
     * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getCompiledLibraryName();

    /**
     * @brief newFilterInstance Reimplemented from @see AbstractFilter class
     */
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);

    /**
     * @brief getGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getGroupName();

    /**
     * @brief getSubGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getSubGroupName();

    /**
     * @brief getHumanLabel Reimplemented from @see AbstractFilter class
     */
    virtual const QString getHumanLabel();

    /**
     * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void setupFilterParameters();

    /**
     * @brief writeFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
     * @brief readFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
     * @brief execute Reimplemented from @see AbstractFilter class
     */
    virtual void execute();

    /**
    * @brief preflight Reimplemented from @see AbstractFilter class
    */
    virtual void preflight();

  protected:
    FindGBCD();

    /**
     * @brief dataCheckSurfaceMesh Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheckSurfaceMesh();

    /**
     * @brief dataCheckVoxel Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheckVoxel();

    /**
     * @brief sizeGBCD Determines the sizing for the GBCD arrays
     * @param faceChunkSize Number of triangles per chunk
     * @param numMisoReps Dimensionality for bins
     */
    void sizeGBCD(size_t faceChunkSize, size_t numMisoReps);

  private:
    DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshFaceAreas)
    DEFINE_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFaceLabels)
    DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshFaceNormals)
    DEFINE_DATAARRAY_VARIABLE(float, FeatureEulerAngles)
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
    DEFINE_DATAARRAY_VARIABLE(unsigned int, CrystalStructures)
    DEFINE_DATAARRAY_VARIABLE(double, GBCD)

    FloatArrayType::Pointer m_GbcdDeltasArray;
    Int32ArrayType::Pointer m_GbcdSizesArray;
    FloatArrayType::Pointer m_GbcdLimitsArray;
    Int32ArrayType::Pointer m_GbcdBinsArray;
    BoolArrayType::Pointer m_GbcdHemiCheckArray;

    float* m_GbcdDeltas;
    int32_t* m_GbcdSizes;
    float* m_GbcdLimits;
    int32_t* m_GbcdBins;
    bool* m_HemiCheck;

    FindGBCD(const FindGBCD&); // Copy Constructor Not Implemented
    void operator=(const FindGBCD&); // Operator '=' Not Implemented
};

#endif /* _FindGBCD_H_ */
