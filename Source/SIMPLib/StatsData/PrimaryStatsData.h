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


#ifndef _PrimaryStatsDataTUPLE_H_
#define _PrimaryStatsDataTUPLE_H_

#include <hdf5.h>


#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/StatsData/StatsData.h"

typedef QVector<FloatArrayType::Pointer> VectorOfFloatArray;

/**
 * @class PrimaryStatsData PrimaryStatsData.h DREAM3DLib/StatsData/PrimaryStatsData.h
 * @brief This class holds the statistical data for a single phase of a microstructure.
 * There are several statistics that are held by this class in a varying number of
 * storage types. Some types have specific ordering of the sub arrays with in them. Those
 * are enumerated here:@n
 * For the distributions a QVector holds objects of the DataArray<float>::Pointer which is
 * typedef'ed to FloatArrayType @see DataArray.hpp. The order of the FloatArrayType
 * objects in the QVector is as follows:
 *   <b>Beta Distribution</b>@n
 *    @li Alpha
 *    @li Beta
 *    <b>Log Normal Distribution</b>@n
 *    @li Average
 *    @li Standard Deviation
 *    <b>Power Law Distribution</b>@n
 *    @li Alpha
 *    @li K
 *    @li Beta
 *
 * The Feature Diameter Info is an array of 3 floats where the values are encoded as:@n
 * @li [0] = Average
 * @li [1] = Max
 * @li [2] = Min
 *
 * The Feature Size Distribution is an array of 2 floats where the values are encoded as:@n
 * @li [0] = Average
 * @li [1] = Standard Deviation
 *
 *
 * @date Feb 9, 2012
 * @version 1.0
 */
class SIMPLib_EXPORT PrimaryStatsData : public StatsData
{
  public:
    SIMPL_SHARED_POINTERS(PrimaryStatsData)
    SIMPL_STATIC_NEW_MACRO(PrimaryStatsData)
    SIMPL_TYPE_MACRO_SUPER(PrimaryStatsData, StatsData)

    virtual ~PrimaryStatsData();

    /**
     * @brief GetTypeName Returns a string representation of the type of data that is stored by this class. This
     * can be a primitive like char, float, int or the name of a class.
     * @return
     */
    void getXdmfTypeAndSize(QString& xdmfTypeName, int& precision)
    {
      xdmfTypeName = getNameOfClass();
      precision = 0;
    }

    virtual QString getStatsType();
    virtual unsigned int getPhaseType();

    SIMPL_INSTANCE_PROPERTY(float, BoundaryArea)
    SIMPL_INSTANCE_PROPERTY(float, PhaseFraction)


    /**
      * @breif this will generate the Bin Numbers values;
      */
    FloatArrayType::Pointer generateBinNumbers();
    /**
     * @brief The values are encoded into 3 floats: Average, Max, Min
     */
    SIMPL_INSTANCE_VEC3_PROPERTY(float, FeatureDiameterInfo)
    void setBinStepSize(float v) { m_FeatureDiameterInfo[0] = v;}
    float getBinStepSize() { return m_FeatureDiameterInfo[0]; }

    void setMaxFeatureDiameter(float v) { m_FeatureDiameterInfo[1] = v;}
    float getMaxFeatureDiameter() { return m_FeatureDiameterInfo[1]; }

    void setMinFeatureDiameter(float v) { m_FeatureDiameterInfo[2] = v;}
    float getMinFeatureDiameter() { return m_FeatureDiameterInfo[2]; }

    /**
      * @brief The values are encoded into float arrays
      */
    SIMPL_INSTANCE_PROPERTY(VectorOfFloatArray, FeatureSizeDistribution)
    SIMPL_INSTANCE_PROPERTY(uint32_t, FeatureSize_DistType)

    SIMPL_INSTANCE_PROPERTY(FloatArrayType::Pointer, BinNumbers)

    size_t getNumberOfBins()
    {
      return (m_BinNumbers.get() == NULL) ? 0 : m_BinNumbers->getSize();
    }


    SIMPL_INSTANCE_PROPERTY(VectorOfFloatArray, FeatureSize_BOverA)
    SIMPL_INSTANCE_PROPERTY(uint32_t, BOverA_DistType)

    SIMPL_INSTANCE_PROPERTY(VectorOfFloatArray, FeatureSize_COverA)
    SIMPL_INSTANCE_PROPERTY(uint32_t, COverA_DistType)

    SIMPL_INSTANCE_PROPERTY(VectorOfFloatArray, FeatureSize_Neighbors)
    SIMPL_INSTANCE_PROPERTY(uint32_t, Neighbors_DistType)

    SIMPL_INSTANCE_PROPERTY(VectorOfFloatArray, FeatureSize_Omegas)
    SIMPL_INSTANCE_PROPERTY(uint32_t, Omegas_DistType)

    SIMPL_INSTANCE_PROPERTY(FloatArrayType::Pointer, MisorientationBins)
    /* 3 Vectors: Angles, Axis, Weights */
    SIMPL_INSTANCE_PROPERTY(VectorOfFloatArray, MDF_Weights)

    SIMPL_INSTANCE_PROPERTY(FloatArrayType::Pointer, ODF)
    /* 5 Vectors: Euler 1, Euler 2, Euler 3, Sigma, Weights */
    SIMPL_INSTANCE_PROPERTY(VectorOfFloatArray, ODF_Weights)

    SIMPL_INSTANCE_PROPERTY(FloatArrayType::Pointer, AxisOrientation)
    /* 5 Vectors: Euler 1, Euler 2, Euler 3, Sigma, Weights */
    SIMPL_INSTANCE_PROPERTY(VectorOfFloatArray, AxisODF_Weights)

    virtual void initialize();

    virtual int writeHDF5Data(hid_t groupId);
    virtual int readHDF5Data(hid_t groupId);

  protected:
    PrimaryStatsData();

  private:
    PrimaryStatsData(const PrimaryStatsData&); // Copy Constructor Not Implemented
    void operator=(const PrimaryStatsData&); // Operator '=' Not Implemented
};

#endif /* _PrimaryStatsDataTUPLE_H_ */

