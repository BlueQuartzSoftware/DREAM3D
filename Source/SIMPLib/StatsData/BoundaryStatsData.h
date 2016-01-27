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


#ifndef _BoundaryStatsDataTUPLE_H_
#define _BoundaryStatsDataTUPLE_H_

#include <hdf5.h>


#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/StatsData/StatsData.h"

typedef QVector<FloatArrayType::Pointer> VectorOfFloatArray;

/**
 * @class BoundaryStatsData BoundaryStatsData.h DREAM3DLib/StatsData/BoundaryStatsData.h
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
class SIMPLib_EXPORT BoundaryStatsData : public StatsData
{
  public:
    SIMPL_SHARED_POINTERS(BoundaryStatsData)
    SIMPL_STATIC_NEW_MACRO(BoundaryStatsData)
    SIMPL_TYPE_MACRO_SUPER(BoundaryStatsData, StatsData)

    virtual ~BoundaryStatsData();

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

    /**
     * @brief Creates the Vector of FloatArrayType for a give distribution type
     * and sized according to numBins
     * @param distributionType The type of distribution as laid out in the DREAM3D::DistributionType
     * namespace which should be found in Constants.h.
     * @param numBins The number of bins that the array should be sized to
     * @return a VectorOfFloatArray object which could be empty if an unrecognized
     * distribution type is passed in.
     */
    static VectorOfFloatArray CreateCorrelatedDistributionArrays(uint32_t distributionType, size_t numBins);
    static FloatArrayType::Pointer CreateDistributionArrays(uint32_t distributionType);


    SIMPL_INSTANCE_PROPERTY(float, BoundaryArea)
    SIMPL_INSTANCE_PROPERTY(float, PhaseFraction)
    SIMPL_INSTANCE_PROPERTY(float, ParentPhase)

    virtual void initialize();

    virtual int writeHDF5Data(hid_t groupId);
    virtual int readHDF5Data(hid_t groupId);

    virtual QString getStatsType();
    virtual unsigned int getPhaseType();
  protected:
    BoundaryStatsData();

  private:
    BoundaryStatsData(const BoundaryStatsData&); // Copy Constructor Not Implemented
    void operator=(const BoundaryStatsData&); // Operator '=' Not Implemented
};

#endif /* _BoundaryStatsDataTUPLE_H_ */

