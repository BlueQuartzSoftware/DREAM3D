/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _FindGBCD_H_
#define _FindGBCD_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "SurfaceMeshing/SurfaceMeshingFilters/SurfaceMeshFilter.h"

/**
 * @brief The FindGBCD class. See [Filter documentation](@ref findgbcd) for details.
 */
class FindGBCD : public SurfaceMeshFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(FindGBCD)
    DREAM3D_STATIC_NEW_MACRO(FindGBCD)
    DREAM3D_TYPE_MACRO_SUPER(FindGBCD, SurfaceMeshFilter)

    virtual ~FindGBCD();

    DREAM3D_FILTER_PARAMETER(QString, FaceEnsembleAttributeMatrixName)
    Q_PROPERTY(QString FaceEnsembleAttributeMatrixName READ getFaceEnsembleAttributeMatrixName WRITE setFaceEnsembleAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(float, GBCDRes)
    Q_PROPERTY(float GBCDRes READ getGBCDRes WRITE setGBCDRes)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceLabelsArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceNormalsArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshFaceNormalsArrayPath READ getSurfaceMeshFaceNormalsArrayPath WRITE setSurfaceMeshFaceNormalsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceAreasArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshFaceAreasArrayPath READ getSurfaceMeshFaceAreasArrayPath WRITE setSurfaceMeshFaceAreasArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeatureEulerAnglesArrayPath)
    Q_PROPERTY(DataArrayPath FeatureEulerAnglesArrayPath READ getFeatureEulerAnglesArrayPath WRITE setFeatureEulerAnglesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
    Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
    Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, GBCDArrayName)
    Q_PROPERTY(QString GBCDArrayName READ getGBCDArrayName WRITE setGBCDArrayName)

    DREAM3D_INSTANCE_PROPERTY(QVector<ComparisonInput_t>, GBCDArrayNames)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const QString getHumanLabel();

    /**
    * @brief This method will instantiate all the end user settable options/parameters
    * for this filter
    */
    virtual void setupFilterParameters();

    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
    * @brief Reimplemented from @see AbstractFilter class
    */
    virtual void execute();

    /**
    * @brief This function runs some sanity checks on the DataContainer and inputs
    * in an attempt to ensure the filter can process the inputs.
    */
    virtual void preflight();

    int GBCDIndex (float* gbcddelta, int* gbcdsz, float* gbcdlimits, float* eulerN, float* xstl_norm_sc);

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    FindGBCD();

    void dataCheckSurfaceMesh();
    void dataCheckVoxel();

    void sizeGBCD(int faceChunkSize, int numMisoReps);

  private:
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(double, SurfaceMeshFaceAreas)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFaceLabels)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(double, SurfaceMeshFaceNormals)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, FeatureEulerAngles)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(unsigned int, CrystalStructures)

    DEFINE_CREATED_DATAARRAY_VARIABLE(double, GBCD)

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
